from sim_models import sim_models
import random 
class le_audio_src(sim_models):
    def __init__(self, *args):
        super().__init__(*args)
        self.sbc_packet_obj = None
        self.sm_handlers = {"SCHEDULE_TX": "schedule_tx_handler", "SCHEDULE_RX" : "schedule_rx_handler", "SCHEDULE_SLEEP": "schedule_sleep_handler"}
        self.burst_duration = 0
        self.burst_start_tsf = 0
        self.tx_count = 0
        #self.hss_processing_delay =  - 10000#@Bn 5, 80Mhz
        self.hss_processing_delay = 28000 - 20000#Bn 10, 80 Mhz
        #self.hss_processing_delay = 23000 - 10000#Bn 5, 120Mhz
        #self.hss_processing_delay = 23000 - 20000 #Bn 10, 120 Mhz
        #self.hss_processing_delay = 17000 - 10000#Bn 5, 120Mhz
        #self.hss_processing_delay = 00 #Bn 10, 120 Mhz

    def schedule_tx_handler(self):
        tx_tsf = self.env.now + 150
        self.sim_printf(f"TX {tx_tsf} {self.tx_pkt_duration}")
        return f"activity_monitor schedule_tx le_audio_src {tx_tsf} {self.tx_pkt_duration}"

    def tx_complete_handler(self, args):
        self.sm_next_action = "SCHEDULE_RX"
        self.sim_printf(f"tx_complete")
        return self.sm_step()

    def schedule_rx_handler(self):
        rx_tsf = self.env.now + 150
        return f"activity_monitor schedule_rx le_audio_src {rx_tsf} 60"
    
    def rx_complete_handler(self, args):
        peer_response = random.choices(["ACK", "NACK"], [97,3])
        if peer_response[0] == "ACK":
            self.tx_count += 1
            self.sim_printf("rx_complete ACK")
            if (self.tx_count == self.b_n):
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
        return f"activity_monitor lpwss_schedule_sleep le_audio_src {wakeup_tsf}"

    def lpwss_sleep_start_handler(self, args):
        return f"activity_monitor hss_schedule_sleep le_audio_src {self.wakeup_tsf - self.hss_processing_delay}"
        #return f""


    def sleep_complete_handler(self, args):
        self.sim_printf(f"sleep_complete")
        self.sm_next_action = "SCHEDULE_TX"
        self.tx_count = 0
        self.burst_start_tsf = self.env.now
        return self.sm_step()

    def start_streaming(self, args):
        self.tx_pkt_duration = int(args[0])
        self.b_n = int(args[1])
        self.iso_interval = int(args[2])
        self.sm_next_action = "SCHEDULE_TX"
        self.burst_start_tsf = self.env.now
        return self.sm_step()

    def sm_step(self):
        handler = self.sm_handlers[self.sm_next_action]
        handler = getattr(self, handler)
        return handler()