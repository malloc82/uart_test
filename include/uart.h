#ifndef UART_H
#define UART_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <termios.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern struct termios oldtio, newtio;

/* Initialize the tty device specified in the first parameter,
   with a given baud rate (second parameter).
   Current tty setting will be stored into oldtio.

   Detailed tty setting, need to look into the src code. */
int initializePort(const char *, const unsigned int);

/* Restore tty setting from oldtio, and close the device. */
int closePort(int);

void print_hex_string(const unsigned char *, const int);

void debug_hex_string(const unsigned char *, const int, const unsigned char, const int);

#endif /* UART_H */
