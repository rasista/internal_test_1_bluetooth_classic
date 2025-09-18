from sim_models import sim_models
from util_libraries.sbc_pkt.sbc_pkt import sbc_pkt
import random 
class le_idle_mode(sim_models):
    def __init__(self, *args):
        super().__init__(*args)
        self.sm_handlers = {"SCHEDULE_TX": "schedule_tx_handler", "SCHEDULE_RX" : "schedule_rx_handler", "SCHEDULE_SLEEP": "schedule_sleep_handler"}
        self.ci = 200000
        self.tx_length = 80
        self.rx_length = 180
        self.wakeup_buffer = 500
        self.current_ce_tsf = 0

    def schedule_tx_handler(self):
        self.current_ce_tsf += self.ci
        return f"activity_monitor schedule_tx le_idle_mode {self.current_ce_tsf} {self.tx_length}"

    def tx_complete_handler(self, args):
        self.sm_next_action = "SCHEDULE_RX"
        self.sim_printf(f"tx_complete")
        return self.sm_step()

    def schedule_rx_handler(self):
        return f"activity_monitor schedule_rx le_idle_mode {self.env.now + 150} {self.rx_length}"
    
    def rx_complete_handler(self, args):
        self.sm_next_action = "SCHEDULE_SLEEP"
        return self.sm_step()

    def schedule_sleep_handler(self):
        wakeup_tsf = self.current_ce_tsf + self.ci - self.wakeup_buffer #assuming an irregularity of a slot in wakeup, worst case scenario
        wakeup_tsf = int(wakeup_tsf)
        return f"activity_monitor lpwss_schedule_sleep le_idle_mode {wakeup_tsf}"

    def lpwss_sleep_start_handler(self, args):
        #return f"activity_monitor hss_schedule_sleep a2dp {self.wakeup_tsf}"
        return ""


    def sleep_complete_handler(self, args):
        self.sim_printf(f"sleep_complete")
        self.sm_next_action = "SCHEDULE_TX"
        return self.sm_step()


    def set_tx_length(self, args):
        self.tx_length = int(args[0])
        return ""
    
    def set_rx_length(self, args):
        self.rx_length = int(args[0])
        return ""

    def start_ci(self, args):
        self.sm_next_action = "SCHEDULE_TX"
        return self.sm_step()

    def sm_step(self):
        handler = self.sm_handlers[self.sm_next_action]
        handler = getattr(self, handler)
        return handler()