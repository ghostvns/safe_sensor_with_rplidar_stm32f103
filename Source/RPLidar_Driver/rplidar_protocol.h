/*
 * Copyright (c) 2014, RoboPeak
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 *  RoboPeak LIDAR System
 *  Data Packet IO protocol definition for RP-LIDAR
 *
 *  Copyright 2009 - 2014 RoboPeak Team
 *  http://www.robopeak.com
 *  
 */
#ifndef __RPLIDAR_PROTOCOL_H
#define __RPLIDAR_PROTOCOL_H

#pragma once


#include "stm32f10x.h"

// RP-Lidar Input Packets

#define RPLIDAR_CMD_SYNC_BYTE        0xA5
#define RPLIDAR_CMDFLAG_HAS_PAYLOAD  0x80


#define RPLIDAR_ANS_SYNC_BYTE1       0xA5
#define RPLIDAR_ANS_SYNC_BYTE2       0x5A

#define RPLIDAR_ANS_PKTFLAG_LOOP     0x1


#if defined(_WIN32)
#pragma pack(1)
#endif

typedef struct _rplidar_cmd_packet_t {
    uint8_t syncByte; //must be RPLIDAR_CMD_SYNC_BYTE
    uint8_t cmd_flag; 
    uint8_t size;
    uint8_t data[1];
} __attribute__((packed)) rplidar_cmd_packet_t;


typedef struct _rplidar_ans_header_t {
    uint8_t  syncByte1; // must be RPLIDAR_ANS_SYNC_BYTE1
    uint8_t  syncByte2; // must be RPLIDAR_ANS_SYNC_BYTE2
    uint32_t size:30;
    uint32_t subType:2;
    uint8_t  type;
} __attribute__((packed)) rplidar_ans_header_t;

#endif
