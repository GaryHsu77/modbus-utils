#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import logging

from pymodbus.datastore import ModbusSequentialDataBlock
from pymodbus.datastore import ModbusSlaveContext, ModbusServerContext
from pymodbus.server.sync import ModbusSerialServer, ModbusTcpServer
from pymodbus.transaction import ModbusRtuFramer, ModbusSocketFramer

co_data = [False, False, True]
di_data = [True, False]
ir_data = [
    0xFFFF,
    0x8000,     # int16
    0xFFFF, 0xFFFF,     # uint32
    0xFFFF, 0xFFFF,     # int32
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,     # uint64
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,     # int64
    0x4048, 0xF5C3,     # float32
    0x4009, 0x1EB8, 0x51EB, 0x851F,     # float64
    0x4D6F, 0x6462, 0x7573, 0x456D, 0x7500,     # string
    0x7856, 0x3412      # byte-order (DCBA)
]
hr_data = [
    0x0000,
    0x5678, 0x1234,     # byte-order
    0x0001,         # scaling
    0x0001,         # scaling2
    0x1234, 0x5678, # nv2
    0x1234, 0x5678, 0x9ABC, 0xDEF0,  # bytearray
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF
]

baud_supported_list = [9600, 19200, 38400, 57600, 115200]


def build_server_context():
    store = ModbusSlaveContext(
        di=ModbusSequentialDataBlock(1, di_data),
        co=ModbusSequentialDataBlock(1, co_data),
        hr=ModbusSequentialDataBlock(1, hr_data),
        ir=ModbusSequentialDataBlock(1, ir_data))
    context = ModbusServerContext(
        slaves=store,
        single=True)
    return context


def validate_tcp_args(args):
    if not args.host:
        return False

    if not args.port:
        return False
    if (int(args.port) <= 0) or (int(args.port) > 65535):
        return False

    return True


def run_tcp(args, server_context):
    server = ModbusTcpServer(
        context=server_context,
        framer=ModbusSocketFramer,
        address=(args.host, int(args.port)))
    server.serve_forever()


def validate_rtu_args(args):
    if not args.path:
        return False

    if not args.baud:
        return False

    return True


def run_rtu(args, server_context):
    server = ModbusSerialServer(
        context=server_context,
        framer=ModbusRtuFramer,
        port=args.path,
        baudrate=args.baud,
        timeout=0.005)
    server.serve_forever()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Modbus Emulator.')
    parser.add_argument(
        '--emu-type',
        required=True,
        choices=['tcp', 'rtu'],
        help='Emulate type.')
    parser.add_argument(
        '--slave-id',
        default=0x00,
        help='Modbus device slave ID/unit ID.')

    tcp_group = parser.add_argument_group(
        'Modbus TCP',
        'These options only effect at emu-type=tcp.')
    tcp_group.add_argument('--host', help='Modbus TCP host IP.')
    tcp_group.add_argument(
        '--port',
        type=int,
        help='Modbus TCP port.')

    rtu_group = parser.add_argument_group(
        'Modbus RTU',
        'These options only effect at emu-type=rtu.')
    rtu_group.add_argument('--path', help='Modbus RTU UART port path.')
    rtu_group.add_argument(
        '--baud',
        type=int,
        choices=baud_supported_list,
        help='UART baud rate')

    args = parser.parse_args()

    server_context = build_server_context()

    if args.emu_type == 'tcp':
        if not validate_tcp_args(args):
            parser.print_help()
            exit()
        run_tcp(args, server_context)
    elif args.emu_type == 'rtu':
        if not validate_rtu_args(args):
            parser.print_help()
            exit()
        run_rtu(args, server_context)
    else:
        pass