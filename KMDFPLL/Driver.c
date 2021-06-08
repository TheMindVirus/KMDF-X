#include "KMDFPLL.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#endif

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    debug("[CALL]: %s START", "DriverEntry"); 
    NTSTATUS status = STATUS_SUCCESS;
    PKMDFPLL_CONTEXT context = NULL;
    WDF_DRIVER_CONFIG DriverConfig;

    status = IoAllocateDriverObjectExtension(DriverObject, KMDFPLL_TAG_EXTENSION, sizeof(KMDFPLL_CONTEXT), &context);
    if (!NT_SUCCESS(status)) { debug("[WARN]: IoAllocateDriverObjectExtension: 0x%016llX", status); return status; }

    WDF_DRIVER_CONFIG_INIT(&DriverConfig, KMDFPLLDeviceAdd);
    DriverConfig.EvtDriverUnload = KMDFPLLDriverUnload;
    status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &DriverConfig, WDF_NO_HANDLE);
    if (!NT_SUCCESS(status)) { debug("[WARN]: %s: 0x%016llX", "WdfDriverCreate", status); return status; }
    
    debug("[CALL]: %s END", "DriverEntry");
    return STATUS_SUCCESS;
}

NTSTATUS KMDFPLLDeviceAdd(WDFDRIVER Driver, PWDFDEVICE_INIT DeviceInit)
{
    UNREFERENCED_PARAMETER(Driver);
    debug("[CALL]: %s START", "KMDFPLLDeviceAdd");
    NTSTATUS status = STATUS_SUCCESS;
    WDF_PNPPOWER_EVENT_CALLBACKS PnPevents;
    WDFDEVICE Device;
    
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&PnPevents);
    PnPevents.EvtDevicePrepareHardware = KMDFPLLPrepareHardware;
    PnPevents.EvtDeviceReleaseHardware = KMDFPLLReleaseHardware;
    PnPevents.EvtDeviceD0Entry = KMDFPLLPowerUp;
    PnPevents.EvtDeviceD0Exit = KMDFPLLPowerDown;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &PnPevents);

    status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &Device);
    if (!NT_SUCCESS(status)) { debug("[WARN]: %s: 0x%016llX", "WdfDeviceCreate", status); return status; }

    debug("[CALL]: %s END", "KMDFPLLDeviceAdd");
    return STATUS_SUCCESS;
}

void KMDFPLLDriverUnload(WDFDRIVER Driver)
{
    UNREFERENCED_PARAMETER(Driver);
    debug("[CALL]: %s", "KMDFPLLDriverUnload");
}

NTSTATUS KMDFPLLPrepareHardware(WDFDEVICE Device, WDFCMRESLIST ResourcesRaw, WDFCMRESLIST ResourcesTranslated)
{
    UNREFERENCED_PARAMETER(ResourcesRaw);
    UNREFERENCED_PARAMETER(ResourcesTranslated);
    debug("[CALL]: %s START", "KMDFPLLPrepareHardware");
    NTSTATUS status = STATUS_SUCCESS;
    PKMDFPLL_CONTEXT context = NULL;
    PHYSICAL_ADDRESS base;

    context = IoGetDriverObjectExtension(WdfDriverWdmGetDriverObject(WdfDeviceGetDriver(Device)), KMDFPLL_TAG_EXTENSION);
    if (!context) { status = STATUS_WDF_INTERNAL_ERROR; debug("[WARN]: %s: 0x%016llX", "IoGetDriverObjectExtension", status); return status; }

    base.QuadPart = A2W_PLL_BASE;
    context->a2w_base = (PULONG)MmMapIoSpace(base, A2W_PLL_SIZE, MmNonCached);
    if (!(context->a2w_base)) { debug("[WARN]: %s: 0x%016llX", "MmMapIoSpace", STATUS_NONE_MAPPED); }

    base.QuadPart = CLOCK_MUX_BASE;
    context->cm_base = (PULONG)MmMapIoSpace(base, CLOCK_MUX_SIZE, MmNonCached);
    if (!(context->cm_base)) { debug("[WARN]: %s: 0x%016llX", "MmMapIoSpace", STATUS_NONE_MAPPED); }

    debug("[CALL]: %s END", "KMDFPLLPrepareHardware");
    return STATUS_SUCCESS;
}

