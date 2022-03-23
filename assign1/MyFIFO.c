/** @file MyFIFO.c
 * @brief Main file with the creation of the queue and related functions.
 * 
 * This file contains the main function with the treatment of the queue for the user.
 * It also contains the functions for each interaction
 * 
 * @author José Mestre Batista and Renato Rocha
 * @date 23 March 2022
 */


/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include "MyFIFO.h"

int* MyFIFOInit();
void MyFIFOInsert(int*);
void MyFIFORemove(int*);
void MyFIFOPeep(int*);
void MyFIFOSize(int*);

int write_pointer;
int read_pointer;


int main (void)
{
    int* pfifo;
    pfifo = MyFIFOInit();

    printf("You have initiated a FIFO\n");

    int n = 0;

    while(n != 5) {

        for(int t = 0; t<10;t++)
        {
            printf("%d -> %d // ",t,pfifo[t]);
        }
        printf("\n");

        printf("1 - ADD AN ELEMENT TO THE FIFO \n");
        printf("2 - REMOVES AN ELEMENT TO THE FIFO \n");
        printf("3 - RETURN THE OLDEST ELEMENT IN THE FIFO \n");
        printf("4 - NUMBER OF ELEMENTS IN THE FIFO\n");
        printf("5 - EXIT\n");

        printf("Please enter your option: ");
        scanf("%d",&n);
        printf("\n");
        printf("Valor lido : %d \n",n);

        if (n == 1) MyFIFOInsert(pfifo);
        if (n == 2) MyFIFORemove(pfifo);
        if (n == 3) MyFIFOPeep(pfifo);
        if (n == 4) MyFIFOSize(pfifo);

    }
    return 0;
}


int* MyFIFOInit()
{
    static int fifo [10];
    for(int i = 0; i<10;i++)
    {
        fifo[i] = 0;
    }
    read_pointer = 0;
    write_pointer = 0;

    return fifo;
}

void MyFIFOInsert(int* fifo)
{
    int new_n;
    printf("What number you want to add?\n");
    scanf("%d",&new_n);
    if (fifo[write_pointer] == 0)
    {
        fifo[write_pointer] = new_n;
        write_pointer++;
        if(write_pointer == 10) write_pointer = 0;
    }
    else
    {
        printf("FIFO is full. Please Remove one before adding\n");
    }

}


void MyFIFORemove(int *fifo)
{
    fifo[read_pointer] = 0;
    read_pointer++;
    int check = 0;
    for(int i = 0;i<10;i++)
    {
        if(fifo[i] != 0)
            check = 1;
    }
    if(check == 0)
    {
        printf("FIFO is empty. Please Add an element before removing\n");
        write_pointer = 0;
        read_pointer = 0;
    }
}

void MyFIFOPeep(int *fifo)
{
    printf("Elemento mais antigo : %d \n",fifo[read_pointer]);
}

void MyFIFOSize(int *fifo)
{
    int count = 0;
    for(int i = 0;i<10;i++)
    {
        if(fifo[i] != 0) count++;
    }
    printf("The total number of elements in the fifo is -> %d \n",count);

}
