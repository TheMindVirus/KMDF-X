#ifndef KMDFGPIO_H
#define KMDFGPIO_H

#include <ntddk.h>
#include <wdf.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define DEBUG_FILENAME   L"KMDFDriver.log"
#include "debug.h"

#define HIGH   1
#define LOW    0

#define INPUT    0b000
#define OUTPUT   0b001
#define ALT0     0b100
#define ALT1     0b101
#define ALT2     0b110
#define ALT3     0b111
#define ALT4     0b011
#define ALT5     0b010

#define PULL_NONE   0b00
#define PULL_UP     0b01
#define PULL_DOWN   0b10
#define PULL_BOTH   0b11

#define GPIO_BASE   0xFE200000 //BCM2711 GPIO Base Address (previously 0x7E200000 and not 0x7E215000) //not 0x4EE215000, not 0x53E215000, not 0x17A215000, not 0x17DA15000, not 0x4FA215000, 
#define GPIO_SIZE   0xF4//0xFFF//0xF4 //bytes offset from base
#define GPIO_MAX    58 //pins

typedef struct
{
	PCM_RESOURCE_LIST resources;
	PULONG membase;
	PULONG iobase;
}   KMDFGPIO_CONTEXT, *PKMDFGPIO_CONTEXT;
#define KMDFGPIO_TAG_EXTENSION   (PVOID)1
#define KMDFGPIO_TAG_RESOURCES   (ULONG)2

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KMDFGPIODeviceAdd;
EVT_WDF_DRIVER_UNLOAD KMDFGPIODriverUnload;
EVT_WDF_DEVICE_PREPARE_HARDWARE KMDFGPIOPrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE KMDFGPIOReleaseHardware;
EVT_WDF_DEVICE_D0_ENTRY KMDFGPIOPowerUp;
EVT_WDF_DEVICE_D0_EXIT KMDFGPIOPowerDown;

#endif//KMDFGPIO_H