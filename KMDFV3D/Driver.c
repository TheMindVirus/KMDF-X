#include "KMDFV3D.h"

#ifdef ALLOC_PRAGMA
	#pragma alloc_text (INIT, DriverEntry)
#endif

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	debug("[CALL]: DriverEntry");
	NTSTATUS status = STATUS_SUCCESS;
	DRIVER_INITIALIZATION_DATA config = { 0 };

	status = IoAllocateDriverObjectExtension(DriverObject, KMDFV3D_TAG_EXTENSION, sizeof(KMDFV3D_CONTEXT), &ctx);
	if (!NT_SUCCESS(status)) { debug("[WARN]: IoAllocateDriverObjectExtension: 0x%016llX", status); return status; }
	DriverObject->DriverUnload = DriverUnload;

	if (!ARGUMENT_PRESENT(DriverObject)
	||  !ARGUMENT_PRESENT(RegistryPath))
	{
		debug("[WARN]: The Specified DriverObject or RegistryPath is Invalid");
		return STATUS_INVALID_PARAMETER;
	}

	DxgkDdiCallbackPatch(&config);
	status = DxgkInitialize(DriverObject, RegistryPath, &config);
	if (NT_ERROR(status)) { debug("[WARN]: DxgkInitialize() Failed"); return status; }
	return STATUS_SUCCESS;
}

void DriverUnload(PDRIVER_OBJECT DriverObject)
{
	debug("[CALL]: DriverUnload");
	DxgkUnInitialize(DriverObject);
	if (!ctx) { debug("[WARN]: Failed to Allocate Driver Extension while Unloading"); return; }
	if (ctx->iobase_v3d) { MmUnmapIoSpace(ctx->iobase_v3d, V3D_PERI_SIZE); ctx->iobase_v3d = NULL; }
}

//DXGKDDI.h OVERRIDE
NTSTATUS WINAPI DxgkDdiAddDevice(IN_CONST_PDEVICE_OBJECT PhysicalDeviceObject, OUT_PPVOID MiniportDeviceContext)
{
	debug("[CALL]: DxgkDdiAddDevice");
	UNREFERENCED_PARAMETER(PhysicalDeviceObject);
	UNREFERENCED_PARAMETER(MiniportDeviceContext);
	PHYSICAL_ADDRESS start = { 0 };
	PHYSICAL_ADDRESS end = { 0 };
	PHYSICAL_ADDRESS fence = { 0 };

	start.QuadPart = V3D_PERI_BASE;
	end.QuadPart = V3D_PERI_BASE + V3D_PERI_SIZE;
	fence.QuadPart = 0;
 
	if (!ctx) { debug("[WARN]: Failed to Allocate Driver Extension while Loading"); return STATUS_ABANDONED; }
	ctx->iobase_v3d = (PULONG)MmMapIoSpace(start, V3D_PERI_SIZE, MmNonCached);
	if (ctx->iobase_v3d)
	{
		debug("[INFO]: BEGIN_READ");
		for (ULONG i = 0; i < V3D_PERI_SIZE / 4; ++i)
		{
			PHYSICAL_ADDRESS address = MmGetPhysicalAddress(ctx->iobase_v3d + i);
			//WRITE_REGISTER_ULONG(ctx->iobase_v3d + i, 0b00000000000000000000000000000000);
			debug("[MMIO]: Offset: 0x%02llX | Address: 0x%016llX | Raw Read: 0x%016llX | Macro Read: 0x%016llX",
				i * 4, address.QuadPart, *(ctx->iobase_v3d + i), READ_REGISTER_ULONG(ctx->iobase_v3d + i));
		}
		debug("[INFO]: END_READ");
	}
	else { debug("[WARN]: Failed to Map I/O Space Memory"); return STATUS_NONE_MAPPED; }

	return STATUS_SUCCESS;
}

/*
void WINAPI DxgkDdiNotifyInterrupt(IN_CONST_HANDLE hAdapter, IN_CONST_PDXGKARGCB_NOTIFY_INTERRUPT_DATA Arg2)
{
	debug("[CALL]: DxgkDdiNotifyInterrupt");
	UNREFERENCED_PARAMETER(hAdapter);
	UNREFERENCED_PARAMETER(Arg2);
}

void WINAPI DxgkDdiNotifyDPC(IN_CONST_HANDLE hAdapter)
{
	debug("[CALL]: DxgkDdiNotifyDPC");
	UNREFERENCED_PARAMETER(hAdapter);
}
*/