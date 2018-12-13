.PHONY: all tcp rtu client clean

#------------------------------------------------------------------
OUTPUT_PATH = ./bin

ARGS = -DVERSION=\"$(VERSION)\"
CFLAGS = 
INCLUDE = 
LDFLAGS = `pkg-config --libs --cflags libmodbus`
#------------------------------------------------------------------

all: tcp rtu

tcp:
	mkdir -p $(OUTPUT_PATH)
	gcc ./tcp/modbuscli.c -o $(OUTPUT_PATH)/tcpmodbuscli $(LDFLAGS) 

rtu:
	mkdir -p $(OUTPUT_PATH)
	gcc ./rtu/modbuscli.c -o $(OUTPUT_PATH)/rtumodbuscli $(LDFLAGS) 

client:
	mkdir -p $(OUTPUT_PATH)
	gcc ./master/client.c -o $(OUTPUT_PATH)/rtucli $(LDFLAGS) 

clean:
	rm -rf $(OUTPUT_PATH)