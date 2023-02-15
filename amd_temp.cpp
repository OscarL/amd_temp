/*
 * Copyright 2023, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		 Oscar Lesta
 */

#include <Drivers.h>
#include <KernelExport.h>
#include <PCI.h>

#include <stdio.h>
#include <string.h>

#include "hw_sensor.h"

//-----------------------------------------------------------------------------

#define TRACE_AMD_TEMP
#ifdef TRACE_AMD_TEMP
#	define TRACE(x...) dprintf("amd_temp: " x)
#else
#	define TRACE(x...)
#endif
#define ERROR(x...)	dprintf("amd_temp: " x)

#define AMD_TEMP_DEVICE_NAME	"amd_temp"

//-----------------------------------------------------------------------------

int32 api_version = B_CUR_DRIVER_API_VERSION;

static pci_module_info* gPCI;

//-----------------------------------------------------------------------------
// #pragma mark - device hooks

status_t
device_open(const char name[], uint32 flags, void** cookie)
{
	*cookie = NULL;
	return B_OK;
}


status_t
device_close(void* cookie)
{
	return B_OK;
}


status_t
device_free(void* cookie)
{
	return B_OK;
}


status_t
device_control(void* cookie, uint32 op, void* arg, size_t length)
{
	switch (op) {
		case SENSOR_GET_SENSORS_COUNT:
		{
			int count = 1;
			if (user_memcpy(arg, &count, sizeof(int)) != B_OK)
				return B_BAD_ADDRESS;

			return B_OK;
		};
		case SENSOR_GET_SENSORS:
		{
			sensor_type_t sensors[] = {
				 { 
					 .kind = SENSOR_KIND_TEMPERATURE,
					 .unit = SENSOR_UNIT_CELSIUS, // Not really.
					 .name = {'A', 'M', 'D', 't', 0}, // GCC bug doesn't allows = "AMDt".
					 .index = 0,
				 }
			};

			if (user_memcpy(arg, &sensors, sizeof(sensors)) != B_OK)
				return B_BAD_ADDRESS;

			return B_OK;
		};
/*
		case SENSOR_GET_KIND:
		{
			sensor_kind_t sensor;

			if (user_memcpy(&sensor, arg, sizeof(sensor_kind_t)) != B_OK)
				return B_BAD_ADDRESS;

			sensor = SENSOR_KIND_TEMPERATURE;

			if (user_memcpy(arg, &sensor, sizeof(sensor_kind_t)) != B_OK)
				return B_BAD_ADDRESS;

			return B_OK;
		};
*/
		case SENSOR_READ_SCALAR:
		{
			sensor_ioctl_scalar_t temp;
			if (user_memcpy(&temp, arg, sizeof(sensor_ioctl_scalar_t)) != B_OK)
				return B_BAD_ADDRESS;

			temp.unit = SENSOR_UNIT_CELSIUS;
			temp.resolution = 8;

			temp.value = gPCI->read_pci_config(0x0, 0x18, 0x3, 0xA4, 4);
			if (temp.value < 0) {
				ERROR(AMD_TEMP_DEVICE_NAME
					": read_pci_config(): wrong value = %" B_PRId64 ".\n", temp.value);
				return B_BAD_ADDRESS;
			}

			temp.value = (temp.value >> 21);

			if (user_memcpy(arg, &temp, sizeof(sensor_ioctl_scalar_t)) != B_OK)
				return B_BAD_ADDRESS;

			return B_OK;
		}
	}

	return B_BAD_VALUE;
}


status_t
device_read(void* cookie, off_t position, void* buffer, size_t* numBytes)
{
	if (*numBytes < 1)
		return B_IO_ERROR;

	if (position) {
		*numBytes = 0;
		return B_OK;
	}

	size_t max_len = *numBytes;
	char* str = (char*) buffer;

	sensor_ioctl_scalar_t temp;
	status_t status = device_control(cookie, SENSOR_READ_SCALAR, &temp, 0);
	if (status == B_OK) {
		snprintf(str, max_len, "AMD Thermal Reading: %" B_PRIu32 ".%" B_PRIu32 " C\n",
			(uint32) (temp.value / temp.resolution),
			(uint32) (temp.value % temp.resolution));
		*numBytes = strlen((char*) buffer);
	} else {
		*numBytes = 0;
	}

	return B_OK;
}


status_t
device_write(void* cookie, off_t position, const void* buffer, size_t* numBytes)
{
	*numBytes = 0;
	return B_NOT_ALLOWED;
}


//-----------------------------------------------------------------------------
// #pragma mark - driver hooks


#define VENDOR_ID_AMD	0x1022

static uint16 amd_device_list[] = {

	0x1103,	// 0x0F NB/HOST MISC CTRL
	0x1203,	// 0x10 NB/HOST MISC CTRL
	0x1303,	// 0x11 NB/HOST MISC CTRL
	0x1703,	// 0x14 Func.3
	0x1603,	// 0x15 Func.3
	0x1403,	// 0x15 M10H
	0x141d,	// 0x15 M30H
//	0x1576,	// 0x15 M60H ROOT
	0x1533,	// 0x16
	0x1583,	// 0x16 M30H
/*
	0x1450,	// 17H
	0x15d0,	// 17H M10H
	0x1480,	// 17H M30H
	0x1630,	// 17H M60H
*/
	0
};


static struct {
	uint16	vendor;
	uint16*	devices;
} SupportedDevices[] = {
	{ VENDOR_ID_AMD, amd_device_list },
	{ 0x0000, NULL}
};


status_t
init_hardware(void)
{
	if (get_module(B_PCI_MODULE_NAME, (module_info**) &gPCI) != B_OK)
		return B_ERROR;

	bool found_one = false;
	pci_info pciInfo;
	int pci_index = 0;

	while ((*gPCI->get_nth_pci_info)(pci_index, &pciInfo) == B_NO_ERROR) {
		int vendor = 0;
		while (SupportedDevices[vendor].vendor) {
			if (SupportedDevices[vendor].vendor == pciInfo.vendor_id) {
				uint16* devices = SupportedDevices[vendor].devices;
				while (*devices) {
					if (*devices == pciInfo.device_id ) {
						found_one = true;
						goto done;
					}
					devices++;
				}
			}
			vendor++;
		}
		pci_index++;
	}

done:
	put_module(B_PCI_MODULE_NAME);
	TRACE("sensor %sfound.\n", found_one ? "" : "not ");
	return (found_one ? B_OK : B_DEVICE_NOT_FOUND);
}


status_t
init_driver(void)
{
	if (get_module(B_PCI_MODULE_NAME, (module_info**) &gPCI) < B_OK)
		return ENOSYS;

	return B_OK;
}


void
uninit_driver(void)
{
	put_module(B_PCI_MODULE_NAME);
}


const char**
publish_devices(void)
{
	static const char* names[] = {
	    "sensor/" AMD_TEMP_DEVICE_NAME,
	    NULL
	};

	return names;
}


device_hooks*
find_device(const char name[])
{
	static device_hooks hooks = {
		device_open,
		device_close,
		device_free,
		device_control,
		device_read,
		device_write,
	};

	return &hooks;
}
