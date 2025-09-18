from random import randint
from datetime import datetime
import yaml
class baseclass:
    def __init__(self, parent_object,env):
        self.env = env
        self.timeout = 0
        self.parent_object = parent_object
        self.trigger_freq = 0
        self.max_delay = 0
        self.tx_count = 0
        #self.filename = ""
    
    def secondary_init_function(self, index, dict_model_params):
        self.trigger_freq = int(dict_model_params['init_params']['test_adv_frequency'])
        self.max_delay = int(dict_model_params['init_params']['test_adv_variance'])
        self.instances = int(dict_model_params['init_params']['instances'])
        self.filename = dict_model_params['init_params']['file_name']
        

    def start_test_handler(self,args):
        self.tx_count = self.tx_count + 1 
        self.timeout = self.env.now + self.trigger_freq + randint(0, self.max_delay)
        return "mesh_advertiser_1 adv_req generate_id"

    def get_results_handler(self, args):
        return_cmd = ""
        '''
        for i in range(2,self.instances + 1):
            return_cmd += f"mesh_scanner_{i} get_results\n"
        '''
        Node_Dict = {}
        sample_data = {}
        model_dict = {}
        
        object_lut = self.parent_object.dict_global_models_vs_object_lut
        i = 1
        current_time = datetime.now()
        date_string = current_time.strftime("%Y-%m-%d %H:%M:%S")
        Node_Dict[date_string] = sample_data
        target_models = list()
        for i in range(2, self.instances + 1):
            model = f"mesh_scanner_{i}"
            target_models.append(model)
        
        for model in target_models:
            #print("Manas-->" , object_lut[model].obj_model_specific.rx_done_queue)
            #print("Model-->", model)
            sample_data[model] = dict()
            sample_data[model]["Latencies"] = object_lut[model].obj_model_specific.rx_done_queue
            sample_data[model]["ON_AIR_COLLISSION_COUNT"] = object_lut[model].obj_model_specific.tx_on_air_collison_count
            sample_data[model]["WIFI_PUNCTURE_COUNT"] = object_lut[model].obj_model_specific.wifi_puncture_count

        with open(self.filename, 'a') as outfile:
            yaml.dump(Node_Dict, outfile, default_flow_style=False)
            
        return return_cmd

    def timeout_handler(self):
        self.tx_count = self.tx_count + 1 
        if self.tx_count <= 100:
            self.timeout = self.env.now + self.trigger_freq + randint(0, self.max_delay)
            return "mesh_advertiser_1 adv_req generate_id"
        else :
            self.timeout = 0
            return ""