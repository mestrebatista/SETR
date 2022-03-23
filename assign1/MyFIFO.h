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
 * @brief Brief decription of function1 prototype.
 *
 * Here it goes the long description of fucntion 1.
 * Description of what the function does. Usually this part describes 
 * whatr the fiunction does and its interface, namelly input 
 * arguments and return value.
 * Verbatim text can also be used:
 * @verbatim 
 * This is my email:pbrp@ua.pt
 * @endverbatim
 * You can also include an example of usage:
 * @code
 * res = function1(param1, param2);
 * printf("res=%d\n",res);
 * @endcode
 * You can uset HTML tags e.g. to make <b>bold text</b>
 * @param arg1 Description of the first parameter of the function.
 * @param arg2 Description of the second parameter of the function.
 * @return Describe the function return value 
 */
int main(void);
/**
 * @brief Brief decription of function1 prototype.
 *
 * Here it goes the long description of fucntion 1.
 * Description of what the function does. Usually this part describes 
 * whatr the fiunction does and its interface, namelly input 
 * arguments and return value.
 * Verbatim text can also be used:
 * @verbatim 
 * This is my email:pbrp@ua.pt
 * @endverbatim
 * You can also include an example of usage:
 * @code
 * res = function1(param1, param2);
 * printf("res=%d\n",res);
 * @endcode
 * You can uset HTML tags e.g. to make <b>bold text</b>
 * @param arg1 Description of the first parameter of the function.
 * @param arg2 Description of the second parameter of the function.
 * @return Describe the function return value 
 */
int* MyFIFOInit();
/**
 * @brief Brief decription of function1 prototype.
 *
 * Here it goes the long description of fucntion 1.
 * Description of what the function does. Usually this part describes 
 * whatr the fiunction does and its interface, namelly input 
 * arguments and return value.
 * Verbatim text can also be used:
 * @verbatim 
 * This is my email:pbrp@ua.pt
 * @endverbatim
 * You can also include an example of usage:
 * @code
 * res = function1(param1, param2);
 * printf("res=%d\n",res);
 * @endcode
 * You can uset HTML tags e.g. to make <b>bold text</b>
 * @param arg1 Description of the first parameter of the function.
 * @param arg2 Description of the second parameter of the function.
 * @return Describe the function return value 
 */
void MyFIFOInsert(int*);
/**
 * @brief Brief decription of function1 prototype.
 *
 * Here it goes the long description of fucntion 1.
 * Description of what the function does. Usually this part describes 
 * whatr the fiunction does and its interface, namelly input 
 * arguments and return value.
 * Verbatim text can also be used:
 * @verbatim 
 * This is my email:pbrp@ua.pt
 * @endverbatim
 * You can also include an example of usage:
 * @code
 * res = function1(param1, param2);
 * printf("res=%d\n",res);
 * @endcode
 * You can uset HTML tags e.g. to make <b>bold text</b>
 * @param arg1 Description of the first parameter of the function.
 * @param arg2 Description of the second parameter of the function.
 * @return Describe the function return value 
 */
void MyFIFORemove(int*);
/**
 * @brief Brief decription of function1 prototype.
 *
 * Here it goes the long description of fucntion 1.
 * Description of what the function does. Usually this part describes 
 * whatr the fiunction does and its interface, namelly input 
 * arguments and return value.
 * Verbatim text can also be used:
 * @verbatim 
 * This is my email:pbrp@ua.pt
 * @endverbatim
 * You can also include an example of usage:
 * @code
 * res = function1(param1, param2);
 * printf("res=%d\n",res);
 * @endcode
 * You can uset HTML tags e.g. to make <b>bold text</b>
 * @param arg1 Description of the first parameter of the function.
 * @param arg2 Description of the second parameter of the function.
 * @return Describe the function return value 
 */
void MyFIFOPeep(int*);
/**
 * @brief Brief decription of function1 prototype.
 *
 * Here it goes the long description of fucntion 1.
 * Description of what the function does. Usually this part describes 
 * whatr the fiunction does and its interface, namelly input 
 * arguments and return value.
 * Verbatim text can also be used:
 * @verbatim 
 * This is my email:pbrp@ua.pt
 * @endverbatim
 * You can also include an example of usage:
 * @code
 * res = function1(param1, param2);
 * printf("res=%d\n",res);
 * @endcode
 * You can uset HTML tags e.g. to make <b>bold text</b>
 * @param arg1 Description of the first parameter of the function.
 * @param arg2 Description of the second parameter of the function.
 * @return Describe the function return value 
 */
void MyFIFOSize(int*);
