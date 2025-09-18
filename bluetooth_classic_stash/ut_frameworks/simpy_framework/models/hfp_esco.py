from sim_models import sim_models
import random 
class hfp_esco(sim_models):
    def __init__(self, *args):
        super().__init__(*args)
        self.sm_handlers = {"SCHEDULE_TX": "schedule_tx_handler", "SCHEDULE_RX" : "schedule_rx_handler", "SCHEDULE_SLEEP": "schedule_sleep_handler"}
        self.burst_duration = 0
        self.burst_start_tsf = 0
        self.tx_count = 0

    def schedule_tx_handler(self):
        tx_tsf = self.env.now + 150
        self.sim_printf(f"TX {tx_tsf} {self.tx_pkt_duration}")
        return f"activity_monitor schedule_tx hfp_esco {tx_tsf} {self.tx_pkt_duration}"

    def tx_complete_handler(self, args):
        self.sm_next_action = "SCHEDULE_RX"
        self.sim_printf(f"tx_complete")
        return self.sm_step()

    def schedule_rx_handler(self):
        rx_tsf = self.env.now + 150
        return f"activity_monitor schedule_rx hfp_esco {rx_tsf} {self.rx_pkt_duration}"
    
    def rx_complete_handler(self, args):
        peer_response = random.choices(["ACK", "NACK"], [97,3])
        if peer_response[0] == "ACK":
            self.tx_count += 1
            self.sim_printf("rx_complete ACK")
            if (self.tx_count == 1):
                self.sm_next_action = "SCHEDULE_SLEEP"
            else:
                self.sm_next_action = "SCHEDULE_TX"
        else:
            self.sim_printf("rx_complete NACK/Noresp")
            self.sm_next_action = "SCHEDULE_TX"
        return self.sm_step()

    def schedule_sleep_handler(self):
        wakeup_tsf = self.burst_start_tsf + self.iso_interval #assuming an irregularity of a slot in wakeup, worst case scenario
        wakeup_tsf = int(wakeup_tsf)
        self.burst_duration = 0
        self.wakeup_tsf = wakeup_tsf
        self.sim_printf(f"lpwss_schedule_sleep {wakeup_tsf}")
        return f"activity_monitor lpwss_schedule_sleep hfp_esco {wakeup_tsf}"

    def lpwss_sleep_start_handler(self, args):
        return f"activity_monitor hss_schedule_sleep hfp_esco {self.wakeup_tsf}"


    def sleep_complete_handler(self, args):
        self.sim_printf(f"sleep_complete")
        self.sm_next_action = "SCHEDULE_TX"
        self.tx_count = 0
        self.burst_start_tsf = self.env.now
        return self.sm_step()

    def start_streaming(self, args):
        self.tx_pkt_duration = int(args[0])
        self.rx_pkt_duration = int(args[1])
        self.iso_interval = int(args[2])
        self.sm_next_action = "SCHEDULE_TX"
        self.burst_start_tsf = self.env.now
        return self.sm_step()

    def sm_step(self):
        handler = self.sm_handlers[self.sm_next_action]
        handler = getattr(self, handler)
        return handler()