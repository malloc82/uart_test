#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/select.h>

#include "uart.h"

extern char * optarg;
#define BUF_SIZE 255
#define BAUDRATE B9600

struct termios oldtio, newtio;

int initializePort(const char *, const unsigned int);
int closePort(int);

int main(int argc, char *argv[])
{
    unsigned int output_length = 1024; /* By default, reads 1K */
    int opt, n, i, size;
    unsigned char input_buf[BUF_SIZE];
    unsigned char * return_buf = NULL;
    char device[BUF_SIZE];
    device[0] = '\0';
    unsigned int baudrate = B9600;
    unsigned char quiet = 0;
    struct timeval timeout;
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;

    while ((opt = getopt(argc, argv, "B:l:L:i:d:q")) != -1) {
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
                /* } else if (strncmp(optarg, "1200", 4) == 0) { */
                /*     baudrate = B1200; */
                /* } else if (strncmp(optarg, "1800", 4) == 0) { */
                /*     baudrate = B1800; */
                /* } else if (strncmp(optarg, "2400", 4) == 0) { */
                /*     baudrate = B2400; */
                /* } else */
                if (strncmp(optarg, "4800", 4) == 0) {
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
            case 'i':
                n = strlen(optarg);
                for (i = 0, size = 0; i < n && size < BUF_SIZE-1; i+=2, ++size) {
                    /* printf("i = %d, c = %d\n", i, c); */
                    sscanf(optarg + i, "%2hhx", input_buf + size);
                }
                input_buf[size] = '\0';
                break;
            case 'q':
                quiet = 1;
                break;
            default:
                fprintf(stderr,
                        "Usage: %s [-d device] [-B baudrate]"
                        " [-L <expected output length>] [-i <input hex string>] [-q]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (strlen(device) == 0) {
        fprintf(stderr, "Error, need to specify a tty device");
        exit(EXIT_FAILURE);
    }

    if (!quiet) {
        printf("output length = %u \n", output_length);
        printf("input_string = %s\n", input_buf);
    }

    int fd = initializePort(device, baudrate);
    fd_set rfd;
    FD_ZERO(&rfd);
    FD_SET(fd, &rfd);

    write(fd, input_buf, size);
    return_buf = malloc((output_length+1)*sizeof(unsigned char));
    if (!return_buf) {
        fprintf(stderr, "Cannot allocate buff of size %d\n", output_length + 1);
        exit(EXIT_FAILURE);
    }
    int length, res, rv;
    if (!quiet)
        puts("reading ...");
    else
        puts("");

    unsigned char * current = NULL;
    for (length = 0, res = 0, current = return_buf;
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
            res = read(fd, current, 10);
        }
        if (!quiet) {
            printf("res = %d : ", res);
            for (i = 0; i < res; ++i){
                printf("0x%02x ", *(current + i));
            }
            puts("");
            fflush(stdout);
        }

    }
    if (!quiet)
        puts("done");

__timeout:
    return_buf[length] = '\0';
    if (!quiet)
        printf("\nreturn data (%d bytes) : ", length);
    for (i = 0; i < length; ++i) {
        printf("%02x ", *(return_buf + i));
        /* printf("return buf = %s\n", return_buf); */
    }
    puts("");


__cleanup:
    closePort(fd);
    free(return_buf);
    return 0;
}

