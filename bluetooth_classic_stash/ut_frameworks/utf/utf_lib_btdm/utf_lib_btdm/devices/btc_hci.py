from utf.devices import Device
import logging
logger = logging.getLogger()


def create_device(name: str, config_data=None):
    """
    Called when to create device object of type Si353

    Args:
        name: ``string`` name value to set
        config_data: ``dict`` configuration data object to set the configuration of the device

    Returns:
        Instance of ``si353``

    """
    return BTC(name, config_data)


class BTC(Device):
    def __init__(self, name: str, config_data):
        super().__init__(name, config_data)
        logger.info(f"Creating BTC device with name {name}")