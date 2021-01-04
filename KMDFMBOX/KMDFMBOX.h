#ifndef KMDFMBOX_H
#define KMDFMBOX_H

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
}   KMDFMBOX_CONTEXT, *PKMDFMBOX_CONTEXT;
#define KMDFMBOX_TAG_EXTENSION   (PVOID)1
#define KMDFMBOX_TAG_RESOURCES   (ULONG)2

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KMDFMBOXDeviceAdd;
EVT_WDF_DRIVER_UNLOAD KMDFMBOXDriverUnload;
EVT_WDF_DEVICE_PREPARE_HARDWARE KMDFMBOXPrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE KMDFMBOXReleaseHardware;
EVT_WDF_DEVICE_D0_ENTRY KMDFMBOXPowerUp;
EVT_WDF_DEVICE_D0_EXIT KMDFMBOXPowerDown;

#include "debug.h"

///https://datasheets.raspberrypi.org/bcm2711/bcm2711-peripherals.pdf
///https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface

//ARM<->ARM communication - ARM_LOCAL Mailboxes (Chapter 6.5.2 / 13)
//ARM<->VPU communication - ARMC Mailboxes (Chapter 6.5.3)

#define ARM_LOCAL_BASE   0xff800000
#define ARMC_BASE        0xfe00b000

#define MBOX_ARM_BASE   ARM_LOCAL_BASE
#define MBOX_ARM_SIZE   0xff

#define MBOX_VPU_BASE   ARMC_BASE
#define MBOX_VPU_SIZE   0x3ff

#endif//KMDFMBOX_H