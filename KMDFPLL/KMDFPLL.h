#ifndef KMDFPLL_H
#define KMDFPLL_H

#define DBG 1

#include <ntddk.h>
#include <wdf.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct
{
	PULONG a2w_base;
	PULONG cm_base;
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

//BCM2835 Clock Mux Registers (Scarcely Documented)
///https://elinux.org/BCM2835_registers#CM
///https://elinux.org/The_Undocumented_Pi - further registers on Pi4

#define PI4_PERI_BASE     0xFE000000

#define A2W_PLL_BASE      0x00102000 + PI4_PERI_BASE
#define A2W_PLL_SIZE      0x00000FFF
#define A2W_PLL_ID        0x00613277
#define A2W_PLL_MAGIC     0x5A000000

#define CLOCK_MUX_BASE    0x00101000 + PI4_PERI_BASE
#define CLOCK_MUX_SIZE    0x00000240
#define CLOCK_MUX_ID      0x0000636D
#define CLOCK_MUX_MAGIC   0x5A000000

#include "debug.h"

#endif//KMDFPLL_H