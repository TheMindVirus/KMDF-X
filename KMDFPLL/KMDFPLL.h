#ifndef KMDFPLL_H
#define KMDFPLL_H

//#define DBG 1

#include <ntddk.h>
#include <wdf.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct
{
	PULONG iobase;
}   KMDFPLL_CONTEXT, *PKMDFPLL_CONTEXT;
#define KMDFPLL_TAG_EXTENSION   (PVOID)1

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KMDFPLLDeviceAdd;
EVT_WDF_DRIVER_UNLOAD KMDFPLLDriverUnload;
EVT_WDF_DEVICE_PREPARE_HARDWARE KMDFPLLPrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE KMDFPLLReleaseHardware;
EVT_WDF_DEVICE_D0_ENTRY KMDFPLLPowerUp;
EVT_WDF_DEVICE_D0_EXIT KMDFPLLPowerDown;

//BCM2835 PLL Registers (Scarcely Documented)
///https://elinux.org/BCM2835_registers#A2W

#define PI4_PERI_BASE   0xFE000000

#define A2W_PLL_BASE    0x00102000 + PI4_PERI_BASE
#define A2W_PLL_SIZE    0x00000FFF
#define A2W_PLL_ID      0x00613277
#define A2W_PLL_MAGIC   0x5A000000

#include "debug.h"

#endif//KMDFPLL_H