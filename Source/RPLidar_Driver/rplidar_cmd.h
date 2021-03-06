#ifndef __RPLIDAR_CMD_H
#define __RPLIDAR_CMD_H


#pragma once

#include "rplidar_protocol.h"

// Commands
//-----------------------------------------

// Commands without payload and response
#define RPLIDAR_CMD_STOP               0x25
#define RPLIDAR_CMD_SCAN               0x20
#define RPLIDAR_CMD_FORCE_SCAN         0x21
#define RPLIDAR_CMD_RESET              0x40


// Commands without payload but have response
#define RPLIDAR_CMD_GET_DEVICE_INFO      0x50
#define RPLIDAR_CMD_GET_DEVICE_HEALTH    0x52


// Response
// ------------------------------------------
#define RPLIDAR_ANS_TYPE_MEASUREMENT      0x81

#define RPLIDAR_ANS_TYPE_DEVINFO          0x4
#define RPLIDAR_ANS_TYPE_DEVHEALTH        0x6


#define RPLIDAR_STATUS_OK                 0x0
#define RPLIDAR_STATUS_WARNING            0x1
#define RPLIDAR_STATUS_ERROR              0x2

#define RPLIDAR_RESP_MEASUREMENT_SYNCBIT        (0x1<<0)
#define RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT  2
#define RPLIDAR_RESP_MEASUREMENT_CHECKBIT       (0x1<<0)
#define RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT    1



typedef struct _rplidar_response_measurement_node_t {
		uint8_t    sync_quality;      // syncbit:1;syncbit_inverse:1;quality:6;
		uint16_t   angle_q6_checkbit; // check_bit:1;angle_q6:15;
		uint16_t   distance_q2;
} __attribute__((packed)) rplidar_response_measurement_node_t;

typedef struct _rplidar_response_device_info_t {
    uint8_t   model;
    uint16_t  firmware_version;
    uint8_t   hardware_version;
    uint8_t   serialnum[16];
} __attribute__((packed)) rplidar_response_device_info_t;

typedef struct _rplidar_response_device_health_t {
    uint8_t   status;
    uint16_t  error_code;
} __attribute__((packed)) rplidar_response_device_health_t;

#if defined(_WIN32)
#pragma pack(1)
#endif



#endif
