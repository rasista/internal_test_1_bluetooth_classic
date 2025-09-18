import sys
sys.path.insert(0,'..')
from util_libraries.btc_pkt.btc_pkt import *
from util_libraries.sbc_frame.sbc_frame import *

#AVDTP-13 + L2CAP-Header-4
class sbc_pkt():
    def __init__(self, pkt_type):
        self.obj_sbc_frame = sbc_frame()
        self.obj_btc_pkt = btc_pkt(pkt_type)
        self.num_sbc_frames = ((self.obj_btc_pkt.packet_length - 17) // self.obj_sbc_frame.frame_length)
        self.audio_content = self.num_sbc_frames * self.obj_sbc_frame.audio_duration
        self.payload = 17 + self.num_sbc_frames * self.obj_sbc_frame.frame_length
        self.duration = self.obj_btc_pkt.get_packet_duration(pkt_type, self.payload)
        self.wasted = self.obj_btc_pkt.packet_length - self.payload
