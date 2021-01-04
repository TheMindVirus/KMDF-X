#include "KMDFHDMI.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#endif

//#ifndef IO_RESOURCE_REQUIRED
//#define IO_RESOURCE_REQUIRED   0
//#endif

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    debug("[CALL]: %s START", "DriverEntry"); 
    NTSTATUS status = STATUS_SUCCESS;
    PKMDFHDMI_CONTEXT context = NULL;
    //BOOLEAN ResourceConflict = FALSE;
    WDF_DRIVER_CONFIG DriverConfig;

    status = IoAllocateDriverObjectExtension(DriverObject, KMDFHDMI_TAG_EXTENSION, sizeof(KMDFHDMI_CONTEXT), &context);
    if (!NT_SUCCESS(status)) { debug("[WARN]: IoAllocateDriverObjectExtension: 0x%016llX", status); return status; }

    /*ULONG ResourcesSize = sizeof(CM_RESOURCE_LIST) + (sizeof(CM_PARTIAL_RESOURCE_LIST) + sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)) * 2;
    context->resources = (PCM_RESOURCE_LIST)ExAllocatePoolWithTag(PagedPool, ResourcesSize, KMDFHDMI_TAG_RESOURCES);
    if (!(context->resources)) { status = STATUS_MEMORY_NOT_ALLOCATED; debug("[WARN]: ExAllocatePoolWithTag: 0x%016llX", status); return status; }

    context->resources->Count = 2;
    context->membase = NULL;
    context->iobase = NULL;

    context->resources->List[0].PartialResourceList.Version = 1;
    context->resources->List[0].PartialResourceList.Revision = 1;
    context->resources->List[0].PartialResourceList.Count = 1;
    context->resources->List[0].PartialResourceList.PartialDescriptors->Type = CmResourceTypeMemory;
    context->resources->List[0].PartialResourceList.PartialDescriptors->Flags = CM_RESOURCE_MEMORY_READ_WRITE; // | CM_RESOURCE_MEMORY_COMBINEDWRITE;
    context->resources->List[0].PartialResourceList.PartialDescriptors->ShareDisposition = CmResourceShareShared;
    context->resources->List[0].PartialResourceList.PartialDescriptors->u.Memory.Start.QuadPart = GPIO_BASE;
    context->resources->List[0].PartialResourceList.PartialDescriptors->u.Memory.Length = GPIO_SIZE;

    context->resources->List[1].PartialResourceList.Version = 1;
    context->resources->List[1].PartialResourceList.Revision = 1;
    context->resources->List[1].PartialResourceList.Count = 1;
    context->resources->List[1].PartialResourceList.PartialDescriptors->Type = CmResourceTypePort;
    context->resources->List[1].PartialResourceList.PartialDescriptors->Flags = CM_RESOURCE_PORT_MEMORY;
    context->resources->List[1].PartialResourceList.PartialDescriptors->ShareDisposition = CmResourceShareShared;
    context->resources->List[1].PartialResourceList.PartialDescriptors->u.Port.Start.QuadPart = GPIO_BASE;
    context->resources->List[1].PartialResourceList.PartialDescriptors->u.Port.Length = GPIO_SIZE;
    
    status = IoReportResourceForDetection(DriverObject, context->resources, context->resources->Count, NULL, NULL, 0, &ResourceConflict);
    if (!NT_SUCCESS(status)) { debug("[WARN]: IoReportResourceForDetection: 0x%016llX (%s)", status, (ResourceConflict) ? "Conflict" : "No Conflict"); return status; }
    else { debug("[INFO]: IoReportResourceForDetection (%s)", (ResourceConflict) ? "Conflict" : "No Conflict"); }
    ExFreePoolWithTag(context->resources, KMDFHDMI_TAG_RESOURCES);*/

    WDF_DRIVER_CONFIG_INIT(&DriverConfig, KMDFHDMIDeviceAdd);
    DriverConfig.EvtDriverUnload = KMDFHDMIDriverUnload;
    status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &DriverConfig, WDF_NO_HANDLE);
    if (!NT_SUCCESS(status)) { debug("[WARN]: %s: 0x%016llX", "WdfDriverCreate", status); return status; }
    
    debug("[CALL]: %s END", "DriverEntry");
    return STATUS_SUCCESS;
}

