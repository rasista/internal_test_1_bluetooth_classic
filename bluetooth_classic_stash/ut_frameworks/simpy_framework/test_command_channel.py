import pytest
import sim
from models.beacon_receive import beacon_receive
import json
def test_command_channel():
    command_channel = sim.listen_for_command_channel()
    bcn_rcv = beacon_receive(model_name="beacon_receive")
    sync_beacon = {}
    sync_beacon["next_beacon"] = 1000
    sync_beacon["per"] = 1
    sync_beacon["tbtt"] = 100
    sync_beacon["skip_count"] = 9
    bcn_rcv.sync_beacon(channel=command_channel, args=json.dumps(sync_beacon))
    bcn_rcv.sync_beacon(channel=command_channel, args=json.dumps(sync_beacon))
    bcn_rcv.sync_beacon(channel=command_channel, args=json.dumps(sync_beacon))
    bcn_rcv.sync_beacon(channel=command_channel, args=json.dumps(sync_beacon))
    bcn_rcv.sync_beacon(channel=command_channel, args=json.dumps(sync_beacon))
    bcn_rcv.sync_beacon(channel=command_channel, args=json.dumps(sync_beacon))
    bcn_rcv.sync_beacon(channel=command_channel, args=json.dumps(sync_beacon))
    bcn_rcv.sync_beacon(channel=command_channel, args=json.dumps(sync_beacon))
    bcn_rcv.sync_beacon(channel=command_channel, args=json.dumps(sync_beacon))
    