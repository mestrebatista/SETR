/** @file MyFIFO.h
 * @brief header support file 
 *
 * 
 * This file consists on the header for the MyFIFO file.
 * It helps in the organization.
 * 
 * 
 * @author José Mestre Batista and Renato Rocha
 * @date 23 March 2022
 */

#ifndef _MyFIFO_h
#define _MyFIFO_h



/**
 * @brief brief description of elem typedef struct.
 * 
 *
 * Then it may follow a detailed explanation of the elem structure.
 */ 
typedef struct 
{
    
    int write_pointer; /**< Some info for this elem struct member */
	int read_pointer;  /**< Some info for this elem struct member */
} elem;


/**
 * @brief Interface UI
 * This function call all other functions, dependent to what the users
 * chooses. It also prints the FIFO after any interaction from the user.
 * You can also include an example of usage:
 * @code
 *   int* pfifo;
 *   pfifo = MyFIFOInit();
 *
 *   printf("You have initiated a FIFO\n");
 *
 *   int n = 0;
 *
 *   while(n != 5) {
 *
 *       for(int t = 0; t<10;t++)
 *       {
 *           printf("%d -> %d // ",t,pfifo[t]);
 *       }
 +       printf("\n");
 *
 *       printf("1 - ADD AN ELEMENT TO THE FIFO \n");
 *       printf("2 - REMOVES AN ELEMENT TO THE FIFO \n");
 *       printf("3 - RETURN THE OLDEST ELEMENT IN THE FIFO \n");
 *       printf("4 - NUMBER OF ELEMENTS IN THE FIFO\n");
 *       printf("5 - EXIT\n");
 *
 *       printf("Please enter your option: ");
 *       scanf("%d",&n);
 *       printf("\n");
 *       printf("Valor lido : %d \n",n);
 *
 *       if (n == 1) MyFIFOInsert(pfifo);
 *       if (n == 2) MyFIFORemove(pfifo);
 *       if (n == 3) MyFIFOPeep(pfifo);
 *       if (n == 4) MyFIFOSize(pfifo);
 *
 *   }
 *   return 0;
 * @endcode
int main(void);
/**
 * @brief initiates the fifo
 * This function creates an array of integers with a size of 10 elements
 * and puts all the elements to 0. It also sets the read_pointer variable
 * and the write_pointer variable to 0.
 * You can also include an example of usage:
 * @code
 *    static int fifo [10];
 *   for(int i = 0; i<10;i++)
 *   {
 *       fifo[i] = 0;
 *   }
 *   read_pointer = 0;
 *   write_pointer = 0;
 *
 *   return fifo;
 * @endcode
 * @return Returns an array of integers full of 0
 */
int* MyFIFOInit();
/**
 * @brief Adds an element to the FIFO
 * This function asks the user to choose a number to add to the FIFO
 * and then the function adds it to the next position of the FIFO
 * The function also checks if the FIFO is full, and warns the user
 * if that is the case
 * You can also include an example of usage:
 * @code
 *   int new_n;
 *   printf("What number you want to add?\n");
 *   scanf("%d",&new_n);
 *   if (fifo[write_pointer] == 0)
 *   {
 *       fifo[write_pointer] = new_n;
 *       write_pointer++;
 *       if(write_pointer == 10) write_pointer = 0;
 *   }
 *   else
 *   {
 *       printf("FIFO is full. Please Remove one before adding\n");
 *   }
 * @endcode
 * @param fifo array of numbers of FIFO
 */
void MyFIFOInsert(int*);
/**
 * @brief Removes the oldest element from the FIFO
 * This function uses the read_pointer to find out the oldest element
 * on the FIFO, and removes it from the FIFO. It also checks if the FIFO is empty and
 * if it is, both read_pointer and write_pointer variables return to 0.
 * @code
 *   fifo[read_pointer] = 0;
 *   read_pointer++;
 *   int check = 0;
 *   for(int i = 0;i<10;i++)
 *   {
 *       if(fifo[i] != 0)
 *           check = 1;
 *   }
 *   if(check == 0)
 *   {
 *       printf("FIFO is empty. Please Add an element before removing\n");
 *       write_pointer = 0;
 *       read_pointer = 0;
 *   }
 * @endcode
 * @param fifo array of numbers of FIFO
 */
void MyFIFORemove(int*);
/**
 * @brief Returns the oldest element on the FIFO, but does not remove it
 * This function uses the read_pointer to find out the oldest element
 * on the FIFO, and prints it on the terminal
 * @code
 * printf("Elemento mais antigo : %d \n",fifo[read_pointer]);
 * @endcode
 * @param fifo array of numbers of FIFO
 */
void MyFIFOPeep(int*);
/**
 * @brief Returns the number of elements on the FIFO
 * This function counts the number of occupied positions on the FIFO,
 * for this, the function uses a for loop to check all the positions
 * @code
 *     int count = 0;
 *   for(int i = 0;i<10;i++)
 *   {
 *       if(fifo[i] != 0) count++;
 *   }
 *   printf("The total number of elements in the fifo is -> %d \n",count);
 * @endcode
 * You can uset HTML tags e.g. to make <b>bold text</b>
 * @param fifo array of numbers of FIFO
 */
void MyFIFOSize(int*);
#endif
