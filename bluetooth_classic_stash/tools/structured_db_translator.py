import argparse
import json
import yaml
import xmltodict

def read_input_file(input_file):
    with open(input_file, 'r') as file:
        return file.read()

def write_output_file(output_file, data):
    with open(output_file, 'w') as file:
        file.write(data)

def convert_json_to_yaml(data):
    return yaml.dump(json.loads(data), default_flow_style=False)

def convert_yaml_to_json(data):
    return json.dumps(yaml.safe_load(data), indent=2)

def convert_xml_to_json(data):
    return json.dumps(xmltodict.parse(data), indent=2)

def convert_json_to_xml(data):
    return xmltodict.unparse(json.loads(data), pretty=True)

def main():
    parser = argparse.ArgumentParser(description="Convert JSON, YAML, or XML files.")
    parser.add_argument("input_file", help="Path to the input file")
    parser.add_argument("output_file", help="Path to the output file")
    parser.add_argument("--input-format", choices=["json", "yaml", "xml"], help="Input file format")
    parser.add_argument("--output-format", choices=["json", "yaml", "xml"], help="Output file format")

    args = parser.parse_args()

    input_format = args.input_format
    output_format = args.output_format

    # If input_format or output_format is not provided, try to infer from file extensions
    if not input_format:
        input_format = args.input_file.split('.')[-1]
    if not output_format:
        output_format = args.output_file.split('.')[-1]

    input_data = read_input_file(args.input_file)

    if input_format == 'json' and output_format == 'yaml':
        output_data = convert_json_to_yaml(input_data)
    elif input_format == 'yaml' and output_format == 'json':
        output_data = convert_yaml_to_json(input_data)
    elif input_format == 'xml' and output_format == 'json':
        output_data = convert_xml_to_json(input_data)
    elif input_format == 'json' and output_format == 'xml':
        output_data = convert_json_to_xml(input_data)
    else:
        print("Invalid input/output format combination")
        return

    write_output_file(args.output_file, output_data)
    print(f"File {args.input_file} ({input_format}) has been converted to {output_format} and saved as {args.output_file}")

if __name__ == "__main__":
    main()
