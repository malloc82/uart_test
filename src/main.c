#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/select.h>

#include "uart.h"

extern char * optarg;
#define BUF_SIZE 255
#define BAUDRATE B9600

// bit location in format
#define QUIET_MODE 1
#define DEBUG_MODE 2
#define ASCII_MODE 4

struct termios oldtio, newtio;

int initializePort(const char *, const unsigned int);
int closePort(int);

int main(int argc, char *argv[])
{
    unsigned char format = 0x00;
    unsigned int output_length = 1024; /* By default, reads 1K */
    int opt, n, i;
    unsigned char input_buf[BUF_SIZE];
    unsigned int input_length = 0;
    unsigned char * return_buf = NULL;
    char device[BUF_SIZE] = "/dev/ttyS1";
    char end_str[16];
    unsigned int end_str_length = 0;
    unsigned int baudrate = B9600;
    struct timeval timeout;
    timeout.tv_sec  = 3;
    timeout.tv_usec = 0;

    while ((opt = getopt(argc, argv, "B:l:L:i:a:e:d:Dq")) != -1) {
        switch (opt) {
            case 'B':
                /* if (strncmp(optarg, "50", 2) == 0) { */
                /*     baudrate = B50; */
                /* } else if (strncmp(optarg, "75", 2) == 0) { */
                /*     baudrate = B75; */
                /* } else if (strncmp(optarg, "110", 3) == 0) { */
                /*     baudrate = B110; */
                /* } else if (strncmp(optarg, "134", 3) == 0) { */
                /*     baudrate = B134; */
                /* } else if (strncmp(optarg, "150", 3) == 0) { */
                /*     baudrate = B150; */
                /* } else if (strncmp(optarg, "200", 3) == 0) { */
                /*     baudrate = B200; */
                /* } else if (strncmp(optarg, "300", 3) == 0) { */
                /*     baudrate = B300; */
                /* } else if (strncmp(optarg, "600", 3) == 0) { */
                /*     baudrate = B600; */
                /* } else */
                if (strncmp(optarg, "1200", 4) == 0) {
                    baudrate = B1200;
                } else if (strncmp(optarg, "1800", 4) == 0) {
                    baudrate = B1800;
                } else if (strncmp(optarg, "2400", 4) == 0) {
                    baudrate = B2400;
                } else if (strncmp(optarg, "4800", 4) == 0) {
                    baudrate = B4800;
                } else if (strncmp(optarg, "9600", 4) == 0) {
                    baudrate = B9600;
                } else if (strncmp(optarg, "19200", 5) == 0) {
                    baudrate = B19200;
                } else if (strncmp(optarg, "38400", 5) == 0) {
                    baudrate = B38400;
                } else if (strncmp(optarg, "57600", 5) == 0) {
                    baudrate = B57600;
                } else if (strncmp(optarg, "115200", 6) == 0) {
                    baudrate = B115200;
                /* } else if (strncmp(optarg, "230400", 6) == 0) { */
                /*     baudrate = B230400; */
                /* } else if (strncmp(optarg, "460800", 6) == 0) { */
                /*     baudrate = B460800; */
                /* } else if (strncmp(optarg, "500000", 6) == 0) { */
                /*     baudrate = B500000; */
                /* } else if (strncmp(optarg, "576000", 6) == 0) { */
                /*     baudrate = B576000; */
                /* } else if (strncmp(optarg, "921600", 6) == 0) { */
                /*     baudrate = B921600; */
                /* } else if (strncmp(optarg, "1000000", 7) == 0) { */
                /*     baudrate = B1000000; */
                /* } else if (strncmp(optarg, "1152000", 7) == 0) { */
                /*     baudrate = B1152000; */
                /* } else if (strncmp(optarg, "2000000", 7) == 0) { */
                /*     baudrate = B2000000; */
                /* } else if (strncmp(optarg, "2500000", 7) == 0) { */
                /*     baudrate = B2500000; */
                /* } else if (strncmp(optarg, "3000000", 7) == 0) { */
                /*     baudrate = B3000000; */
                /* } else if (strncmp(optarg, "3500000", 7) == 0) { */
                /*     baudrate = B3500000; */
                /* } else if (strncmp(optarg, "4000000", 7) == 0) { */
                /*     baudrate = B4000000; */
                } else {
                    fprintf(stderr, "Error: Unsupported baudrate: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'L':
                output_length = atoi(optarg);
                break;
            case 'd':
                strncpy(device, optarg, BUF_SIZE);
                if (access(device, F_OK) == -1) {
                    fprintf(stderr, "Error: cannot find file %s\n", device);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'D':
                format = format | DEBUG_MODE;
                break;
            case 'i':
                n = strlen(optarg);
                for (i = 0, input_length = 0; i < n && input_length < BUF_SIZE-1; i+=2, ++input_length) {
                    /* printf("i = %d, c = %d\n", i, c); */
                    sscanf(optarg + i, "%2hhx", input_buf + input_length);
                }
                break;
            case 'a':
                format = format | ASCII_MODE;
                strncpy((char *)input_buf, optarg, BUF_SIZE);
                input_length = strlen(optarg);
                break;
            case 'e':
                n = strlen(optarg);
                for (i = 0, end_str_length = 0; i < n && end_str_length < 16; i+= 2, ++end_str_length) {
                    sscanf(optarg + i, "%2hhx", end_str + end_str_length);
                }
                break;
            case 'q':
                format = format | QUIET_MODE;
                break;
            default:
                fprintf(stderr,
                        "Usage: %s [-d device] [-B baudrate]"
                        " [-L <expected output length>] [-i <input hex string>] [-q]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (end_str_length > 0) {
        strncpy((char *)input_buf + input_length, end_str, end_str_length); /* Need to use a min here*/
        input_length += end_str_length;
    } else {
        input_buf[input_length] = '\0';
    }

    if (strlen(device) == 0) {
        fprintf(stderr, "Error, need to specify a tty device");
        exit(EXIT_FAILURE);
    }

    if (!(format & QUIET_MODE)) {
        printf("output length    : %u \n", output_length);
        if (format & ASCII_MODE) {
            printf("input ascii string: %s\n", input_buf);
        }
        printf("input hex string  : ");
        print_hex_string(input_buf, input_length);
        puts("");

        printf("timeout          : %us, %uus\n",
               (unsigned int) timeout.tv_sec, (unsigned int)timeout.tv_usec);
        fflush(stdout);
    }

    int fd = initializePort(device, baudrate);
    fd_set rfd;
    FD_ZERO(&rfd);
    FD_SET(fd, &rfd);

    write(fd, input_buf, input_length);
    return_buf = malloc((output_length)*sizeof(unsigned char));
    if (!return_buf) {
        fprintf(stderr, "Cannot allocate buff of size %d\n", output_length);
        exit(EXIT_FAILURE);
    }
    int length, res, rv;
    if (!(format & QUIET_MODE))
        puts("\nreading ...");
    else
        puts("");

    unsigned char * current = NULL;
    unsigned int unread_limit = 100, unread_count = 0;
    for (length = 0, res = 0, current = return_buf, unread_count = 0;
         length < output_length;
         length += res, current = return_buf + length)
    {
        rv = select(fd + 1, &rfd, NULL, NULL, &timeout);
        if (rv == -1) {
            perror("select");
            goto __cleanup;
            break;
        } else if (rv == 0) {
            printf("timeout ... total %d bytes read.\n", length);
            goto __timeout;
            break;
        } else {
            res = read(fd, current, output_length - length);
        }
        /* res = read(fd, current, output_length - length); */
        if (!(format & QUIET_MODE)) {
            printf("bytes read = %d : ", res);
            print_hex_string(current, res);
            puts("");
            fflush(stdout);
        }
        if (res == 0) {
            ++unread_count;
        } else {
            unread_count = 0;
        }
        if (unread_count > unread_limit) {
            fprintf(stderr, "RX not responding ... quit");
            goto __timeout;
        }
    }
    if (!(format & QUIET_MODE))
        puts("done");

__timeout:
    return_buf[length] = '\0';
    if (!(format & QUIET_MODE))
        printf("\nreturn data (%d bytes) : ", length);
    /* print_hex_string(return_buf, length); */
    if (format & DEBUG_MODE)
        debug_hex_string(return_buf, length, 0xdd, 13);
    else
        print_hex_string(return_buf, length);
    puts("");

__cleanup:
    closePort(fd);
    free(return_buf);
    return 0;
}

