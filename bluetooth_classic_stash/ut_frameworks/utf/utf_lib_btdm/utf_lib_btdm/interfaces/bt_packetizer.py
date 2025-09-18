import threading
from utf.interfaces import PacketizerInterface


def create_interface(config):
    return BTPacketizer(config)


class BTPacketizer(PacketizerInterface):
    def __init__(self, config):
        super(BTPacketizer, self).__init__(config)
        self.opcode = 0
        self.ogf = 0
        self.ocf  = 0

    def extract_packets(self, byte_stream: bytes):
        current_index = 0
        packet_header_length = 3
        
        self.logger.info(f'**** Received - {byte_stream} in thread - {threading.get_ident()}')
        while current_index < len(byte_stream):
            has_incomplete_header = current_index + packet_header_length > len(byte_stream)
            if has_incomplete_header:
                self.logger.warning(f'incomplete header at current_index {current_index} in thread '
                                    f'- {threading.get_ident()}')
                return

            header = byte_stream[current_index:current_index + packet_header_length]
            data_length = header[packet_header_length-1]
            packet_end = current_index + packet_header_length + data_length

            has_incomplete_packet = packet_end > len(byte_stream)
            if has_incomplete_packet:
                self.logger.warning(f'incomplete packet at current_index {current_index} in thread '
                                    f'- {threading.get_ident()}')
                return

            packet = byte_stream[current_index:packet_end]
            sanitized_packet = packet.replace(b'?', b'')
            current_index = packet_end
            yield packet
        
        
       