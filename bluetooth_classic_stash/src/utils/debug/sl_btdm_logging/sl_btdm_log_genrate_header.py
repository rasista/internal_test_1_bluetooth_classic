# python file takes a list of yaml files and merge them together to form a single yaml file
# It then calls the genrate_header function to generate the header file from the merged yaml file
#The yaml file also takes the name of the output file as an argument
import argparse
import re
import yaml
import os
import time
import json
import copy

combined_database = {"sl_btdm_log": {}, "sl_btdm_fast_log": {}}
timestr = ""

# create sl_printf_debug_id format
def generate_debug_id_mapping(combined_database, log_type, component, debug_id):
    if combined_database[log_type][component]["debug_ids"][debug_id]["argnum"] == 0:
        new_line = log_type + "_" + debug_id + "(debug_id, component, level, format)"
    elif combined_database[log_type][component]["debug_ids"][debug_id]["argnum"] == 1:
        new_line = log_type + "_" + debug_id + "(debug_id, component, level, format, a)"
    elif combined_database[log_type][component]["debug_ids"][debug_id]["argnum"] == 2:
        new_line = log_type + "_" + debug_id + "(debug_id, component, level, format, a, b)"
    elif combined_database[log_type][component]["debug_ids"][debug_id]["argnum"] == 3:
        new_line = log_type + "_" + debug_id + "(debug_id, component, level, format, a, b, c)"
    return new_line


# create sl_log format
def generate_call_to_core(combined_database, log_type, component, debug_id):
    if combined_database[log_type][component]["debug_ids"][debug_id]["argnum"] == 0:
        sl_log_line = log_type + "_" + "no" + "_args(" + debug_id + "_SIGNATURE(component, level)) "
    elif combined_database[log_type][component]["debug_ids"][debug_id]["argnum"] == 1:
        sl_log_line = (
            log_type + "_"
            + str(combined_database[log_type][component]["debug_ids"][debug_id]["argnum"])
            + "_args("
            + debug_id
            + "_SIGNATURE(component, level), a)"
        )
    elif combined_database[log_type][component]["debug_ids"][debug_id]["argnum"] == 2:
        sl_log_line = (
            log_type + "_"
            + str(combined_database[log_type][component]["debug_ids"][debug_id]["argnum"])
            + "_args("
            + debug_id
            + "_SIGNATURE(component, level), a, b)"
        )
    elif combined_database[log_type][component]["debug_ids"][debug_id]["argnum"] == 3:
        sl_log_line = (
            log_type + "_"
            + str(combined_database[log_type][component]["debug_ids"][debug_id]["argnum"])
            + "_args("
            + debug_id
            + "_SIGNATURE(component, level), a, b, c)"
        )
    return sl_log_line

def generate_header(output_file, debug_config):
# creating a debug_auto_gen.h file and creating enum of all debug id's
    timestr = time.strftime("%Y/%m/%d-%H:%M:%S")
    with open(output_file, "w+") as fp:
        #strip output file name from path and extension
        output_file = os.path.splitext(os.path.basename(output_file))[0]
        #convert output file name to uppercase
        output_file = output_file.upper()
        fp.writelines(
            [
                "//! This is an Auto Generated Header file",
                f"\n//! do not modifiy this file directly",
                f"\n#ifndef __{output_file}_h__",
                f"\n#define __{output_file}_h__",
                f"\n#include \"sl_btdm_logging.h\"",
            ]
        )
        fp.write("\n#define SL_BTDM_LOG_VERSION  \"" + timestr + "\"")
        
        fp.write("\n\ntypedef enum sl_btdm_log_component_e {\n")
        component_id = 0
        for key in debug_config:
            fp.write(f"{key} = {component_id},\n")
            for log_type in combined_database.keys():
                if key in combined_database[log_type].keys():
                    combined_database[log_type][key]["component_id"] = component_id
            component_id += 1
        fp.write("} sl_btdm_log_component_t;\n")

        fp.write("\ntypedef enum sl_btdm_log_ids_e{\n")
        fp.write("\tsl_btdm_log_SIGNAL_LOST_DID,\n")
        string = ""
        debug_id_key = 1
        for log_type in combined_database.keys():
            for component in combined_database[log_type].keys():
                for debug_id in combined_database[log_type][component]["debug_ids"].keys():
                    string += f"\t{debug_id} = {debug_id_key},\n"
                    combined_database[log_type][component]["debug_ids"][debug_id]["debug_id"] = debug_id_key
                    debug_id_key += 1
            string = string.rstrip(",\n")
        fp.write(string)
        fp.write("\n} sl_btdm_log_ids_t;\n")
        
        for log_type in combined_database.keys():
            fp.write(f"\n#define {log_type}(DEBUG_ID, component, level,...) {log_type}_##DEBUG_ID(DEBUG_ID, component, level, ##__VA_ARGS__)\n")
            ldid_bits = "0x7f"
            ldid_shift = "8"
            hdid_bits = "0x3f80"
            hdid_rshift = "7"
            hdid_lshift = "16"
            ext_did_bits = "0xfc000"
            ext_did_rshift = "14"
            ext_did_lshift = "24"
            arg1_start_bit = "30"
            arg2_start_bit = "6"
            arg3_start_bit1 = "15"
            arg3_start_bit2 = "23"
            for component in combined_database[log_type].keys():
                for key in combined_database[log_type][component]["debug_ids"].keys():
                    arg1_bytes = combined_database[log_type][component]["debug_ids"][key]["arg1bytes"]
                    arg2_bytes = combined_database[log_type][component]["debug_ids"][key]["arg2bytes"]
                    arg3_bytes = combined_database[log_type][component]["debug_ids"][key]["arg3bytes"]
                    fp.write(f"\n#define {key}_SIGNATURE(component, level)  (\
({component} & 0x3f) | \
(({key} & {ldid_bits}) << {ldid_shift}) | \
((({key} & {hdid_bits}) >> {hdid_rshift}) << {hdid_lshift}) | \
((({key} & {ext_did_bits}) >> {ext_did_rshift}) << {ext_did_lshift}) | \
({arg1_bytes} << {arg1_start_bit}) | \
({arg2_bytes} << {arg2_start_bit}) | \
(({arg3_bytes} & 0x1) << {arg3_start_bit1}) | \
((({arg3_bytes} & 0x2) >> 1)<< {arg3_start_bit2}))\
 ")
                    fp.write(
                        "\n#define "
                        + generate_debug_id_mapping(combined_database, log_type, component, key)
                        + "\t"
                        + generate_call_to_core(combined_database, log_type, component, key)
                    )
                    fp.write("\n")
        fp.write("\n#endif")

