/******************************************************************************
*
* @file   sl_btctrl_hci_packet.h
* @brief  Data structures and constants for HCI data packets
*
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* SPDX-License-Identifier: Zlib
*
* The licensor of this software is Silicon Laboratories Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
******************************************************************************/

#ifndef SL_BTCTRL_HCI_PACKET_H
#define SL_BTCTRL_HCI_PACKET_H
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define HCI_COMMAND_HEADER_SIZE     3   // opcode (2 bytes), length (1 byte)
#define HCI_ACL_DATA_HEADER_SIZE    4   // handle (2 bytes), length (2 bytes)

void * hci_read(void *arg);
void *hci_write(void *arg);

void schedule_hci_events_to_host();
#endif // SL_BTCTRL_HCI_PACKET_H
