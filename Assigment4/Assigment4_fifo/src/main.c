/*
 * Paulo Pedreiras, 2022/03
 * Simple ADC example - nRF52840DK_nRF52840 board
 * 
 * 
 * Periodically reads ADC input and prints the corresponding raw and voltage value in the console
 * 
 * Adapted from: 
 *            https://devzone.nordicsemi.com/f/nordic-q-a/80685/using-saadc-in-nrf-connect-sdk/334204#334204
 *            https://github.com/simon-iversen/sdk-nrf/blob/light_controller/samples/light_controller/src/main.c
 * 
 *
 *      HW info
 *          https://infocenter.nordicsemi.com/topic/struct_nrf52/struct/nrf52840.html
 *          Section: nRF52840 Product Specification -> Peripherals -> GPIO / GPIOTE and SAADC 
 *          Board specific HW info can be found in the nRF52840_DK_User_Guide_20201203. I/O pins available at pg 27
 *
 *      Peripheral libs
 *          https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/reference/peripherals/index.html
 *          ADC     
 *
 *      NOTE 1:     **** NEVER APPLY MORE THAN 3 V (VDD) or negative voltages to ANx ****
 *
 *      NOTE 2:     In this board analog inputs are AIN{1,2,4,5,6,7} (see nRF52840_DK_User_Guide_20201203, page 28)
 *                  Some of the examples found in the internet are set to AIN0 and so do not work. The PCB also has a label "A0", which refers to Arduino and induces in error. 
 *    
 *      NOTE 3:     must add "CONFIG_ADC=y" to prj.conf
 * 
 */

/** @file main.c
 * @brief Main file with the creation of all of the threads and structures for the assigment.
 * 
 * This file contains all of the important data for the ADC, PWM, fifos and the logic behind the work.
 * 
 * @author Jose Mestre Batista and Renato Rocha
 * @date 31 May 2022
 */


#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/pwm.h>
#include <drivers/adc.h>
#include <string.h>

#include <devicetree.h>
#include <sys/printk.h>
#include <sys/__assert.h>
#include <timing/timing.h>
#include <stdio.h>
#include <stdlib.h>



/* ########################################################################################################################################## */
/* ##################################                            ADC DEFINITIONS                           ##################################*/
/* ########################################################################################################################################## */

#include <hal/nrf_saadc.h>
#define ADC_NID DT_NODELABEL(adc) 
#define ADC_RESOLUTION 10
#define ADC_GAIN ADC_GAIN_1_4
#define ADC_REFERENCE ADC_REF_VDD_1_4
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 40)
#define ADC_CHANNEL_ID 1  


#define ADC_CHANNEL_INPUT NRF_SAADC_INPUT_AIN1 

#define BUFFER_SIZE 1

/* Other defines */
#define TIMER_INTERVAL_MSEC 10000 /* Interval between ADC samples */

/* ADC channel configuration */
static const struct adc_channel_cfg my_channel_cfg = {
	.gain = ADC_GAIN,
	.reference = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id = ADC_CHANNEL_ID,
	.input_positive = ADC_CHANNEL_INPUT
};

/* Global vars for ADC */
struct k_timer my_timer;
const struct device *adc_dev = NULL;
static uint16_t adc_sample_buffer[BUFFER_SIZE];


/* Takes one sample */
static int adc_sample(void)
{
	int ret;
	const struct adc_sequence sequence = {
		.channels = BIT(ADC_CHANNEL_ID),
		.buffer = adc_sample_buffer,
		.buffer_size = sizeof(adc_sample_buffer),
		.resolution = ADC_RESOLUTION,
	};

	if (adc_dev == NULL) {
            printk("adc_sample(): error, must bind to adc first \n\r");
            return -1;
	}

	ret = adc_read(adc_dev, &sequence);
	if (ret) {
            printk("adc_read() failed with code %d\n", ret);
	}	

	return ret;
}


/* ########################################################################################################################################## */
/* ##################################                            PWM DEFINITIONS                           ##################################*/
/* ########################################################################################################################################## */

#define GPIO0_NID DT_NODELABEL(gpio0) 
#define PWM0_NID DT_NODELABEL(pwm0)
#define BOARDLED_PIN 0x0d

