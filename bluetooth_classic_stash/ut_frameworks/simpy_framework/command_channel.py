from sim_models import sim_models
import json
from sockets.tcp import tcp_client

class command_channel(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def connect_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        self.channel = tcp_client("localhost", int(command["port"]))
        self.sim_print("try connect to command channel")
        self.channel.connect()
        self.sim_print(f"Command Channel connected to port {command['port']}")
        time = dict()
        time["tsf"] = self.env.now
        self.channel.send(json.dumps(time))
        self.wait_on_command(None)
        return ""

    def send_delay_command(self, args):
        #remove the first word from self.delay_command
        command = " ".join(self.delay_command.split(" ")[1:])
        self.sim_print(f"send_delay_command {command}")
        self.register_timeout(timeout=self.env.now, handler="wait_on_command", args=True)
        return command

    def wait_on_command(self, args):
        if args is not None:
            response = {}
            response["tsf"] = self.env.now
            response["data"] = "OK"
            self.channel.send(f"{json.dumps(response)}")
        self.sim_print("waiting on command")
        self.delay_command =  self.channel.receive()
        self.sim_print(f"received command {self.delay_command}")
        cmds = self.delay_command.split(" ")
        timeout = int(cmds[0])
        self.register_timeout(timeout=timeout, handler="send_delay_command")
        return ""
