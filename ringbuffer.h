/*
 * ringbuffer-c
 *
 * Copyright (C) 2017 Andreas Walz
 *
 * Author: Andreas Walz (andreas.walz@hs-offenburg.de)
 *
 * This file is part of ringbuffer-c.
 *
 * ringbuffer-c is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * ringbuffer-c are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ringbuffer-c; if not, see <http://www.gnu.org/licenses/>
 * or write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <stdint.h>
#include <stddef.h>


/*
 * TODO: Add description
 */
typedef size_t rb_size_t;

/*
 * TODO: Add description
 */
typedef struct {

    /* pointer to actual buffer */
    uint8_t* buffer;

    /* size of buffer */
    size_t size;

    /* length of content */
    size_t len;

    /* writing index */
    size_t iw;

    /* reading index */
    size_t ir;

} ringbuffer_t;


/* ========================================================================= */

/*
 * TODO: Add description
 */
void ringbuffer_init(ringbuffer_t* rb, uint8_t* mem, size_t memlen);


/*
 * TODO: Add description
 */
int ringbuffer_get_length(ringbuffer_t* rb);


/*
 * TODO: Add description
 */
int ringbuffer_get_space(ringbuffer_t* rb);


/*
 * TODO: Add description
 */
void ringbuffer_clear(ringbuffer_t* rb);


/*
 * TODO: Add description
 */
int ringbuffer_write(ringbuffer_t* rb, uint8_t* data, size_t len);


/*
 * TODO: Add description
 */
int ringbuffer_read(ringbuffer_t* rb, uint8_t* data, size_t len);


/*
 * TODO: Add description
 */
int ringbuffer_read_memory(ringbuffer_t* rb, uint8_t** data, size_t len);


/*
 * TODO: Add description
 */
int ringbuffer_peek(ringbuffer_t* rb, uint8_t* data, size_t len);


/*
 * TODO: Add description
 */
int ringbuffer_peek_offset(
        ringbuffer_t* rb, size_t offset, uint8_t* data, size_t len);


/*
 * TODO: Add description
 */
int ringbuffer_discard(ringbuffer_t* rb, size_t len);


/* ========================================================================= */
/* Block access                                                              */
/* ========================================================================= */

/*
 * TODO: Add description
 */
size_t ringbuffer_write_block(ringbuffer_t* rb, uint8_t* block, size_t len);


/*
 * TODO: Add description
 */
size_t ringbuffer_read_block(ringbuffer_t* rb, uint8_t* block, size_t len);


/*
 * TODO: Add description
 */
size_t ringbuffer_peek_block(ringbuffer_t* rb, uint8_t* block, size_t len);


/*
 * TODO: Add description
 */
size_t ringbuffer_peek_block_length(ringbuffer_t* rb);


/*
 * TODO: Add description
 */
size_t ringbuffer_discard_block(ringbuffer_t* rb);


/*
 * TODO: Add description
 */
size_t ringbuffer_count_blocks(ringbuffer_t* rb);


/* ========================================================================= */
/* Frame access                                                              */
/* ========================================================================= */

/*
 * TODO: Add description
 */
size_t ringbuffer_write_frame(ringbuffer_t* rb,
		uint8_t* header, size_t hlen, uint8_t* frame, size_t flen);


/*
 * TODO: Add description
 */
size_t ringbuffer_read_frame(ringbuffer_t* rb,
		uint8_t* header, size_t hlen, uint8_t* frame, size_t max_flen);


/*
 * TODO: Add description
 */
size_t ringbuffer_peek_frame(ringbuffer_t* rb,
		uint8_t* header, size_t hlen, uint8_t* frame, size_t max_flen);

#endif

