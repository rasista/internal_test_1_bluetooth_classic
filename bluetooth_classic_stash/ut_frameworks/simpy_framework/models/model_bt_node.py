import sys
import random
#sys.path.insert(0,'..')
from util_libraries.sbc_pkt.sbc_pkt import sbc_pkt

class baseclass():
    def __init__(self, parent_object, env):
        self.env = env
        self.parent_object = parent_object
        self.timeout = 0
        self.wakeup_rf_prog_current = 0
        self.wakeup_rf_prog_duration = 970
        self.rf_standby_current = 0     
        self.pre_slot_isr_current = 0
        self.pre_slot_isr_duration = 180
        self.active_tx_current = 50
        self.turnaround_current = 0
        self.turnaround_duration = 50
        self.poll_duration = 126
        self.active_rx_current = 5
        self.sleep_current = 0.0
        self.first_tx_tsf = 0
        self.tx_count = 0
        self.next_activity = ""
        self.active_2_sleep_duration = 500
        self.active_2_sleep_current = 0
        self.xtal_current = 0
        self.xtal_duration = 1170
        self.entry_to_lmac_prog_current = 0
        self.entry_to_lmac_prog_duration = 2040
        self.lmac_and_bbp_current  = 0
        self.lmac_and_bbp_duration = 370
        self.bbp_to_bt_resume_current = 0
        self.bbp_to_bt_resume_duration = 148
        self.bt_resume_2_rf_prog_current = 0
        self.bt_resume_2_rf_prog_duration = 970
        self.burst_count = 3
        self.sbc_packet_obj = sbc_pkt("2DH3")

    def timeout_handler(self):
        print(self.env.now, self.next_activity)
        if (self.next_activity == "start_pre_slot_isr"):
            return_str = "model_power_meter add_activity " + str(self.pre_slot_isr_current) + " " + str(self.pre_slot_isr_duration)
            self.timeout = self.pre_slot_isr_duration + self.env.now
            self.next_activity = "start_tx"
            return return_str
        elif (self.next_activity == "start_tx"):
            return_str = "model_power_meter add_activity " + str(self.active_tx_current) + " " + str(self.sbc_packet_obj.duration)
            self.timeout = self.env.now + self.sbc_packet_obj.duration
            if self.tx_count == 0:
                self.first_tx_tsf = self.env.now
            self.next_activity = "tx_rx_turnaround"
            return return_str
        elif (self.next_activity == "tx_rx_turnaround"):
            return_str = "model_power_meter add_activity " + str(self.turnaround_current) + " " + str(self.turnaround_duration)
            self.timeout = self.env.now + self.turnaround_duration
            self.next_activity = "tx_standby"
            return return_str
        elif (self.next_activity == "tx_standby"):
            next_master_slot_tsf = self.get_next_master_slot_tsf()
            next_slave_slot_tsf = next_master_slot_tsf - 625
            return_str = "model_power_meter add_activity " + str(self.rf_standby_current) + " " + str(next_slave_slot_tsf - self.env.now)
            self.timeout = next_slave_slot_tsf
            self.next_activity = "receive_poll"
            return return_str
        elif (self.next_activity == "receive_poll"):
            return_str = "model_power_meter add_activity " + str(self.active_rx_current) + " " + str(self.poll_duration)
            self.timeout = self.env.now + self.poll_duration
            self.next_activity = "rx_tx_turnaround"
            return return_str
        elif (self.next_activity == "rx_tx_turnaround"):
            return_str = "model_power_meter add_activity " + str(self.turnaround_current) + " " + str(self.turnaround_duration)
            self.timeout = self.env.now + self.turnaround_duration
            self.next_activity = "rx_tx_standby"
            return return_str
        elif (self.next_activity == "rx_tx_standby"):
            next_master_slot_tsf = self.get_next_master_slot_tsf()
            return_str = "model_power_meter add_activity " + str(self.rf_standby_current) + " " + str(next_master_slot_tsf - self.env.now)
            self.timeout = next_master_slot_tsf
            tx_success = random.choices([0, 1], weights=[13,87])
            print("tx_success = ", tx_success[0])
            if tx_success[0] == 1:
                self.tx_count += 1
            if (self.tx_count < self.burst_count):
                self.next_activity = "start_tx"
            else:
                self.tx_count = 0
                self.next_activity = "trigger_sleep"
            return return_str
        elif(self.next_activity == "trigger_sleep"):
            return_str = "model_power_meter add_activity " + str(self.active_2_sleep_current) + " " + str(self.active_2_sleep_duration)
            self.timeout = self.env.now + self.active_2_sleep_duration
            self.next_activity = "sleep"
            return return_str
        elif(self.next_activity == "sleep"):
            sleep_to_active_duration = self.xtal_duration + self.entry_to_lmac_prog_duration + self.lmac_and_bbp_duration + self.bbp_to_bt_resume_duration + self.bt_resume_2_rf_prog_duration
            wakeup_tsf = self.first_tx_tsf + ((int(self.sbc_packet_obj.audio_content*self.burst_count*1000) + 1250 - 1) // 1250) * 1250 - self.pre_slot_isr_duration - sleep_to_active_duration
            return_str = "model_power_meter add_activity " + str(self.sleep_current) + " " + str(wakeup_tsf - self.env.now)
            self.timeout = wakeup_tsf
            self.next_activity = "wakeup"
            return return_str
        elif (self.next_activity == "wakeup"):
            sleep_2_active_current = self.xtal_current*self.xtal_duration + self.entry_to_lmac_prog_current*self.entry_to_lmac_prog_duration \
            + self.lmac_and_bbp_current*self.lmac_and_bbp_duration + self.bbp_to_bt_resume_current*self.bbp_to_bt_resume_duration \
            + self.bt_resume_2_rf_prog_current*self.bt_resume_2_rf_prog_duration
            sleep_to_active_duration = self.xtal_duration + self.entry_to_lmac_prog_duration + self.lmac_and_bbp_duration + self.bbp_to_bt_resume_duration + self.bt_resume_2_rf_prog_duration
            return_str = "model_power_meter add_activity " + str(sleep_2_active_current/sleep_to_active_duration) + " " + str(sleep_to_active_duration)
            self.timeout = self.env.now + sleep_to_active_duration
            self.next_activity = "start_pre_slot_isr"
            return return_str
        self.timeout = 0
        return "OK"

    def get_next_master_slot_tsf(self) :
        return ((self.env.now + 1250) // 1250) * 1250

    def start_streaming_handler(self, args):
        self.sbc_packet_obj = sbc_pkt(args[0])
        next_master_slot_tsf = self.get_next_master_slot_tsf()
        self.timeout = next_master_slot_tsf - self.pre_slot_isr_duration
        self.next_activity = "start_pre_slot_isr"
        print("Frame Audio content = ", self.sbc_packet_obj.obj_sbc_frame.audio_duration, \
        "Packet Audio Content = ", self.sbc_packet_obj.audio_content, \
        "Burst Audio Content = ", self.sbc_packet_obj.audio_content * self.burst_count)
        return "OK"
