/* Includes */
#include <stdio.h>
#include <stdlib.h>

int main (void)
{
    MyFIFOInit();

    printf("You have initiated a FIFO\n");

    int n = 0;

    while(n != 5) {
        printf("1 - ADD AN ELEMENT TO THE FIFO \n");
        printf("2 - REMOVES AN ELEMENT TO THE FIFO \n");
        printf("3 - RETURN THE OLDEST ELEMENT IN THE FIFO \n");
        printf("4 - NUMBER OF ELEMENTS IN THE FIFO\n");
        printf("5 - EXIT\n");

        printf("Please enter your option: ");
        scanf("%d",&n);
        printf("\n");
        printf("Valor lido : %d \n",n);

        if (n == 1) MyFIFOInsert();
        if (n == 2) MyFIFORemove();
        if (n == 3) MyFIFOPeep();
        if (n == 4) MyFIFOSize();

    }
    return 0;
}


void MyFIFOInit()
{
    int fifo [50];

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


void MyFIFORemove(int* fifo)
{
    for (int i = 0;i<sizeof(fifo);i++)
    {
        if(fifo[i] == NULL)
        {

        }
        else{
            fifo[i] = NULL;
            for (int a = i; a<sizeof(fifo);a++)
            {
                fifo[a] =fifo[a+1];
                fifo[a+1] = NULL;
            }
            printf("The first element of the FIFO has been removed");
            break;
        }
    }
}

void MyFIFOSize(int* fifo)
{
    // int size_fifo = sizeof(fifo);
    // printf("The size of the FIFO is ", size_fifo);
    printf("The Size of the FIFO is : \n",sizeof(fifo) );
}

void MyFIFOPeep(int* fifo)
{
    printf("Elemento mais antigo : %d \n",&fifo[0]);
}