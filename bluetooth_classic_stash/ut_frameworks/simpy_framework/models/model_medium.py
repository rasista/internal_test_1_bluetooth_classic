import re
class baseclass:
    def __init__(self, parent_object,env):
        self.env = env
        self.timeout = 0
        self.parent_object = parent_object
        self.listen_schedules = dict()
        self.tx_schedules = dict()
        self.rx_latencies = dict()
        self.rx_indication_tsf = dict()
    
    def schedule_listen_handler(self, args):
        self.listen_schedules[args[0]] = [args[1], args[2], args[3]]
        return ""

    def get_next_tx_end(self):
        future_tx_ends = list()
        for tx_schedules in self.tx_schedules.values():
            if tx_schedules["tx_end"] > self.env.now:
                future_tx_ends.append(tx_schedules["tx_end"])
        if bool(future_tx_ends):
            return min(future_tx_ends)
        else:
            return 0

    def get_current_tx_end_nodes(self):
        current_tx_end_nodes = list()
        for node, tx_schedules in self.tx_schedules.items():
            if tx_schedules["tx_end"] == self.env.now:
                current_tx_end_nodes.append(node)
        return current_tx_end_nodes

    def get_device_id(self, prefix, string):
        device_id_params = re.split(prefix, string)
        return int(device_id_params[1])

    def get_active_listeners(self, tx_node):
        active_listeners = list()
        transmit_device_id = self.get_device_id("model_le_mesh_advertiser_", tx_node)
        for scanner, listen_interval in self.listen_schedules.items():
            scanner_id = self.get_device_id("model_le_mesh_scanner_", scanner)
            scanner_range = int(listen_interval[2])
            if (transmit_device_id <= scanner_id + scanner_range) and (transmit_device_id >= scanner_id - scanner_range) and (transmit_device_id != scanner_id):
                if (int(listen_interval[0]) < self.tx_schedules[tx_node]["tx_start"]) & (int(listen_interval[1]) > self.tx_schedules[tx_node]["tx_start"]):
                    active_listeners.append(scanner)
        return active_listeners


    def filter_on_air_collisions(self, tx_node, active_scanners):
        clean_scanners = list()
        for scanner in active_scanners:
            scanner_id = self.get_device_id("model_le_mesh_scanner_", scanner)
            listen_params = self.listen_schedules[scanner]
            scanner_range = int(listen_params[2])
            can_listen = 1
            for advertiser, tx_schedule in self.tx_schedules.items():
                transmit_device_id = self.get_device_id("model_le_mesh_advertiser_", advertiser)
                if (transmit_device_id <= scanner_id + scanner_range) and (transmit_device_id >= scanner_id - scanner_range):
                    if (tx_schedule["tx_end"]  < self.tx_schedules[tx_node]["tx_start"]) or (tx_schedule["tx_start"] > self.tx_schedules[tx_node]["tx_end"]):
                        pass
                    else:
                        if advertiser is not tx_node:
                            can_listen = 0
                            #print(f"{self.env.now} {tx_node} collides at {scanner} with {advertiser}")
            if can_listen:
                clean_scanners.append(scanner)
        return clean_scanners

    def handle_tx_end(self, tx_node):
        return_cmd = ""
        listeners = self.get_active_listeners(tx_node)        
        listeners = self.filter_on_air_collisions(tx_node, listeners)
        for scanner in listeners:
            self.rx_indication_tsf[scanner] = self.env.now
            if scanner not in self.rx_latencies.keys():
                self.rx_latencies[scanner] = dict()
            pkt_start_tsf = self.tx_schedules[tx_node]["pkt_start_tsf"]
            if pkt_start_tsf not in self.rx_latencies[scanner].keys():
                self.rx_latencies[scanner][pkt_start_tsf] = self.env.now - int(pkt_start_tsf)
            return_cmd = return_cmd + f"{scanner} rx_done {tx_node} {pkt_start_tsf}\n"
            self.rx_indication_tsf[scanner] = self.env.now
        return return_cmd


    def timeout_handler(self):
        current_tx_end_nodes = self.get_current_tx_end_nodes()
        return_cmd = ""
        for tx_end_node in current_tx_end_nodes:
            return_cmd = return_cmd + self.handle_tx_end(tx_end_node)
        self.timeout = self.get_next_tx_end()
        return return_cmd

    def is_device_scanning(self, scanner, tsf):
        scanning = 0
        if scanner in self.listen_schedules.keys():
            if (int(self.listen_schedules[scanner][0]) < tsf) & (int(self.listen_schedules[scanner][1]) > tsf):
                scanning = 1
        return scanning

    def is_rx_on(self, scanner, tsf):
        scanner_id = self.get_device_id("model_le_mesh_scanner_", scanner)
        listen_params = self.listen_schedules[scanner]
        scanner_range = int(listen_params[2])
        listen_end = 0
        for advertiser, tx_schedule in self.tx_schedules.items():
            transmit_device_id = self.get_device_id("model_le_mesh_advertiser_", advertiser)
            if (transmit_device_id <= scanner_id + scanner_range) and (transmit_device_id >= scanner_id - scanner_range):
                if (tx_schedule["tx_end"]  < tsf) or (tx_schedule["tx_start"] > tsf):
                    pass
                else:
                    if transmit_device_id is not scanner_id:
                        if listen_end < tx_schedule["tx_end"]:
                            listen_end = tx_schedule["tx_end"]
        return listen_end

    def schedule_tx_handler(self, args):
        device_id = self.get_device_id("model_le_mesh_advertiser_", args[0])
        adjoining_scanner = f"model_le_mesh_scanner_{device_id}"
        if self.is_device_scanning(adjoining_scanner, int(args[1])):
            listen_end_tsf = self.is_rx_on(adjoining_scanner, int(args[1]))
            if listen_end_tsf != 0:
                return f"{args[0]} reschedule_tx {listen_end_tsf + 150} {args[2]} {args[3]}"
        if args[0] not in self.tx_schedules.keys():
            self.tx_schedules[args[0]] = dict()
        self.tx_schedules[args[0]]["tx_start"] = int(args[1])
        self.tx_schedules[args[0]]["tx_end"] = int(args[1]) + int(args[2]) 
        self.tx_schedules[args[0]]["pkt_start_tsf"] = int(args[3])
        self.timeout = self.get_next_tx_end()
        return_cmd = ""
        return return_cmd
        
    def display_latencies_handler(self, args):
        for scanner, latencies in self.rx_latencies.items():
            print(f"Latencies, {scanner}, {len(latencies.values())}, {sum(latencies.values())//len(latencies.values())}")
        return ""