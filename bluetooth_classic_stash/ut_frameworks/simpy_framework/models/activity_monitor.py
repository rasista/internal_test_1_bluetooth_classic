import math
from sim_models import sim_models
class activity_monitor(sim_models):
    def __init__(self, *args):
        super().__init__(*args)
        self.hss_em2_base_power = 9.3/1000 #9.3 uW from PRD
        self.hss_em2_retained_blocks = 2 # Assume 2*128 RAm Blocks retained in Host SS
        self.hss_em2_power_per_block = 3/1000 # 3uW from PRD
        self.hss_em2_retained_l2_cache_blocks = 2# Assume 2*64KB L2 retained in Host SS
        self.hss_em2_power_per_l2_cache_block = 2.5/1000 # 2.5uW from Atul's calculator
        self.hss_em2_power = self.hss_em2_base_power + (self.hss_em2_retained_blocks*self.hss_em2_power_per_block) + (self.hss_em2_retained_l2_cache_blocks*self.hss_em2_power_per_l2_cache_block)
        self.hss_em0_frequency = 80 # Assumed
        self.hss_em0_core_power_per_mhz = 72/1000 # 72uW per MHz from PRD, asumed contribution from M55, L1 Cache and L2 Cache
        self.hss_em0_ram_banks = math.ceil(1536/156) #Assued 1536 kB with 156 kB banks
        self.hss_em0_memory_bank_power_per_mhz = 1.33/1000 # 1.33 uW from Atul's excel
        self.hss_l1_hit_ratio = 1
        self.hss_l2_hit_ratio = .90
        self.hss_em0_flash_access_ratio = (1 - self.hss_l1_hit_ratio)*(1 - self.hss_l2_hit_ratio)
        self.hss_em0_flash_standby_ratio = 1 - self.hss_em0_flash_access_ratio
        self.hss_em0_flash_access_power = 24.75 #24.75mW from Atul's ca;cuator
        self.hss_em0_flash_standby_power = self.hss_em0_flash_access_power/1000 
        self.hss_em0_flash_power = self.hss_em0_flash_access_ratio*self.hss_em0_flash_access_power + self.hss_em0_flash_standby_power*self.hss_em0_flash_standby_ratio
        #self.hss_em0_flash_power = 0
        self.hss_em0_power = (self.hss_em0_core_power_per_mhz  + (self.hss_em0_ram_banks*self.hss_em0_memory_bank_power_per_mhz))* self.hss_em0_frequency + self.hss_em0_flash_power
        self.lpwss_em0_frequency = 40 # Assumed
        self.lpwss_em2_base_power = 0 #Assumed it is counted in hss_em2_base_power
        self.lpwss_em2_retained_blocks = 1 # Assume 1*128 RAm Blocks retained in LPW SS
        self.lpwss_em2_power_per_block = 3/1000 # 3uW from PRD
        self.lpwss_em2_retained_l2_cache_blocks = 0# LPWSS does not operate on L2 Cache
        self.lpwss_em2_power_per_l2_cache_block = 2.5/1000 # 2.5uW from Atul's calculator
        self.lpwss_em2_power = self.lpwss_em2_base_power + (self.lpwss_em2_retained_blocks*self.lpwss_em2_power_per_block) + (self.lpwss_em2_retained_l2_cache_blocks*self.lpwss_em2_power_per_l2_cache_block)
        self.lpwss_em0_core_power_per_mhz = 26.86/1000 # 26.86uW per MHz from Atul's calculator, assumed only RISC-V power
        self.lpwss_em0_ram_banks = math.ceil(129/128) #Assumed more than 128 Kb of memory required in LPWSS
        self.lpwss_em0_memory_bank_power_per_mhz = 1.33/1000 # 1.33 uW from Atul's excel
        self.lpwss_em0_power = (self.lpwss_em0_core_power_per_mhz  + (self.lpwss_em0_ram_banks*self.lpwss_em0_memory_bank_power_per_mhz))* self.lpwss_em0_frequency 
        self.tx_power = 90 # 90mW for BTC 11dBm PA from PRD
        self.rx_power = 9 # 9mW for BTC-BR RX from PRD, this is aplicable while receiving ACKs. For Audio stream, need to update
        self.em2_wakeup_duration = 100/1000 # 100 micro seconds from Atul's calculator
        self.em2_wakeup_power = 7.5/1000 #7.5uW from Atul's calcuator
        self.current_state = "EM1P"
        self.pa_rampup_duration = 60 #Assumed
        self.pa_rampup_power = self.tx_power/2 #Assumed
        self.pa_rampdown_duration = 30 #Assumed
        self.pa_rampdown_power = self.tx_power/2 #Assumed
        self.lna_rampup_duration = 60 #Assumed
        self.lna_rampup_power = self.rx_power / 2 #Assumed
        self.lna_rampdown_duration = 30 #Assumed
        self.lna_rampdown_power = self.rx_power / 2 #Assumed
        self.tx_start_pending_queue = dict()
        self.tx_complete_pending_queue = dict()
        self.rx_start_pending_queue = dict()
        self.rx_complete_pending_queue = dict()
        self.lpwss_sleep_complete_pending_queue = dict()
        self.hss_sleep_complete_pending_queue = dict()
        self.total_energy = 0
        self.hss_total_energy = 0
        self.lpwss_em0_start_tsf = 0
        self.lpwss_em2_start_tsf = 0
        self.hss_em0_start_tsf = 0
        self.hss_em2_start_tsf = 0
        self.measurement_start_tsf = 0


    def set_tx_power(self, args):
        self.tx_power = float(args[0])
        self.pa_rampup_power = self.tx_power/2 #Assumed
        self.pa_rampdown_power = self.tx_power/2 #Assumed
        return ""
    
    def set_rx_power(self, args):
        self.rx_power = float(args[0])
        self.lna_rampup_power = self.rx_power / 2 #Assumed
        self.lna_rampdown_power = self.rx_power / 2 #Assumed
        return ""

    def set_lpwss_em0_power(self, args):
        self.lpwss_em0_power = float(args[0])
        self.em2_wakeup_power = self.lpwss_em0_power/2 #Assumed
        print(f"Hasan {self.lpwss_em0_power}")
        return ""

    def schedule_tx_handler(self,args):
        master_module = args[0]
        tx_start_tsf = int(args[1])
        tx_duration = int(args[2])
        tx_start_tsf = tx_start_tsf - self.pa_rampup_duration
        self.tx_start_pending_queue[tx_start_tsf] = dict()
        self.tx_start_pending_queue[tx_start_tsf]["master_module"] = master_module
        self.tx_start_pending_queue[tx_start_tsf]["tx_duration"] = tx_duration
        self.register_timeout(tx_start_tsf, "tx_start_handler")
        return ""
    
    def tx_start_handler(self):
        if self.lpwss_em0_start_tsf == 0:
            self.lpwss_em0_start_tsf = self.env.now 
            self.hss_em0_start_tsf = self.env.now 
            self.measurement_start_tsf = self.env.now
        tx_pending_elem = self.tx_start_pending_queue.pop(self.env.now)
        tx_active_duration = tx_pending_elem["tx_duration"]
        tx_complete_tsf = self.env.now + self.pa_rampup_duration + tx_active_duration + self.pa_rampdown_duration
        self.tx_complete_pending_queue[tx_complete_tsf] = tx_pending_elem
        self.register_timeout(tx_complete_tsf, "tx_complete_handler")
        self.sim_printf(f"lpwss_procedure tx_rampup {self.env.now} {self.pa_rampup_power * 1000}")
        self.sim_printf(f"lpwss_procedure tx_active {self.env.now + self.pa_rampup_duration} {self.tx_power * 1000}")
        self.sim_printf(f"lpwss_procedure tx_rampdown {self.env.now + self.pa_rampup_duration + tx_active_duration} {self.pa_rampdown_power * 1000}")
        return ""
        

    def tx_complete_handler(self):
        tx_pending_elem = self.tx_complete_pending_queue.pop(self.env.now)
        active_tx_duration = tx_pending_elem["tx_duration"]
        self.total_energy += self.pa_rampup_duration*self.pa_rampup_power + self.tx_power*active_tx_duration + self.pa_rampdown_duration*self.pa_rampdown_power
        master_module = tx_pending_elem["master_module"]
        self.sim_printf(f"lpwss_procedure lpwss_em0_idle {self.env.now} {self.lpwss_em0_power * 1000}")
        return f"{master_module} tx_complete"
    

    def schedule_rx_handler(self,args):
        master_module = args[0]
        rx_start_tsf = int(args[1])
        rx_duration = int(args[2])
        rx_start_tsf = rx_start_tsf - self.lna_rampup_duration
        self.rx_start_pending_queue[rx_start_tsf] = dict()
        self.rx_start_pending_queue[rx_start_tsf]["master_module"] = master_module
        self.rx_start_pending_queue[rx_start_tsf]["rx_duration"] = rx_duration
        self.register_timeout(rx_start_tsf, "rx_start_handler")
        return ""
    
    def rx_start_handler(self):
        
        if self.lpwss_em0_start_tsf == 0:
            self.lpwss_em0_start_tsf = self.env.now 
            self.hss_em0_start_tsf = self.env.now 
            self.measurement_start_tsf = self.env.now
        rx_pending_elem = self.rx_start_pending_queue.pop(self.env.now)
        rx_active_duration = rx_pending_elem["rx_duration"]
        rx_complete_tsf = self.env.now + self.lna_rampup_duration + rx_active_duration + self.lna_rampdown_duration
        self.rx_complete_pending_queue[rx_complete_tsf] = rx_pending_elem
        self.register_timeout(rx_complete_tsf, "rx_complete_handler")
        self.sim_printf(f"lpwss_procedure rx_rampup {self.env.now} {self.lna_rampup_power * 1000}")
        self.sim_printf(f"lpwss_procedure rx_start {self.env.now + self.lna_rampup_duration} {self.rx_power * 1000}")
        self.sim_printf(f"lpwss_procedure rx_rampdown {self.env.now + self.lna_rampup_duration + rx_active_duration} {self.lna_rampdown_power * 1000}")
        return ""
        

    def rx_complete_handler(self):
        rx_pending_elem = self.rx_complete_pending_queue.pop(self.env.now)
        active_rx_duration = rx_pending_elem["rx_duration"]
        self.total_energy += self.lna_rampup_duration*self.lna_rampup_power + self.rx_power*active_rx_duration + self.lna_rampdown_duration*self.lna_rampdown_power
        master_module = rx_pending_elem["master_module"]
        self.sim_printf(f"lpwss_procedure lpwss_em0_idle {self.env.now} {self.lpwss_em0_power * 1000}")
        return f"{master_module} rx_complete ACK"

    def lpwss_schedule_sleep_handler(self, args):
        master_model = args[0]
        wakeup_tsf = int(args[1])
        self.lpwss_sleep_complete_pending_queue[wakeup_tsf] = dict()
        self.lpwss_sleep_complete_pending_queue[wakeup_tsf]["master_model"] = master_model
        self.register_timeout(wakeup_tsf, "lpwss_sleep_complete_handler")
        self.em2_start_tsf = self.env.now
        lpwss_em0_duration = self.env.now - self.lpwss_em0_start_tsf
        self.total_energy += self.lpwss_em0_power * lpwss_em0_duration
        self.sim_printf(f"lpwss_procedure lpwss_em2 {self.env.now} {self.lpwss_em2_power * 1000}")
        return f"{master_model} lpwss_sleep_start"

    def hss_schedule_sleep_handler(self, args):
        master_model = args[0]
        wakeup_tsf = int(args[1])
        self.hss_sleep_complete_pending_queue[wakeup_tsf] = dict()
        self.hss_sleep_complete_pending_queue[wakeup_tsf]["master_model"] = master_model
        self.register_timeout(wakeup_tsf, "hss_sleep_complete_handler")
        self.hss_em2_start_tsf = self.env.now
        hss_em0_duration = self.env.now - self.hss_em0_start_tsf
        self.hss_total_energy += self.hss_em0_power * hss_em0_duration
        self.sim_printf(f"hss_procedure hss_em2 {self.env.now} {self.hss_em2_power * 1000}")
        return ""

    def lpwss_sleep_complete_handler(self):
        self.sim_printf("sleep_complete")
        sleep_pending_elem = self.lpwss_sleep_complete_pending_queue.pop(self.env.now)
        sleep_duration = self.env.now - self.em2_wakeup_duration - self.em2_start_tsf
        self.total_energy += self.em2_wakeup_duration*self.em2_wakeup_power + self.lpwss_em2_power*sleep_duration
        self.lpwss_em0_start_tsf = self.env.now
        average_power = self.total_energy / (self.env.now - self.measurement_start_tsf)
        self.sim_printf(f"lpwss_procedure lpwss_em0_idle {self.env.now} {self.lpwss_em0_power * 1000}")
        self.sim_printf(f"Average power {average_power}")
        master_model = sleep_pending_elem["master_model"]
        return f"{master_model} sleep_complete"


    def hss_sleep_complete_handler(self):
        self.sim_printf("hss_sleep_complete")
        sleep_pending_elem = self.hss_sleep_complete_pending_queue.pop(self.env.now)
        sleep_duration = self.env.now - self.em2_wakeup_duration - self.hss_em2_start_tsf
        self.hss_total_energy += self.em2_wakeup_duration*self.em2_wakeup_power + self.hss_em2_power*sleep_duration
        self.hss_em0_start_tsf = self.env.now
        average_power = self.hss_total_energy / (self.env.now - self.measurement_start_tsf)
        self.sim_printf(f"hss_procedure hss_em0 {self.env.now} {self.hss_em0_power * 1000}")
        self.sim_printf(f"HSS Average power {average_power}")
        master_model = sleep_pending_elem["master_model"]
        return f""