NTSTATUS KMDFHDMIDeviceAdd(WDFDRIVER Driver, PWDFDEVICE_INIT DeviceInit)
{
    UNREFERENCED_PARAMETER(Driver);
    debug("[CALL]: %s START", "KMDFHDMIDeviceAdd");
    NTSTATUS status = STATUS_SUCCESS;
    WDF_PNPPOWER_EVENT_CALLBACKS PnPevents;
    WDFDEVICE Device;
    
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&PnPevents);
    PnPevents.EvtDevicePrepareHardware = KMDFHDMIPrepareHardware;
    PnPevents.EvtDeviceReleaseHardware = KMDFHDMIReleaseHardware;
    PnPevents.EvtDeviceD0Entry = KMDFHDMIPowerUp;
    PnPevents.EvtDeviceD0Exit = KMDFHDMIPowerDown;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &PnPevents);

    status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &Device);
    if (!NT_SUCCESS(status)) { debug("[WARN]: %s: 0x%016llX", "WdfDeviceCreate", status); return status; }

    debug("[CALL]: %s END", "KMDFHDMIDeviceAdd");
    return STATUS_SUCCESS;
}

void KMDFHDMIDriverUnload(WDFDRIVER Driver)
{
    UNREFERENCED_PARAMETER(Driver);
    debug("[CALL]: %s", "KMDFHDMIDriverUnload");
}

NTSTATUS KMDFHDMIPrepareHardware(WDFDEVICE Device, WDFCMRESLIST ResourcesRaw, WDFCMRESLIST ResourcesTranslated)
{
    UNREFERENCED_PARAMETER(ResourcesRaw);
    UNREFERENCED_PARAMETER(ResourcesTranslated);
    debug("[CALL]: %s START", "KMDFHDMIPrepareHardware");
    NTSTATUS status = STATUS_SUCCESS;
    PKMDFHDMI_CONTEXT context = NULL;

    PHYSICAL_ADDRESS start;
    PHYSICAL_ADDRESS end;
    PHYSICAL_ADDRESS fence;
    start.QuadPart = HDMI_BASE;
    end.QuadPart = HDMI_BASE + HDMI_SIZE;
    fence.QuadPart = 0;

    context = IoGetDriverObjectExtension(WdfDriverWdmGetDriverObject(WdfDeviceGetDriver(Device)), KMDFHDMI_TAG_EXTENSION);
    if (!context) { status = STATUS_WDF_INTERNAL_ERROR; debug("[WARN]: %s: 0x%016llX", "IoGetDriverObjectExtension", status); return status; }

    context->membase = (PULONG)MmAllocateContiguousNodeMemory(HDMI_SIZE, start, end, fence, PAGE_READWRITE, MM_ANY_NODE_OK); // | PAGE_WRITECOMBINE, MM_ANY_NODE_OK);
    if (!(context->membase)) { debug("[WARN]: %s: 0x%016llX", "MmAllocateContiguousNodeMemory", STATUS_NONE_MAPPED); }

    context->iobase = (PULONG)MmMapIoSpace(start, HDMI_SIZE, MmNonCached);
    //context->iobase = (PULONG)MmMapIoSpace(start, HDMI_SIZE, MmCached); //MmNonCached causes BSoD
    //context->iobase = (PULONG)MmMapIoSpaceEx(start, HDMI_SIZE, PAGE_READWRITE | PAGE_NOCACHE); //MmNonCached causes BSoD
    if (!(context->iobase)) { debug("[WARN]: %s: 0x%016llX", "MmMapIoSpace", STATUS_NONE_MAPPED); }
    
    debug("[CALL]: %s END", "KMDFHDMIPrepareHardware");
    return STATUS_SUCCESS;
}

NTSTATUS KMDFHDMIReleaseHardware(WDFDEVICE Device, WDFCMRESLIST ResourcesTranslated)
{
    UNREFERENCED_PARAMETER(ResourcesTranslated);
    debug("[CALL]: %s START", "KMDFHDMIReleaseHardware");
    NTSTATUS status = STATUS_SUCCESS;
    PKMDFHDMI_CONTEXT context = NULL;

    context = IoGetDriverObjectExtension(WdfDriverWdmGetDriverObject(WdfDeviceGetDriver(Device)), KMDFHDMI_TAG_EXTENSION);
    if (!context) { status = STATUS_WDF_INTERNAL_ERROR; debug("[WARN]: %s: 0x%016llX", "IoGetDriverObjectExtension", status); return status; }

    if (context->iobase) { MmUnmapIoSpace(context->iobase, HDMI_SIZE); context->iobase = NULL; }
    if (context->membase) { MmFreeContiguousMemory(context->membase); context->membase = NULL; }
    
    debug("[CALL]: %s END", "KMDFHDMIReleaseHardware");
    return STATUS_SUCCESS;
}

