import socket
import sys

class udp:
    def __init__(self, recv_ip, recv_port, env):
        self.ip = recv_ip
        self.port = recv_port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.env = env

    def sender(self, command):
        sys_tick_command = "system update_sys_tick "+str(self.env.now)
        print(f"{self.env.now} Command Sent: {sys_tick_command}")
        self.sock.sendto(bytes(sys_tick_command, 'UTF-8'), (self.ip, self.port))
        response = self.receiver()
        print(f"{self.env.now} Command Sent: {command}")
        self.sock.sendto(bytes(command, 'UTF-8'), (self.ip, self.port))
        return self.receiver()

    def receiver(self):
        # self.sock.settimeout(10)
        try:
            data, addr = self.sock.recvfrom(1024)
            data = data.decode('UTF-8')
        except Exception as e:
            print("Timedout Response from Firmware:", e)
            sys.exit(-1)
        print(f"{self.env.now} Command Rcvd: {data}","\n")
        if(data == "SimulationEnded"):
            #self.sock.sendto(bytes("system end", 'UTF-8'), (self.ip, self.port))
            sys.exit(1)
        return data
