try:
	import yaml
	import argparse
	import sys
	from enum import Enum
	from HighLambdaHashing import HighLambdaHashing
	import os
except ModuleNotFoundError as e:
	print("Package import failed :", e)
	sys.exit()

		
def raise_helper(msg):
	raise Exception(msg)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(
		description="converts generic yaml file into state machine specific yaml file")
	parser.add_argument('-i', help='generic yaml file')
	parser.add_argument('-o', help='state_machine specfic yaml file')
	parser.add_argument('-b', help='path to source code base directory')
	args = parser.parse_args()
	try:
		path = (args.i).split("/")
		template_file = path[len(path)-1]
		path = "/".join((path[0:len(path)-1]))
	except:
		print("No input file specified")
		sys.exit()
	if(args.i != None):
		try:
			with open(args.i, 'r') as file:
				yaml_output = yaml.safe_load(file)
			if "includes" in yaml_output.keys():
				includes = yaml_output["includes"]
				for include in includes:
					include_file = args.b + "/" + include
					with open(include_file, 'r') as file:
						secondary_yaml_output = yaml.safe_load(file)
						for key, value in secondary_yaml_output.items():
							yaml_output[key] = value
			dict_state_vs_trigger = yaml_output["states"]
			list_states = list(dict_state_vs_trigger.keys())
			list_triggers = yaml_output["triggers"]
			list_triggers.append("procedures_max_num_triggers")
			dict_state_vs_trigger_final = dict()
			list_handlers = list()
			hash_input = dict()
			enum_states = Enum("enum_states", list_states, start=0)
			enum_triggers = Enum("enum_triggers", list_triggers, start=0)
			for state in list_states:
				dict_state_vs_trigger_final[state] = dict()
				for trigger in list_triggers:
					if trigger in dict_state_vs_trigger[state].keys():
						dict_state_vs_trigger_final[state][trigger] = dict_state_vs_trigger[state][trigger]
						#if dict_state_vs_trigger_final[state][trigger] not in list_handlers:
						list_handlers.append(dict_state_vs_trigger_final[state][trigger])
						hash_input[enum_states[state].value*enum_triggers["procedures_max_num_triggers"].value + enum_triggers[trigger].value] = dict_state_vs_trigger_final[state][trigger]
					else:
						dict_state_vs_trigger_final[state][trigger] = "sm_assert"
			list_handlers.append("sm_assert")
			dict_yaml_data = dict()
			dict_yaml_data["sm_name"] = yaml_output["sm_name"]
			dict_yaml_data["state_details_orig"] = dict_state_vs_trigger
			dict_yaml_data["state_details"] = dict_state_vs_trigger_final
			dict_yaml_data["list_states"] = list_states
			dict_yaml_data["list_triggers"] = list_triggers
			dict_yaml_data["list_handlers"] = list_handlers
			hash_table = HighLambdaHashing(hash_input)
			dict_yaml_data["hashed_details"] = {"hashed_entries":hash_table.hash_entries.tolist(), "hashed_entries_bitmap":hash_table.hash_entries_bitmap.tolist(), "hashed_entries_count":hash_table.hash_entries_count.tolist()}
			with open(args.o, 'w') as file:
				yaml.dump(dict_yaml_data, file, default_flow_style=False)
			
		except Exception as e:
			print(f"File not found : {e}", args.i)
			sys.exit()
	else:
		print("No args passed")
		sys.exit()