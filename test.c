#include "ringbuffer.h"
#include <stdio.h>


int main(int argc , char *argv[]) {

    uint8_t mem[8];

    ringbuffer_t rb;
    ringbuffer_init(&rb, &(mem[0]), sizeof(mem));

    uint8_t data[256];
    for (size_t i = 0; i < 256; i++) {
        data[i] = i;
    }

    ringbuffer_write(&rb, &(data[0]), 12);
    ringbuffer_write(&rb, &(data[12]), 3);

    printf("len = %i\n", ringbuffer_get_length(&rb));

    uint8_t foo[256];

    while (1) {

        int len = ringbuffer_read(&rb, &(foo[0]), 3);
        printf("read: ");
        for (size_t i = 0; i < len; i++) {
            printf("%.2X ", foo[i]);
        }
        printf("\n");

        printf("len = %i\n", ringbuffer_get_length(&rb));

        if (!len) {
            break;
        }

    }

    return 0;
}



