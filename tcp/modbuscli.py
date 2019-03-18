from pymodbus.client.sync import ModbusTcpClient as ModbusClient
import sys
import argparse

IP = '192.168.5.24'
PORT = 502

client = ModbusClient(IP, PORT)

parser = argparse.ArgumentParser(description='Modbus Tester.')
parser.add_argument(
    '--fun',
    required=True,
    choices=['co', 'di', 'ri', 'hr'],
    help='Fun type.')
parser.add_argument('--adr', type=int, help='Address.')
parser.add_argument('--qty', type=int, help='QUANTITY.')
parser.add_argument('--id', type=int, help='Device id.')
parser.add_argument(
    '--op',
    required=True,
    choices=['r', 'w'],
    help='op type.')
parser.add_argument(
    '--val',
    type=int, help='Wrtite value.')

args = parser.parse_args()


try:
    results = None

    if client.connect() == False:
        print("[ERROR] failed to connect.")
        sys.exit(0)

    if args.op == 'w':
        if args.fun == 'hr':
            rq = client.write_register(args.adr, args.val, unit=args.id)
        elif args.fun == 'co':
            rq = client.write_coil(args.adr, args.val, unit=args.id)
        
        print "Write function return code : " + str(rq.function_code)
    else :
        if args.fun == 'hr':
            rr = client.read_holding_registers(args.adr, args.qty, unit=args.id)
            if rr != None: results = rr.registers
        elif args.fun == 'ri':
            rr = client.read_input_registers(args.adr, args.qty, unit=args.id)
            if rr != None: results = rr.registers
        elif args.fun == 'di':
            rr = client.read_discrete_inputs(args.adr, args.qty, unit=args.id)
            if rr != None: results = rr.bits
        elif args.fun == 'co':
            rr = client.read_coils(args.adr, args.qty, unit=args.id)
            if rr != None: results = rr.bits

        if results is not None:
            print "[SUCCESS] value:"
            print "    " + ",".join(hex(n) for n in results)
        else:
            print("[ERROR]")

    client.close()
except:
    print("Unknown Exception")
    raise
