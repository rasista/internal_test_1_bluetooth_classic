#!/usr/bin/env python3
"""
RTT Event Decoder for Bluetooth Classic HSS Events

This script decodes the binary RTT data dumped by log_lpw_hss_event and log_lpw_hss_packet functions.
The binary format varies by event type.

For BTC_DATA_PKT_RECEIVED events (0x07), the new format is:
[event_type][event_status][br_edr_mode][event_size(4 bytes)][entire_hss_event][timestamp]

Usage: python rtt_event_decoder.py host_central_logs.txt
"""

import sys
import struct
from typing import Dict, List, Tuple, Optional

# HSS Event Type definitions (based on btc_internal_event_type_t in host_to_lpw_interface.h)
HSS_EVENT_TYPES = {
    0x00: "inquiry_complete",
    0x01: "inquiry_scan_complete", 
    0x02: "page_complete",
    0x03: "page_scan_complete",
    0x04: "acl_complete",
    0x05: "role_switch_complete",
    0x06: "tx_pkt_ack_received",
    0x07: "data_packet_received"  # BTC_DATA_PKT_RECEIVED (unified format for all data packets)
}

# Packet Type definitions (mapped from btc_ut_hss_event_callbacks.h)
PACKET_TYPES = {
    0x02: "FHS",      # BT_FHS_PKT_TYPE
    0x03: "DM1",      # BT_DM1_PKT_TYPE or EIR 
    0x04: "DH1",      # BT_DH1_PKT_TYPE / BT_2DH1_PKT_TYPE
    0x0B: "DH3",      # BT_DH3_PKT_TYPE / BT_3DH3_PKT_TYPE
    0x0F: "DH5",       # BT_DH5_PKT_TYPE / BT_3DH5_PKT_TYPE
    0x0A: "DM3",      # BT_DM3_PKT_TYPE / BT_2DH3_PKT_TYPE
    0x0E: "DM5",      # BT_DM5_PKT_TYPE / BT_2DH5_PKT_TYPE
}

# HSS Event Status definitions (based on btc_internal_event_status_t in host_to_lpw_interface.h)
HSS_EVENT_STATUS = {
    0x00: "ACTIVITY_DURATION_COMPLETED",
    0x01: "INQUIRY_EIR_TIMEOUT", 
    0x02: "CONNECTION_CREATED",
    0x03: "CONNECTION_RESP_TOUT",
    0x04: "NEW_CONNECTION_TOUT",
    0x05: "CONNECTION_COMPLETED",
    0x06: "DATA_PKT_RECEIVED_SUCCESS",  # BTC_DATA_PKT_RECEIVED_SUCCESS
    0xFF: "UNKNOWN"
}

# BR/EDR Mode definitions
BR_EDR_MODE = {
    0x00: "BASIC_RATE",
    0x01: "ENHANCED_RATE"
}

# Packet header extraction macros (from btc_ut_hss_event_callbacks.h)
PKT_HEADER_PKT_TYPE_MASK = 0xF   # 4-bit TYPE
PKT_HEADER_PKT_TYPE_POS = 3      # bits 3-6

def GET_RX_PKT_HEADER_TYPE(buf):
    """Extract packet type from buffer using the same macro as C code"""
    return ((buf >> PKT_HEADER_PKT_TYPE_POS) & PKT_HEADER_PKT_TYPE_MASK)

