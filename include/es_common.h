/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: es_common.h 
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
#ifndef _ES_COMMON_H_
#define _ES_COMMON_H_

#ifndef NULL
#define NULL (void *)(0)
#endif

#ifndef bool
typedef unsigned char bool;
#define es_false (0)
#define	es_true (1)
#endif

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})



/*
 * min()/max()/clamp() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })


#define ES_SUCCESS (0)
#define ES_FAIL (-1)
#define ES_INVALID_PARAM (-2)

typedef int  es_error_t;


#define ES_DEFAULT_CONFIG_PATH 		"/etc/es_default_config" /*use config file to parse*/
#define ES_DEFAULT_INTERNAL_DEV 	"es_internal_dev"
#define ES_DEFAULT_UNKNOW_DEV 		"es_unknow"

#if ES_DEBUG
#define ES_PRINTF(...) printf
#else
#define ES_PRINTF(...) do{} while(0)
#endif


#endif /* ifndef _ES_COMMON_H_.2016-10-18 21:18:14 zcz */


