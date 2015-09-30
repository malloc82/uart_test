#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uart.h"
#include "PDD_settings.h"

extern char * optarg;
#define BUF_SIZE 255

const double step_2_5v =  2500.0/(1 << 12);
const double step_5v   =  5000.0/(1 << 12);
const double step_10v  = 10000.0/(1 << 12);


double convert_threshold_value(int raw_value, int range_val)
{
    switch(range_val) {
        case 0: /* +/-2.5v */
            return raw_value * step_2_5v;
        case 1: /* +/-5.0v */
            return raw_value * step_5v;
        case 2: /* +/-10.ov */
            return raw_value * step_10v;
        default:
            return -999999.9999;
    }
}

const char * range_lookup(int range_val)
{
    switch(range_val) {
        case 0:
            return "+/-2.5v";
        case 1:
            return "+/-5v";
        case 2:
            return "+/-10v";
        default:
            return "INVALID";
    }
}

const char * state_lookup(int state_val)
{
    switch(state_val){
        case 0:
            return "startup";
        case 1:
            return "reg_update";
        case 2:
            return "t_powerup";
        case 3: /* 3 is t_quiet, which should be part of normal mode */
        case 4:
            return "normal";
        case 5:
            return "fullshutdown";
        case 6:
            return "autostandby";
        case 7:
            return "autoshutdown";
        default:
            return "INVALID";
    }
}

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

    printf(" => return data hex string = ");
    for (i = 0; i < length; ++i) {
        printf("%02x ", *(return_data + i));
        /* printf("return buf = %s\n", return_buf); */
    }
    puts("");

    const unsigned char * MDU = return_data + 7;
    const unsigned char * SDU = return_data + 4;
    const unsigned char * GRD = return_data + 1;

    printf("\nQuantization level: \n"
           "    +/- 2.5v   %.3fmv\n"
           "    +/- 5.0v   %.3fmv\n"
           "    +/-10.0v   %.3fmv\n",  step_2_5v, step_5v, step_10v);

    printf("\nmode = %02x\n", return_data[0]);

    if (return_data[0] & 0x01) {
        puts("\nMDU + Grid : ON  (piezo #9  [use 0xA9 to download its data])\n");
    } else {
        puts("\nMDU + Grid : OFF (piezo #11 [use 0xAB to download its data])\n");
    }
    printf("     threshold            range     empty   full   state         en   running  recording\n");
    printf("     ------------------------------------------------------------------------------------\n");

    int threshold_value = (MDU[0] << 4) | (MDU[1] >> 4);
    int range_value     = (0x0C & MDU[1]) >> 2;
    printf("MDU  %6.2fmv (0x%04x)    %7s   %d       %d      %-12s  %d    %d        %d\n",
           convert_threshold_value(threshold_value, range_value),
           threshold_value,
           range_lookup(range_value),
           (0x02 & MDU[1]) >> 1,
           (0x01 & MDU[1]),
           state_lookup(MDU[2] >> 4),
           (MDU[2] & 0x04) >> 2,
           (MDU[2] & 0x02) >> 1,
           (MDU[2] & 0x01));

    threshold_value = (SDU[0] << 4) | (SDU[1] >> 4);
    range_value     = (0x0C & SDU[1]) >> 2;
    printf("SDU  %6.2fmv (0x%04x)    %7s   %d       %d      %-12s  %d    %d        %d\n",
           convert_threshold_value(threshold_value, range_value),
           threshold_value,
           range_lookup(range_value),
           (0x02 & SDU[1]) >> 1,
           (0x01 & SDU[1]),
           state_lookup(SDU[2] >> 4),
           (SDU[2] & 0x04) >> 2,
           (SDU[2] & 0x02) >> 1,
           (SDU[2] & 0x01));

    threshold_value = (GRD[0] << 4) | (GRD[1] >> 4);
    range_value     = (0x0C & GRD[1]) >> 2;
    printf("GRD  %6.2fmv (0x%04x)    %7s   %d       %d      %-12s  %d    %d        %d\n",
           convert_threshold_value(threshold_value, range_value),
           threshold_value,
           range_lookup(range_value),
           (0x02 & GRD[1]) >> 1,
           (0x01 & GRD[1]),
           state_lookup(GRD[2] >> 4),
           (GRD[2] & 0x04) >> 2,
           (GRD[2] & 0x02) >> 1,
           (GRD[2] & 0x01));

    free(return_data);
    closePort(fd);
    return 0;
}

