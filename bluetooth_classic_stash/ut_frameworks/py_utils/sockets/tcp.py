#The Module implements a class TCP which provides Methods to send and receive data over TCP.
# The class is used by the models to communicate with the firmware.

import socket
import sys
import os

class tcp_server:
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.port_open = False
        self.tsf = 0

    #function to listen on tcp socket
    def listen(self):
        #wait for just one tcp connection
        self.sock.bind((self.ip, self.port))
        self.sock.listen(1)
        self.conn, self.addr = self.sock.accept()
        self.port_open = True

    #function to send data over tcp connection opened
    def send(self, data):
        try:
            self.conn.sendall(data.encode('UTF-8'))
        except Exception as e:
            print("Error in sending data to Firmware:", e)
            sys.exit(-1)
    #function to receive data over tcp connection opened
    def receive(self):
        data = self.conn.recv(1024)
        data = data.decode('UTF-8')
        return data

    def close(self):
        self.conn.close()
        self.port_open = False
    
class tcp_client:
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.port_open = False
    #function to connect to a tcp socket
    def connect(self):
        #try to connect to the tcp socket, keep trying until connected
        while not self.port_open:
            try:
                self.sock.connect((self.ip, self.port))
            except Exception as e:
                #add delay before retrying
                os.system('sleep 1')
                print("Error in connecting to Firmware:", e)
                continue
            self.port_open = True
            break

    #function to send data over tcp connection opened
    def send(self, data):
        try:
            self.sock.sendall(data.encode('UTF-8'))
        except Exception as e:
            print("Error in sending data to Firmware:", e)
            sys.exit(-1)
    
    #function to receive data over tcp connection opened
    def receive(self):
        data = self.sock.recv(1024)
        data = data.decode('UTF-8')
        return data

    def close(self):
        self.sock.close()
        self.port_open = False