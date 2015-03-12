#include "uart.h"

extern struct termios oldtio, newtio;

int initializePort(const char * port, const unsigned int baudrate)
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
    /* printf("size of c_cflag = %d\n", sizeof(newtio.c_cflag)); */
    newtio.c_cflag = baudrate | CS8 | HUPCL | CREAD ;
    newtio.c_iflag = 0;
    newtio.c_oflag = OPOST | OLCUC | OCRNL | NL0 | CR0 | TAB0 | BS0 | VT0 | FF0;
    newtio.c_lflag = 0;

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
