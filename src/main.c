#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char * optarg;


int main(int argc, char *argv[])
{
    unsigned int input_length  = 1;
    unsigned int output_length = 1;
    int opt, n, i, c;
    unsigned char rawbuf[255];
    unsigned char input_buf[255];
    rawbuf[0] = '\0';
    while ((opt = getopt(argc, argv, "l:L:bi:")) != -1) {
        switch (opt) {
            case 'l':
                input_length  = atoi(optarg);
                break;
            case 'L':
                output_length = atoi(optarg);
                break;
            case 'i':
                strncpy(rawbuf, optarg, 255);
                break;
            default:
                fprintf(stderr, "Usage: %s [-l input_length] [-L output_length] [-b] <input chars>\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    printf("input  length = %u \n", input_length);
    printf("output length = %u \n", output_length);

    printf("raw string = %s \n", rawbuf);
    n = strlen(rawbuf);
    for (i = 0, c = 0; i < n; i+=2, ++c) {
        printf("i = %d, c = %d\n", i, c);
        sscanf(rawbuf + i, "%2hhx", input_buf + c);
    }
    input_buf[c] = '\0';
    puts("");
    printf("i = %d\nc = %d\n", i, c);
    printf("input_string = %s\n", input_buf);

    return 0;
}
