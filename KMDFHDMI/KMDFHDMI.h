#ifndef KMDFHDMI_H
#define KMDFHDMI_H

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
}   KMDFHDMI_CONTEXT, *PKMDFHDMI_CONTEXT;
#define KMDFHDMI_TAG_EXTENSION   (PVOID)1
#define KMDFHDMI_TAG_RESOURCES   (ULONG)2

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KMDFHDMIDeviceAdd;
EVT_WDF_DRIVER_UNLOAD KMDFHDMIDriverUnload;
EVT_WDF_DEVICE_PREPARE_HARDWARE KMDFHDMIPrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE KMDFHDMIReleaseHardware;
EVT_WDF_DEVICE_D0_ENTRY KMDFHDMIPowerUp;
EVT_WDF_DEVICE_D0_EXIT KMDFHDMIPowerDown;

#include "debug.h"

///https://github.com/librerpi/rpi-open-firmware/blob/master/docs/hdmi.md

#define HDMI_BASE     0xfef00000
#define HDMI_SIZE      0x20100

#define HDMI_INTR2_SIZE      0x30
#define HDMI_CSC_SIZE        0x80
#define HDMI_DVP_SIZE        0x200
#define HDMI_HDMI_SIZE       0x300
#define HDMI_PHY_SIZE        0x80
#define HDMI_RM_SIZE         0x80
#define HDMI_PACKET_SIZE     0x200
#define HDMI_METADATA_SIZE   0x400
#define HDMI_CEC_SIZE        0x100
#define HDMI_HD_SIZE         0x100

#define HDMI0_INTR2      HDMI_BASE + 0x100
#define HDMI0_CSC        HDMI_BASE + 0x200
#define HDMI0_DVP        HDMI_BASE + 0x300
#define HDMI0_HDMI       HDMI_BASE + 0x700
#define HDMI0_PHY        HDMI_BASE + 0xf00
#define HDMI0_RM         HDMI_BASE + 0xf80
#define HDMI0_PACKET     HDMI_BASE + 0x1b00
#define HDMI0_METADATA   HDMI_BASE + 0x1f00
#define HDMI0_CEC        HDMI_BASE + 0x4300
#define HDMI0_HD         HDMI_BASE + 0x20000

#define HDMI1_INTR2      HDMI_BASE + 0x100
#define HDMI1_CSC        HDMI_BASE + 0x280
#define HDMI1_DVP        HDMI_BASE + 0x5300
#define HDMI1_HDMI       HDMI_BASE + 0x5700
#define HDMI1_PHY        HDMI_BASE + 0x5f00
#define HDMI1_RM         HDMI_BASE + 0x5f80
#define HDMI1_PACKET     HDMI_BASE + 0x6b00
#define HDMI1_METADATA   HDMI_BASE + 0x6f00
#define HDMI1_CEC        HDMI_BASE + 0x9300
#define HDMI1_HD         HDMI_BASE + 0x20000

#endif//KMDFHDMI_H