NTSTATUS KMDFHDMIPowerUp(WDFDEVICE Device, WDF_POWER_DEVICE_STATE PreviousState)
{
    UNREFERENCED_PARAMETER(PreviousState);
    debug("[CALL]: %s", "KMDFHDMIPowerUp");
    NTSTATUS status = STATUS_SUCCESS;
    PKMDFHDMI_CONTEXT context = NULL;

    context = IoGetDriverObjectExtension(WdfDriverWdmGetDriverObject(WdfDeviceGetDriver(Device)), KMDFHDMI_TAG_EXTENSION);
    if (!context) { status = STATUS_WDF_INTERNAL_ERROR; debug("[WARN]: %s: 0x%016llX", "IoGetDriverObjectExtension", status); return status; }
    
    if (context->membase)
    {
        for (ULONG i = 0; i < HDMI_SIZE / 4; ++i)
        {
            PHYSICAL_ADDRESS address = MmGetPhysicalAddress(context->membase + i);
            //WRITE_REGISTER_NOFENCE_ULONG(context->membase + i, 0b00000000000000000000000000000000);
            debug("[MMIO]: Offset: 0x%02llX | Address: 0x%016llX | Raw Read: 0x%016llX | NoFence Read: 0x%016llX",
                i * 4, address.QuadPart, *(context->membase + i), READ_REGISTER_NOFENCE_ULONG(context->membase + i));
        }
    }
    else { debug("[WARN]: %s: 0x%016llX", "KMDFHDMI.membase", STATUS_NONE_MAPPED); }
    
    if (context->iobase)
    {
        debug("[INFO]: BEGIN_READ");
        for (ULONG i = 0; i < HDMI_SIZE / 4; ++i)
        {
            PHYSICAL_ADDRESS address = MmGetPhysicalAddress(context->iobase + i);
            //WRITE_REGISTER_NOFENCE_ULONG(context->iobase + i, 0b00000000000000000000000000000000);
            debug("[PORT]: Offset: 0x%02llX | Address: 0x%016llX | Raw Read: 0x%016llX | NoFence Read: 0x%016llX",
                i * 4, address.QuadPart, *(context->iobase + i), READ_REGISTER_NOFENCE_ULONG(context->iobase + i));
        }
        debug("[INFO]: END_READ");

      ////WRITE_REGISTER_NOFENCE_ULONG(context->iobase + 0x02, 0b00000000000000000000000000001000);
        //WRITE_REGISTER_NOFENCE_ULONG(context->iobase + 0x02, 0b00000000000000000000000000100000);
      ////WRITE_REGISTER_NOFENCE_ULONG(context->iobase + 0x07, 0b00000000001000000000000000000000);

        //WRITE_REGISTER_NOFENCE_ULONG(context->iobase + 0x02, 0b00010000000000000000000000000000);
        //WRITE_REGISTER_NOFENCE_ULONG(context->iobase + 0x02, 0b00000100000000000000000000000000);
        //WRITE_REGISTER_NOFENCE_ULONG(context->iobase + 0x07, 0b00000000000000000000010000000000);
        /*
        debug("[INFO]: BEGIN_READ");
        for (ULONG i = 0; i < HDMI_SIZE / 4; ++i)
        {
            PHYSICAL_ADDRESS address = MmGetPhysicalAddress(context->iobase + i);
            debug("[PORT]: Offset: 0x%02llX | Address: 0x%016llX | Raw Read: 0x%016llX | NoFence Read: 0x%016llX",
                i * 4, address.QuadPart, *(context->iobase + i), READ_REGISTER_NOFENCE_ULONG(context->iobase + i));
        }
        debug("[INFO]: END_READ");
        */
    }
    else { debug("[WARN]: %s: 0x%016llX", "KMDFHDMI.iobase", STATUS_NONE_MAPPED); }
    
    return STATUS_SUCCESS;
}

NTSTATUS KMDFHDMIPowerDown(WDFDEVICE Device, WDF_POWER_DEVICE_STATE TargetState)
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(TargetState);
    debug("[CALL]: %s", "KMDFHDMIPowerDown");
    return STATUS_SUCCESS;
}