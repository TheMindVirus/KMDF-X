#ifndef KMDFDMA_H
#define KMDFDMA_H

#include <ntddk.h>
#include <wdf.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct
{
	PCM_RESOURCE_LIST resources;
	PULONG membase;
	PULONG iobase;
	PULONG iobase_vpu;
}   KMDFDMA_CONTEXT, *PKMDFDMA_CONTEXT;
#define KMDFDMA_TAG_EXTENSION   (PVOID)1
#define KMDFDMA_TAG_RESOURCES   (ULONG)2

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KMDFDMADeviceAdd;
EVT_WDF_DRIVER_UNLOAD KMDFDMADriverUnload;
EVT_WDF_DEVICE_PREPARE_HARDWARE KMDFDMAPrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE KMDFDMAReleaseHardware;
EVT_WDF_DEVICE_D0_ENTRY KMDFDMAPowerUp;
EVT_WDF_DEVICE_D0_EXIT KMDFDMAPowerDown;

#include "debug.h"

///https://datasheets.raspberrypi.org/bcm2711/bcm2711-peripherals.pdf
///Page 60 - DMA Controller Enable Registers

#define DMA_BASE       0xfe007000
#define DMA_SIZE       0x1000

#define DMA_VPU_BASE   0xfee05000
#define DMA_VPU_SIZE   0x100

//Look for PAGE and PAGELITE bits on Pi4 AXI Enable Registers
//They default to 0x0 for power saving reasons. Set to 1 to globally re-enable DMA.

#endif//KMDFDMA_H