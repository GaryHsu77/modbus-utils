#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
 
#include <modbus.h>
 
#define MODBUS_SERIAL_DEV           "/dev/ttyUSB0"
#define MODBUS_SERIAL_BAUDRATE      9600    /* 9600, 38400, 115200, ... */
#define MODBUS_SERIAL_PARITY        'N'     /* 'N', 'E', or 'O' */
#define MODBUS_SERIAL_DATABITS      8       /* 5, 6, 7, or 8 */
#define MODBUS_SERIAL_STOPBITS      1       /* 1 or 2 */
#define MODBUS_DEVICE_ID            1
#define MODBUS_TIMEOUT_SEC          3
#define MODBUS_TIMEOUT_USEC         0
#define MODBUS_DEBUG                ON
#define MODBUS_RO_BITS              32
#define MODBUS_RW_BITS              32
#define MODBUS_RO_REGISTERS         64
#define MODBUS_RW_REGISTERS         64
 
int main(int argc, char *argv[])
{
    modbus_t            *ctx;
    modbus_mapping_t    *data_mapping;
    struct timeval      timeout;
    int                 ret, ii, go = 1;
    uint8_t             query[MODBUS_TCP_MAX_ADU_LENGTH];
    uint8_t             bits[MODBUS_MAX_READ_BITS] = {0};
    uint16_t            regs[MODBUS_MAX_READ_REGISTERS] = {0};

    char *dev_path  = argv[1];
    int  baudrate   = atoi(argv[2]);
    int  parity     = argv[3][0];
    int  mode       = atoi(argv[4]);
    int device_id   = atoi(argv[5]);
    int address     = atoi(argv[6]);
    int qty         = atoi(argv[7]);
    printf("dev_path  : %s\n", dev_path);
    printf("baudrate  : %d\n", baudrate);
    printf("mode      : %d\n", mode);
    printf("device_id : %d\n", device_id);
    printf("address   : %d\n", address);
    printf("qty       : %d\n", qty);

    ctx = modbus_new_rtu(dev_path,
            baudrate,
            parity,
            MODBUS_SERIAL_DATABITS,
            MODBUS_SERIAL_STOPBITS);
 
    if (ctx == NULL) {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        exit(-1);
    }

    /* set slave device ID */
    modbus_set_slave(ctx, device_id);

    /* Debug mode */
    modbus_set_debug(ctx, MODBUS_DEBUG);
 
    /* set timeout */
    // modbus_set_response_timeout(ctx,  MODBUS_TIMEOUT_SEC*1000, (MODBUS_TIMEOUT_SEC*1000%1000)*1000);
    // modbus_set_byte_timeout(ctx, MODBUS_TIMEOUT_SEC*1000/1000, (MODBUS_TIMEOUT_SEC*1000%1000)*1000);
 
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connexion failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        exit(-1);
    }

    /* read coils (0x01 function code) */
    if (mode == 1)
    {
        ret = modbus_read_bits(ctx, address, qty, bits);
        if (ret < 0) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
        } else {
            printf("BITS COILS:\n");
            for (ii=0; ii < ret; ii++) {
                printf("[%d]=%d\n", ii, bits[ii]);
            }
        }
    }
    /* read discrete input (0x02 function code) */
    else if (mode == 2)
    {
        ret = modbus_read_input_bits(ctx, address, qty, bits);
        if (ret < 0) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
        } else {
            printf("BITS DISCRETE:\n");
            for (ii=0; ii < ret; ii++) {
                printf("[%d]=%d\n", ii, bits[ii]);
            }
        }
    }
    /* read holding registers (0x03 function code) */
    else if (mode == 3)
    {
        ret = modbus_read_registers(ctx, address, qty, regs);
        if (ret < 0) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
        } else {
            printf("HOLDING REGISTERS:\n");
            for (ii=0; ii < ret; ii++) {
                printf("[%d]=%d\n", ii, regs[ii]);
            }
        }
    }
    /* read input registers (0x04 function code) */
    else if (mode == 4)
    {
        ret = modbus_read_input_registers(ctx, address, qty, regs);
        if (ret < 0) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
        } else {
            printf("INPUT REGISTERS:\n");
            for (ii=0; ii < ret; ii++) {
                printf("[%d]=%d\n", ii, regs[ii]);
            }
        }
    }
    /* write single coil (0x05 function code) */
    else if (mode == 5)
    {
        int wv = atoi(argv[7]);
        printf("wv        : %d\n", wv);
        if (wv)
            ret = modbus_write_bit(ctx, address, TRUE);
        else
            ret = modbus_write_bit(ctx, address, FALSE);

        if (ret < 0) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
        }else{
            printf("write success\n");
        }
    }
    /* write single register (0x06 function code) */
    else if (mode == 6)
    {
        int wv = atoi(argv[7]);
        printf("wv        : %d\n", wv);
        ret = modbus_write_register(ctx, address, wv);
        if (ret < 0) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
        }else{
            printf("write success\n");
        }
    }
    
// TODO    /* write multi coil (0x0F function code) */
// TODO    bits[1] = TRUE;
// TODO    bits[2] = FALSE;
// TODO    ret = modbus_write_bits(ctx, 1, 2, &bits[1]);
// TODO    if (ret < 0) {
// TODO        fprintf(stderr, "%s\n", modbus_strerror(errno));
// TODO    }
// TODO 
// TODO    /* write multi registers (0x10 function code) */
// TODO    regs[1] = 0x1234;
// TODO    regs[2] = 0x5678;
// TODO    ret = modbus_write_registers(ctx, 1, 2, &regs[1]);
// TODO    if (ret < 0) {
// TODO        fprintf(stderr, "%s\n", modbus_strerror(errno));
// TODO    }
 
    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);
 
    exit(0);
}