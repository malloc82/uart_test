#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>


extern char * optarg;
#define BUF_SIZE 255
#define BAUDRATE B9600

struct termios oldtio, newtio;

int initializePort(const char *);
int closePort(int);

int main(int argc, char *argv[])
{
    unsigned int output_length = 1;
    int opt, n, i, size;
    unsigned char input_buf[BUF_SIZE];
    unsigned char * return_buf = NULL;
    char device[BUF_SIZE];
    while ((opt = getopt(argc, argv, "l:L:i:d:")) != -1) {
        switch (opt) {
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
            default:
                fprintf(stderr, "Usage: %s [-d device] [-L output_length] [-i input string]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    printf("output length = %u \n", output_length);

    printf("input_string = %s\n", input_buf);

    int fd = initializePort(device);
    write(fd, input_buf, size);
    return_buf = malloc((output_length+1)*sizeof(unsigned char));
    if (!return_buf) {
        fprintf(stderr, "Cannot allocate buff of size %d\n", output_length + 1);
        exit(EXIT_FAILURE);
    }
    int length, res;
    puts("reading ...");
    for (length = 0, res = 0; length < output_length; length += res) {
        res = read(fd, return_buf + length, 10);
        printf("res = %d\n", res);
    }
    puts("done");
    return_buf[length] = '\0';
    printf("return buf = %s\n", return_buf);
    closePort(fd);
    free(return_buf);
    return 0;
}

int initializePort(const char * port)
/* Return a file descriptor of the serial port
 */
{
    int fd,c, res;
    char buf[255];
    /*
      Open modem device for reading and writing and not as controlling tty
      because we don't want to get killed if linenoise sends CTRL-C.
    */
    fd = open(port, O_RDWR | O_NOCTTY );
    if (fd <0) {
        perror(port);
        exit(-1);
    }

    tcgetattr(fd,&oldtio); /* save current serial port settings */
    bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

    /*
      initialize all control characters
      default values can be found in /usr/include/termios.h, and are given
      in the comments, but we don't need them here
    */
    newtio.c_cc[VINTR]    = 0x03;     /* Ctrl-c */
    newtio.c_cc[VQUIT]    = 0x1c;     /* Ctrl-\ */
    newtio.c_cc[VERASE]   = 0x7f;     /* del */
    newtio.c_cc[VKILL]    = 0x40;     /* @ */
    newtio.c_cc[VEOF]     = 0x04;     /* Ctrl-d */
    newtio.c_cc[VTIME]    = 0;        /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;        /* blocking read until 1 character arrives */
    newtio.c_cc[VSWTC]    = 0;        /* '\0' */
    newtio.c_cc[VSTART]   = 0x11;     /* Ctrl-q */
    newtio.c_cc[VSTOP]    = 0x13;     /* Ctrl-s */
    newtio.c_cc[VSUSP]    = 0x1a;     /* Ctrl-z */
    newtio.c_cc[VEOL]     = 0;        /* '\0' */
    newtio.c_cc[VREPRINT] = 0x12;     /* Ctrl-r */
    newtio.c_cc[VDISCARD] = 0x0f;     /* Ctrl-u, flush */
    newtio.c_cc[VWERASE]  = 0x17;     /* Ctrl-w */
    newtio.c_cc[VLNEXT]   = 0x16;     /* Ctrl-v */
    newtio.c_cc[VEOL2]    = 0;        /* '\0' */

    /*
      now clean the modem line and activate the settings for the port
    */
    newtio.c_cflag = BAUDRATE | CS8 | HUPCL | CREAD ;
    newtio.c_iflag = 0;
    newtio.c_lflag = 0;
    newtio.c_oflag = OPOST | OLCUC | OCRNL | NL0 | CR0 | TAB0 | BS0 | VT0 | FF0;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);

    return fd;
}


int closePort(int fd)
{
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
