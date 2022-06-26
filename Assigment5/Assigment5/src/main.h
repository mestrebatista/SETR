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
 * \n Every thread has it's specific porpouse for the treatment of the signal.
 * \n It is also the fuction where the menu logic it's placed.
 * 
 * 
 * \n Snippet of code where the initialization is done:
 * @code
 * 
 *  k_sem_init(&sem_1a, 0, 1);
 *  k_sem_init(&sem_ab, 0, 1);
 *  k_sem_init(&sem_bc, 0, 1);
 *  k_sem_init(&sem_cd, 0, 1);
 *
 *  thread_1_tid = k_thread_create(&thread_1_data, thread_1_stack,
 *        K_THREAD_STACK_SIZEOF(thread_1_stack), thread_1_code,
 *        NULL, NULL, NULL, thread_1_prio, 0, K_NO_WAIT);
 *
 *    thread_A_tid = k_thread_create(&thread_A_data, thread_A_stack,
 *    K_THREAD_STACK_SIZEOF(thread_A_stack), thread_A_code,
 *    NULL, NULL, NULL, thread_A_prio, 0, K_NO_WAIT);    
 *
 *   thread_B_tid = k_thread_create(&thread_B_data, thread_B_stack,
 *       K_THREAD_STACK_SIZEOF(thread_B_stack), thread_B_code,
 *       NULL, NULL, NULL, thread_B_prio, 0, K_NO_WAIT);
 *   
 *   thread_C_tid = k_thread_create(&thread_C_data, thread_C_stack,
 *       K_THREAD_STACK_SIZEOF(thread_C_stack), thread_C_code,
 *       NULL, NULL, NULL, thread_C_prio, 0, K_NO_WAIT);
 *   
 *   thread_D_tid = k_thread_create(&thread_D_data, thread_D_stack,
 *       K_THREAD_STACK_SIZEOF(thread_D_stack), thread_D_code,
 *       NULL, NULL, NULL, thread_D_prio, 0, K_NO_WAIT);
 * 
 * @endcode
 * 
 * 
 */
void main(void);


/**
 * @brief 
 * 
 * The thread 1 has the porpouse of the logic behind the toogle betwen the manual mode and the automatic mode set by the problem.
 * \n It also serves deal with the use of the nordic buttons.
 *  
 * @code
    if(toggle_mode) 
    {
        if(dcToggleFlag1 == 1) 
        {       
           toggle_mode = 0;
           choice = '10';
           printf("\n\x1b[2J\r");
           printf("MODO MANUAL\n");
           dcToggleFlag1 = 0;
        }
        CHECK_DATE();
        k_sem_give(&sem_1a);
    } 
    else 
    {
       if(dcToggleFlag2 == 1) 
       {
          toggle_mode = 1;
          dcToggleFlag2 = 0;
       }
       if(dcToggleFlag3 == 1) 
       {
          ref-=10;
          if(ref<= 0) ref = 0;
          printk("ref -> %d\n",(100-ref));
          dcToggleFlag3 = 0;
       }
       if(dcToggleFlag4 == 1) 
       {
          ref+=10;
          if(ref>= 100) ref = 100;
          printk("ref -> %d\n",(100-ref));
          dcToggleFlag4 = 0;
       }
        var_cd = ref;
        k_sem_give(&sem_cd);
    }
 * @endcode
 * 
 */
void thread_1_code(void *argA , void *argB, void *argC);

/**
 * @brief 
 * 
 * The thread A has the porpouse of reading the adc sampling and implement it in a semaphore.
 * \n Then it deals with the timing to read such samples.
 *  
 * @code
    err = adc_sample();
    if (err) 
    {
      printk("adc_sample() failed with error code %d\n\r", err);
    } 
    else 
    {
      if (adc_sample_buffer[0] > 1023) 
      {
        //printk("adc reading out of range\n\r");
      } 
      else 
      {
        var_ab = adc_sample_buffer[0];
      }
    }
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
 *  The thread C deals with the values for the LED.
 *  \n Using the data from the mean done previously it's possible to calculate de nits for the led, this porcesses is supported by a controller for a better signal conditioning.
 *
 * @code
    nits = (uint16_t)(3000 * var_bc / ((float)1023));
    nits = -0.0455 * nits + 136.36;

    if (flag) {

      error = u_mid - u;
      integral += error;
      if (integral > 100)
        integral = 100; // Positive clamping to avoid wind-up
      if (integral < 0)
        integral = 0; // Negative clamping to avoid wind-up

      u += ((Kp * error) + (Ki * integral / 20));

      if (u > 100) {
        u_mid = 100;
      } else if (u < 0) {
        u_mid = 0;
      } else {
        u_mid = u;
      }
    }
 * @endcode
 * 
 */
void thread_C_code(void *argA , void *argB, void *argC);

/**
 * @brief 
 *  The thread D deals with the values for the PWM.
 *  \n Using the data from the mean done previously it is possible to set the output pwm.
 * 
 * @code
 *  k_sem_take(&sem_cd , K_FOREVER);
    ret = 0;

    ret = pwm_pin_set_usec(pwm0_dev, BOARDLED_PIN,
        pwmPeriod_us, (unsigned int)((pwmPeriod_us * var_cd) / 100), PWM_POLARITY_NORMAL);

    if (ret) 
    {
      printk("Error %d: failed to set pulse width\n", ret);
      return;
    }
 * @endcode
 * 
 */
void thread_D_code(void *argA , void *argB, void *argC);

/**
 * @brief Function for the setup of the hours, minutes and seconds.	
 */
void clock();

/**
 * @brief Function for the configuration of all the buttons.	
 */
void CONFIG_BUTTONS();

/**
 * @brief Function for the Menu prints.
 * 
 * @verbatim
  ->  Modo Automatico - SENSOR DE LUZ
  ->  1 - Modificar data atual
  ->  2 - Escolher periodo de ON/OFF
  ->  3 - Escolher luminosidade
  ->  4 - Verficar data atual
  ->  5 - Verificar PWM atual
  ->  6 - Verificar Periodo ON/OFF
  ->  7 - Mostrar tudo
  Escolha ->
   @endverbatim
 *
 */
void MENU();

/**
 * @brief Function for the change of the date.	
 */
void CHANGE_DATE ();

/**
 * @brief Function to establish the time where the automatic mode will operate.	
 */
void SET_DATE_PWM ();

/**
 * @brief Function to set the luminosity of the LED based on the pwm.	
 */
void SET_PWM();

/**
 * @brief Function only to verify if the input dates are in order.	
 */
void CHECK_DATE();


/**
 * @brief This function only serves to update the respective flag
 * and to inform that the button 1 was pressed.
 */ 
void but1press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
/**
 * @brief This function only serves to update the respective flag
 * and to inform that the button 2 was pressed.
 */ 
void but2press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
/**
 * @brief This function only serves to update the respective flag
 * and to inform that the button 3 was pressed.
 */ 
void but3press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
/**
 * @brief This function only serves to update the respective flag
 * and to inform that the button 4 was pressed.
 */ 
void but4press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins);







