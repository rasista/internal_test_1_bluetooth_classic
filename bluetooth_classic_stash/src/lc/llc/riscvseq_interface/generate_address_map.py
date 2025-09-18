# python file reads an elf file and generates a C header file with the address map 
# of all symbols in the elf file. The Elf file and the output 
# file name are passed as command line arguments.

import sys
import os
import subprocess
import re
import argparse

def get_section_address_map(elf_file, toolchain_nm=None):
    # Get the address map of all global variables in the given section
    cmd = f"{toolchain_nm} -n {elf_file}"
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    (output, err) = p.communicate()
    p_status = p.wait()
    if p_status != 0:
        print("Error in running command: " + cmd)
        sys.exit(1)
    output = output.decode("utf-8")
    lines = output.split("\n")
    address_map = {}
    for line in lines:
        parts = line.split()
        if len(parts) < 3:
            continue
        address = int(parts[0], 16)
        name = parts[2]
        address_map[name] = address
    return address_map

# API to create a header file with the address map of all symbols in the elf file
# the symbol names are transformed to uppercase and the address map is written as hash defines
def create_address_map_header(elf_file, header_file, toolchain_nm=None):
    address_map = get_section_address_map(elf_file, toolchain_nm)
    #get the ifndef string from file name. ie if file name is test.h, ifndef string is __TEST_H__
    ifndef_str = "__" + os.path.basename(header_file).replace(".", "_").upper() + "__"
    #prefix the symbol names with the file name without extension
    prefix = os.path.basename(elf_file).replace(".", "_").upper()
    with open(header_file, "w") as f:
        f.write(f"#ifndef {ifndef_str}\n")
        f.write(f"#define {ifndef_str}\n")
        f.write("\n")
        for name, address in address_map.items():
            name = name.upper()
            address = format(address, "x")
            f.write(f"#define  {prefix}_{name} 0x{address}UL\n")
        f.write("\n")
        f.write("#endif\n") 

if __name__ == "__main__":
    #use -h to get help, -e <elf_file> -o <header_file> -t <toolchain-nm> to generate the address map
    argparser = argparse.ArgumentParser(description='Generate address map header file from elf file')
    argparser.add_argument('-e', '--elf', help='elf file', required=True)
    argparser.add_argument('-o', '--output', help='output header file', required=True)
    argparser.add_argument('-t', '--toolchain-nm', help='toolchain nm', required=False)
    args = argparser.parse_args()
    create_address_map_header(args.elf, args.output, args.toolchain_nm)
