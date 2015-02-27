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
                fprintf(stderr, "Usage: %s [-l input_length] [-L output_length] [-b] <input chars>\n",
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
      BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
      CRTSCTS : output hardware flow control (only used if the cable has
      all necessary lines. See sect. 7 of Serial-HOWTO)
      CS8     : 8n1 (8bit,no parity,1 stopbit)
      CLOCAL  : local connection, no modem contol
      CREAD   : enable receiving characters
    */
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;

    /*
      IGNPAR  : ignore bytes with parity errors
      ICRNL   : map CR to NL (otherwise a CR input on the other computer
      will not terminate input)
      otherwise make device raw (no other input processing)
    */
    newtio.c_iflag = IGNPAR | ICRNL;

    /*
      Raw output.
    */
    newtio.c_oflag = 0;

    /*
      ICANON  : enable canonical input
      disable all echo functionality, and don't send signals to calling program
    */
    newtio.c_lflag = ICANON;

    /*
      initialize all control characters
      default values can be found in /usr/include/termios.h, and are given
      in the comments, but we don't need them here
    */
    newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */
    newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtio.c_cc[VERASE]   = 0;     /* del */
    newtio.c_cc[VKILL]    = 0;     /* @ */
    newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
    newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
    newtio.c_cc[VSWTC]    = 0;     /* '\0' */
    newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */
    newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newtio.c_cc[VEOL]     = 0;     /* '\0' */
    newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newtio.c_cc[VEOL2]    = 0;     /* '\0' */

    /*
      now clean the modem line and activate the settings for the port
    */
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
