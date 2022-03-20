#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int dev = open("/dev/mydevice", O_RDONLY);
    if (dev == -1) {
        printf("failed to open");
        return -1;
    }
    printf("opening success");
    close(dev);
    return 0;
}
