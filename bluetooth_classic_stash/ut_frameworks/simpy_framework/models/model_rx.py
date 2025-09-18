import random
from udp import *
import simpy
class baseclass:
    def __init__(self, parent_object,env):
        self.timeout = 0
        self.parent_object = parent_object
        self.env = env
        self.PER_ack = 50
        self.PER_nack = 50
        self.PER_noresp = 0
        self.packet_result = []
        self.time_taken_rx = 12 #312 #625
        self.recv_packet_args = []
        self.channel = udp('localhost', 5005, env)
        self.dict_peer_response = {"ack" : 98, "nack" : 1.5, "noresp": 0.5}

    def timeout_handler(self):
        self.timeout = 0 
        choice = []
        weight = []
        if self.recv_packet_args[0] == 'poll':
            peer_response = random.choices([i for i in self.dict_peer_response.keys()] ,weights = [99,0,1])
            return "model_ull rx_done " + peer_response[0]
        for resp, probability in self.dict_peer_response.items():
            choice.append(resp)
            weight.append(probability)
        peer_response = random.choices(choice ,weights = (weight))
        return "model_ull rx_done 225"

    def handle_start_rx(self, args):
        #print(self.parent_object.env.now,",RX,1")
        if(int(args[2])==0):
            self.channel.sender(f"efr_scheduler packet_rate {args[0]} 1 3")
        else:
            #add packet fate logic
            print("Receiving Packet")
        return ""

    def handle_set_phy_params(self, args):
        self.dict_peer_response["ack"] = int(args[0])
        self.dict_peer_response["nack"] = int(args[1])
        self.dict_peer_response["noresp"] = int(args[2])
        return ""
