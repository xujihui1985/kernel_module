#ifndef IOCTL_TEST_H
#define IOCTL_TEST_H

struct mystruct {
    int repeat;
    char name[64];
};

#define WR_VALUE _IOW('a', 'a', int32_t*)
#define RD_VALUE _IOR('b', 'b', int32_t*)
#define GREETER _IOR('a', 'b', struct mystruct*)

#endif
