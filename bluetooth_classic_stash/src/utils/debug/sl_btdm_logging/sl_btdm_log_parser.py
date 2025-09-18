#The Python script opens a target .c file to parse the occurance on sl_btdm_log and sl_btdm_fast_log occurances
#script takes -i as input to specify the target file and -o to specify the output yaml file to write the results

import argparse
import re
import yaml

# obtain number of args
def get_numargs(line):
    #count occurances of "%" in the line
    return line.count("%")

# obtain format type
def get_format(line, filename, c_line):
    format_list = []
    format_bytes = [0, 0, 0]
    index = 0
    for val in range(len(line)):
        if line[val] == "%":
            if line[val + 1] == "b":
                format_list.append("1")
                format_bytes[index] = 1
            elif line[val + 1] == "c":
                format_list.append("1")
                format_bytes[index] = 1
            elif line[val + 1] == "w":
                format_list.append("2")
                format_bytes[index] = 2
            elif line[val + 1] == "d":
                format_list.append("3")
                format_bytes[index] = 3
            elif (line[val + 1] == "1" and line[val + 2] == "x") or (
                line[val + 1] == "0" and line[val + 2] == "1" and line[val + 3] == "x"
            ):
                format_list.append("1")
                format_bytes[index] = 1
            elif (line[val + 1] == "2" and line[val + 2] == "x") or (
                line[val + 1] == "0" and line[val + 2] == "2" and line[val + 3] == "x"
            ):
                format_list.append("2")
                format_bytes[index] = 2
            elif (line[val + 1] == "4" and line[val + 2] == "x") or (
                line[val + 1] == "0" and line[val + 2] == "4" and line[val + 3] == "x"
            ):
                format_list.append("3")
                format_bytes[index] = 3
            else:
                raise RuntimeError(
                    "Error: Wrong Format in - " + c_line + " in file -  " + filename
                )
            index += 1
    return format_list, format_bytes
def process_file_for_function_calls(file_content, token):
    """
    Processes file content line by line to clean up specific function calls.
    Handles cases where function calls span multiple lines, removes inline and block comments,
    and ensures strict token matching.

    Args:
        file_content (list of str): Lines of the file as a list of strings.
        token (str): The function name to search for and clean up.
    
    Returns:
        list of str: List of cleaned-up function calls or empty strings for lines without valid matches.
    """
    cleaned_lines = []
    accumulated_line = ""
    open_bracket_found = False
    in_block_comment = False

    # Escape the token to handle special regex characters
    escaped_token = re.escape(token)
    
    # Regular expression to strictly match the function name and handle multiline arguments
    pattern = rf'\b{escaped_token}\s*\((.*?)\)\s*;'

    for line in file_content:
        # Check for block comment start
        if not in_block_comment:
            # Remove inline comments
            line = re.sub(r'//.*', '', line)
            block_comment_start = line.find("/*")   
            if block_comment_start != -1:
                block_comment_end = line.find("*/", block_comment_start + 2)
                if block_comment_end != -1:
                    # Block comment starts and ends on the same line
                    line = line[:block_comment_start] + line[block_comment_end + 2:]
                else:
                    # Block comment starts but does not end on the same line
                    in_block_comment = True
                    line = line[:block_comment_start]
         
        # If inside a block comment, check for its end
        if in_block_comment:
            block_comment_end = line.find("*/")
            if block_comment_end != -1:
                in_block_comment = False
                line = line[block_comment_end + 2:]  # Resume processing after the block comment
            else:
                # Skip the entire line if the block comment hasn't ended
                continue

        # Accumulate the line
        accumulated_line += line.strip()
        # Check for an open bracket
        if '(' in line:
            open_bracket_found = True
        
        # Check for a closing bracket
        if ')' in line and open_bracket_found:
            # Now we have a complete function call, try to match it
            match = re.search(pattern, accumulated_line, re.DOTALL)
            if match:
                arguments = match.group(1).strip()
                cleaned_lines.append(f"{token}({arguments});")
            
            # Reset the accumulator for the next function
            accumulated_line = ""
            open_bracket_found = False
        if not open_bracket_found:
            accumulated_line = ""

    return cleaned_lines
