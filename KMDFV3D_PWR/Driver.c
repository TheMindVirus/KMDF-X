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
	if (ctx->iobase_pwr) { MmUnmapIoSpace(ctx->iobase_pwr, V3D_PWR_SIZE); ctx->iobase_pwr = NULL; }
	if (ctx->iobase_asb) { MmUnmapIoSpace(ctx->iobase_asb, V3D_PWR_ASB_SIZE); ctx->iobase_asb = NULL; }
	if (ctx->iobase_v3d) { MmUnmapIoSpace(ctx->iobase_v3d, V3D_PERI_SIZE); ctx->iobase_v3d = NULL; }
}

//DXGKDDI.h OVERRIDE
NTSTATUS WINAPI DxgkDdiAddDevice(IN_CONST_PDEVICE_OBJECT PhysicalDeviceObject, OUT_PPVOID MiniportDeviceContext)
{
	debug("[CALL]: DxgkDdiAddDevice");
	UNREFERENCED_PARAMETER(PhysicalDeviceObject);
	UNREFERENCED_PARAMETER(MiniportDeviceContext);
	PHYSICAL_ADDRESS pwr = { 0 };
	PHYSICAL_ADDRESS asb = { 0 };
	PHYSICAL_ADDRESS v3d = { 0 };

	pwr.QuadPart = V3D_PWR_REG;
	asb.QuadPart = V3D_PWR_ASB_REG;
	v3d.QuadPart = V3D_PERI_BASE;
 
	if (!ctx) { debug("[WARN]: Failed to Allocate Driver Extension while Loading"); return STATUS_ABANDONED; }
	ctx->iobase_pwr = (PULONG)MmMapIoSpace(pwr, V3D_PWR_SIZE, MmNonCached);
	ctx->iobase_asb = (PULONG)MmMapIoSpace(asb, V3D_PWR_ASB_SIZE, MmNonCached);
	ctx->iobase_v3d = (PULONG)MmMapIoSpace(v3d, V3D_PERI_SIZE, MmNonCached);
	if ((ctx->iobase_pwr) && (ctx->iobase_asb) && (ctx->iobase_v3d))
	{
		debug("[INFO]: INIT_HARDWARE");

		ULONG pwrreg = READ_REGISTER_ULONG(ctx->iobase_pwr);
		KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[V3D]: PWR_READ | Address: 0x%016llX | Value: 0x%08lX |\n",
			MmGetPhysicalAddress(ctx->iobase_pwr).QuadPart, pwrreg));
		pwrreg |= V3D_PWR_CMD;
		pwrreg |= V3D_PWR_ON;
		WRITE_REGISTER_ULONG(ctx->iobase_pwr, pwrreg);
		KeStallExecutionProcessor(2000000);
		pwrreg = READ_REGISTER_ULONG(ctx->iobase_pwr);
		KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[V3D]: PWR_WRITE | Address: 0x%016llX | Value: 0x%08lX |\n",
			MmGetPhysicalAddress(ctx->iobase_pwr).QuadPart, pwrreg));

		ULONG asbsreg = READ_REGISTER_ULONG(ctx->iobase_asb);
		KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[V3D]: ASB_S_READ | Address: 0x%016llX | Value: 0x%08lX |\n",
			MmGetPhysicalAddress(ctx->iobase_asb).QuadPart, asbsreg));
		asbsreg |= V3D_PWR_ASB_CMD;
		asbsreg &= V3D_PWR_ASB_MASK;
		WRITE_REGISTER_ULONG(ctx->iobase_asb, asbsreg);
		KeStallExecutionProcessor(2000000);
		asbsreg = READ_REGISTER_ULONG(ctx->iobase_asb);
		KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[V3D]: ASB_S_WRITE | Address: 0x%016llX | Value: 0x%08lX |\n",
			MmGetPhysicalAddress(ctx->iobase_asb).QuadPart, asbsreg));

		ULONG asbmreg = READ_REGISTER_ULONG(ctx->iobase_asb + 1);
		KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[V3D]: ASB_M_READ | Address: 0x%016llX | Value: 0x%08lX |\n",
			MmGetPhysicalAddress(ctx->iobase_asb + 1).QuadPart, asbmreg));
		asbmreg |= V3D_PWR_ASB_CMD;
		asbmreg &= V3D_PWR_ASB_MASK;
		WRITE_REGISTER_ULONG(ctx->iobase_asb + 1, asbmreg);
		KeStallExecutionProcessor(2000000);
		asbmreg = READ_REGISTER_ULONG(ctx->iobase_asb + 1);
		KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[V3D]: ASB_M_WRITE | Address: 0x%016llX | Value: 0x%08lX |\n",
			MmGetPhysicalAddress(ctx->iobase_asb + 1).QuadPart, asbmreg));

		//WRITE_REGISTER_ULONG(ctx->iobase_v3d + V3D_VPMBASE, 0b00000000000000000000000000000000);
		KeStallExecutionProcessor(2000000);
		

		debug("[INFO]: BEGIN_READ");
		for (ULONG i = 0; i <= V3D_PERI_SIZE / 4; ++i)
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