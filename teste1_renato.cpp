//
// Created by renat on 15/03/2022.
//

#include "teste1_renato.h"

int main(void)
{
    while (1)
    {
        for(int n=0;n<10;n++)
        {
            for(int m = 0;m<10;m++)
            {
                int o = add_numbers(n,m);
                printf("O numero somado é : %d \n",&o);
            }
        }

    }

}

int add_numbers(x,y)
{
   int z = x+y;
   return z;
}