try:	
	import yaml
	from jinja2 import Environment, FileSystemLoader, Template
	import argparse
	import sys 
	import os 
	import json
	import re
except ModuleNotFoundError as e:
	print("Package import failed :", e)
	sys.exit()


'''
	This function finds the key within a nested dictionary.
	Function takes the dictionary and key_to_be_found as arguments
	return a key, value(could be of any type) pair
	returns None if a key is not present
'''
def finding_nested_key(yaml_dict, key_to_be_found):
	if(type(yaml_dict) is dict):
		for key, value in yaml_dict.items():
			if(key == key_to_be_found):
				yield (key, value)
				break
			else:
				yield from finding_nested_key(yaml_dict[key], key_to_be_found)

# '''
# 	This function receives the yaml dictionary as input and parse the
# 	parameters and create a linear buffer
# '''
# def linearize_(yaml_dict):

def type_check(var, var_type):
	return str(var.__class__.__name__)==var_type

def set_env_vars(yaml_dict):
	includes = yaml_dict['includes']
	include_paths_list = []
	path_matcher = re.compile(r"\$\{([^}^{]+)\}")
	for paths in includes:
		var = path_matcher.match(paths)
		if var:
			env_var = var.group()[2:-1]
			abs_path = os.getenv(env_var)
			paths = re.sub(r"\$\{([^}^{]+)\}", abs_path, paths)
			include_paths_list.append(paths)
		else:
			include_paths_list.append(paths)
	yaml_dict['includes'] = include_paths_list
	return yaml_dict

def raise_helper(msg):
	raise Exception(msg)
if __name__=="__main__":
	parser = argparse.ArgumentParser(description = "Takes xml/yaml file as input produces output against jinja2 template")
	file_group = parser.add_mutually_exclusive_group()
	file_group.add_argument('-x', help='xml_file')
	file_group.add_argument('-y', help='yaml_file')
	parser.add_argument('-t', help='Jinja Template file')
	parser.add_argument('-o', help='output file')
	args = parser.parse_args()
	try :
		path = (args.t).split("/")
		template_file = path[len(path)-1]
		path = "/".join((path[0:len(path)-1]))
		file_loader = FileSystemLoader(path)
		env = Environment(loader=file_loader, lstrip_blocks=True, trim_blocks=True)
		env.globals['raise'] = raise_helper
		env.globals['type_check'] = type_check
		env.add_extension('jinja2.ext.do')
	except:
		print("No template file specified")
		sys.exit()
	if(args.y != None):
		try:
			#Read yaml file, keep attempting while file is not found
			yaml_output = None
			while yaml_output is None:
				with open(args.y, 'r') as file:
					yaml_output = yaml.safe_load(file)
		except Exception as e:
			print(f"File not found : {e}", args.y)
			sys.exit()
		else:
			if "includes" in yaml_output.keys():
				yaml_output = set_env_vars(yaml_output)
				includes = yaml_output["includes"]
				for include in includes:
					with open(include, 'r') as file:
						secondary_yaml_output = yaml.safe_load(file)
					'''
						common_key in yaml file gives on which key we need to combine two yaml files
						if common_key is not present merging of two yaml files logic proceeds with generic key to value mapping 
					'''
					if "common_key" in yaml_output.keys():
						#Search for dictionary associated with the key in parent yaml
						for key, value in finding_nested_key(secondary_yaml_output, yaml_output['common_key']):
							common_dict = value
						'''
						Search for dictionary associated with the key in child yaml and 
						append the parent yaml details to child yaml to generate 
						merged dictionary
						'''
						for key, value in finding_nested_key(yaml_output, yaml_output['common_key']):
							value.update(common_dict)

					else:
						for key, value in secondary_yaml_output.items():
							yaml_output[key] = value
			template = env.get_template(template_file)
			output = template.render(yaml_output)
			try:
				with open(args.o, 'w') as file:
					file.write(output)
			except Exception as e:
				print(f"Output file not specified {e}")
	else:
		print("No args passed")
		sys.exit()