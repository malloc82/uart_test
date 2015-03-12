#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uart.h"

extern char * optarg;
#define BUF_SIZE 255
#define BAUDRATE B9600

int main(int argc, char *argv[])
{
    char device[BUF_SIZE];
    device[0] = '\0';
    if (argc < 2) {
        fprintf(stderr, "Error: Need to specify a tty device as argv[1].\n");
        exit(EXIT_FAILURE);
    }
    strncpy(device, argv[1], BUF_SIZE);

    unsigned char input_buf = 0x10;
    printf("Input hex string : 0x%02x\n", input_buf);

    int fd = initializePort(device, BAUDRATE);
    write(fd, &input_buf, 1);

    int length, res, i;
    unsigned char return_code = 0xff;
    unsigned short data_length = 0;
    unsigned char * return_data = NULL;

    /* read return code */
    read(fd, &return_code, 1);
    printf(" => Return code = 0x%2x\n", return_code);

    /* determine if there is an error */
    if ((return_code & 0x0F) != 0x00) {
        puts("return code indicates an error has occurred");
        return 0;
    }

    /* read next two bytes for data length */
    for (length = 2, res = 0; length > 0; length -= res) {
        res = read(fd, ((unsigned char *)(&data_length)) + (length - 1), 1);
        printf("res = %d, length = %d\n", res, length);
    }
    printf(" => data_length = %d\n", data_length);

    if (data_length == 0) {
        printf("data lenght = 0, nothing to expect.\n");
        return 0;
    }

    /* read data according to the data length just received. */
    return_data = malloc(data_length * sizeof(unsigned char));

    for (length = 0, res = 0; length < data_length; length += res) {
        res = read(fd, return_data + length, data_length - length);
        printf("res = %d\n", res);
    }

    printf(" => return data hex string = ");
    for (i = 0; i < length; ++i) {
        printf("%02x ", *(return_data + i));
        /* printf("return buf = %s\n", return_buf); */
    }
    puts("");
    free(return_data);
    closePort(fd);
    return 0;
}

