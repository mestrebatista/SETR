/* Includes */
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	/* Variable declaration and init */
	//static int vector_fill = 0;
	volatile int arr[9];	
	
	for (int i = 0; i <=  sizeof arr; i++)
	{
		arr[i]=i;
		printf("valores");
	}
	
	/* processing ....*/  	
  	//res=function1(var1,var2);

	
	/* And finish */
	return 0;
}
