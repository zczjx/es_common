/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: es_fifo.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-10-18    
*                
* @brief:          
*                  
*                  
* @details:        
*                 
*    
*    
* @comment           
*******************************************************************************/
#include <es_fifo.h> 
#include <stdlib.h>
#include <string.h>

static void _es_fifo_init(struct es_fifo *fifo, void *buffer,
		unsigned int size)
{
	fifo->buffer = buffer;
	fifo->size = size;

	es_fifo_reset(fifo);
}

/**
 * es_fifo_init - initialize a FIFO using a preallocated buffer
 * @fifo: the fifo to assign the buffer
 * @buffer: the preallocated buffer to be used.
 * @size: the size of the internal buffer, this has to be a power of 2.
 *
 */
void es_fifo_init(struct es_fifo *fifo, void *buffer, unsigned int size)
{
	/* size must be a power of 2 */

	_es_fifo_init(fifo, buffer, size);
}


/**
 * es_fifo_alloc - allocates a new FIFO internal buffer
 * @fifo: the fifo to assign then new buffer
 * @size: the size of the buffer to be allocated, this have to be a power of 2.
 * @gfp_mask: get_free_pages mask, passed to kmalloc()
 *
 * This function dynamically allocates a new fifo internal buffer
 *
 * The size will be rounded-up to a power of 2.
 * The buffer will be release with es_fifo_free().
 * Return 0 if no error, otherwise the an error code
 */
int es_fifo_alloc(struct es_fifo *fifo, unsigned int size)
{
	unsigned char *buffer;


	buffer = malloc(size);
	if (!buffer) {
		_es_fifo_init(fifo, NULL, 0);
		return -1;
	}
	_es_fifo_init(fifo, buffer, size);
	return 0;
}

/**
 * es_fifo_free - frees the FIFO internal buffer
 * @fifo: the fifo to be freed.
 */
void es_fifo_free(struct es_fifo *fifo)
{
	free(fifo->buffer);
	_es_fifo_init(fifo, NULL, 0);
}

/**
 * es_fifo_skip - skip output data
 * @fifo: the fifo to be used.
 * @len: number of bytes to skip
 */
void es_fifo_skip(struct es_fifo *fifo, unsigned int len)
{
	if (len < es_fifo_len(fifo)) {
		__es_fifo_add_out(fifo, len);
		return;
	}
	es_fifo_reset_out(fifo);
}

static inline void __es_fifo_in_data(struct es_fifo *fifo,
		const void *from, unsigned int len, unsigned int off)
{
	unsigned int l;

	/*
	 * Ensure that we sample the fifo->out index -before- we
	 * start putting bytes into the es_fifo.
	 */


	off = __es_fifo_off(fifo, fifo->in + off);

	/* first put the data starting from fifo->in to buffer end */
	l = min(len, fifo->size - off);
	memcpy(fifo->buffer + off, from, l);
	/* then put the rest (if any) at the beginning of the buffer */
	memcpy(fifo->buffer, from + l, len - l);
}

static inline void __es_fifo_out_data(struct es_fifo *fifo,
		void *to, unsigned int len, unsigned int off)
{
	unsigned int l;

	/*
	 * Ensure that we sample the fifo->in index -before- we
	 * start removing bytes from the es_fifo.
	 */
	off = __es_fifo_off(fifo, fifo->out + off);

	/* first get the data from fifo->out until the end of the buffer */
	l = min(len, fifo->size - off);
	memcpy(to, fifo->buffer + off, l);

	/* then get the rest (if any) from the beginning of the buffer */
	memcpy(to + l, fifo->buffer, len - l);
}

unsigned int __es_fifo_in_n(struct es_fifo *fifo,
	const void *from, unsigned int len, unsigned int recsize)
{
	if (es_fifo_avail(fifo) < len + recsize)
		return len + 1;

	__es_fifo_in_data(fifo, from, len, recsize);
	return 0;
}

/**
 * es_fifo_in - puts some data into the FIFO
 * @fifo: the fifo to be used.
 * @from: the data to be added.
 * @len: the length of the data to be added.
 *
 * This function copies at most @len bytes from the @from buffer into
 * the FIFO depending on the free space, and returns the number of
 * bytes copied.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */
unsigned int es_fifo_in(struct es_fifo *fifo, const void *from,
				unsigned int len)
{
	len = min(es_fifo_avail(fifo), len);

	__es_fifo_in_data(fifo, from, len, 0);
	__es_fifo_add_in(fifo, len);
	return len;
}


unsigned int __es_fifo_out_n(struct es_fifo *fifo,
	void *to, unsigned int len, unsigned int recsize)
{
	if (es_fifo_len(fifo) < len + recsize)
		return len;

	__es_fifo_out_data(fifo, to, len, recsize);
	__es_fifo_add_out(fifo, len + recsize);
	return 0;
}


/**
 * es_fifo_out - gets some data from the FIFO
 * @fifo: the fifo to be used.
 * @to: where the data must be copied.
 * @len: the size of the destination buffer.
 *
 * This function copies at most @len bytes from the FIFO into the
 * @to buffer and returns the number of copied bytes.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */
unsigned int es_fifo_out(struct es_fifo *fifo, void *to, unsigned int len)
{
	len = min(es_fifo_len(fifo), len);

	__es_fifo_out_data(fifo, to, len, 0);
	__es_fifo_add_out(fifo, len);

	return len;
}

/**
 * es_fifo_out_peek - copy some data from the FIFO, but do not remove it
 * @fifo: the fifo to be used.
 * @to: where the data must be copied.
 * @len: the size of the destination buffer.
 * @offset: offset into the fifo
 *
 * This function copies at most @len bytes at @offset from the FIFO
 * into the @to buffer and returns the number of copied bytes.
 * The data is not removed from the FIFO.
 */
unsigned int es_fifo_out_peek(struct es_fifo *fifo, void *to, unsigned int len,
			    unsigned offset)
{
	len = min(es_fifo_len(fifo), len + offset);

	__es_fifo_out_data(fifo, to, len, offset);
	return len;
}





unsigned int __es_fifo_peek_generic(struct es_fifo *fifo, unsigned int recsize)
{
	if (recsize == 0)
		return es_fifo_avail(fifo);

	return __es_fifo_peek_n(fifo, recsize);
}

