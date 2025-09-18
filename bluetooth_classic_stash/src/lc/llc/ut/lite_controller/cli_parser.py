import yaml
import os

# ANSI escape codes for colors
RED = '\033[91m'
RESET = '\033[0m'

def process_parameters(params, param_list):
    """Convert parameters to command string based on parameter order."""
    result = []
    param_info = []  # Store parameter name:value pairs
    
    for param_name in param_list:
        value = params[param_name]
        if isinstance(value, list):
            # Handle arrays
            result.extend(str(x) for x in value)
            param_info.append(f"{param_name}:[{','.join(str(x) for x in value)}]")
        else:
            result.append(str(value))
            param_info.append(f"{param_name}:{value}")
            
    return ' '.join(result), ' | '.join(param_info)

def generate_cli_commands(yaml_file, output_file):
    """Generate CLI commands from YAML config and write to output file."""
    # Read current configuration
    with open(yaml_file, 'r') as f:
        current_config = yaml.safe_load(f)
    
    # Check if previous config exists and read it
    previous_config = None
    if os.path.exists('previous_config.yaml'):
        with open('previous_config.yaml', 'r') as f:
            previous_config = yaml.safe_load(f)
    
    # Store both commands and their parameter info
    commands = []
    command_info = []
    changed_commands = []
    changed_command_info = []
    
    for cmd_name, cmd_config in current_config['commands'].items():
        params_str, param_info = process_parameters(cmd_config['parameters'], 
                                                  cmd_config['order'])
        command = f"{cmd_name} {params_str}"
        info = f"# {cmd_name} parameters: {param_info}"
        
        commands.append(command)
        command_info.append(info)
        
        # Check if command parameters changed from previous config
        if previous_config and cmd_name in previous_config['commands']:
            prev_params = previous_config['commands'][cmd_name]['parameters']
            if prev_params != cmd_config['parameters']:
                changed_commands.append(command)
                changed_command_info.append(info)

    # Write all commands to output file
    with open(output_file, 'w') as f:
        f.write(f"{RED}WARNING: Please verify all commands and parameters before using!{RESET}\n\n")
        for cmd, info in zip(commands, command_info):
            f.write(f"{info}\n{cmd}\n\n")
    
    print(f"Generated CLI commands in {output_file}")
    
    # Print warning message in red
    print(f"\n{RED}WARNING: Please verify all commands and parameters before using!{RESET}")
    
    # Print changed commands only if there are changes
    if changed_commands:
        print("\nChanged commands:")
        for cmd, info in zip(changed_commands, changed_command_info):
            print(info)
            print(cmd)
            print()
    elif previous_config:
        print("\nNo parameter changes detected.")
    else:
        print("\nInitial configuration created. Run again after making changes to see updates.")
    
    # Save current config for next comparison
    with open('previous_config.yaml', 'w') as f:
        yaml.dump(current_config, f)

if __name__ == "__main__":
    yaml_file = "cli_config.yaml"
    output_file = "cli_commands.txt"
    generate_cli_commands(yaml_file, output_file)