class RTTPacketDecoder:
    """Decodes RTT packets containing HSS event data"""
    
    def __init__(self):
        self.event_count = 0
        self.decoding_errors = 0
        
    def get_event_size(self, event_type: int, event_status: int = None) -> int:
        """Get the expected size for a given event type based on C code log_lpw_hss_event calls"""
        base_size = 6  # event_type + event_status + timestamp(4 bytes)
        
        if event_type == 0x00:  # inquiry_complete
            return base_size  # event_type + event_status + timestamp
        elif event_type == 0x01:  # inquiry_scan_complete
            return base_size  # event_type + event_status + timestamp
        elif event_type == 0x02:  # page_complete
            return base_size + 1  # event_type + event_status + connection_info_idx + timestamp
        elif event_type == 0x03:  # page_scan_complete
            return base_size + 7  # event_type + event_status + connection_info_idx + remote_bd_address[6] + timestamp
        elif event_type == 0x04:  # acl_complete
            return base_size  # event_type + event_status + timestamp
        elif event_type == 0x05:  # role_switch_complete
            return base_size  # event_type + event_status + timestamp
        elif event_type == 0x06:  # tx_pkt_ack_received
            return base_size  # event_type + event_status + timestamp
        elif event_type == 0x07:  # BTC_DATA_PKT_RECEIVED
            # New format: [event_type][event_status][br_edr_mode][event_size(4 bytes)][entire_hss_event][timestamp]
            # For data packets, we need to read the event_size (4 bytes) to determine the actual size
            return 256  # Maximum size for data packets - will be adjusted based on event_size (4 bytes)
        else:
            return base_size  # Default: event_type + event_status + timestamp (for unknown event types)
        
    def decode_hss_event(self, event_data: bytes) -> Dict:
        """Decode HSS event data based on event type"""
        if len(event_data) < 6:  # Minimum: event_type + event_status + timestamp(4 bytes)
            return {"error": "Insufficient data for HSS event"}
            
        event_type = event_data[0]
        event_status = event_data[1]
        
        # Extract timestamp from last 4 bytes (little-endian)
        timestamp = (event_data[-4] | 
                    (event_data[-3] << 8) | 
                    (event_data[-2] << 16) | 
                    (event_data[-1] << 24))
        
        # Determine event type name
        event_type_name = HSS_EVENT_TYPES.get(event_type, f"UNKNOWN_{event_type:02X}")
            
        decoded = {
            "event_type": event_type,
            "event_type_name": event_type_name,
            "event_status": event_status,
            "event_status_name": HSS_EVENT_STATUS.get(event_status, f"UNKNOWN_{event_status:02X}"),
            "timestamp": timestamp,
            "raw_data": event_data.hex(),
            "data_length": len(event_data)
        }
        
        # Handle BTC_DATA_PKT_RECEIVED events (new generic format)
        if event_type == 0x07:  # BTC_DATA_PKT_RECEIVED
            # New format: [event_type][event_status][br_edr_mode][event_size(4 bytes)][entire_hss_event][timestamp]
            if len(event_data) >= 11:  # event_type + event_status + br_edr_mode + event_size(4 bytes) + at least 1 byte hss_event + timestamp
                # Extract br_edr_mode (2nd byte after event_type and event_status)
                br_edr_mode = event_data[2]
                decoded["br_edr_mode"] = br_edr_mode
                decoded["br_edr_mode_name"] = BR_EDR_MODE.get(br_edr_mode, f"UNKNOWN_{br_edr_mode:02X}")
                
                # Extract event_size (4 bytes, little-endian) starting from 3rd byte after event_type and event_status
                event_size = (event_data[3] | 
                             (event_data[4] << 8) | 
                             (event_data[5] << 16) | 
                             (event_data[6] << 24))
                decoded["event_size"] = event_size
                
                # Extract hss_event data (everything between event_size and timestamp)
                hss_event_start = 7  # After event_type, event_status, br_edr_mode, event_size(4 bytes)
                hss_event_end = len(event_data) - 4  # Exclude timestamp
                
                if hss_event_end >= hss_event_start:
                    hss_event_data = event_data[hss_event_start:hss_event_end]
                    decoded["hss_event_data"] = hss_event_data.hex()
                    decoded["hss_event_length"] = len(hss_event_data)
                    
                    # Extract packet_type from first 2 bytes of hss_event_data using GET_RX_PKT_HEADER_TYPE macro
                    if len(hss_event_data) >= 2:
                        # First two bytes contain the packet header (16-bit)
                        packet_header = (hss_event_data[1] << 8) | hss_event_data[0]  # Little-endian
                        
                        # Extract packet type using the same macro as C code
                        packet_type = GET_RX_PKT_HEADER_TYPE(packet_header)
                        decoded["packet_type"] = packet_type
                        decoded["packet_type_name"] = PACKET_TYPES.get(packet_type, f"UNKNOWN_{packet_type:02X}")
                        
                        # Extract HEC (3rd byte of hss_event_data)
                        if len(hss_event_data) >= 3:
                            hec = hss_event_data[2]
                            decoded["hec"] = hec
                        
                        # Extract payload (everything after HEC in hss_event_data)
                        if len(hss_event_data) >= 4:
                            payload_start = 3  # After packet_type(2) + HEC in hss_event_data
                            payload_end = len(hss_event_data)
                            if payload_end >= payload_start:
                                payload = hss_event_data[payload_start:payload_end]
                                decoded["payload"] = payload.hex()
                                decoded["payload_length"] = len(payload)
                
                # For FHS packets, extract additional information from payload
                if decoded.get('packet_type') == 0x02:  # BT_FHS_PKT_TYPE
                    if 'payload' in decoded and decoded['payload_length'] >= 18:
                        # Convert hex string back to bytes for FHS parsing
                        fhs_data = bytes.fromhex(decoded['payload'])
                        
                        # Extract LAP (Lower Address Part) from bytes 4, 5, 6 of FHS data
                        # Using EXTRACT_LAP_ADDR_BYTE logic: ((value) >> 2) & 0xFF
                        lap_bytes = [(fhs_data[4] >> 2) & 0xFF, (fhs_data[5] >> 2) & 0xFF, (fhs_data[6] >> 2) & 0xFF]
                        decoded["lap_address"] = ':'.join(f'{b:02X}' for b in lap_bytes)
                        
                        # Extract LT address from byte 14 (last 3 bits)
                        decoded["lt_address"] = fhs_data[14] & 0x07
                        
                        # Extract BD address (LAP + UAP + NAP) - same as C code
                        # LAP: bytes 4-6 (using EXTRACT_LAP_ADDR_BYTE logic)
                        # UAP: bytes 8-10
                        bd_addr = [
                            (fhs_data[4] >> 2) & 0xFF,  # LAP[0] - EXTRACT_LAP_ADDR_BYTE
                            (fhs_data[5] >> 2) & 0xFF,  # LAP[1] - EXTRACT_LAP_ADDR_BYTE
                            (fhs_data[6] >> 2) & 0xFF,  # LAP[2] - EXTRACT_LAP_ADDR_BYTE
                            fhs_data[8],                # UAP[0]
                            fhs_data[9],                # UAP[1]
                            fhs_data[10]                # UAP[2]
                        ]
                        decoded["bd_address"] = ':'.join(f'{b:02X}' for b in bd_addr)
                
                # For DH1, DH3, DH5 packets, determine packet type name based on BR/EDR mode
                elif decoded.get('packet_type') in [0x04, 0x0B, 0x0F , 0x0A , 0X0E , 0x03]:  # BT_DH1_PKT_TYPE, BT_DH3_PKT_TYPE, BT_DH5_PKT_TYPE
                    if decoded.get('packet_type') == 0x04:  # DH1
                        if decoded.get('br_edr_mode') == 0x00:  # BASIC_RATE
                            decoded["packet_type_name"] = "DH1"
                        else:  # ENHANCED_RATE
                            decoded["packet_type_name"] = "2DH1"
                    elif decoded.get('packet_type') == 0x0B:  # DH3
                        if decoded.get('br_edr_mode') == 0x00:  # BASIC_RATE
                            decoded["packet_type_name"] = "DH3"
                        else:  # ENHANCED_RATE
                            decoded["packet_type_name"] = "3DH3"
                    elif decoded.get('packet_type') == 0x0F:  # DH5
                        if decoded.get('br_edr_mode') == 0x00:  # BASIC_RATE
                            decoded["packet_type_name"] = "DH5"
                        else:  # ENHANCED_RATE
                            decoded["packet_type_name"] = "3DH5"
                    elif decoded.get('packet_type') == 0x0A:  # DM3
                        if decoded.get('br_edr_mode') == 0x00:  # BASIC_RATE
                            decoded["packet_type_name"] = "DM3"
                        else:  # ENHANCED_RATE
                            decoded["packet_type_name"] = "2DH3"
                    elif decoded.get('packet_type') == 0X0E:  # DM5
                        if decoded.get('br_edr_mode') == 0x00:  # BASIC_RATE
                            decoded["packet_type_name"] = "DM5"
                        else:  # ENHANCED_RATE
                            decoded["packet_type_name"] = "2DH5"
                    elif decoded.get('packet_type') == 0x03:  # DM1
                        decoded["packet_type_name"] = "DM1"
        
        # Decode specific event types (payload is between event_status and timestamp)
        payload_start = 2
        payload_end = len(event_data) - 4  # Exclude timestamp
        
        if event_type == 0x02 and payload_end >= payload_start + 1:  # page_complete
            decoded["connection_info_idx"] = event_data[payload_start]
            
        elif event_type == 0x03 and payload_end >= payload_start + 7:  # page_scan_complete
            decoded["connection_info_idx"] = event_data[payload_start]
            # Format BD address as XX:XX:XX:XX:XX:XX
            bd_addr = event_data[payload_start + 1:payload_start + 7]
            decoded["remote_bd_address"] = ':'.join(f'{b:02X}' for b in bd_addr)
            
        return decoded
        
    def decode_rtt_data(self, rtt_data: bytes) -> List[Dict]:
        """Decode RTT data containing multiple HSS events"""
        events = []
        offset = 0
        
        while offset < len(rtt_data):
            # Need at least 2 bytes for event_type and event_status
            if offset + 2 > len(rtt_data):
                break
                
            event_type = rtt_data[offset]
            event_status = rtt_data[offset + 1] if offset + 1 < len(rtt_data) else None
            

            
            # For data packets (event_type 0x07), use the new format with event_size (4 bytes)
            if event_type == 0x07:  # BTC_DATA_PKT_RECEIVED
                # New format: [event_type][event_status][br_edr_mode][event_size(4 bytes)][entire_hss_event][timestamp]
                if offset + 7 > len(rtt_data):  # Need at least 7 bytes to read event_size (4 bytes)
                    print(f"Warning: Incomplete data packet data at offset {offset}")
                    break
                
                # Read event_size (4 bytes, little-endian) starting from byte 3
                event_size = (rtt_data[offset + 3] | 
                             (rtt_data[offset + 4] << 8) | 
                             (rtt_data[offset + 5] << 16) | 
                             (rtt_data[offset + 6] << 24))
                
                # Calculate total event size: event_type + event_status + br_edr_mode + event_size(4 bytes) + entire_hss_event + timestamp(4)
                total_event_size = 7 + event_size + 4  # 7 metadata bytes + entire_hss_event + 4 timestamp bytes
                
                # Check if we have enough data for the complete event
                if offset + total_event_size > len(rtt_data):
                    print(f"Warning: Incomplete data packet event at offset {offset}, expected {total_event_size} bytes")
                    break
                    
                # Extract event data
                event_data = rtt_data[offset:offset + total_event_size]
                
                # Decode the event
                try:
                    decoded_event = self.decode_hss_event(event_data)
                    if "error" not in decoded_event:
                        events.append(decoded_event)
                        self.event_count += 1
                    else:
                        print(f"Error decoding data packet: {decoded_event['error']}")
                        self.decoding_errors += 1
                except Exception as e:
                    print(f"Error decoding data packet at offset {offset}: {e}")
                    self.decoding_errors += 1
                    
                offset += total_event_size
                
            else:
                # For non-data packets, use the old method
                event_size = self.get_event_size(event_type, event_status)
                
                # Check if we have enough data for the complete event
                if offset + event_size > len(rtt_data):
                    print(f"Warning: Incomplete event data for event type {event_type:02X}")
                    break
                    
                # Extract event data
                event_data = rtt_data[offset:offset + event_size]
                
                # Decode the event
                try:
                    decoded_event = self.decode_hss_event(event_data)
                    if "error" not in decoded_event:
                        events.append(decoded_event)
                        self.event_count += 1
                    else:
                        print(f"Error decoding event: {decoded_event['error']}")
                        self.decoding_errors += 1
                except Exception as e:
                    print(f"Error decoding event type {event_type:02X}: {e}")
                    self.decoding_errors += 1
                    
                offset += event_size
                
        return events
        
    def print_event_summary(self, event: Dict):
        """Print a formatted summary of a decoded event"""
        print(f"\n{'='*60}")
        print(f"Event #{self.event_count}: {event['event_type_name']}")
        print(f"{'='*60}")
        print(f"Event Type: {event['event_type']:02X} ({event['event_type_name']})")
        print(f"Event Status: {event['event_status']:02X} ({event['event_status_name']})")
        print(f"Data Length: {event['data_length']} bytes")
        print(f"Raw Data: {event['raw_data']}")
        
        # Print specific fields
        if 'connection_info_idx' in event:
            print(f"Connection Index: {event['connection_info_idx']}")
            
        if 'remote_bd_address' in event:
            print(f"Remote BD Address: {event['remote_bd_address']}")
            
        # Print data packet information for BTC_DATA_PKT_RECEIVED events
        if event['event_type'] == 0x07 and 'packet_type_name' in event:  # BTC_DATA_PKT_RECEIVED
            print(f"Packet Type: {event['packet_type_name']}")
            if 'br_edr_mode' in event:
                print(f"BR/EDR Mode: {event['br_edr_mode']} ({event['br_edr_mode_name']})")

            if 'event_size' in event:
                print(f"Event Size: {event['event_size']}")

            if 'packet_type' in event:
                print(f"Packet Type: {event['packet_type']:02X}")

            if 'payload' in event:
                print(f"Payload: {event['payload']}")
            
            # For FHS packets, show additional information
            if event.get('packet_type_name') == 'FHS':
                if 'bd_address' in event:
                    print(f"BD Address: {event['bd_address']}")
                if 'lap_address' in event:
                    print(f"LAP Address: {event['lap_address']}")
                if 'lt_address' in event:
                    print(f"LT Address: {event['lt_address']}")

    def write_event_summary(self, event: Dict, file_handle, event_number: int):
        """Write a formatted summary of a decoded event to file"""
        file_handle.write(f"\n{'='*60}\n")
        file_handle.write(f"Event #{event_number}: {event['event_type_name']}\n")
        file_handle.write(f"{'='*60}\n")
        
        # Always write the raw data for validation with proper byte spacing
        raw_data = event['raw_data']
        # Format raw data with spaces after every byte for readability
        formatted_raw_data = ' '.join([raw_data[i:i+2] for i in range(0, len(raw_data), 2)])
        file_handle.write(f"Raw Data: {formatted_raw_data}\n")
        file_handle.write(f"Data Length: {event['data_length']} bytes\n")
        
        # For data packets, show only essential info
        if event['event_type'] == 0x07:  # BTC_DATA_PKT_RECEIVED
            file_handle.write(f"Event Type: {event['event_type']:02X} ({event['event_type_name']})\n")
            file_handle.write(f"Event Status: {event['event_status']:02X} ({event['event_status_name']})\n")
            file_handle.write(f"Timestamp: {event['timestamp']}\n")
            
            # Only write packet_type_name once
            if 'packet_type_name' in event:
                file_handle.write(f"packet_type: {event['packet_type_name']}\n")

            # Only write br_edr_mode once
            if 'br_edr_mode' in event:
                file_handle.write(f"br_edr_mode: {event['br_edr_mode']} ({event['br_edr_mode_name']})\n")

            # Write event_size if present
            if 'event_size' in event:
                file_handle.write(f"event_size: {event['event_size']}\n")

            # For FHS packets, show additional information
            if event.get('packet_type_name') == 'FHS':
                if 'bd_address' in event:
                    file_handle.write(f"bd_address: {event['bd_address']}\n")
                if 'lap_address' in event:
                    file_handle.write(f"lap_address: {event['lap_address']}\n")
                if 'lt_address' in event:
                    file_handle.write(f"lt_address: {event['lt_address']}\n")
        else:
            # For other events, show full details
            file_handle.write(f"Event Type: {event['event_type']:02X} ({event['event_type_name']})\n")
            file_handle.write(f"Event Status: {event['event_status']:02X} ({event['event_status_name']})\n")
            file_handle.write(f"Timestamp: {event['timestamp']}\n")
        
        # Write payload fields if present (only for non-data-packet events)
        if event['event_type'] != 0x07:  # Not BTC_DATA_PKT_RECEIVED
            if 'connection_info_idx' in event:
                file_handle.write(f"connection_info_idx: {event['connection_info_idx']}\n")
                
            if 'remote_bd_address' in event:
                file_handle.write(f"remote_bd_address: {event['remote_bd_address']}\n")

