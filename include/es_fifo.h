/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: es_fifo.h 
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
#ifndef _ES_FIFO_H_
#define _ES_FIFO_H_
#include <es_common.h>


struct es_fifo {
	unsigned char *buffer;	/* the buffer holding the data */
	unsigned int size;	/* the size of the allocated buffer */
	unsigned int in;	/* data is added at offset (in % size) */
	unsigned int out;	/* data is extracted from off. (out % size) */
};

/*
 * Macros for declaration and initialization of the es_fifo datatype
 */

/* helper macro */
#define __es_fifo_initializer(s, b) \
	(struct es_fifo) { \
		.size	= s, \
		.in	= 0, \
		.out	= 0, \
		.buffer = b \
	}

/**
 * DECLARE_ES_FIFO - macro to declare a es_fifo and the associated buffer
 * @name: name of the declared es_fifo datatype
 * @size: size of the fifo buffer. Must be a power of two.
 *
 * Note1: the macro can be used inside struct or union declaration
 * Note2: the macro creates two objects:
 *  A es_fifo object with the given name and a buffer for the es_fifo
 *  object named name##es_fifo_buffer
 */
#define DECLARE_ES_FIFO(name, size) \
union { \
	struct es_fifo name; \
	unsigned char name##es_fifo_buffer[size + sizeof(struct es_fifo)]; \
}

/**
 * INIT_ES_FIFO - Initialize a es_fifo declared by DECLARE_ES_FIFO
 * @name: name of the declared es_fifo datatype
 */
