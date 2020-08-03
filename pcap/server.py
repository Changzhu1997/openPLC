#!/usr/bin/env python3

from threading import Thread

from subprocess import Popen, PIPE

from pymodbus.server.sync import StartTcpServer
from pymodbus.device import ModbusDeviceIdentification
from pymodbus.datastore import ModbusSparseDataBlock
from pymodbus.datastore import ModbusSlaveContext, ModbusServerContext

import logging

FORMAT = ('%(asctime)-15s %(threadName)-15s'
          ' %(levelname)-8s %(module)-15s:%(lineno)-8s %(message)s')


class Server(Thread):
    def __init__(self, block):
        super().__init__()

        self.identity = ModbusDeviceIdentification()
        self.identity.VendorName = 'Pymodbus'
        self.identity.ProductCode = 'PM'
        self.identity.VendorUrl = 'http://github.com/bashwork/pymodbus/'
        self.identity.ProductName = 'Pymodbus Server'
        self.identity.ModelName = 'Pymodbus Server'
        self.identity.MajorMinorRevision = '2.3.0'

        self.store = ModbusSlaveContext(ir=block, zero_mode=True)
        self.context = ModbusServerContext(slaves=self.store, single=True)

    def run(self):
        StartTcpServer(context=self.context, identity=self.identity, address=("10.176.150.92", 5020))


if __name__ == "__main__":
    logging.basicConfig(format=FORMAT, level=logging.DEBUG)

    block = ModbusSparseDataBlock({0x00: 0, 0x01: 1})
    modbus_server = Server(block)
    modbus_server.start()

    while True:
        process = Popen(["sudo", "network/net"], stdout=PIPE, stderr=PIPE)
        out, err = process.communicate()
        print(int(out.decode("utf-8")))
        block.setValues(0, int(out.decode("utf-8")))