# API 

#The API takes a file name as input. The file comprise of list of yaml files in yaml format
#the API then reads all the file and merge them together to form a single database
'''
	This function finds the key within a nested dictionary.
	Function takes the dictionary and key_to_be_found as arguments
	return a key, value(could be of any type) pair
	returns None if a key is not present
'''
def merge_yamls(list_of_yamls):
    temp_database = {}
    for yaml_file in list_of_yamls:
        with open(yaml_file, 'r') as stream:
            try:
                database = yaml.safe_load(stream)
                for log_type in database.keys():
                    if log_type in temp_database.keys():
                        for component in database[log_type].keys():
                            if component in temp_database[log_type].keys():
                                for debug_id in database[log_type][component]["debug_ids"].keys():
                                    temp_database[log_type][component]["debug_ids"][debug_id] = copy.deepcopy(database[log_type][component]["debug_ids"][debug_id])
                            else:
                                temp_database[log_type][component] = copy.deepcopy(database[log_type][component])
                    else:
                        temp_database[log_type] = copy.deepcopy(database[log_type])
            except yaml.YAMLError as exc:
                print(exc)
    global combined_database 
    combined_database = copy.deepcopy(temp_database)
    

if __name__=="__main__":
    parser = argparse.ArgumentParser(description="takes a list of yaml files with logging infomation and generates the header file")
    parser.add_argument("-i", help="input yaml file comprisng of list of yaml files")
    parser.add_argument("-d", help="input yaml file comprisng of possible debug configurations")
    parser.add_argument("-c", help="debug configuration to be used from the debug yaml file")
    parser.add_argument("-o", help="output file name")
    args = parser.parse_args()
    input_file = args.i
    output_file = args.o
    with open(args.d, 'r') as stream:
        try:
            debug_database = yaml.safe_load(stream)
        except yaml.YAMLError as exc:
            print(exc)
    with open(input_file, 'r') as stream:
        try:
            list_of_yamls=yaml.safe_load(stream)
            merge_yamls(list_of_yamls)
            generate_header(output_file, debug_database[args.c])
        except yaml.YAMLError as exc:
            print(exc)
    # JSON Dump
    combined_database["SL_LOG_VERSION"] = timestr
    #get path of the output file
    json_path = os.path.dirname(output_file)
    manifest = {}
    for log_type in combined_database.keys():
        if isinstance(combined_database[log_type], dict):
            for component in combined_database[log_type].keys():
                manifest[component] = dict()
                manifest[component]["component_id"] = combined_database[log_type][component]["component_id"]
                manifest[component]["debug_ids"] = dict()
                for debug_id in combined_database[log_type][component]["debug_ids"].keys():
                    debug_id_int = combined_database[log_type][component]["debug_ids"][debug_id]["debug_id"]
                    manifest[component]["debug_ids"][debug_id_int] = combined_database[log_type][component]["debug_ids"][debug_id]
                    manifest[component]["debug_ids"][debug_id_int]["debug_id"] = debug_id
    with open(os.path.join(json_path, "manifest.json"), "w+") as fp:
        json.dump(manifest, fp, indent=4)
