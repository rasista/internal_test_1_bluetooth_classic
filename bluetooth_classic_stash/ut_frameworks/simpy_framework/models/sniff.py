from sim_models import sim_models
from util_libraries.sbc_pkt.sbc_pkt import sbc_pkt
import random 
class sniff(sim_models):
    def __init__(self, *args):
        super().__init__(*args)
        self.sm_handlers = {"SCHEDULE_TX": "schedule_tx_handler", "SCHEDULE_RX" : "schedule_rx_handler", "SCHEDULE_SLEEP": "schedule_sleep_handler"}
        self.sniff_attempts = 2
        self.sniff_attempt_count = 0
        self.sniff_anchor_point_tsf = 0
        self.sniff_duration = 200000

    def set_sniff_duration(self, args):
        self.sniff_duration = int(args[0])
        return ""

    def set_sniff_attempts(self, args):
        self.sniff_attempts = int(args[0])
        return ""

    def schedule_tx_handler(self):
        next_master_slot_tsf = self.get_next_master_slot_tsf()
        self.sim_printf(f"schedule_tx {next_master_slot_tsf} 128")
        if self.sniff_attempt_count == 0:
            next_master_slot_tsf += 1250 #forcing a worst case scenario of waking a slot early
            self.sniff_anchor_point_tsf = self.env.now

        if (next_master_slot_tsf - self.env.now) < 60:
            next_master_slot_tsf += 1250
        return f"activity_monitor schedule_tx sniff {next_master_slot_tsf} 126"

    def tx_complete_handler(self, args):
        self.sm_next_action = "SCHEDULE_RX"
        self.sim_printf(f"tx_complete")
        return self.sm_step()

    def schedule_rx_handler(self):
        next_slave_slot_tsf = self.get_next_slave_slot_tsf()
        return f"activity_monitor schedule_rx sniff {next_slave_slot_tsf} 126"
    
    def rx_complete_handler(self, args):
        peer_response = random.choices(["ACK", "NACK"], [100,0])
        if peer_response[0] == "ACK":
            self.sniff_attempt_count += 1
            self.sim_printf("rx_complete ACK")
            if self.sniff_attempts == self.sniff_attempt_count:
                self.sm_next_action = "SCHEDULE_SLEEP"
            else:
                self.sm_next_action = "SCHEDULE_TX"
        else:
            self.sim_printf("rx_complete NACK/Noresp")
            self.sm_next_action = "SCHEDULE_TX"
        return self.sm_step()

    def schedule_sleep_handler(self):
        wakeup_tsf = self.sniff_anchor_point_tsf + self.sniff_duration - 1250 #assuming an irregularity of a slot in wakeup, worst case scenario
        wakeup_tsf = int(wakeup_tsf)
        self.sim_printf(f"schedule_sleep {wakeup_tsf}")
        return f"activity_monitor schedule_sleep sniff {wakeup_tsf}"


    def sleep_complete_handler(self, args):
        self.sim_printf(f"sleep_complete")
        self.sniff_attempt_count = 0
        self.sm_next_action = "SCHEDULE_TX"
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
    
    def start_sniff(self, args):
        self.sm_next_action = "SCHEDULE_TX"
        return self.sm_step()

    def sm_step(self):
        handler = self.sm_handlers[self.sm_next_action]
        handler = getattr(self, handler)
        return handler()