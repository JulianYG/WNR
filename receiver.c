#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdint.h>
#define MODEMDEVICE "/dev/cu.usbmodem1411"

int read_port(int fd)
{
    uint8_t buffer[20];
    
    if (read(fd, buffer, sizeof(buffer)) < 0) {
        fputs("Unable to read from port /dev/tty.usbmodem1411 \n", stderr);
        return -1;
    }

    for (int i = 0; i < 20; ++i) {
        printf("%c", buffer[i]);  

    }
    printf("END\n");
    return 1;
}

int main(void)
{
    int fd = open(MODEMDEVICE, O_RDONLY | O_NOCTTY);
    if (fd < 0) {
        fputs("Unable to open port /dev/tty.usbmodem1411 \n", stderr);
    }

    while (1) {
        if (read_port(fd) < 0)
            break;
    }

    close(fd);

    return 0;
}
