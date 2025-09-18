from utf.devices import Device


def create_device(name: str, config_data=None):####create _devices
    return BTC(name, config_data)


class BTC(Device):
    def __init__(self, name: str, config_data):
        super().__init__(name, config_data)
        self.logger.info(f'si353_c created')
        self.logger.info(f"si353_p created")

    # def close():
    #     for i in self.interfaces:
    #         if hasattr(i, 'close'):
    #             i.close()
