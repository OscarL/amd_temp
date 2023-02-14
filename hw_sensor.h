/*
 * Copyright 2023, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		 Oscar Lesta
 */
#ifndef _HW_SENSOR_H_
#define _HW_SENSOR_H_


#ifdef __cplusplus
extern "C" {
#endif

// Sensor's IOCTL
enum {
	SENSOR_OP_CODES = B_DEVICE_OP_CODES_END + 'sens',
	SENSOR_GET_KIND = SENSOR_OP_CODES + 1,
	SENSOR_GET_SENSORS_COUNT,
	SENSOR_GET_SENSORS,
	SENSOR_READ_SCALAR,
	SENSOR_READ_BOOLEAN,
};


typedef enum sensor_kind {
	SENSOR_KIND_UNKNOWN		= 0x00,
	SENSOR_KIND_TEMPERATURE	= 0x01,
	SENSOR_KIND_VOLTAGE		= 0x02,
	SENSOR_KIND_RPM			= 0x04,
	SENSOR_KIND_CURRENT		= 0x08,
	SENSOR_KIND_LIGHT		= 0x10,
	SENSOR_KIND_PROXIMITY	= 0x20,
} sensor_kind_t;


typedef enum sensor_unit {
	SENSOR_UNIT_UNKNOWN = 0x00,
	SENSOR_UNIT_CELSIUS,
	SENSOR_UNIT_FAHRENHEIT,
	SENSOR_UNIT_KELVIN,
	SENSOR_UNIT_VOLTS,
	SENSOR_UNIT_RPMS,
	SENSOR_UNIT_AMPS,
	SENSOR_UNIT_BOOLEAN,
	SENSOR_UNIT_CENTIMETERS,
} sensor_unit_t;


typedef struct sensor_ioctl_scalar {
	uint32	unit;
	int32	resolution;	// >0: divide by this, <0: multiply by this.
	uint32	precision;
	uint32	_paddding;
	int64	value;
} sensor_ioctl_scalar_t;


typedef struct sensor_type {
	sensor_kind_t kind;
	sensor_unit_t unit;
	const char name[NAME_MAX];
	uint index;
} sensor_type_t;

#ifdef __cplusplus
}
#endif

#endif	// _HW_SENSOR_H_
