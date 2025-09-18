import argparse
from json import load as jload
from textwrap import wrap
from sys import argv
import json
import threading

# for testing only
def tostrbits(data):
    ret = ""
    for byte in data:
        #print(byte)
        ret += bin(byte)[2:].zfill(8)
        # ret += bin(int.from_bytes(byte, 'big'))[2:]
    return ret


def b2i(byte):
    return int.from_bytes(byte, "little")


class Decoder:
    """Decoder class"""

    def __init__(self):
        # head indicates current position of state machine
        self.head = 0
        # cumulative tsf
        self.cum_tsf = 0
        # debug data
        self.data = []
        # length of debug data
        self.length = 0
        #manifest file
        self.manifest = dict()
        self.eof = False

    def _hex_to_byte(self, s):
        """Convert one byte of hex(ASCII) to bytes format"""
        return bytes([int(s, 16)])

    def _int_to_byte(self, x):
        """Convert one byte of int to bytes format"""
        return bytes([x])

    def load_file_json(self, path):
        """Load a JSON file (manifest)"""
        with open(path) as file:
            #append content of JSON file to self.json, do not overrwrite
            manifest_content = json.load(file)
            self.manifest.update(manifest_content)

    def load_file_bin(self, path):
        """Load binary debug data"""
        with open(path, "rb") as file:
            self.data.extend(list(map(self._int_to_byte, file.read())))
            self.length = 8 * len(self.data)

    def load_pipe_data(self, pipe_data):
        """Load data from a pipe"""
        while True:
            data = pipe_data.read(0)
            if data:
                self.data.extend(list(map(self._int_to_byte, data)))
                self.length = 8 * len(self.data)
            else:
                break

    def load_file_hex(self, path):
        """Load hexadecimal debug data"""
        with open(path, "r") as file:
            self.data.extend(list(map(self._hex_to_byte, file.read().split())))
            self.length = 8 * len(self.data)

    def load_file_srec(self, path):
        """Load SREC debug data"""
        data = []
        with open(path) as file:
            file.readline()
            for line in file.readlines():
                line = wrap(line[9:-3], 2)
                data.extend(line)
        self.data.extend(list(map(self._hex_to_byte, data)))
        self.length = 8 * len(self.data)

    def load_file_ascii(self, path):
        """Load possibly polluted ASCII debug data"""
        data = []
        with open(path) as file:
            for line in file.readlines():
                spline = line.split()
                if spline and spline[0] == "&^$":
                    if spline[-1][0] == '<':
                        spline.pop()
                    data.extend(spline[1:])
        self.data.extend(list(map(self._hex_to_byte, data)))
        self.length = 8 * len(self.data)

    def get_byte(self):
        """Get the next byte from self.data, wait till atleast 1 byte is available"""

        byte_found = False
        while not byte_found:
            h = self.head
            bh = h // 8
            if bh < len(self.data):
                byte_found = True
            else:
                pass

        ret = int.from_bytes(self.data[bh], "little")
        self.head += 8
        return ret

    def _print_remaining(self):
        """
        Prints remaining contents of self.data
        For debugging only
        """

        for byte in self.data[self.head // 8 :]:
            print(hex(b2i(byte))[2:].zfill(2), end=" ")
        print()

    def get_log_version(self):
        """Get the log version of manifest file"""

        return self.json["SL_LOG_VERSION"]

    def data_to_be_read(self):
        """Return true if there is data to be read"""
        return self.head < self.length

    def get_tsf_format(self, byte):
        return (byte >> 6) & 0x1
    
    def get_tsf_present(self, byte):
        return (byte >> 7) & 0x1
    
    def get_hdid_present(self, byte):
        return (byte >> 7) & 0x1
    
    def get_component(self, byte):
        return byte & 0x3f
        
    
    
    def get_did(self, ldid, hdid, e_did):
        LDID_BITS = 7
        HDID_BITS = 7
        return (e_did << (LDID_BITS + HDID_BITS)) | (hdid << LDID_BITS) | ldid

    def get_ldid(self, byte):
        return byte & 0x7F
    
    def get_hdid(self, byte):
        return byte & 0x1F
    
    def get_rfu(self, byte):
        return (byte >> 6) & 0x3
    
    def get_e_did_present(self, byte):
        return (byte >> 7) & 0x1
    
    def get_e_did(self, byte):
        return byte & 0x3F
    
    def read_next_debug_entry(self):
        hdid = 0
        e_did = 0

        debug_byte = self.get_byte()
        tsf_format = self.get_tsf_format(debug_byte)
        tsf_present = self.get_tsf_present(debug_byte)
        component = self.get_component(debug_byte)
        debug_byte = self.get_byte()
        hdid_present = self.get_hdid_present(debug_byte)
        component_valid = 0
        for component_str, component_details in self.manifest.items():
            if component_details["component_id"] == component:
                component_valid = 1
                break
        if component_valid == 0:
            raise RuntimeError(f"Invalid component {component}.")
        else:
            component_details = self.manifest[component_str]
        
        
        ldid = self.get_ldid(debug_byte)
        if hdid_present:
            debug_byte = self.get_byte()
            hdid = self.get_hdid(debug_byte)
            e_did_present = self.get_e_did_present(debug_byte)
            if e_did_present:
                debug_byte = self.get_byte()
                e_did = self.get_e_did(debug_byte)


        did = self.get_did(ldid, hdid, e_did)
        did = str(did)
        did_valid = 0
        for did_int, did_details in component_details["debug_ids"].items():
            if did_int == did:
                did_valid = 1
                break
        if did_valid == 0:
            raise RuntimeError(f"Invalid debug ID {did}.")
        else:
            did_details = component_details["debug_ids"][did_int]
        self.previous_cum_tsf = self.cum_tsf
        if tsf_present: 
            ltsf1 = self.get_byte()
            ltsf2 = self.get_byte()
            ltsf = (ltsf2 << 8) | ltsf1
            if tsf_format:
                htsf1 = self.get_byte()
                htsf2 = self.get_byte()
                htsf = (htsf2 << 8) | htsf1
                tsf = htsf << 16 | ltsf
                self.cum_tsf = tsf
            else:
                self.cum_tsf += ltsf


        #if signal_lost:
            #self._print_remaining()
            #raise RuntimeError("Signal lost.")

        info = did_details
        parsee = info["formatted_string"]
        num_args = info["argnum"]

        for argpos in range(num_args):
            idx = parsee.find("%")
            sub = parsee[idx : idx + 2]
            if sub[-1].isnumeric():
                sub += parsee[idx + 2]

            argtype = info["format_specifiers"][argpos]

            if argtype == "1" and sub in ("%b", "%c", "%1x"):
                data = self.get_byte()
                if sub == "%b":
                    parsee = parsee.replace("%b", str(data), 1)
                elif sub == "%c":
                    parsee = parsee.replace("%c", chr(data), 1)
                elif sub == "%1x":
                    parsee = parsee.replace("%1x", hex(data), 1)

            elif argtype == "2" and sub in ("%2x", "%w"):
                data_l = self.get_byte()
                data_h = self.get_byte()
                data = data_h << 8 | data_l
                if sub == "%2x":
                    parsee = parsee.replace("%2x", hex(data), 1)
                if sub == "%w":
                    parsee = parsee.replace("%w", hex(data), 1)

            elif argtype == "3" and sub in ("%4x", "%d"):
                data_8_l = self.get_byte()
                data_8_h = self.get_byte()
                data_16_l = data_8_h << 8 | data_8_l
                data_8_l = self.get_byte()
                data_8_h = self.get_byte()
                data_16_h = data_8_h << 8 | data_8_l
                data = data_16_h << 16 | data_16_l

                if sub == "%d":
                    parsee = parsee.replace("%d", str(data), 1)
                elif sub == "%4x":
                    parsee = parsee.replace("%4x", hex(data), 1)

            else:
                raise RuntimeError("Format mismatch.")
        debug_id_str = info["debug_id"]
        tsf_delta = self.cum_tsf - self.previous_cum_tsf
        debug_line = f"{str(self.cum_tsf).zfill(6)}\t{str(tsf_delta)}\t{component_str}\t{debug_id_str}\t\t{parsee}"
        return debug_line
def read_data(decoder, debug_data_files, binary_data_files):
    if debug_data_files:
        for debug_datsa_file in debug_data_files:
            decoder.load_file_ascii(debug_datsa_file)

    if binary_data_files:
        for binary_data_file in binary_data_files:
            decoder.load_file_bin(binary_data_file)
    
    decoder.eof = True

def decode_data(decoder):
    while (decoder.data_to_be_read() == True) or (decoder.eof == False):
        print(decoder.read_next_debug_entry())


def main():

    parser = argparse.ArgumentParser(description="takes a manifest file and a list of debug data files and decodes them")
    parser.add_argument("-m", help="manifest file", nargs="+")
    parser.add_argument("-d", help="debug data file", nargs="+")
    parser.add_argument("-b", help="debug data file", nargs="+")
    #option to read from a pipe

    args = parser.parse_args()
    manifest_files = args.m
    debug_data_files = args.d
    binary_data_files = args.b

    decoder = Decoder()
    #merge all manifest files into one. Manifest files are expected to be in JSON format, so load all files in one common dictionary
    for manifest_file in manifest_files:
        decoder.load_file_json(manifest_file)


    #create two separate threads. one for reading data either from ascii or bin files
    #other for reading the read data and decoding it

       # Create threads
    read_thread = threading.Thread(target=read_data, args=(decoder, debug_data_files, binary_data_files))
    decode_thread = threading.Thread(target=decode_data, args=(decoder,))

    # Start threads
    read_thread.start()
    decode_thread.start()

    # Join threads
    read_thread.join()
    decode_thread.join()





if __name__ == "__main__":
    main()
