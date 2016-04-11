#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "greatest.h"
#include "heatshrink_decoder.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/cu.usbmodem1411"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP = FALSE; 
static heatshrink_decoder hsd;
int decode_data(uint8_t *data, int data_size);

int decode_data
(uint8_t * data, int sz)
{   
    size_t count = 0;
    size_t desz = sz + sz / 2 + 4;
    int sunk = 0;
    int polled = 0;
    uint8_t *decomp = malloc(desz);

    while (sunk < sz) {
        heatshrink_decoder_sink(&hsd, &data[sunk], sz - sunk, &count);
        sunk += count;
        HSD_poll_res pres;

        do {
            pres = heatshrink_decoder_poll(&hsd, &decomp[polled], desz - polled, &count);
            polled += count;
        } while (pres == HSDR_POLL_MORE);

        if (sunk == sz) {
            HSD_finish_res fres = heatshrink_decoder_finish(&hsd);
        }
    }
    
    fprintf(stdout, "%s", (char *) decomp);
    free(decomp);
}

int main
(void)
{
    int fd, res;
    struct termios oldtio, newtio;
    uint8_t buf[20];

    fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK); 

    if (fd < 0) {
        perror(MODEMDEVICE); 
        exit(-1); 
    }

    tcgetattr(fd, &oldtio); /* save current port settings */

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
     
    newtio.c_cc[VTIME]    = 0;    /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 20;   /* blocking read until 20 chars received */

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);

    uint8_t buffered_data[2048 + 1024 + 4];  /* Maximum of data packet sum length */

    write(fd, "O", 1);

    while (STOP == FALSE) {       /* loop for input */

        res = read(fd, buf, 19);
        buf[res] = '\0';
        printf("%s", buf);  
        
        printf(" END\n");
        // if (strstr(buf, "BCD") != NULL) {
        //     decode_data(buffered_data, buf[16] * 1000 + buf[17] * 100 + buf[18] * 10 + buf[19]);
        //     printf("DATA OUT!!!");
        //     for (int i = 0; i < 2048 + 1024 + 4; ++i) {
        //         buffered_data[i] = 0;
        //     }
        // } else {
        //     strcat(buffered_data, buf);
        // }
    }

    tcsetattr(fd, TCSANOW, &oldtio);
    return 0;
}


