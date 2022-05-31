/** @file main.h
 * @brief header support file 
 *
 * 
 * This file consists on the header for the main file.
 * \n It helps in the organization.
 * 
 * 
 * @author José Mestre Batista and Renato Rocha
 * @date 31 May 2022
 */

#ifndef _main_h
#define _main_h


/**
 * @brief 
 * 
 * Where the semaphores are initialized and the each thread is created.
 * \n Every thread has it's specific porpouse for the treatment of the signal
 * 
 * 
 * \n Snippet of code where the initialization is done:
 * @code
 * 
 * k_sem_init(&sem_ab, 0, 1);
 * k_sem_init(&sem_bc, 0, 1);
 *
 * thread_A_tid = k_thread_create(&thread_A_data, thread_A_stack,
 *      K_THREAD_STACK_SIZEOF(thread_A_stack), thread_A_code,
 *      NULL, NULL, NULL, thread_A_prio, 0, K_NO_WAIT);
 *
 * thread_B_tid = k_thread_create(&thread_B_data, thread_B_stack,
 *      K_THREAD_STACK_SIZEOF(thread_B_stack), thread_B_code,
 *      NULL, NULL, NULL, thread_B_prio, 0, K_NO_WAIT);
 *  
 * thread_C_tid = k_thread_create(&thread_C_data, thread_C_stack,
 *      K_THREAD_STACK_SIZEOF(thread_C_stack), thread_C_code,
 *      NULL, NULL, NULL, thread_C_prio, 0, K_NO_WAIT); 
 * 
 * @endcode
 * 
 * 
 */
void main(void);
/**
 * @brief 
 * 
 * The thread A has the porpouse of reading the adc sampling and implement it in a semaphore.
 * \n Then it deals with the timing to read such samples.
 *  
 * @code
 *  k_sem_give(&sem_ab);
 *
 *   fin_time = k_uptime_get();
 *
 *   if (fin_time < release_time) 
 *   {
 *     k_msleep(release_time - fin_time);
 *     release_time += thread_A_period;
 *   }
 * }
 * timing_stop();
 * @endcode
 * 
 */
void thread_A_code(void *argA , void *argB, void *argC);

/**
 * @brief 
 *  This thread deals with the information from the semphores and calculates the mean with samples that doesn't have too much deviation from the overall values.
 *  \n The resulting value it's given to the next semaphore
 * 
 * \n Main code for the mean
 * @code
 *for (i = 0; i < WINDOW_SIZE; i++) 
 *  {
 *    if ((media + desvio > buffer[i]) && (media - desvio < buffer[i])) {
 *      media_2 = media_2 + buffer[i];
 *      count = count + 1;
 *    }
 *  }
 *  if (count != 0) 
 *  {
 *    var_bc = media_2 / count;
 *  }  
 * @endcode
 * 
 */
void thread_B_code(void *argA , void *argB, void *argC);

/**
 * @brief 
 *  The thread C deals with the values for the PWM.
 *  \n Using the data from the mean done previously it is possible to see the output on the led of the board and on the screen.
 * 
 * @code
 * ret = pwm_pin_set_usec(pwm0_dev, BOARDLED_PIN,
 *       pwmPeriod_us, (unsigned int)((pwmPeriod_us * var_bc) / 1023), PWM_POLARITY_NORMAL);
 *
 *   if (ret) 
 *   {
 *     printk("Error %d: failed to set pulse width\n", ret);
 *    return;
 *   }
 *
 *   var_bc = (uint16_t)(1000 * var_bc * ((float)3 / 1023));
 *   printk("PWM -> %4u \n\r",(unsigned int)((pwmPeriod_us * var_bc) / 3000));
 * @endcode
 * 
 */
void thread_C_code(void *argA , void *argB, void *argC);





