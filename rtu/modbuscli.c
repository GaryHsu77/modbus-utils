#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
 #include <signal.h>
#include <modbus.h>
 
#define MODBUS_SERVER_IP            "10.144.49.109"
#define MODBUS_SERVER_PORT          502
#define MODBUS_DEVICE_ID            1
#define MODBUS_TIMEOUT_SEC          3
#define MODBUS_TIMEOUT_USEC         0
#define MODBUS_DEBUG                ON
 
#define MODBUS_DISCRETE_ADDR        0
#define MODBUS_DISCRETE_LEN         32
#define MODBUS_COIL_ADDR            0
#define MODBUS_COIL_LEN             32
#define MODUBS_INPUT_ADDR           0
#define MODUBS_INPUT_LEN            2
#define MODUBS_HOLDING_ADDR         0
#define MODUBS_HOLDING_LEN          64

int count = 0;

void handler()
{
    
    printf("%d\n", count);
    alarm(1);
    count = 0;
}

int main(int argc, char *argv[])
{
    modbus_t        *ctx;
    struct timeval  timeout;
    int             ret, ii;
    uint8_t         bits[MODBUS_MAX_READ_BITS] = {0};
    uint16_t        regs[MODBUS_MAX_READ_REGISTERS] = {0};
 
    ctx = modbus_new_rtu("/dev/ttyM0",
            115200,
            'N',
            8,
            1);
 
    /* set device ID */
    modbus_set_slave(ctx, MODBUS_DEVICE_ID);
 
    /* Debug mode */
    //modbus_set_debug(ctx, MODBUS_DEBUG);
 
    /* set timeout */
    timeout.tv_sec = MODBUS_TIMEOUT_SEC;
    timeout.tv_usec = MODBUS_TIMEOUT_USEC;
    modbus_get_byte_timeout(ctx, (uint32_t*)&(timeout.tv_sec), (uint32_t*)&(timeout.tv_usec));
 
    timeout.tv_sec = MODBUS_TIMEOUT_SEC;
    timeout.tv_usec = MODBUS_TIMEOUT_USEC;
    modbus_set_response_timeout(ctx, MODBUS_TIMEOUT_SEC, MODBUS_TIMEOUT_USEC);
 
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connexion failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        exit(-1);
    }

    signal(SIGALRM, handler);
    alarm(1);

    int lastv = 0;
    while (1)
    {
        /* read input registers (0x04 function code) */
        ret = modbus_read_input_registers(ctx,
                MODUBS_INPUT_ADDR, MODUBS_INPUT_LEN, regs);
        if (ret < 0) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            //break;
        }

        //printf("%d, %d, %d, %d\n", ret, lastv, (int)regs[0], (int)regs[1]);
        if (lastv != (int)regs[1])
        {
            count++;
            lastv = (int)regs[1];
        }
    }
 
    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);
 
    exit(0);
}