def main():
    if len(sys.argv) != 2:
        print("Usage: python rtt_event_decoder.py <log_file>")
        print("Example: python rtt_event_decoder.py host_central_logs.txt")
        print("Output will be saved to: <log_file>_decoded.txt")
        sys.exit(1)
        
    log_file = sys.argv[1]
    # Handle both .txt and .bin files
    if log_file.endswith('.txt'):
        output_file = log_file.replace('.txt', '_decoded.txt')
    elif log_file.endswith('.bin'):
        output_file = log_file.replace('.bin', '_decoded.txt')
    else:
        output_file = log_file + '_decoded.txt'
    
    try:
        # Read the log file
        with open(log_file, 'rb') as f:
            rtt_data = f.read()
            
        # Create decoder and process data
        decoder = RTTPacketDecoder()
        events = decoder.decode_rtt_data(rtt_data)
        
        # Write output to file
        with open(output_file, 'w') as f:
            f.write(f"RTT Event Decoder Output\n")
            f.write(f"{'='*60}\n")
            f.write(f"Input file: {log_file}\n")
            f.write(f"Total bytes: {len(rtt_data)}\n")
            f.write(f"Decoded {len(events)} events:\n\n")
            
            # Write each event
            for i, event in enumerate(events, 1):
                decoder.write_event_summary(event, f, i)
                
            f.write(f"\n{'='*60}\n")
            f.write(f"DECODING SUMMARY\n")
            f.write(f"{'='*60}\n")
            f.write(f"Total Events: {decoder.event_count}\n")
            f.write(f"Decoding Errors: {decoder.decoding_errors}\n")
            if decoder.event_count + decoder.decoding_errors > 0:
                success_rate = decoder.event_count/(decoder.event_count + decoder.decoding_errors)*100
                f.write(f"Success Rate: {success_rate:.1f}%\n")
            else:
                f.write(f"Success Rate: 0.0%\n")
        
        print(f"Decoding completed successfully!")
        print(f"Input file: {log_file}")
        print(f"Output file: {output_file}")
        print(f"Total events decoded: {len(events)}")
        
    except FileNotFoundError:
        print(f"Error: File '{log_file}' not found")
        sys.exit(1)
    except Exception as e:
        print(f"Error processing file: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()