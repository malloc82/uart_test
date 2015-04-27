#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uart.h"
#include "PDD_settings.h"

extern char * optarg;
#define BUF_SIZE 255

int main(int argc, char *argv[])
{
    unsigned char input_cmd = 0xA0;
    unsigned char piezo_id = 0x00;
    if (argc >= 2) {
        sscanf(argv[1], "%2hhx", &piezo_id);
    }
    input_cmd = input_cmd | (0x0f & piezo_id);

    printf("input command (hex string): ");
    print_hex_string(&input_cmd, 1);
    puts("");

    /* unsigned char raw_data[2]; */
    /* raw_data[0] = 0xFF; */
    /* raw_data[1] = 0XFE; */
    /* short val = (raw_data[0] << 8) | raw_data[1]; */

    /* printf("raw_data = %d\n", val); */
    /* printf("raw_data = %d\n", ((short *)raw_data)[0]); */

    int fd = initializePort(PORT, BAUDRATE);
    write(fd, &input_cmd, 1);

    int length, res, i;
    unsigned char return_code = 0xff;
    unsigned short data_length = 0;
    unsigned char * return_data = NULL;
    short * raw_data = NULL;

    /* read return code */
    read(fd, &return_code, 1);
    printf(" => Return code = 0x%2x\n", return_code);

    /* determine if there is an error */
    if ((return_code & 0x0F) != 0x00) {
        puts("return code indicates an error has occurred");
        return 0;
    }

    /* read next two bytes for data length */
    for (length = 0, res = 0; length < 2; length += res) {
        res = read(fd, ((unsigned char *)(&data_length)) + length, 2 - length);
        /* printf("res = %d, length = %d\n", res, length); */
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
        /* printf("res = %d\n", res); */
    }
    printf(" => bytes read = %d\n", length);

    /* printf(" => return data hex string = "); */
    /* for (i = 0; i < length; ++i) { */
    /*     printf("%02x ", *(return_data + i)); */
    /*     /\* printf("return buf = %s\n", return_buf); *\/ */
    /* } */
    /* puts(""); */


    /* unsigned char temp; */
    /* for (i = 0; i < (length >> 1); ++i) { */
    /*     temp               = return_data[2*i]; */
    /*     return_data[2*i]   = return_data[2*i+1]; */
    /*     return_data[2*i+1] = temp; */
    /* } */

    raw_data = (short *)return_data;

    for (i = 0; i < (length >> 1); ++i) {
        printf("%d ", raw_data[i]);
    }
    puts("");

    if (argc >= 3) {
        FILE * output_file = fopen(argv[2], "w");
        for (i = 0; i < (length >> 1); ++i) {
            fprintf(output_file, "%d ", raw_data[i]);
        }
        fprintf(output_file, "\n");
        fclose(output_file);
    }
    free(return_data);
    closePort(fd);
    return 0;
}