/* ########################################################################################################################################## */
/* ##################################                         THREAD DEFINITIONS                           ##################################*/
/* ########################################################################################################################################## */

#define STACK_SIZE 1024

#define thread_A_prio 1
#define thread_B_prio 1
#define thread_C_prio 1

#define thread_A_period 1000


K_THREAD_STACK_DEFINE(thread_A_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_B_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_C_stack, STACK_SIZE);

struct k_thread thread_A_data;
struct k_thread thread_B_data;
struct k_thread thread_C_data;

k_tid_t thread_A_tid;
k_tid_t thread_B_tid;
k_tid_t thread_C_tid;

void thread_A_code(void *, void *, void *);
void thread_B_code(void *, void *, void *);
void thread_C_code(void *, void *, void *);


struct k_fifo fifo_ab;
struct k_fifo fifo_bc;


struct data_item_t {
    void *fifo_reserved;    /* 1st word reserved for use by FIFO */
    uint16_t data;          /* Actual data */
};

/*############*/

/* ########################################################################################################################################## */
/* ###################################                         GLOBAL VARIABLES                           ###################################*/
/* ########################################################################################################################################## */

#define WINDOW_SIZE 10

volatile uint16_t media = 0;
volatile uint16_t buffer[WINDOW_SIZE] = {0,0,0,0,0,0,0,0,0,0};
volatile uint16_t media_2 = 0;
volatile uint16_t desvio = 0;


void main(void)
{    
    int err=0;   
    /* ADC setup: bind and initialize */
    adc_dev = device_get_binding(DT_LABEL(ADC_NID));
	if (!adc_dev) {
        printk("ADC device_get_binding() failed\n");
    } 
    err = adc_channel_setup(adc_dev, &my_channel_cfg);
    if (err) {
        printk("adc_channel_setup() failed with error code %d\n", err);
    }
    
    NRF_SAADC->TASKS_CALIBRATEOFFSET = 1;

    /*###################### FIFO's Creation #####################*/
    k_fifo_init(&fifo_ab);
    k_fifo_init(&fifo_bc);
    /*###############################################################*/
    
    /*####################### Tasks Creation #######################*/

    thread_A_tid = k_thread_create(&thread_A_data, thread_A_stack,
        K_THREAD_STACK_SIZEOF(thread_A_stack), thread_A_code,
        NULL, NULL, NULL, thread_A_prio, 0, K_NO_WAIT);

    thread_B_tid = k_thread_create(&thread_B_data, thread_B_stack,
        K_THREAD_STACK_SIZEOF(thread_B_stack), thread_B_code,
        NULL, NULL, NULL, thread_B_prio, 0, K_NO_WAIT);
    
    thread_C_tid = k_thread_create(&thread_C_data, thread_C_stack,
        K_THREAD_STACK_SIZEOF(thread_C_stack), thread_C_code,
        NULL, NULL, NULL, thread_C_prio, 0, K_NO_WAIT);

    /*##############################################################*/
    
    
    return;
}
/* ########################################################################################################################################## */
/* ##################################                         THREAD IMPLEMENTATION                        ##################################*/
/* ########################################################################################################################################## */

/* ###################    THREAD A    ######################*/

void thread_A_code(void *argA , void *argB, void *argC)
{
  int err=0;
  int count = 0;
  int i;
  int64_t fin_time=0, release_time = 0;
  int ret = 0;

  /*Fifo variable*/
  struct data_item_t data_ab;

  printk("Thread A Init\n\r");

  release_time = k_uptime_get() + thread_A_period;

  while(1)
  {
    printk("Thread A Activated\n\r");

    err = adc_sample();
    if (err) 
    {
      printk("adc_sample() failed with error code %d\n\r", err);
    } 
    else 
    {
      if (adc_sample_buffer[0] > 1023) 
      {
        printk("adc reading out of range\n\r");
      } 
      else 
      {
        data_ab.data= adc_sample_buffer[0];
        printk("adc reading  raw: %4u / %4u mV: \n\r", data_ab.data, (uint16_t)(1000 * data_ab.data * ((float)3 / 1023)));
      }
    }

    k_fifo_put(&fifo_ab, &data_ab);

    fin_time = k_uptime_get();

    if (fin_time < release_time) 
    {
      k_msleep(release_time - fin_time);
      release_time += thread_A_period;
    }
  }
  timing_stop();
}

