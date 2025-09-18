import copy 
import ctypes
import json 
import numpy as np
import serial
import logging

logger = logging.getLogger()



def padded_bin(i, width):
	final = ""
	param_type = type(i)
	if(param_type == list):
		width = 8
		for j in range(len(i)):
			s = bin(i[j])
			z = (s[:2] + s[2:].zfill(width))[2:]
			final = final + z
		return final
	elif(param_type == str):
		return_str = ''.join(format(x, 'b').zfill(8) for x in bytearray(i, 'utf-8'))
		return return_str[0:width]
	else:
		s = bin(i)
		return((s[:2] + s[2:].zfill(width))[2:])




##command : inquiry
class inquiry_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 1
        self.lap = kwargs.get('lap', int(0))
        self.inquiry_length = kwargs.get('inquiry_length', int(0))
        self.num_responses = kwargs.get('num_responses', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 5
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.lap , 24)
        linear_buffer += padded_bin(self.inquiry_length , 8)
        linear_buffer += padded_bin(self.num_responses , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_inquiry(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.lap = copy.deepcopy(json_cmd_string[key]["lap"])
  
            self.inquiry_length = copy.deepcopy(json_cmd_string[key]["inquiry_length"])
  
            self.num_responses = copy.deepcopy(json_cmd_string[key]["num_responses"])
                        
        return self.linearize(args)  

class inquiry_cancel_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 2
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_inquiry_cancel(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : periodic_inquiry_mode
class periodic_inquiry_mode_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 3
        self.max_period_length = kwargs.get('max_period_length', int(0))
        self.min_period_length = kwargs.get('min_period_length', int(0))
        self.lap = kwargs.get('lap', int(0))
        self.inquiry_length = kwargs.get('inquiry_length', int(0))
        self.num_responses = kwargs.get('num_responses', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 9
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.max_period_length , 16)
        linear_buffer += padded_bin(self.min_period_length , 16)
        linear_buffer += padded_bin(self.lap , 24)
        linear_buffer += padded_bin(self.inquiry_length , 8)
        linear_buffer += padded_bin(self.num_responses , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_periodic_inquiry_mode(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.max_period_length = copy.deepcopy(json_cmd_string[key]["max_period_length"])
  
            self.min_period_length = copy.deepcopy(json_cmd_string[key]["min_period_length"])
  
            self.lap = copy.deepcopy(json_cmd_string[key]["lap"])
  
            self.inquiry_length = copy.deepcopy(json_cmd_string[key]["inquiry_length"])
  
            self.num_responses = copy.deepcopy(json_cmd_string[key]["num_responses"])
                        
        return self.linearize(args)  

class exit_periodic_inquiry_mode_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 4
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_exit_periodic_inquiry_mode(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : create_connection
class create_connection_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 5
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.pkt_type = kwargs.get('pkt_type', int(0))
        self.page_scan_rep_mode = kwargs.get('page_scan_rep_mode', int(0))
        self.reserved = kwargs.get('reserved', int(0))
        self.clock_offset = kwargs.get('clock_offset', int(0))
        self.allow_role_switch = kwargs.get('allow_role_switch', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 13
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.pkt_type , 16)
        linear_buffer += padded_bin(self.page_scan_rep_mode , 8)
        linear_buffer += padded_bin(self.reserved , 8)
        linear_buffer += padded_bin(self.clock_offset , 16)
        linear_buffer += padded_bin(self.allow_role_switch , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_create_connection(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.pkt_type = copy.deepcopy(json_cmd_string[key]["pkt_type"])
  
            self.page_scan_rep_mode = copy.deepcopy(json_cmd_string[key]["page_scan_rep_mode"])
  
            self.reserved = copy.deepcopy(json_cmd_string[key]["reserved"])
  
            self.clock_offset = copy.deepcopy(json_cmd_string[key]["clock_offset"])
  
            self.allow_role_switch = copy.deepcopy(json_cmd_string[key]["allow_role_switch"])
                        
        return self.linearize(args)  
##command : disconnect
class disconnect_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 6
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.reason = kwargs.get('reason', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 3
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.reason , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_disconnect(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.reason = copy.deepcopy(json_cmd_string[key]["reason"])
                        
        return self.linearize(args)  
##command : create_connection_cancel
class create_connection_cancel_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 8
        self.bd_addr = kwargs.get('bd_addr', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 6
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_create_connection_cancel(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
                        
        return self.linearize(args)  
##command : accept_connection_request
class accept_connection_request_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 9
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.role = kwargs.get('role', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 7
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.role , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_accept_connection_request(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.role = copy.deepcopy(json_cmd_string[key]["role"])
                        
        return self.linearize(args)  
##command : reject_connection_request
class reject_connection_request_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 10
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.reason = kwargs.get('reason', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 7
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.reason , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_reject_connection_request(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.reason = copy.deepcopy(json_cmd_string[key]["reason"])
                        
        return self.linearize(args)  
##command : link_key_request_reply
class link_key_request_reply_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 11
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.link_key = kwargs.get('link_key', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 22
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.link_key , 128)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_link_key_request_reply(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.link_key = copy.deepcopy(json_cmd_string[key]["link_key"])
                        
        return self.linearize(args)  
##command : link_key_request_negative_reply
class link_key_request_negative_reply_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 12
        self.bd_addr = kwargs.get('bd_addr', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 6
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_link_key_request_negative_reply(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
                        
        return self.linearize(args)  
##command : pin_code_request_reply
class pin_code_request_reply_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 13
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.pin_code_length = kwargs.get('pin_code_length', int(0))
        self.pin_code = kwargs.get('pin_code', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 23
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.pin_code_length , 8)
        linear_buffer += padded_bin(self.pin_code , 128)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_pin_code_request_reply(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.pin_code_length = copy.deepcopy(json_cmd_string[key]["pin_code_length"])
  
            self.pin_code = copy.deepcopy(json_cmd_string[key]["pin_code"])
                        
        return self.linearize(args)  
##command : pin_code_request_negative_reply
class pin_code_request_negative_reply_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 14
        self.bd_addr = kwargs.get('bd_addr', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 6
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_pin_code_request_negative_reply(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
                        
        return self.linearize(args)  
##command : change_connection_packet_type
class change_connection_packet_type_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 15
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.pkt_type = kwargs.get('pkt_type', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 4
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.pkt_type , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_change_connection_packet_type(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.pkt_type = copy.deepcopy(json_cmd_string[key]["pkt_type"])
                        
        return self.linearize(args)  
##command : authentication_requested
class authentication_requested_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 17
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_authentication_requested(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  
##command : set_connection_encryption
class set_connection_encryption_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 19
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.encryption_enable = kwargs.get('encryption_enable', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 3
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.encryption_enable , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_set_connection_encryption(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.encryption_enable = copy.deepcopy(json_cmd_string[key]["encryption_enable"])
                        
        return self.linearize(args)  
##command : change_connection_link_key
class change_connection_link_key_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 21
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_change_connection_link_key(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  
##command : remote_name_request
class remote_name_request_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 25
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.page_scan_rep_mode = kwargs.get('page_scan_rep_mode', int(0))
        self.reserved = kwargs.get('reserved', int(0))
        self.clk_offset = kwargs.get('clk_offset', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 10
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.page_scan_rep_mode , 8)
        linear_buffer += padded_bin(self.reserved , 8)
        linear_buffer += padded_bin(self.clk_offset , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_remote_name_request(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.page_scan_rep_mode = copy.deepcopy(json_cmd_string[key]["page_scan_rep_mode"])
  
            self.reserved = copy.deepcopy(json_cmd_string[key]["reserved"])
  
            self.clk_offset = copy.deepcopy(json_cmd_string[key]["clk_offset"])
                        
        return self.linearize(args)  
##command : remote_name_request_cancel
class remote_name_request_cancel_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 26
        self.bd_addr = kwargs.get('bd_addr', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 6
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_remote_name_request_cancel(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
                        
        return self.linearize(args)  
##command : read_remote_supported_features
class read_remote_supported_features_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 27
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_remote_supported_features(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  
##command : read_remote_extended_features
class read_remote_extended_features_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 28
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.page_number = kwargs.get('page_number', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 3
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.page_number , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_remote_extended_features(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.page_number = copy.deepcopy(json_cmd_string[key]["page_number"])
                        
        return self.linearize(args)  
##command : read_remote_version_information
class read_remote_version_information_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 29
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_remote_version_information(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  
##command : read_clock_offset
class read_clock_offset_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 31
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_clock_offset(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  
##command : setup_synchronous_connection
class setup_synchronous_connection_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 40
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.transmit_bandwidth = kwargs.get('transmit_bandwidth', int(0))
        self.receive_bandwidth = kwargs.get('receive_bandwidth', int(0))
        self.max_latency = kwargs.get('max_latency', int(0))
        self.voice_setting = kwargs.get('voice_setting', int(0))
        self.retransmission_effort = kwargs.get('retransmission_effort', int(0))
        self.pkt_type = kwargs.get('pkt_type', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 17
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.transmit_bandwidth , 32)
        linear_buffer += padded_bin(self.receive_bandwidth , 32)
        linear_buffer += padded_bin(self.max_latency , 16)
        linear_buffer += padded_bin(self.voice_setting , 16)
        linear_buffer += padded_bin(self.retransmission_effort , 8)
        linear_buffer += padded_bin(self.pkt_type , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_setup_synchronous_connection(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.transmit_bandwidth = copy.deepcopy(json_cmd_string[key]["transmit_bandwidth"])
  
            self.receive_bandwidth = copy.deepcopy(json_cmd_string[key]["receive_bandwidth"])
  
            self.max_latency = copy.deepcopy(json_cmd_string[key]["max_latency"])
  
            self.voice_setting = copy.deepcopy(json_cmd_string[key]["voice_setting"])
  
            self.retransmission_effort = copy.deepcopy(json_cmd_string[key]["retransmission_effort"])
  
            self.pkt_type = copy.deepcopy(json_cmd_string[key]["pkt_type"])
                        
        return self.linearize(args)  
##command : accept_synchronous_connection_request
class accept_synchronous_connection_request_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 41
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.transmit_bandwidth = kwargs.get('transmit_bandwidth', int(0))
        self.receive_bandwidth = kwargs.get('receive_bandwidth', int(0))
        self.max_latency = kwargs.get('max_latency', int(0))
        self.voice_setting = kwargs.get('voice_setting', int(0))
        self.retransmission_effort = kwargs.get('retransmission_effort', int(0))
        self.pkt_type = kwargs.get('pkt_type', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 17
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.transmit_bandwidth , 32)
        linear_buffer += padded_bin(self.receive_bandwidth , 32)
        linear_buffer += padded_bin(self.max_latency , 16)
        linear_buffer += padded_bin(self.voice_setting , 16)
        linear_buffer += padded_bin(self.retransmission_effort , 8)
        linear_buffer += padded_bin(self.pkt_type , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_accept_synchronous_connection_request(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.transmit_bandwidth = copy.deepcopy(json_cmd_string[key]["transmit_bandwidth"])
  
            self.receive_bandwidth = copy.deepcopy(json_cmd_string[key]["receive_bandwidth"])
  
            self.max_latency = copy.deepcopy(json_cmd_string[key]["max_latency"])
  
            self.voice_setting = copy.deepcopy(json_cmd_string[key]["voice_setting"])
  
            self.retransmission_effort = copy.deepcopy(json_cmd_string[key]["retransmission_effort"])
  
            self.pkt_type = copy.deepcopy(json_cmd_string[key]["pkt_type"])
                        
        return self.linearize(args)  
##command : reject_synchronous_connection_request
class reject_synchronous_connection_request_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 42
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.reason = kwargs.get('reason', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 7
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.reason , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_reject_synchronous_connection_request(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.reason = copy.deepcopy(json_cmd_string[key]["reason"])
                        
        return self.linearize(args)  
##command : io_capability_request_reply
class io_capability_request_reply_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 43
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.io_capability = kwargs.get('io_capability', int(0))
        self.oob_data_present = kwargs.get('oob_data_present', int(0))
        self.authentication_requirements = kwargs.get('authentication_requirements', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 9
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.io_capability , 8)
        linear_buffer += padded_bin(self.oob_data_present , 8)
        linear_buffer += padded_bin(self.authentication_requirements , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_io_capability_request_reply(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.io_capability = copy.deepcopy(json_cmd_string[key]["io_capability"])
  
            self.oob_data_present = copy.deepcopy(json_cmd_string[key]["oob_data_present"])
  
            self.authentication_requirements = copy.deepcopy(json_cmd_string[key]["authentication_requirements"])
                        
        return self.linearize(args)  
##command : user_confirmation_request_reply
class user_confirmation_request_reply_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 44
        self.bd_addr = kwargs.get('bd_addr', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 6
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_user_confirmation_request_reply(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
                        
        return self.linearize(args)  
##command : user_confirmation_request_negative_reply
class user_confirmation_request_negative_reply_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 45
        self.bd_addr = kwargs.get('bd_addr', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 6
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_user_confirmation_request_negative_reply(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
                        
        return self.linearize(args)  
##command : user_passkey_request_reply
class user_passkey_request_reply_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 46
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.numeric_value = kwargs.get('numeric_value', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 10
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.numeric_value , 32)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_user_passkey_request_reply(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.numeric_value = copy.deepcopy(json_cmd_string[key]["numeric_value"])
                        
        return self.linearize(args)  
##command : user_passkey_request_negative_reply
class user_passkey_request_negative_reply_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 47
        self.bd_addr = kwargs.get('bd_addr', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 6
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_user_passkey_request_negative_reply(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
                        
        return self.linearize(args)  
##command : remote_oob_data_request_reply
class remote_oob_data_request_reply_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 48
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.c = kwargs.get('c', int(0))
        self.r = kwargs.get('r', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 38
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.c , 128)
        linear_buffer += padded_bin(self.r , 128)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_remote_oob_data_request_reply(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.c = copy.deepcopy(json_cmd_string[key]["c"])
  
            self.r = copy.deepcopy(json_cmd_string[key]["r"])
                        
        return self.linearize(args)  
##command : remote_oob_data_request_negative_reply
class remote_oob_data_request_negative_reply_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 51
        self.bd_addr = kwargs.get('bd_addr', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 6
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_remote_oob_data_request_negative_reply(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
                        
        return self.linearize(args)  
##command : io_capability_request_negative_reply
class io_capability_request_negative_reply_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 1
        self.ocf = 52
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.reason = kwargs.get('reason', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 7
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.reason , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_io_capability_request_negative_reply(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.reason = copy.deepcopy(json_cmd_string[key]["reason"])
                        
        return self.linearize(args)  
##command : sniff_mode
class sniff_mode_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 2
        self.ocf = 3
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.sniff_max_interval = kwargs.get('sniff_max_interval', int(0))
        self.sniff_min_interval = kwargs.get('sniff_min_interval', int(0))
        self.sniff_attempt = kwargs.get('sniff_attempt', int(0))
        self.sniff_timeout = kwargs.get('sniff_timeout', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 10
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.sniff_max_interval , 16)
        linear_buffer += padded_bin(self.sniff_min_interval , 16)
        linear_buffer += padded_bin(self.sniff_attempt , 16)
        linear_buffer += padded_bin(self.sniff_timeout , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_sniff_mode(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.sniff_max_interval = copy.deepcopy(json_cmd_string[key]["sniff_max_interval"])
  
            self.sniff_min_interval = copy.deepcopy(json_cmd_string[key]["sniff_min_interval"])
  
            self.sniff_attempt = copy.deepcopy(json_cmd_string[key]["sniff_attempt"])
  
            self.sniff_timeout = copy.deepcopy(json_cmd_string[key]["sniff_timeout"])
                        
        return self.linearize(args)  
##command : exit_sniff_mode
class exit_sniff_mode_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 2
        self.ocf = 4
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_exit_sniff_mode(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  
##command : qos_setup
class qos_setup_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 2
        self.ocf = 7
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.unused = kwargs.get('unused', int(0))
        self.service_type = kwargs.get('service_type', int(0))
        self.token_rate = kwargs.get('token_rate', int(0))
        self.peak_bandwidth = kwargs.get('peak_bandwidth', int(0))
        self.latency = kwargs.get('latency', int(0))
        self.delay_variation = kwargs.get('delay_variation', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 20
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.unused , 8)
        linear_buffer += padded_bin(self.service_type , 8)
        linear_buffer += padded_bin(self.token_rate , 32)
        linear_buffer += padded_bin(self.peak_bandwidth , 32)
        linear_buffer += padded_bin(self.latency , 32)
        linear_buffer += padded_bin(self.delay_variation , 32)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_qos_setup(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.unused = copy.deepcopy(json_cmd_string[key]["unused"])
  
            self.service_type = copy.deepcopy(json_cmd_string[key]["service_type"])
  
            self.token_rate = copy.deepcopy(json_cmd_string[key]["token_rate"])
  
            self.peak_bandwidth = copy.deepcopy(json_cmd_string[key]["peak_bandwidth"])
  
            self.latency = copy.deepcopy(json_cmd_string[key]["latency"])
  
            self.delay_variation = copy.deepcopy(json_cmd_string[key]["delay_variation"])
                        
        return self.linearize(args)  
##command : role_discovery
class role_discovery_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 2
        self.ocf = 9
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_role_discovery(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  
##command : switch_role
class switch_role_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 2
        self.ocf = 11
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.role = kwargs.get('role', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 7
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.role , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_switch_role(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.role = copy.deepcopy(json_cmd_string[key]["role"])
                        
        return self.linearize(args)  
##command : read_link_policy_settings
class read_link_policy_settings_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 2
        self.ocf = 12
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_link_policy_settings(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  
##command : write_link_policy_settings
class write_link_policy_settings_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 2
        self.ocf = 13
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.link_policy_settings = kwargs.get('link_policy_settings', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 4
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.link_policy_settings , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_link_policy_settings(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.link_policy_settings = copy.deepcopy(json_cmd_string[key]["link_policy_settings"])
                        
        return self.linearize(args)  

class read_default_link_policy_settings_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 2
        self.ocf = 14
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_default_link_policy_settings(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_default_link_policy_settings
class write_default_link_policy_settings_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 2
        self.ocf = 15
        self.default_link_policy_settings = kwargs.get('default_link_policy_settings', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.default_link_policy_settings , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_default_link_policy_settings(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.default_link_policy_settings = copy.deepcopy(json_cmd_string[key]["default_link_policy_settings"])
                        
        return self.linearize(args)  
##command : sniff_subrating
class sniff_subrating_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 2
        self.ocf = 18
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.max_latency = kwargs.get('max_latency', int(0))
        self.min_remote_timeout = kwargs.get('min_remote_timeout', int(0))
        self.min_local_timeout = kwargs.get('min_local_timeout', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 8
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.max_latency , 16)
        linear_buffer += padded_bin(self.min_remote_timeout , 16)
        linear_buffer += padded_bin(self.min_local_timeout , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_sniff_subrating(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.max_latency = copy.deepcopy(json_cmd_string[key]["max_latency"])
  
            self.min_remote_timeout = copy.deepcopy(json_cmd_string[key]["min_remote_timeout"])
  
            self.min_local_timeout = copy.deepcopy(json_cmd_string[key]["min_local_timeout"])
                        
        return self.linearize(args)  
##command : set_event_mask
class set_event_mask_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 1
        self.event_mask = kwargs.get('event_mask', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 8
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.event_mask , 64)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_set_event_mask(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.event_mask = copy.deepcopy(json_cmd_string[key]["event_mask"])
                        
        return self.linearize(args)  

class reset_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 3
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_reset(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : set_event_filter
class set_event_filter_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 5
        self.filter_type = kwargs.get('filter_type', int(0))
        self.filter_condition_type = kwargs.get('filter_condition_type', int(0))
        self.condition = kwargs.get('condition', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 3
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.filter_type , 8)
        linear_buffer += padded_bin(self.filter_condition_type , 8)
        linear_buffer += padded_bin(self.condition , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_set_event_filter(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.filter_type = copy.deepcopy(json_cmd_string[key]["filter_type"])
  
            self.filter_condition_type = copy.deepcopy(json_cmd_string[key]["filter_condition_type"])
  
            self.condition = copy.deepcopy(json_cmd_string[key]["condition"])
                        
        return self.linearize(args)  

class read_pin_type_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 9
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_pin_type(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_pin_type
class write_pin_type_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 10
        self.pin_type = kwargs.get('pin_type', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.pin_type , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_pin_type(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.pin_type = copy.deepcopy(json_cmd_string[key]["pin_type"])
                        
        return self.linearize(args)  
##command : read_stored_link_key
class read_stored_link_key_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 13
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.read_all_flag = kwargs.get('read_all_flag', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 7
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.read_all_flag , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_stored_link_key(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.read_all_flag = copy.deepcopy(json_cmd_string[key]["read_all_flag"])
                        
        return self.linearize(args)  
##command : write_stored_link_key
class write_stored_link_key_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 17
        self.num_keys_to_write = kwargs.get('num_keys_to_write', int(0))
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.link_key = kwargs.get('link_key', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 23
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.num_keys_to_write , 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.link_key , 128)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_stored_link_key(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.num_keys_to_write = copy.deepcopy(json_cmd_string[key]["num_keys_to_write"])
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.link_key = copy.deepcopy(json_cmd_string[key]["link_key"])
                        
        return self.linearize(args)  
##command : delete_stored_link_key
class delete_stored_link_key_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 18
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.delete_all_flag = kwargs.get('delete_all_flag', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 7
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.delete_all_flag , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_delete_stored_link_key(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.delete_all_flag = copy.deepcopy(json_cmd_string[key]["delete_all_flag"])
                        
        return self.linearize(args)  
##command : write_local_name
class write_local_name_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 19
        self.local_name = kwargs.get('local_name', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 0
        self.payload_len += len(self.local_name)
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.local_name , len(self.local_name)*8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_local_name(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.local_name = copy.deepcopy(json_cmd_string[key]["local_name"])
                        
        return self.linearize(args)  
##command : read_local_name
class read_local_name_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 20
        self.sampledata = kwargs.get('sampledata', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.sampledata , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_local_name(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.sampledata = copy.deepcopy(json_cmd_string[key]["sampledata"])
                        
        return self.linearize(args)  

class read_connection_accept_timeout_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 21
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_connection_accept_timeout(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_connection_accept_timeout
class write_connection_accept_timeout_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 22
        self.conn_accept_timeout = kwargs.get('conn_accept_timeout', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.conn_accept_timeout , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_connection_accept_timeout(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.conn_accept_timeout = copy.deepcopy(json_cmd_string[key]["conn_accept_timeout"])
                        
        return self.linearize(args)  

class read_page_timeout_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 23
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_page_timeout(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_page_timeout
class write_page_timeout_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 24
        self.page_timeout = kwargs.get('page_timeout', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.page_timeout , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_page_timeout(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.page_timeout = copy.deepcopy(json_cmd_string[key]["page_timeout"])
                        
        return self.linearize(args)  

class read_scan_enable_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 25
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_scan_enable(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_scan_enable
class write_scan_enable_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 26
        self.scan_enable = kwargs.get('scan_enable', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.scan_enable , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_scan_enable(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.scan_enable = copy.deepcopy(json_cmd_string[key]["scan_enable"])
                        
        return self.linearize(args)  

class read_page_scan_activity_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 27
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_page_scan_activity(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_page_scan_activity
class write_page_scan_activity_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 28
        self.page_scan_interval = kwargs.get('page_scan_interval', int(0))
        self.page_scan_window = kwargs.get('page_scan_window', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 4
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.page_scan_interval , 16)
        linear_buffer += padded_bin(self.page_scan_window , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_page_scan_activity(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.page_scan_interval = copy.deepcopy(json_cmd_string[key]["page_scan_interval"])
  
            self.page_scan_window = copy.deepcopy(json_cmd_string[key]["page_scan_window"])
                        
        return self.linearize(args)  

class read_inquiry_scan_activity_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 29
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_inquiry_scan_activity(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_inquiry_scan_activity
class write_inquiry_scan_activity_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 30
        self.inquiry_scan_interval = kwargs.get('inquiry_scan_interval', int(0))
        self.inquiry_scan_window = kwargs.get('inquiry_scan_window', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 4
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.inquiry_scan_interval , 16)
        linear_buffer += padded_bin(self.inquiry_scan_window , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_inquiry_scan_activity(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.inquiry_scan_interval = copy.deepcopy(json_cmd_string[key]["inquiry_scan_interval"])
  
            self.inquiry_scan_window = copy.deepcopy(json_cmd_string[key]["inquiry_scan_window"])
                        
        return self.linearize(args)  

class read_authentication_enable_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 31
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_authentication_enable(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_authentication_enable
class write_authentication_enable_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 32
        self.authentication_enable = kwargs.get('authentication_enable', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.authentication_enable , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_authentication_enable(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.authentication_enable = copy.deepcopy(json_cmd_string[key]["authentication_enable"])
                        
        return self.linearize(args)  

class read_class_of_device_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 35
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_class_of_device(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_class_of_device
class write_class_of_device_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 36
        self.class_of_device = kwargs.get('class_of_device', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 3
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.class_of_device , 24)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_class_of_device(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.class_of_device = copy.deepcopy(json_cmd_string[key]["class_of_device"])
                        
        return self.linearize(args)  

class read_voice_setting_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 37
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_voice_setting(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_voice_setting
class write_voice_setting_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 38
        self.voice_setting = kwargs.get('voice_setting', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.voice_setting , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_voice_setting(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.voice_setting = copy.deepcopy(json_cmd_string[key]["voice_setting"])
                        
        return self.linearize(args)  
##command : read_automatic_flush_timeout
class read_automatic_flush_timeout_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 39
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_automatic_flush_timeout(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  
##command : write_automatic_flush_timeout
class write_automatic_flush_timeout_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 40
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.flush_timeout = kwargs.get('flush_timeout', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 4
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.flush_timeout , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_automatic_flush_timeout(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.flush_timeout = copy.deepcopy(json_cmd_string[key]["flush_timeout"])
                        
        return self.linearize(args)  
##command : read_transmit_power_level
class read_transmit_power_level_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 45
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.type = kwargs.get('type', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 3
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.type , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_transmit_power_level(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.type = copy.deepcopy(json_cmd_string[key]["type"])
                        
        return self.linearize(args)  
##command : read_link_supervision_timeout
class read_link_supervision_timeout_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 54
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_link_supervision_timeout(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  
##command : write_link_supervision_timeout
class write_link_supervision_timeout_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 55
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.link_supervision_timeout = kwargs.get('link_supervision_timeout', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 4
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.link_supervision_timeout , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_link_supervision_timeout(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.link_supervision_timeout = copy.deepcopy(json_cmd_string[key]["link_supervision_timeout"])
                        
        return self.linearize(args)  

class read_number_of_supported_iac_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 56
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_number_of_supported_iac(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  

class read_current_iac_lap_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 57
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_current_iac_lap(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_current_iac_lap
class write_current_iac_lap_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 58
        self.num_current_iac = kwargs.get('num_current_iac', int(0))
        self.lap = kwargs.get('lap', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 0
        self.payload_len += 1
        self.payload_len += 3
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.num_current_iac , 8)
        linear_buffer += padded_bin(self.lap , 24)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_current_iac_lap(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.num_current_iac = copy.deepcopy(json_cmd_string[key]["num_current_iac"])
  
            self.lap = copy.deepcopy(json_cmd_string[key]["lap"])
                        
        return self.linearize(args)  
##command : set_afh_host_channel_classification
class set_afh_host_channel_classification_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 63
        self.channel_map = kwargs.get('channel_map', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 10
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.channel_map , 80)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_set_afh_host_channel_classification(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.channel_map = copy.deepcopy(json_cmd_string[key]["channel_map"])
                        
        return self.linearize(args)  

class read_inquiry_scan_type_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 66
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_inquiry_scan_type(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_inquiry_scan_type
class write_inquiry_scan_type_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 67
        self.inquiry_scan_type = kwargs.get('inquiry_scan_type', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.inquiry_scan_type , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_inquiry_scan_type(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.inquiry_scan_type = copy.deepcopy(json_cmd_string[key]["inquiry_scan_type"])
                        
        return self.linearize(args)  

class read_inquiry_mode_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 68
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_inquiry_mode(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_inquiry_mode
class write_inquiry_mode_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 69
        self.inquiry_mode = kwargs.get('inquiry_mode', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.inquiry_mode , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_inquiry_mode(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.inquiry_mode = copy.deepcopy(json_cmd_string[key]["inquiry_mode"])
                        
        return self.linearize(args)  

class read_page_scan_type_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 70
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_page_scan_type(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_page_scan_type
class write_page_scan_type_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 71
        self.page_scan_type = kwargs.get('page_scan_type', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.page_scan_type , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_page_scan_type(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.page_scan_type = copy.deepcopy(json_cmd_string[key]["page_scan_type"])
                        
        return self.linearize(args)  

class read_afh_channel_assessment_mode_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 72
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_afh_channel_assessment_mode(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_afh_channel_assessment_mode
class write_afh_channel_assessment_mode_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 73
        self.afh_channel_assessment_mode = kwargs.get('afh_channel_assessment_mode', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.afh_channel_assessment_mode , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_afh_channel_assessment_mode(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.afh_channel_assessment_mode = copy.deepcopy(json_cmd_string[key]["afh_channel_assessment_mode"])
                        
        return self.linearize(args)  

class read_extended_inquiry_response_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 81
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_extended_inquiry_response(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_extended_inquiry_response
class write_extended_inquiry_response_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 82
        self.fec_required = kwargs.get('fec_required', int(0))
        self.extended_inquiry_response = kwargs.get('extended_inquiry_response', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 241
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.fec_required , 8)
        linear_buffer += padded_bin(self.extended_inquiry_response , 1920)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_extended_inquiry_response(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.fec_required = copy.deepcopy(json_cmd_string[key]["fec_required"])
  
            self.extended_inquiry_response = copy.deepcopy(json_cmd_string[key]["extended_inquiry_response"])
                        
        return self.linearize(args)  
##command : refresh_encryption_key
class refresh_encryption_key_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 83
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_refresh_encryption_key(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  

class read_simple_pairing_mode_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 85
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_simple_pairing_mode(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_simple_pairing_mode
class write_simple_pairing_mode_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 86
        self.simple_pairing_mode = kwargs.get('simple_pairing_mode', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.simple_pairing_mode , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_simple_pairing_mode(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.simple_pairing_mode = copy.deepcopy(json_cmd_string[key]["simple_pairing_mode"])
                        
        return self.linearize(args)  

class read_local_oob_data_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 87
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_local_oob_data(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  

class read_inquiry_response_transmit_power_level_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 88
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_inquiry_response_transmit_power_level(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_inquiry_transmit_power_level
class write_inquiry_transmit_power_level_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 89
        self.tx_power_level = kwargs.get('tx_power_level', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.tx_power_level , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_inquiry_transmit_power_level(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.tx_power_level = copy.deepcopy(json_cmd_string[key]["tx_power_level"])
                        
        return self.linearize(args)  
##command : send_keypress_notification
class send_keypress_notification_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 96
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.notification_type = kwargs.get('notification_type', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 7
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.notification_type , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_send_keypress_notification(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.notification_type = copy.deepcopy(json_cmd_string[key]["notification_type"])
                        
        return self.linearize(args)  
##command : enhanced_flush
class enhanced_flush_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 95
        self.handle = kwargs.get('handle', int(0))
        self.packet_type = kwargs.get('packet_type', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 3
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.handle , 16)
        linear_buffer += padded_bin(self.packet_type , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_enhanced_flush(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.handle = copy.deepcopy(json_cmd_string[key]["handle"])
  
            self.packet_type = copy.deepcopy(json_cmd_string[key]["packet_type"])
                        
        return self.linearize(args)  
##command : read_enhanced_transmit_power_level
class read_enhanced_transmit_power_level_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 104
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.type = kwargs.get('type', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 3
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.type , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_enhanced_transmit_power_level(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.type = copy.deepcopy(json_cmd_string[key]["type"])
                        
        return self.linearize(args)  

class read_secure_connections_host_support_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 121
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_secure_connections_host_support(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_secure_connections_host_support
class write_secure_connections_host_support_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 122
        self.secure_connections_host_support = kwargs.get('secure_connections_host_support', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.secure_connections_host_support , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_secure_connections_host_support(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.secure_connections_host_support = copy.deepcopy(json_cmd_string[key]["secure_connections_host_support"])
                        
        return self.linearize(args)  

class read_extended_page_timeout_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 126
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_extended_page_timeout(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_extended_page_timeout
class write_extended_page_timeout_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 127
        self.extended_page_timeout = kwargs.get('extended_page_timeout', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 4
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.extended_page_timeout , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_extended_page_timeout(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.extended_page_timeout = copy.deepcopy(json_cmd_string[key]["extended_page_timeout"])
                        
        return self.linearize(args)  

class read_extended_inquiry_length_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 128
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_extended_inquiry_length(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_extended_inquiry_length
class write_extended_inquiry_length_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 3
        self.ocf = 129
        self.extended_inquiry_length = kwargs.get('extended_inquiry_length', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.extended_inquiry_length , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_extended_inquiry_length(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.extended_inquiry_length = copy.deepcopy(json_cmd_string[key]["extended_inquiry_length"])
                        
        return self.linearize(args)  
##command : read_local_version_information
class read_local_version_information_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 4
        self.ocf = 1
        self.sampledata = kwargs.get('sampledata', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.sampledata , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_local_version_information(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.sampledata = copy.deepcopy(json_cmd_string[key]["sampledata"])
                        
        return self.linearize(args)  

class read_local_supported_commands_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 4
        self.ocf = 2
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_local_supported_commands(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  

class read_local_supported_features_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 4
        self.ocf = 3
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_local_supported_features(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : read_local_extended_features
class read_local_extended_features_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 4
        self.ocf = 4
        self.page_number = kwargs.get('page_number', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.page_number , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_local_extended_features(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.page_number = copy.deepcopy(json_cmd_string[key]["page_number"])
                        
        return self.linearize(args)  

class read_buffer_size_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 4
        self.ocf = 5
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_buffer_size(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  

class read_bd_addr_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 4
        self.ocf = 9
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_bd_addr(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : read_link_quality
class read_link_quality_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 5
        self.ocf = 3
        self.handle = kwargs.get('handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_link_quality(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.handle = copy.deepcopy(json_cmd_string[key]["handle"])
                        
        return self.linearize(args)  
##command : read_rssi
class read_rssi_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 5
        self.ocf = 5
        self.handle = kwargs.get('handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_rssi(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.handle = copy.deepcopy(json_cmd_string[key]["handle"])
                        
        return self.linearize(args)  
##command : read_afh_channel_map
class read_afh_channel_map_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 5
        self.ocf = 6
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_afh_channel_map(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  
##command : read_clock
class read_clock_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 5
        self.ocf = 7
        self.connection_handle = kwargs.get('connection_handle', int(0))
        self.which_clock = kwargs.get('which_clock', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 3
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        linear_buffer += padded_bin(self.which_clock , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_clock(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
  
            self.which_clock = copy.deepcopy(json_cmd_string[key]["which_clock"])
                        
        return self.linearize(args)  
##command : read_encryption_key_size
class read_encryption_key_size_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 5
        self.ocf = 8
        self.connection_handle = kwargs.get('connection_handle', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connection_handle , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_read_encryption_key_size(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connection_handle = copy.deepcopy(json_cmd_string[key]["connection_handle"])
                        
        return self.linearize(args)  

class enable_device_under_test_mode_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 6
        self.ocf = 3
        
    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1,8) + padded_ocf[2:10] + padded_bin(self.ogf,6) + padded_ocf[0:2]
        linear_buffer += padded_bin(0, 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_enable_device_under_test_mode(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
        return self.linearize(args)  
##command : write_simple_pairing_debug_mode
class write_simple_pairing_debug_mode_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 6
        self.ocf = 4
        self.simple_pairing_debug_mode = kwargs.get('simple_pairing_debug_mode', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.simple_pairing_debug_mode , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_write_simple_pairing_debug_mode(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.simple_pairing_debug_mode = copy.deepcopy(json_cmd_string[key]["simple_pairing_debug_mode"])
                        
        return self.linearize(args)  
##command : echo
class echo_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 63
        self.ocf = 1
        self.echo_str = kwargs.get('echo_str', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.echo_str , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_echo(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.echo_str = copy.deepcopy(json_cmd_string[key]["echo_str"])
                        
        return self.linearize(args)  
##command : virtual_listen_connection
class virtual_listen_connection_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 63
        self.ocf = 2
        self.connect_port = kwargs.get('connect_port', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 2
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connect_port , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_virtual_listen_connection(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connect_port = copy.deepcopy(json_cmd_string[key]["connect_port"])
                        
        return self.linearize(args)  
##command : virtual_cancel_listen
class virtual_cancel_listen_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 63
        self.ocf = 3
        self.empty_byte = kwargs.get('empty_byte', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.empty_byte , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_virtual_cancel_listen(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.empty_byte = copy.deepcopy(json_cmd_string[key]["empty_byte"])
                        
        return self.linearize(args)  
##command : virtual_connect
class virtual_connect_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 63
        self.ocf = 4
        self.connect_ip = kwargs.get('connect_ip', int(0))
        self.connect_port = kwargs.get('connect_port', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 6
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.connect_ip , 32)
        linear_buffer += padded_bin(self.connect_port , 16)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_virtual_connect(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.connect_ip = copy.deepcopy(json_cmd_string[key]["connect_ip"])
  
            self.connect_port = copy.deepcopy(json_cmd_string[key]["connect_port"])
                        
        return self.linearize(args)  
##command : virtual_cancel_connect
class virtual_cancel_connect_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 63
        self.ocf = 5
        self.empty_byte = kwargs.get('empty_byte', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.empty_byte , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_virtual_cancel_connect(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.empty_byte = copy.deepcopy(json_cmd_string[key]["empty_byte"])
                        
        return self.linearize(args)  
##command : internal_event_trigger_from_lpw
class internal_event_trigger_from_lpw_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 63
        self.ocf = 6
        self.meta_data = kwargs.get('meta_data', int(0))
        self.lpw_evnt_type = kwargs.get('lpw_evnt_type', int(0))
        self.bd_addr = kwargs.get('bd_addr', int(0))
        self.data = kwargs.get('data', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 0
        self.payload_len += 4
        self.payload_len += 1
        self.payload_len += 6
        self.payload_len += len(self.data)
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.meta_data , 32)
        linear_buffer += padded_bin(self.lpw_evnt_type , 8)
        linear_buffer += padded_bin(self.bd_addr , 48)
        linear_buffer += padded_bin(self.data , len(self.data)*8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_internal_event_trigger_from_lpw(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.meta_data = copy.deepcopy(json_cmd_string[key]["meta_data"])
  
            self.lpw_evnt_type = copy.deepcopy(json_cmd_string[key]["lpw_evnt_type"])
  
            self.bd_addr = copy.deepcopy(json_cmd_string[key]["bd_addr"])
  
            self.data = copy.deepcopy(json_cmd_string[key]["data"])
                        
        return self.linearize(args)  
##command : kill_firmware
class kill_firmware_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 63
        self.ocf = 7
        self.empty_byte = kwargs.get('empty_byte', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 1
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.empty_byte , 8)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_kill_firmware(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.empty_byte = copy.deepcopy(json_cmd_string[key]["empty_byte"])
                        
        return self.linearize(args)  
##command : set_local_lmp_features
class set_local_lmp_features_t():
    def __init__(self, **kwargs):
        self.cmd_type = int(1)
        self.ogf = 63
        self.ocf = 8
        self.local_features = kwargs.get('local_features', int(0))

    def prep_hci(self):
        write_bytes = self.linearize()
        return write_bytes
        pass
    
    def linearize(self):
        logger = logging.getLogger()
        padded_ocf = padded_bin(self.ocf, 10)
        linear_buffer = padded_bin(1, 8) + padded_ocf[2:10] + padded_bin(self.ogf, 6) + padded_ocf[0:2]
        self.payload_len = 8
        linear_buffer += padded_bin(self.payload_len, 8)
        linear_buffer += padded_bin(self.local_features , 64)
        #logger.info(f"linear_buffer --> {linear_buffer}")
        #logger.info(f"linear_buffer --> {linear_buffer}")
        chunks = [linear_buffer[i:i+8] for i in range(0, len(linear_buffer), 8)]
        #logger.info(f"chunks --> {chunks}")
        hex_array = [int(chunk, 2) for chunk in chunks] #[2:] for chunk in chunks
        return np.array(hex_array, dtype=np.uint8).tobytes()

    def prep_payload_set_local_lmp_features(self, args):
        json_cmd_string = json.loads(args)
        print(f"json_cmd_string --> {json_cmd_string}")
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            self.cmd_type = copy.deepcopy(json_cmd_string[key]["cmd_type"])
            self.ogf = copy.deepcopy(json_cmd_string[key]["OGF"])
            self.ocf = copy.deepcopy(json_cmd_string[key]["OCF"]) 
  
            self.local_features = copy.deepcopy(json_cmd_string[key]["local_features"])
                        
        return self.linearize(args)  
