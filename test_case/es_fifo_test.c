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
#include <es_fifo.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int ret = 0;
	int i = 0;
	struct es_fifo fifo;
	char arr[] = {4, 2, 3 ,1};
	char tmp[4];

	ret = es_fifo_alloc(&fifo, 1024);
	printf("alloc ret is %d \n", ret);
	ret = es_fifo_in(&fifo, arr, sizeof(arr));
	printf("fifo in ret is %d \n", ret);

	ret = es_fifo_out(&fifo, tmp, sizeof(tmp));
	printf("fifo out ret is %d \n", ret);
	for(i = 0; i < 4; i++)
	{
		printf(" %d \n", tmp[i]);
	}
	
	es_fifo_free(&fifo);
	printf("es_fifo test OK! \n");
	return 0;
}