#define INIT_ES_FIFO(name) \
	name = __es_fifo_initializer(sizeof(name##es_fifo_buffer) - \
				sizeof(struct es_fifo), \
				name##es_fifo_buffer + sizeof(struct es_fifo))

/**
 * DEFINE_ES_FIFO - macro to define and initialize a es_fifo
 * @name: name of the declared es_fifo datatype
 * @size: size of the fifo buffer. Must be a power of two.
 *
 * Note1: the macro can be used for global and local es_fifo data type variables
 * Note2: the macro creates two objects:
 *  A es_fifo object with the given name and a buffer for the es_fifo
 *  object named name##es_fifo_buffer
 */
#define DEFINE_ES_FIFO(name, size) \
	unsigned char name##es_fifo_buffer[size]; \
	struct es_fifo name = __es_fifo_initializer(size, name##es_fifo_buffer)

extern  int es_fifo_alloc(struct es_fifo *fifo, unsigned int size);
extern void es_fifo_free(struct es_fifo *fifo);
extern unsigned int es_fifo_in(struct es_fifo *fifo,
				const void *from, unsigned int len);
extern  unsigned int es_fifo_out(struct es_fifo *fifo,
				void *to, unsigned int len);
extern  unsigned int es_fifo_out_peek(struct es_fifo *fifo,
				void *to, unsigned int len, unsigned offset);

/**
 * es_fifo_initialized - Check if es_fifo is initialized.
 * @fifo: fifo to check
 * Return %true if FIFO is initialized, otherwise %false.
 * Assumes the fifo was 0 before.
 */
static inline bool es_fifo_initialized(struct es_fifo *fifo)
{
	return fifo->buffer != NULL;
}

/**
 * es_fifo_reset - removes the entire FIFO contents
 * @fifo: the fifo to be emptied.
 */
static inline void es_fifo_reset(struct es_fifo *fifo)
{
	fifo->in = fifo->out = 0;
}

/**
 * es_fifo_reset_out - skip FIFO contents
 * @fifo: the fifo to be emptied.
 */
static inline void es_fifo_reset_out(struct es_fifo *fifo)
{
	fifo->out = fifo->in;
}

/**
 * es_fifo_size - returns the size of the fifo in bytes
 * @fifo: the fifo to be used.
 */
static inline  unsigned int es_fifo_size(struct es_fifo *fifo)
{
	return fifo->size;
}

/**
 * es_fifo_len - returns the number of used bytes in the FIFO
 * @fifo: the fifo to be used.
 */
static inline unsigned int es_fifo_len(struct es_fifo *fifo)
{
	register unsigned int	out;

	out = fifo->out;
	return fifo->in - out;
}

/**
 * es_fifo_is_empty - returns true if the fifo is empty
 * @fifo: the fifo to be used.
 */
static inline  int es_fifo_is_empty(struct es_fifo *fifo)
{
	return fifo->in == fifo->out;
}

/**
 * es_fifo_is_full - returns true if the fifo is full
 * @fifo: the fifo to be used.
 */
static inline  int es_fifo_is_full(struct es_fifo *fifo)
{
	return es_fifo_len(fifo) == es_fifo_size(fifo);
}

/**
 * es_fifo_avail - returns the number of bytes available in the FIFO
 * @fifo: the fifo to be used.
 */
static inline  unsigned int es_fifo_avail(struct es_fifo *fifo)
{
	return es_fifo_size(fifo) - es_fifo_len(fifo);
}

extern void es_fifo_skip(struct es_fifo *fifo, unsigned int len);


/*
 * __es_fifo_add_out internal helper function for updating the out offset
 */
static inline void __es_fifo_add_out(struct es_fifo *fifo,
				unsigned int off)
{
	
	fifo->out += off;
}

/*
 * __es_fifo_add_in internal helper function for updating the in offset
 */
static inline void __es_fifo_add_in(struct es_fifo *fifo,
				unsigned int off)
{
	
	fifo->in += off;
}

/*
 * __es_fifo_off internal helper function for calculating the index of a
 * given offeset
 */
static inline unsigned int __es_fifo_off(struct es_fifo *fifo, unsigned int off)
{
	return off & (fifo->size - 1);
}

/*
 * __es_fifo_peek_n internal helper function for determinate the length of
 * the next record in the fifo
 */
static inline unsigned int __es_fifo_peek_n(struct es_fifo *fifo,
				unsigned int recsize)
{
#define __ES_FIFO_GET(fifo, off, shift) \
	((fifo)->buffer[__es_fifo_off((fifo), (fifo)->out+(off))] << (shift))

	unsigned int l;

	l = __ES_FIFO_GET(fifo, 0, 0);

	if (--recsize)
		l |= __ES_FIFO_GET(fifo, 1, 8);

	return l;
#undef	__ES_FIFO_GET
}

/*
 * __es_fifo_poke_n internal helper function for storing the length of
 * the next record into the fifo
 */
static inline void __es_fifo_poke_n(struct es_fifo *fifo,
			unsigned int recsize, unsigned int n)
{
#define __ES_FIFO_PUT(fifo, off, val, shift) \
		( \
		(fifo)->buffer[__es_fifo_off((fifo), (fifo)->in+(off))] = \
		(unsigned char)((val) >> (shift)) \
		)

	__ES_FIFO_PUT(fifo, 0, n, 0);

	if (--recsize)
		__ES_FIFO_PUT(fifo, 1, n, 8);
#undef	__ES_FIFO_PUT
}

/*
 * __es_fifo_in_... internal functions for put date into the fifo
 * do not call it directly, use es_fifo_in_rec() instead
 */
extern unsigned int __es_fifo_in_n(struct es_fifo *fifo,
	const void *from, unsigned int n, unsigned int recsize);

/*
 * __es_fifo_out_... internal functions for get date from the fifo
 * do not call it directly, use es_fifo_out_rec() instead
 */
extern unsigned int __es_fifo_out_n(struct es_fifo *fifo,
	void *to, unsigned int reclen, unsigned int recsize);


/*
 * __es_fifo_peek_... internal functions for peek into the next fifo record
 * do not call it directly, use es_fifo_peek_rec() instead
 */
extern unsigned int __es_fifo_peek_generic(struct es_fifo *fifo,
				unsigned int recsize);

#endif /* ifndef _ES_FIFO_H_.2016-10-18 23:09:43 zcz */

