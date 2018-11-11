# Modbus test utils

## install libmodbus env
```
wget http://libmodbus.org/site_media/build/libmodbus-3.0.5.tar.gz
sudo chmod 755 libmodbus-3.0.5.tar.gz
tar zxvf libmodbus-3.0.5.tar.gz -C .
cd libmodbus-3.0.5/
./configure
make
sudo make install
```

## install pymodbus env
```
apt-get install python-pip python-dev
pip install pymodbus
```

## build 
```
make all
ls -l ./bin
```

## run tcp slave
```
python ./modbusemulator/modbusemulator.py --emu-type tcp --slave-id 1 --host 127.0.0.1 --port 55688
```

## run modbus tcp master test
```
./bin/tcpmodbuscli
```

## run modbus rtu master test
```
./bin/rtumodbuscli
```