# Parse argument function takes a cleaned up line and identifies the number of strings between opening and closing brackets
# The first string has to be a single word identified as debug id
# The second string has to be a single word identified as log level
# The third string is a formated string strictly within ""
# remaining arguments are optional(minimum 0, maximum 4) and have to match number % in the formated string
def parse_arguments(filename, line):
    # Split the line into function name and arguments
    function_name, arguments = line.split("(", 1)
    arguments = arguments.rsplit(")", 1)[0]

    # Split the arguments by commas
    args = arguments.split(",")

    # Check for the number of arguments
    num_args = len(args)

    # Check for the debug id
    debug_id = args[0].strip()
    if not re.match(r'^[a-zA-Z_]\w*$', debug_id):
        assert False, f"Invalid debug id: {debug_id}"

    # Check for the log component
    log_component = args[1].strip()
    if not re.match(r'^[a-zA-Z_]\w*$', log_component):
        assert False, f"Invalid log level: {log_level}"

    # Check for the log level
    log_level = args[2].strip()
    if not re.match(r'^[a-zA-Z_]\w*$', log_level):
        assert False, f"Invalid log level: {log_level}"


    # Check for the formatted string
    formatted_string = args[3].strip()
    if not re.match(r'^".*"$', formatted_string):
        assert False, f"Invalid formatted string: {formatted_string}"

    # Check for the number of % in the formatted string
    num_format_specifiers = get_numargs(formatted_string)
    format_specifiers, format_bytes = get_format(formatted_string, filename, line)


    # Check for the number of arguments
    if num_args - 4 != num_format_specifiers:
        assert False, f"Number of arguments does not match the number of format specifiers: {num_args - 4} != {num_format_specifiers}"

    return {
            "component": log_component, 
                    "component_details" : {
                    "debug_id_str" : debug_id ,
                    "debug_id": None,
                    "argnum": num_args - 4,
                    "log_level": log_level,
                    "formatted_string": formatted_string[1:-1],
                    "format_specifiers": format_specifiers,
                    "arg1bytes": format_bytes[0],
                    "arg2bytes": format_bytes[1],
                    "arg3bytes": format_bytes[2],
            }
    }
def parse_file(input_file):
    sl_btdm_logs = {}
    sl_btdm_fast_logs = {}
    with open(input_file, "r", encoding="utf-8", errors="replace") as f:
        file_content = f.readlines()
        sl_btdm_log_lines = process_file_for_function_calls(file_content, "sl_btdm_log")
        sl_btdm_fast_log_lines = process_file_for_function_calls(file_content, "sl_btdm_fast_log")
        for line in sl_btdm_log_lines:
            debug_info = parse_arguments(input_file, line)
            if debug_info["component"] not in sl_btdm_logs:
                sl_btdm_logs[debug_info["component"]] = {"component_id": None, "debug_ids": {}}
            sl_btdm_logs[debug_info["component"]]["debug_ids"][debug_info["component_details"]["debug_id_str"]] = debug_info["component_details"]
        for line in sl_btdm_fast_log_lines:
            debug_info = parse_arguments(input_file, line)
            if debug_info["component"] not in sl_btdm_fast_logs:
                sl_btdm_fast_logs[debug_info["component"]] = {"component_id": None, "debug_ids": {}}
            sl_btdm_fast_logs[debug_info["component"]]["debug_ids"][debug_info["component_details"]["debug_id_str"]] = debug_info["component_details"]
    output_file = input_file + ".logdata.yaml"
    with open(output_file, "w") as f:
        yaml.dump({"sl_btdm_log": sl_btdm_logs, "sl_btdm_fast_log": sl_btdm_fast_logs}, f)

if __name__=="__main__":
    parser = argparse.ArgumentParser(description="Takes a .c file as input and produces a yaml file with the results")
    parser.add_argument("-i", help="input file")
    args = parser.parse_args()
    input_file = args.i
    parse_file(input_file)
