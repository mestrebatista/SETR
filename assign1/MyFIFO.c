/* Includes */
#include <stdio.h>
#include <stdlib.h>

int main(void)
{	
	/* And finish */
	return 0;
}

void MyFIFOInit()
{
    int fifo [50];

    return fifo;
}

void MyFIFOInsert(int* fifo)
{
    volatile int count=sizeof(fifo);
    volatile int new_n;

    for (size_t i = 0; i < count; i++)
    {
        if (fifo[i] == NULL)
        {
            printf("What number you want to add?\n");
            scanf("%d",&new_n);
            break;
        }
        
    }    
    return 0;
}


void MyFIFORemove()
{

    return 0;
}

void MyFIFOPeep()
{

    return 0;
}

void MyFIFOSize()
{

    return 0;
}