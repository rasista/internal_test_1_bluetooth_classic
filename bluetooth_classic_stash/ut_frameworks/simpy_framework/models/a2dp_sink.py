from sim_models import sim_models
from util_libraries.sbc_pkt.sbc_pkt import sbc_pkt
import random 
class a2dp_sink(sim_models):
    def __init__(self, *args):
        super().__init__(*args)
        self.sbc_packet_obj = None
        self.sm_handlers = {"SCHEDULE_TX": "schedule_tx_handler", "SCHEDULE_RX" : "schedule_rx_handler", "SCHEDULE_SLEEP": "schedule_sleep_handler"}
        self.burst_duration = 0
        self.burst_start_tsf = 0

    def schedule_tx_handler(self):
        next_slave_slot_tsf = self.get_next_slave_slot_tsf()
        return f"activity_monitor schedule_tx a2dp_sink {next_slave_slot_tsf} 128"

    def tx_complete_handler(self, args):
        if self.data_receive_pending:
            self.sm_next_action = "SCHEDULE_RX"
        else :
            self.sm_next_action = "SCHEDULE_SLEEP"

        self.sim_printf(f"tx_complete")
        return self.sm_step()

    def schedule_rx_handler(self):
        next_master_slot_tsf = self.get_next_master_slot_tsf()
        if (next_master_slot_tsf - self.env.now) < 60:
            next_master_slot_tsf += 1250
        next_master_slot_tsf += 1250 #forcing a worst case scenario of waking a slot early
        self.sim_printf(f"schedule_rx {next_master_slot_tsf} {self.sbc_packet_obj.duration}")
        self.burst_start_tsf = next_master_slot_tsf
        return f"activity_monitor schedule_rx a2dp_sink {next_master_slot_tsf} {self.sbc_packet_obj.duration}"
    
    def rx_complete_handler(self, args):
        peer_response = random.choices(["ACK", "NACK"], [97,3])
        self.data_receive_pending = 0
        print(f"Hasan --> {peer_response[0]}")
        if peer_response[0] == "ACK":
            self.sim_printf("rx_complete ACK")
            self.sm_next_action = "SCHEDULE_TX"
        else:
            self.sim_printf("rx_complete NACK/Noresp")
            self.sm_next_action = "SCHEDULE_TX"
            self.data_receive_pending = 1
        return self.sm_step()

    def schedule_sleep_handler(self):
        wakeup_tsf = self.burst_start_tsf + (self.sbc_packet_obj.audio_content*1000) - 1250 #assuming an irregularity of a slot in wakeup, worst case scenario
        wakeup_tsf = int(wakeup_tsf)
        self.burst_duration = 0
        self.wakeup_tsf = wakeup_tsf
        self.sim_printf(f"lpwss_schedule_sleep {wakeup_tsf} {self.sbc_packet_obj.audio_content * 1000} {self.burst_start_tsf}")
        return f"activity_monitor lpwss_schedule_sleep a2dp_sink {wakeup_tsf}"

    def lpwss_sleep_start_handler(self, args):
        return f"activity_monitor hss_schedule_sleep a2dp_sink {self.wakeup_tsf}"


    def sleep_complete_handler(self, args):
        self.sim_printf(f"sleep_complete")
        self.sm_next_action = "SCHEDULE_RX"
        return self.sm_step()


    def get_next_master_slot_tsf(self) :
        bd_clk = self.env.now // 312.5
        slot_type = bd_clk % 4
        if slot_type == 0:
            next_master_bd_clk = bd_clk + 4
        elif slot_type == 1:
            next_master_bd_clk = bd_clk + 3
        elif slot_type == 2:
            next_master_bd_clk = bd_clk + 2
        elif slot_type == 3:
            next_master_bd_clk = bd_clk + 1
        return int(next_master_bd_clk * 312.5)
    
    def get_next_slave_slot_tsf(self) :
        bd_clk = self.env.now // 312.5
        slot_type = bd_clk % 4
        if slot_type == 0:
            next_master_bd_clk = bd_clk + 2
        elif slot_type == 1:
            next_master_bd_clk = bd_clk + 1
        elif slot_type == 2:
            next_master_bd_clk = bd_clk + 4
        elif slot_type == 3:
            next_master_bd_clk = bd_clk + 3
        return int(next_master_bd_clk * 312.5)
    
    def start_streaming(self, args):
        self.sbc_packet_obj = sbc_pkt(args[0])
        self.sm_next_action = "SCHEDULE_RX"
        print(self.sbc_packet_obj.audio_content)
        return self.sm_step()

    def sm_step(self):
        handler = self.sm_handlers[self.sm_next_action]
        handler = getattr(self, handler)
        return handler()