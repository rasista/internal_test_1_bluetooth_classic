from udp import *
class baseclass:
    def __init__(self, parent_object,env):
        self.timeout = 0
        self.env = env
        self.parent_object = parent_object
        self.total_num_packets = 1000
        self.present_packet_num = 1
        self.wake_up_time = 625*7
        self.next_wake_up_time = 0
        self.packets_perSlot = 3 
        self.idle_time_period = 100
        self.prev_NoResp = 0
        self.channel = udp('localhost', 5005, env)
        self.radio_state = "Unknown"

    def next_schedule_handler(self, args):
        sleep_duration = max(0, int(args[0]) - self.env.now)
        self.radio_state = "Radio Off"
        self.channel.sender("efr_scheduler radio_off")
        cmds = "model_pre_slot_isr radio_off\n"
        return cmds + "model_coex_manager sleep_request " + str(sleep_duration)

    def add_process_request_handler(self, args):
        extra = ""
        for i in args:
            extra += i + " "
        response = self.channel.sender("efr_scheduler add_process "+extra)
        response_id = response.split()
        if response_id[1] == "next_schedule":
            sleep_duration = max(0, int(response_id[2]) - self.env.now)
            return "model_coex_manager sleep_request " + str(sleep_duration)
        return "OK"
    
    def set_invalid_handler(self, args):
        task = args[0]
        self.channel.sender("efr_scheduler set_invalid "+task)
        return "OK"

    def remove_process_request_handler(self, args):
        response = self.channel.sender("efr_scheduler remove_process " +args[0])
        response_id = response.split()
        print(response_id)
        try:
            if response_id[1] == "next_schedule":
                sleep_duration = max(0, int(response_id[2]) - self.env.now)
                return "model_coex_manager sleep_request " + str(sleep_duration)
        except:
            #print("---------------------------------------------------------------------------------------------------------------------",response_id)
            pass
        return "OK"

    def resume_protocol_handler(self, args):
        self.radio_state = "Radio On"
        self.channel.sender("efr_scheduler radio_on")
        return "model_pre_slot_isr radio_on"

    def handle_start_streaming(self, args) :
        #self.timeout = self.wake_up_time
        print(self.parent_object.env.now,",controller,1")
        return "model_tx start_tx " + "".join(args[0:])

    def handle_tx_done(self, args):
        print(self.parent_object.env.now,",TX,0")
        return "model_rx start_rx 225"

    
    def handle_rx_done(self, args):
        print(self.parent_object.env.now, ",RX,0")
        timeout = 225-(12+12)
        return f""
        
        '''if args[0] == 'ack':
            print(self.parent_object.env.now, ",ACK,")
            if self.present_packet_num % self.packets_perSlot == 0 :
                self.next_wake_up_time = self.wake_up_time
                self.timeout = self.next_wake_up_time
                self.present_packet_num += 1
                print(self.parent_object.env.now,",controller,0")
                return ""
            else:
                self.present_packet_num += 1
                return "model_tx start_tx 2DH3"
        elif args[0] == 'nack':
            print(self.parent_object.env.now, ",NACK,")
            return "model_tx start_tx 2DH3"
        elif args[0] == 'noresp':
            print(self.parent_object.env.now, ",NORESP,")
            print(self.parent_object.env.now,",controller,0")
            self.timeout = self.idle_time_period
            self.prev_NoResp = 1
            return ""'''
            