/** @file app.c
 * @brief app.c file brief decription 
 *
 * Follows the detailed description of app1.c. It is separated from 
 * the brief one by a blank line.
 * In this case app.c is the file that contains the main() function. 
 * 
 * @author Paulo Pedreiras
 * @date 3 March 2022
 * @bug No known bugs.
 */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include "module1.h"


/**
 * @brief Brief decription of main().
 *
 * Here it goes the long description of main()
 * main has no input arguments.
 * It declares two variables and calls function1() using them 
 * as argument. 
 * It then prints the result and returns.
 * 
 * @return main() always returns 0
 */
int main(void)
{
	/* Variable declaration and init */
	int var1=1, var2=2, res=0;  	
	
	/* processing ....*/  	
  	res=function1(var1,var2);
  	printf("var1 = %d, var2 =%d, function1(var1,var2) returns %d\n",var1,var2,res);
	
	/* And finish */
	return 0;
}
