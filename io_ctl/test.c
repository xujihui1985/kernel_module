#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ioctl_test.h"

int main() {
    int answer;
    struct mystruct test = {4, "hello"};
    int dev = open("/dev/ioctl_dummy", O_RDWR);
    if (dev == -1) {
        printf("failed to open");
        return -1;
    }
    ioctl(dev, RD_VALUE, &answer);
    printf("answer is %d\n", answer);

    answer = 123;
    ioctl(dev, WR_VALUE, &answer);
    ioctl(dev, RD_VALUE, &answer);
    printf("answer is %d\n", answer);

    ioctl(dev, GREETER, &test);

    printf("opening success\n");
    close(dev);
    return 0;
}
