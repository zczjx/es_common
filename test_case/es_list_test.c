/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: es_list.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-10-17    
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
#include <es_list.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	ES_LIST_HEAD(test_list);

	INIT_ES_LIST_HEAD(&test_list);
	printf("es_list test OK! \n");
	return 0;
}



