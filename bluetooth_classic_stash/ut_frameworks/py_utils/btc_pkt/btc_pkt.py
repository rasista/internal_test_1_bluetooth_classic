class btc_pkt:
    rate_3m_packet_type_len = {
        "3DH1": 83,
        "3DH3": 552,
        "3DH5": 1021
    }
    rate_2m_packet_type_len = {
        "2DH1": 54,
        "2DH3": 367,
        "2DH5": 679,
    }
    rate_1m_packet_type_len = {
        "DH1": 27,
        "DH2": 183,
        "DH3": 339,
    }

    def __init__(self, packet_type):
        if(packet_type in self.rate_1m_packet_type_len.keys()):
            self.packet_type = packet_type
            self.packet_length = self.rate_1m_packet_type_len[self.packet_type]
        elif(packet_type in self.rate_2m_packet_type_len.keys()):
            self.packet_type = packet_type
            self.packet_length = self.rate_2m_packet_type_len[self.packet_type]
        elif(packet_type in self.rate_3m_packet_type_len.keys()):
            self.packet_type = packet_type
            self.packet_length = self.rate_3m_packet_type_len[self.packet_type]
        else:
            print("Incorrect Packet Selection. expected packet type in ", self.rate_1m_packet_type_len.keys(
            ), self.rate_2m_packet_type_len.keys(), self.rate_2m_packet_type_len.keys())

    def get_packet_duration(self, packet_type, payload_size):
        if(packet_type in self.rate_1m_packet_type_len.keys()):
            duration = self.get_br_packet_duration(payload_size)
        elif(packet_type in self.rate_2m_packet_type_len.keys()):
            duration = self.get_edr_packet_duration(payload_size, 2)
        elif(packet_type in self.rate_3m_packet_type_len.keys()):
            duration = self.get_edr_packet_duration(payload_size, 3)
        return duration

    def get_br_packet_duration(self, payload_size):
        preamble = 4
        access_code = 64
        trailer = 4
        phy_header = 54
        payload_header = 1 * 8
        payload = payload_size * 8
        crc = 24
        return (preamble + access_code + trailer + phy_header + payload_header + payload + crc)

    def get_edr_packet_duration(self, payload_size, rate):
        preamble = 4
        access_code = 64
        trailer = 4
        phy_header = 54
        edr_gaurd = 5
        edr_sync_sequence = 11
        payload_header = 2 * 8
        payload = payload_size * 8
        crc = 24
        edr_trailer = 2
        return (preamble + access_code + trailer + phy_header + edr_gaurd + edr_sync_sequence + (((payload_header + payload + crc + rate) - 1)//rate) + edr_trailer)


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(
        description="Calculates packet duration for BTC packets")
    parser.add_argument('-t', type=str, help='packet_type')
    parser.add_argument('-l', type=int, help='Payload length')
    args = parser.parse_args()

    obj_btc_packet = btc_pkt(args.t)
    print("Duration = ", obj_btc_packet.get_packet_duration(args.t, args.l))
