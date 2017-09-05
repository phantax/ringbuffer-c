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

#include "ringbuffer.h"
#include <string.h>


/*
 * ___________________________________________________________________________
 */
void ringbuffer_init(ringbuffer_t* rb, uint8_t* mem, size_t memlen) {

    if (rb == 0 || mem == 0) {
        /* >>> Invalid pointer to ringbuffer or memory >>> */
        return;
    }

    /* Set memory */
    rb->buffer = mem;
    rb->size = memlen;

    /* Reset read/write pointers */
    ringbuffer_clear(rb);
}


/*
 * ___________________________________________________________________________
 */
void ringbuffer_clear(ringbuffer_t* rb) {

    if (rb == 0) {
        /* >>> Invalid pointer to ringbuffer >>> */
        return;
    }

    rb->len = 0;
    rb->iw = 0;
    rb->ir = 0;
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_get_length(ringbuffer_t* rb) {

    if (rb == 0) {
        /* >>> Invalid pointer to ringbuffer >>> */
        return -1;
    }

    return rb->len;
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_get_space(ringbuffer_t* rb) {

    if (rb == 0) {
        /* >>> Invalid pointer to ringbuffer >>> */
        return -1;
    }

    /* assuming <len> never exceeds <size> (which should never happen) */
    return rb->size - rb->len;
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_write(ringbuffer_t* rb, uint8_t* data, size_t len) {

    if (rb == 0 || data == 0) {
        /* >>> Invalid pointer to ringbuffer or data buffer >>> */
        return -1;
    }

    /* Sanity check: don't write more than the ringbuffer can hold */
    if (len > (size_t)(rb->size - rb->len)) {
        /* >>> Ringbuffer too small to write data >>> */
        return -1;
    }

    /* Determine the amount of data that can be written linearly */
    size_t linlen = (size_t)(rb->size - rb->iw);

    if (len <= linlen) {

        /* Copy data to ringbuffer linearly */
        memcpy(rb->buffer + rb->iw, data, len);

        /* Advance write index */
        rb->iw += len;
        if (len == linlen) {
            /* >>> rb->iw == rb->size >>> */
            /* Wrap write index */
            rb->iw = 0;
        }

    } else {

        /* Copy first part of data linearly */
        memcpy(rb->buffer + rb->iw, data, linlen);

        /* Move write index such that it reflects the correct
         * state AFTER all data has been written (implicit wrap) */
        rb->iw = len - linlen;

        /* Copy remaining data linearly to beginning of ringbuffer */
        memcpy(rb->buffer, data + linlen, rb->iw);

    }

    rb->len += len;

    return len;
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_read(ringbuffer_t* rb, uint8_t* data, size_t len) {

    if (rb == 0 || data == 0) {
        /* >>> Invalid pointer to ringbuffer or data buffer >>> */
        return -1;
    }

    /* the number of bytes actually read from ringbuffer */
    size_t lenRead = 0;

    /* don't read more than there is data */
    if (len > rb->len) {
        len = rb->len;
    }

    rb->len -= len;

    /* assuming read index never exceeds size */
    size_t linlen = (size_t)(rb->size - rb->ir);
    if (len <= linlen) {

        /* copy data until end of buffer */
        memcpy(data, rb->buffer + rb->ir, len);

        /* advance read index */
        rb->ir += len;
        if (len == linlen) {
            /* here: rb->ir == rb->size, so wrap read index */
            rb->ir = 0;
        }

    } else {

        /* copy data until end of buffer (assuming
         * write index never exceeds size) */
        memcpy(data, rb->buffer + rb->ir, linlen);

        /* read index implicitly wrapped */
        rb->ir = len - linlen;

        /* copy remaining data to beginning of buffer */
        memcpy(data + linlen, rb->buffer, rb->ir);

    }

    return len;
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_read_memory(ringbuffer_t* rb, uint8_t** data, size_t len) {

	return 0;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_peek(ringbuffer_t* rb, uint8_t* data, size_t len) {

    if (rb == 0 || data == 0) {
        /* >>> Invalid pointer to ringbuffer or data buffer >>> */
        return -1;
    }

    /* Make sure not to read more data than there is available */
    if (len > rb->len) {
        len = rb->len;
    }

    /* Determine the length of data readable without wrap-
     * around (assuming read index never exceeds size) */
    size_t linlen = rb->size - rb->ir;

    if (len <= linlen) {

        /* copy data until end of buffer */
        memcpy(data, rb->buffer + rb->ir, len);

    } else {

        /* copy data until end of buffer (assuming
         * write index never exceeds size) */
        memcpy(data, rb->buffer + rb->ir, linlen);

        /* copy remaining data to beginning of buffer */
        memcpy(data + linlen, rb->buffer, len - linlen);

    }

    return len;
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_peek_offset(
        ringbuffer_t* rb, size_t offset, uint8_t* data, size_t len) {

    if (rb == 0 || data == 0) {
        /* >>> Invalid pointer to ringbuffer or data buffer >>> */
        return -1;
    }

    /* vLen is the "virtual" length of the ringbuffer's
     *  content after considering data to disregard (offset) */
    size_t vLen = rb->len;
    if (offset < vLen) {
        vLen -= offset;
    } else {
        vLen = 0;
    }

    /* don't read more than there is data */
    if (len > vLen) {
        len = vLen;
    }

    /* the "virtual" read index of the ringbuffer's
     * after considering data to disregard (offset) */
    size_t vir = rb->ir + offset;
    if (vir >= rb->size) {
        vir -= rb->size;
    }

    /* assuming read index never exceeds size */
    size_t linlen = (size_t)(rb->size - vir);
    if (len <= linlen) {

        /* copy data until end of buffer */
        memcpy(data, rb->buffer + vir, len);

    } else {

        /* copy data until end of buffer (assuming
         * write index never exceeds size) */
        memcpy(data, rb->buffer + vir, linlen);

        /* copy remaining data to beginning of buffer */
        memcpy(data + linlen, rb->buffer, len - linlen);

    }

    return len;
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_discard(ringbuffer_t* rb, size_t len) {

    /* the number of bytes actually discarded from ringbuffer */
    size_t lenDiscarded = 0;

    if (rb != 0) {

        /* don't discard more than there is data */
        if (len > rb->len) {
            len = rb->len;
        }
        rb->len -= len;
        lenDiscarded = len;

        /* assuming read index never exceeds size */
        size_t linlen = (size_t)(rb->size - rb->ir);
        if (len <= linlen) {

            /* advance read index */
            rb->ir += len;
            if (len == linlen) {
                /* here: rb->ir == rb->size, so wrap read index */
                rb->ir = 0;
            }

        } else {

            /* read index implicitly wrapped */
            rb->ir = len - linlen;

        }
    }

    return lenDiscarded;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_write_block(ringbuffer_t* rb, uint8_t* block, size_t len) {

    /* the number of bytes from block written to ringbuffer */
    size_t lenWritten = 0;

    if (rb != 0) {

        /* only write block if there is enough space for
         * the full block (assuming len never exceeds size) */
        size_t space = (size_t)(rb->size - rb->len);
        if ((len + sizeof(size_t)) <= space) {

            /* prepend and write block length */
            ringbuffer_write(rb, (uint8_t*)&len, sizeof(size_t));

            /* write the actual block */
            lenWritten = ringbuffer_write(rb, block, len);
        }
    }

    return lenWritten;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_read_block(ringbuffer_t* rb, uint8_t* block, size_t len) {

    if (rb == 0 || block == 0) {
        return 0;
    }

    /* Read the Block length */
    size_t bl = 0;
    if (ringbuffer_peek(rb, (uint8_t*)&bl, sizeof(size_t)) != sizeof(size_t)) {
        /* >>> Invalid block >>> */
        return 0;
    }

    /* Sanity check: make sure that ...
     *  -> the apparent payload is not longer than there is data
     *  -> the user-provided buffer is sufficiently large to hold the payload
     */
    if (bl + sizeof(size_t) > rb->len || len < bl) {
        /* >>> Invalid block or invalid request >>> */
        return 0;
    }

    /* Discard header (we already read it) */
    ringbuffer_discard(rb, sizeof(size_t));
    
    /* Read payload */
    return ringbuffer_read(rb, block, bl);
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_peek_block(ringbuffer_t* rb, uint8_t* block, size_t len) {

    if (block == 0) {
        return 0;
    }

    /* Read the block length with sanity checks */
    size_t bl = ringbuffer_peek_block_length(rb);

    if (bl == 0) {
        /* >>> Invalid block >>> */
        return 0;
    }

    if (len < bl) {
        /* >>> Block is read only partially >>> */
        bl = len;
    }

    /* Peek payload */
    return ringbuffer_peek_offset(rb, sizeof(size_t), block, bl);
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_peek_block_length(ringbuffer_t* rb) {

    if (rb == 0) {
        return 0;
    }

    /* Read the Block length */
    size_t bl = 0;
    if (ringbuffer_peek(rb, (uint8_t*)&bl, sizeof(size_t)) != sizeof(size_t)) {
        /* >>> Invalid block >>> */
        return 0;
    }

    /* Sanity check: make sure the block is complete */
    if (bl + sizeof(size_t) > rb->len) {
        /* >>> Invalid block >>> */
        return 0;
    }

    return bl;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_discard_block(ringbuffer_t* rb) {

    /* Read the block length with sanity checks */
    size_t bl = ringbuffer_peek_block_length(rb);

    if (bl == 0) {
        /* >>> Invalid block >>> */
        return 0;
    }

    /* Discard block */
    return ringbuffer_discard(rb, bl + sizeof(size_t));
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_count_blocks(ringbuffer_t* rb) {

    if (rb == 0) {
        return 0;
    }

    /* The number of blocks */
    size_t n = 0;

    /* Length of a block */
	size_t bl;

    /* Traverse ringbuffer and count blocks */
	size_t len = rb->len;
	size_t offset = 0;
	while (len > sizeof(size_t)) {
		len -= sizeof(size_t);
		if (ringbuffer_peek_offset(rb, offset, (uint8_t*)&bl, sizeof(size_t))
				== sizeof(size_t) && bl <= len) {
            /* >>> Found one more block */
    		++n;
			/* Step over this block */
    		len -= bl;
    		offset += sizeof(size_t) + bl;
		} else {
			/* something is wrong */
            return 0;
		}
	}

    return n;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_write_frame(ringbuffer_t* rb,
		uint8_t* header, size_t hlen, uint8_t* frame, size_t flen) {

    /* the total number of bytes written to ringbuffer */
    size_t n = 0;

    if (rb != 0) {

        /* only write frame if there is enough space for
         * the full frame (assuming len never exceeds size) */
        if ((sizeof(size_t) + hlen + flen) <= (size_t)(rb->size - rb->len)) {

        	/* the total frame length including header */
        	size_t len = hlen + flen;

            /* prepend and write total frame length */
            n += ringbuffer_write(rb, (uint8_t*)&len, sizeof(size_t));

            /* write header */
            n += ringbuffer_write(rb, header, hlen);

            /* write frame */
            n += ringbuffer_write(rb, frame, flen);

        }
    }

    return n;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_read_frame(ringbuffer_t* rb,
		uint8_t* header, size_t hlen, uint8_t* frame, size_t max_flen) {

    /* the number of frame bytes read from ringbuffer */
    size_t n = 0;

    if (rb != 0) {

    	/* the length of the next frame in the ringbuffer */
        size_t len = 0;

        if (ringbuffer_peek(rb, (uint8_t*)&len, sizeof(size_t))
                == sizeof(size_t)) {

            if (len + sizeof(size_t) <= rb->len
            		&& (hlen + max_flen) >= len) {

                /* discard length (we already know it) */
                ringbuffer_discard(rb, sizeof(size_t));

                /* the header */
                ringbuffer_read(rb, header, hlen);

                /* the frame */
                n = ringbuffer_read(rb, frame, len - hlen);

            }
        }
    }

    return n;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_peek_frame(ringbuffer_t* rb,
		uint8_t* header, size_t hlen, uint8_t* frame, size_t max_flen) {

    /* the number of frame bytes read from ringbuffer */
    size_t n = 0;

    if (rb != 0) {

    	/* the length of the next frame in the ringbuffer */
        size_t len = 0;

        if (ringbuffer_peek(rb, (uint8_t*)&len, sizeof(size_t))
                == sizeof(size_t)) {

            if (len + sizeof(size_t) <= rb->len
            		&& (hlen + max_flen) >= len) {

                /* the header */
                ringbuffer_peek_offset(rb, sizeof(size_t), header, hlen);

                /* the frame */
                n = ringbuffer_peek_offset(rb, sizeof(size_t) + hlen,
                		frame, len - hlen);

            }
        }
    }

    return n;
}