NTSTATUS KMDFPLLReleaseHardware(WDFDEVICE Device, WDFCMRESLIST ResourcesTranslated)
{
    UNREFERENCED_PARAMETER(ResourcesTranslated);
    debug("[CALL]: %s START", "KMDFPLLReleaseHardware");
    NTSTATUS status = STATUS_SUCCESS;
    PKMDFPLL_CONTEXT context = NULL;

    context = IoGetDriverObjectExtension(WdfDriverWdmGetDriverObject(WdfDeviceGetDriver(Device)), KMDFPLL_TAG_EXTENSION);
    if (!context) { status = STATUS_WDF_INTERNAL_ERROR; debug("[WARN]: %s: 0x%016llX", "IoGetDriverObjectExtension", status); return status; }
    if (context->a2w_base) { MmUnmapIoSpace(context->a2w_base, A2W_PLL_SIZE); context->a2w_base = NULL; }
    if (context->cm_base) { MmUnmapIoSpace(context->cm_base, A2W_PLL_SIZE); context->cm_base = NULL; }
    
    debug("[CALL]: %s END", "KMDFPLLReleaseHardware");
    return STATUS_SUCCESS;
}

NTSTATUS KMDFPLLPowerUp(WDFDEVICE Device, WDF_POWER_DEVICE_STATE PreviousState)
{
    UNREFERENCED_PARAMETER(PreviousState);
    debug("[CALL]: %s", "KMDFPLLPowerUp");
    NTSTATUS status = STATUS_SUCCESS;
    PKMDFPLL_CONTEXT context = NULL;

    context = IoGetDriverObjectExtension(WdfDriverWdmGetDriverObject(WdfDeviceGetDriver(Device)), KMDFPLL_TAG_EXTENSION);
    if (!context) { status = STATUS_WDF_INTERNAL_ERROR; debug("[WARN]: %s: 0x%016llX", "IoGetDriverObjectExtension", status); return status; }

    if (context->a2w_base)
    {
        debug("[INFO]: BEGIN_READ");
        for (ULONG i = 0; i <= A2W_PLL_SIZE / 4; ++i)
        {
            PHYSICAL_ADDRESS address = MmGetPhysicalAddress(context->a2w_base + i); UNREFERENCED_PARAMETER(address);
            //WRITE_REGISTER_NOFENCE_ULONG(context->a2w_base + i, 0b00000000000000000000000000000000);
            debug("[A2W_]: Offset: 0x%02llX | Address: 0x%016llX | Raw Read: 0x%016llX | NoFence Read: 0x%016llX",
                i * 4, address.QuadPart, *(context->a2w_base + i), READ_REGISTER_NOFENCE_ULONG(context->a2w_base + i));
        }
        debug("[INFO]: END_READ");
    }
    else { debug("[WARN]: %s: 0x%016llX", "KMDFPLL.a2w_base", STATUS_NONE_MAPPED); }

    if (context->cm_base)
    {
        debug("[INFO]: BEGIN_READ");
        for (ULONG i = 0; i <= CLOCK_MUX_SIZE / 4; ++i)
        {
            PHYSICAL_ADDRESS address = MmGetPhysicalAddress(context->cm_base + i); UNREFERENCED_PARAMETER(address);
            //WRITE_REGISTER_NOFENCE_ULONG(context->cm_base + i, 0b00000000000000000000000000000000);
            debug("[CMUX]: Offset: 0x%02llX | Address: 0x%016llX | Raw Read: 0x%016llX | NoFence Read: 0x%016llX",
                i * 4, address.QuadPart, *(context->cm_base + i), READ_REGISTER_NOFENCE_ULONG(context->cm_base + i));
        }
        debug("[INFO]: END_READ");
    }
    else { debug("[WARN]: %s: 0x%016llX", "KMDFPLL.cm_base", STATUS_NONE_MAPPED); }

    return STATUS_SUCCESS;
}

NTSTATUS KMDFPLLPowerDown(WDFDEVICE Device, WDF_POWER_DEVICE_STATE TargetState)
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(TargetState);
    debug("[CALL]: %s", "KMDFPLLPowerDown");
    return STATUS_SUCCESS;
}