#include "ringbuffer.h"
#include <stdio.h>

void print(ringbuffer_t* rb);


int main(int argc , char *argv[]) {

    uint8_t mem[8];

    ringbuffer_t rb;
    ringbuffer_init(&rb, &(mem[0]), sizeof(mem));

    uint8_t data[256];
    for (size_t i = 0; i < 256; i++) {
        data[i] = i;
    }

    ringbuffer_write(&rb, &(data[0]), 7);

    print(&rb);
    printf("find = %i\n", ringbuffer_find(&rb, 1, &(data[0]), 2));

    ringbuffer_discard(&rb, 4);

    print(&rb);
    printf("find = %i\n", ringbuffer_find(&rb, 0, &(data[4]), 3));

    ringbuffer_write(&rb, &(data[0]), 10);

    print(&rb);
    printf("find = %i\n", ringbuffer_find(&rb, 4, &(data[0]), 5));

    ringbuffer_discard(&rb, 10);

    print(&rb);
    printf("find = %i\n", ringbuffer_find(&rb, 0, &(data[0]), 1));

    return 0;
}



void print(ringbuffer_t* rb) {

    size_t index = rb->ir;

    for (size_t i = 0; i < rb->len; i++) {

        printf("%02X ", rb->buffer[index]);

        if (++index >= rb->size) {
            index = 0;
            printf("| ");
        }
    }

    printf("\n");
}




