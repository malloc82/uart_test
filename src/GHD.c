#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uart.h"
#include "PDD_settings.h"

extern char * optarg;
#define BUF_SIZE 255

int main(int argc, char *argv[])
{
    unsigned char input_cmd = 0x10;
    printf("Input hex string : 0x%02x\n", input_cmd);

    int fd = initializePort(PORT, BAUDRATE);
    write(fd, &input_cmd, 1);

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

    printf(" => return data hex string = ");
    for (i = 0; i < length; ++i) {
        printf("%02x ", *(return_data + i));
        /* printf("return buf = %s\n", return_buf); */
    }
    puts("");

    unsigned char MDU[3];
    unsigned char SDU[3];
    unsigned char GRD[3];

    memcpy(MDU, return_data + 7, 3);
    memcpy(SDU, return_data + 4, 3);
    memcpy(GRD, return_data + 1, 3);

    printf("\nmode = %02x\n\n", return_data[0]);

    puts("     startup      = 0\n"
         "     reg_update   = 1\n"
         "     t_powerup    = 2\n"
         "     t_quiet      = 3\n"
         "     normal       = 4\n"
         "     fullshutdown = 5\n"
         "     autostandby  = 6\n");

    printf("     threshold   range   empty   full   state   en   running  recording\n");
    printf("MDU  %4d        %d       %d       %d      %d       %d    %d        %d\n",
           (MDU[0] << 4) | (MDU[1] >> 4),
           (0x0C & MDU[1]) >> 2,
           (0x02 & MDU[1]) >> 1,
           (0x01 & MDU[1]),
           MDU[2] >> 4,
           (MDU[2] & 0x04) >> 2,
           (MDU[2] & 0x02) >> 1,
           (MDU[2] & 0x01));

    printf("SDU  %4d        %d       %d       %d      %d       %d    %d        %d\n",
           (SDU[0] << 4) | (SDU[1] >> 4),
           (0x0C & SDU[1]) >> 2,
           (0x02 & SDU[1]) >> 1,
           (0x01 & SDU[1]),
           SDU[2] >> 4,
           (SDU[2] & 0x04) >> 2,
           (SDU[2] & 0x02) >> 1,
           (SDU[2] & 0x01));

    printf("GRD  %4d        %d       %d       %d      %d       %d    %d        %d\n",
           (GRD[0] << 4) | (GRD[1] >> 4),
           (0x0C & GRD[1]) >> 2,
           (0x02 & GRD[1]) >> 1,
           (0x01 & GRD[1]),
           GRD[2] >> 4,
           (GRD[2] & 0x04) >> 2,
           (GRD[2] & 0x02) >> 1,
           (GRD[2] & 0x01));


    free(return_data);
    closePort(fd);
    return 0;
}

