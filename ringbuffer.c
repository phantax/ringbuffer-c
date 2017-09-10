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
int ringbuffer_init(ringbuffer_t* rb, uint8_t* mem, size_t memlen) {

    if (rb == 0 || mem == 0) {
        /* >>> Invalid pointer to ringbuffer or memory >>> */
        return -1;
    }

    /* Set memory */
    rb->buffer = mem;
    rb->size = memlen;

    /* Reset read/write pointers */
    return ringbuffer_clear(rb);
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_clear(ringbuffer_t* rb) {

    if (rb == 0) {
        /* >>> Invalid pointer to ringbuffer >>> */
        return -1;
    }

    /* Reset length and read/write indices */
    rb->len = 0;
    rb->iw = 0;
    rb->ir = 0;

    /* Return the ringbuffer's size */
    return rb->size;
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_get_length(ringbuffer_t* rb) {

    if (rb == 0) {
        /* >>> Invalid pointer to ringbuffer >>> */
        return -1;
    }

    /* Return the ringbuffer's length */
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

    /* Return the ringbuffer's space. We assume here that <len>
     * never exceeds <size> (which should actually never happen) */
    return (size_t)(rb->size - rb->len);
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_write(ringbuffer_t* rb, uint8_t* data, size_t len) {

    if (rb == 0 || data == 0) {
        /* >>> Invalid pointer to ringbuffer or data buffer >>> */
        return -1;
    }

    /* Don't write more data than the ringbuffer can hold */
    size_t space = (size_t)(rb->size - rb->len);
    if (len > space) {
    	/* >>> Requested to write more data than the ringbuffer can hold >>> */
    	/* Truncate write request */
    	len = space;
    }

    /* Determine the amount of data that can be written linearly */
    size_t linlen = (size_t)(rb->size - rb->iw);

    if (len <= linlen) {
    	/* >>> The whole write request can be performed linearly >>> */

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
    	/* >>> The write request requires two steps >>> */

        /* Copy first part of data linearly */
        memcpy(rb->buffer + rb->iw, data, linlen);

        /* Move write index such that it reflects the correct
         * state AFTER all data has been written (implicit wrap) */
        rb->iw = len - linlen;

        /* Copy remaining data linearly to beginning of ringbuffer */
        memcpy(rb->buffer, data + linlen, rb->iw);

    }

    /* <len> bytes have been written to the ringbuffer */
    rb->len += len;

    /* Return the number of bytes writte to ringbuffer */
    return len;
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_write_all(ringbuffer_t* rb, uint8_t* data, size_t len) {

    if (rb == 0 || data == 0) {
        /* >>> Invalid pointer to ringbuffer or data buffer >>> */
        return -1;
    }

    /* Make sure all data can be written to ringbuffer */
    if (len > (size_t)(rb->size - rb->len)) {
        /* >>> Ringbuffer too small to write data >>> */
        return -1;
    }

    /* Determine the amount of data that can be written linearly */
    size_t linlen = (size_t)(rb->size - rb->iw);

    if (len <= linlen) {
    	/* >>> The whole write request can be performed linearly >>> */

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
    	/* >>> The write request requires two steps >>> */

        /* Copy first part of data linearly */
        memcpy(rb->buffer + rb->iw, data, linlen);

        /* Move write index such that it reflects the correct
         * state AFTER all data has been written (implicit wrap) */
        rb->iw = len - linlen;

        /* Copy remaining data linearly to beginning of ringbuffer */
        memcpy(rb->buffer, data + linlen, rb->iw);

    }

    /* <len> bytes have been written to the ringbuffer */
    rb->len += len;

    /* Return the number of bytes writte to ringbuffer */
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

    /* don't read more than there is data */
    if (len > rb->len) {
        len = rb->len;
    }

    rb->len -= len;

    /* Determine the amount of data that can be read linearly */
    size_t linlen = (size_t)(rb->size - rb->iw);

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
int ringbuffer_peek(ringbuffer_t* rb, uint8_t* data, size_t len) {

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

    if (rb == 0) {
        /* >>> Invalid pointer to ringbuffer or data buffer >>> */
        return -1;
    }

	/* don't discard more than there is data */
	if (len > rb->len) {
		len = rb->len;
	}

	rb->len -= len;

	/* assuming read index never exceeds size */
	size_t linlen = (size_t)(rb->size - rb->ir);

	if (len < linlen) {
		/* advance read index */
		rb->ir += len;
	} else if (len == linlen) {
		/* here: rb->ir == rb->size, so wrap read index */
		rb->ir = 0;
	} else {
		/* read index implicitly wrapped */
		rb->ir = len - linlen;
	}

    return len;
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_write_block(ringbuffer_t* rb, uint8_t* block, size_t len) {

	/* Sanity check: make sure input pointer are ok */
    if (rb == 0 || block == 0) {
    	/* >>> Invalid pointer to ringbuffer or block data to write >>> */
        return 0;
    }

	/* only write block if there is enough space for
	 * the full block (assuming len never exceeds size) */
	size_t space = (size_t)(rb->size - rb->len);

	if ((len + sizeof(size_t)) > space) {
		/* >>> No enough space to write block >>> */
		return -1;
	}

	/* Write block length */
	if (ringbuffer_write_all(rb, (uint8_t*)&len, sizeof(size_t)) < 0) {
		/* >>> Writing block length failed >>> */
		return -1;
	}

	/* Write block data */
	if (ringbuffer_write_all(rb, block, len) < 0) {
		/* >>> Writing block data failed >>> */
		/* We are in an inconsistent state now because writing the
		 * block length apparently succeeded, while writing the
		 * actual block data failed => return -2 instead of -1 here */
		return -2;
	}

	/* Return the total number of bytes written to the ringbuffer */
    return len + sizeof(size_t);
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_read_block(ringbuffer_t* rb, uint8_t* block, size_t len) {

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
int ringbuffer_peek_block(ringbuffer_t* rb, uint8_t* block, size_t len) {

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
int ringbuffer_peek_block_length(ringbuffer_t* rb) {

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
        return -1;
    }

    return bl;
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_discard_block(ringbuffer_t* rb) {

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
int ringbuffer_count_blocks(ringbuffer_t* rb) {

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
int ringbuffer_write_frame(ringbuffer_t* rb,
		uint8_t* header, size_t hlen, uint8_t* data, size_t flen) {

	/* Sanity check: make sure input pointer are ok */
    if (rb == 0 || header == 0 || data == 0) {
    	/* >>> Invalid pointer(s) >>> */
        return -1;
    }

	/* The total frame length including header */
	size_t len = hlen + flen;

	/* only write frame if there is enough space for
	 * the full frame (assuming len never exceeds size) */
	if ((sizeof(size_t) + len) > (size_t)(rb->size - rb->len)) {
		/* >>> Ringbuffer too small to write frame >>> */
		return -1;
	}

	/* prepend and write total frame length */
	if (ringbuffer_write_all(rb, (uint8_t*)&len, sizeof(size_t)) < 0) {
		/* >>> Writing total length failed >>> */
		return -1;
	}

	/* Write header */
	if (ringbuffer_write_all(rb, header, hlen) < 0) {
		/* >>> Writing header failed >>> */
		/* We are in an inconsistent state now because writing the
		 * total length apparently succeeded, while writing the
		 * header failed => return -2 instead of -1 here */
		return -2;
	}

	/* Write data */
	if (ringbuffer_write_all(rb, data, flen) < 0) {
		/* >>> Writing data failed >>> */
		/* We are in an inconsistent state now because writing the
		 * header apparently succeeded, while writing the
		 * data failed => return -2 instead of -1 here */
		return -2;
	}

	/* Return the total number of bytes written to the ringbuffer */
    return len + sizeof(size_t);
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_read_frame(ringbuffer_t* rb,
		uint8_t* header, size_t hlen, uint8_t* data, size_t max_flen) {

	/* Sanity check: make sure input pointer are ok */
    if (rb == 0 || header == 0 || data == 0) {
    	/* >>> Invalid pointer(s) >>> */
        return -1;
    }

	/* The length of the next frame in the ringbuffer */
	size_t len = 0;

	if (ringbuffer_peek(rb, (uint8_t*)&len, sizeof(size_t))
			== sizeof(size_t)) {
		/* >>> Reading frame length failed >>> */
		return -1;
	}

	if (len + sizeof(size_t) > rb->len) {
		/* >>> Frame seems longer than there is data in the ringbuffer >>> */
		return -1;
	}

	if (len > (hlen + max_flen)) {
		/* >>> USer provided buffer too small to hold frame data >>> */
		return -1;
	}

	/* discard length (we already know it) */
	ringbuffer_discard(rb, sizeof(size_t));

	/* the header */
	ringbuffer_read(rb, header, hlen);

	/* the frame */
	ringbuffer_read(rb, data, len - hlen);

	/* Return the length of the frame's data part */
    return len;
}


/*
 * ___________________________________________________________________________
 */
int ringbuffer_peek_frame(ringbuffer_t* rb,
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