/* ###################    THREAD B    ######################*/

void thread_B_code(void *argA , void *argB, void *argC)
{
  int err=0;
  uint16_t count = 0;
  int i;

  struct data_item_t *data_ab;
  struct data_item_t data_bc;

  printk("Thread B Init\n\r");

  while(1)
  {
   
   data_ab = k_fifo_get(&fifo_ab, K_FOREVER);
   //k_sem_take(&sem_ab, K_FOREVER);
   printk("Thread B Activated\n\r");

   media = 0;
   media_2 = 0;
   count = 0;

   for (i = 0; i < WINDOW_SIZE - 1; i++) 
   {
     buffer[i] = buffer[i + 1];
     /*printk("adc reading %d: raw:%4u / %4u mV: \n\r", i, buffer[i], (uint16_t)(1000 * buffer[i] * ((float)3 / 1023))); */
   }
     buffer[WINDOW_SIZE - 1] = data_ab->data;
     /*printk("adc reading %d: raw:%4u / %4u mV: \n\r", 9, buffer[9], (uint16_t)(1000 * buffer[9] * ((float)3 / 1023)));*/

   for (i = 0; i < WINDOW_SIZE; i++) 
   {
     media = media + buffer[i];
   }

   media = media / WINDOW_SIZE;
   desvio = 0.1 * media;

   for (i = 0; i < WINDOW_SIZE; i++) 
   {
     if ((media + desvio > buffer[i]) && (media - desvio < buffer[i])) {
       media_2 = media_2 + buffer[i];
       count = count + 1;
     }
   }
   if (count != 0) 
   {
     data_bc.data= media_2 / count;
   } 
   else 
   {
     data_bc.data= 0;
   }

   printk("adc reading %d: raw:%4u / %4u mV: \n\r", count, data_bc.data, (uint16_t)(1000 * data_bc.data * ((float)3 / 1023)));
    
   //data_bc.data=var_bc;
   k_fifo_put(&fifo_bc, &data_bc);
   
  }
}


/* ###################    THREAD C    ######################*/

void thread_C_code(void *argA , void *argB, void *argC)
{
  int i;
  int ret = 0;

  struct data_item_t *data_bc;

  printk("Thread C Init\n\r");

  const struct device *gpio0_dev;

  gpio0_dev = device_get_binding(DT_LABEL(GPIO0_NID));

  if (gpio0_dev == NULL) {
    printk("Error: Failed to bind to GPIO0\n\r");
    return;
  } else {
    printk("Bind to GPIO0 successfull \n\r");
  }

  ret = gpio_pin_configure(gpio0_dev, BOARDLED_PIN, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    printk("gpio_pin_configure() failed with error %d\n\r", ret);
    return;
  }

  const struct device *pwm0_dev;

  pwm0_dev = device_get_binding(DT_LABEL(PWM0_NID));

  if (pwm0_dev == NULL) {
    printk("Error: Failed to bind to PWM0\n r");
    return;
  } else {
    printk("Bind to PWM0 successful\n\r");
  }

  unsigned int pwmPeriod_us = 250000;                       

  while(1)
  {
    data_bc= k_fifo_get(&fifo_bc, K_FOREVER);

    printk("Thread C Activated\n\r");

    ret = 0;

    ret = pwm_pin_set_usec(pwm0_dev, BOARDLED_PIN,
        pwmPeriod_us, (unsigned int)((pwmPeriod_us * data_bc->data) / 1023), PWM_POLARITY_NORMAL);

    if (ret) 
    {
      printk("Error %d: failed to set pulse width\n", ret);
      return;
    }

    data_bc->data = (uint16_t)(1000 * data_bc->data * ((float)3 / 1023));
    printk("PWM -> %4u \n\r",(unsigned int)((pwmPeriod_us * data_bc->data) / 3000));
  }
}
/* ########################################################################################################################################## */
/* ##################################                      THREAD IMPLEMENTATION FINITO                    ##################################*/
/* ########################################################################################################################################## */