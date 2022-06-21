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
 * This file contains all of the important data for the ADC, PWM, semaphores and the logic behind the work.
 * 
 * @author Jose Mestre Batista and Renato Rocha
 * @date 23 June 2022
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/pwm.h>
#include <drivers/adc.h>
#include <drivers/uart.h>
#include <string.h>

#include <devicetree.h>
#include <sys/printk.h>
#include <sys/__assert.h>
#include <timing/timing.h>
#include <stdio.h>
#include <stdlib.h>
#include <console/console.h>


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
/* ##################################                            PWM DEFINITIONS                           ################################## */
/* ########################################################################################################################################## */

#define GPIO0_NID DT_NODELABEL(gpio0) 
#define PWM0_NID DT_NODELABEL(pwm0)
#define BOARDLED_PIN 0x0d

/* ########################################################################################################################################## */
/* ##################################                         THREAD DEFINITIONS                           ################################## */
/* ########################################################################################################################################## */

#define STACK_SIZE 1024

#define thread_1_prio 1
#define thread_A_prio 1
#define thread_B_prio 1
#define thread_C_prio 1
#define thread_D_prio 1

#define thread_1_period 500
/*#define thread_B_period 10000*/
/*#define thread_C_period 10000*/

K_THREAD_STACK_DEFINE(thread_1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_A_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_B_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_C_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_D_stack, STACK_SIZE);

struct k_thread thread_1_data;
struct k_thread thread_A_data;
struct k_thread thread_B_data;
struct k_thread thread_C_data;
struct k_thread thread_D_data;

k_tid_t thread_1_tid;
k_tid_t thread_A_tid;
k_tid_t thread_B_tid;
k_tid_t thread_C_tid;
k_tid_t thread_D_tid;

void thread_1_code(void *argA, void *argB, void *argC);
void thread_A_code(void *argA, void *argB, void *argC);
void thread_B_code(void *argA, void *argB, void *argC);
void thread_C_code(void *argA, void *argB, void *argC);
void thread_D_code(void *argA, void *argB, void *argC);

/* Semaphores */
int var_ab = 0;
int var_bc = 0;
int var_cd = 0;

struct k_sem sem_1a;
struct k_sem sem_ab;
struct k_sem sem_bc;
struct k_sem sem_cd;

/*############*/

/* ########################################################################################################################################## */
/* ###################################                         BUTTON VARIABLES                           ###################################*/
/* ########################################################################################################################################## */

#define BOARDBUT1 0xb /* Pin at which BUT1 is connected. Addressing is direct (i.e., pin number) */
#define BOARDBUT2 0xc
#define BOARDBUT3 0x18
#define BOARDBUT4 0x19

static struct gpio_callback but1_cb_data; /* Callback structure */
static struct gpio_callback but2_cb_data; /* Callback structure */
static struct gpio_callback but3_cb_data; /* Callback structure */
static struct gpio_callback but4_cb_data; /* Callback structure */

volatile int dcToggleFlag1 = 0; /* Flag to signal a BUT1 press */
volatile int dcToggleFlag2 = 0; /* Flag to signal a BUT1 press */
volatile int dcToggleFlag3 = 0; /* Flag to signal a BUT1 press */
volatile int dcToggleFlag4 = 0; /* Flag to signal a BUT1 press */

void but1press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{    
    /* Inform that button was hit
    printk("But1 pressed at %d\n\r", k_cycle_get_32()); */
    
    /* Update Flag*/
    dcToggleFlag1 = 1;
}

void but2press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{    
    /* Inform that button was hit
    printk("But2 pressed at %d\n\r", k_cycle_get_32()); */
    
    /* Update Flag*/
    dcToggleFlag2 = 1;
}

void but3press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{    
    /* Inform that button was hit
    printk("But3 pressed at %d\n\r", k_cycle_get_32());*/
    
    /* Update Flag*/
    dcToggleFlag3 = 1;
}

void but4press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{    
    /* Inform that button was hit
    printk("But4 pressed at %d\n\r", k_cycle_get_32());*/
    
    /* Update Flag*/
    dcToggleFlag4 = 1;
}

/* ########################################################################################################################################## */
/* ###################################                         GLOBAL VARIABLES                           ###################################*/
/* ########################################################################################################################################## */

#define WINDOW_SIZE 10
#define ref_max 100
#define ref_min 0

volatile uint16_t media = 0;
volatile uint16_t buffer[WINDOW_SIZE] = {0,0,0,0,0,0,0,0,0,0};
volatile uint16_t media_2 = 0;
volatile uint16_t desvio = 0;


volatile int count = 0;
volatile int seconds = 0;
int minutes = 0;
int hours = 0;
int days = 0;

volatile int seconds_init = 0;
int minutes_init = 0;
int hours_init = 0;
int days_init = 0;

volatile int seconds_finito = 0;
int minutes_finito = 0;
int hours_finito = 0;
int days_finito = 0;

int ref = 50;

char choice;
int scan;
int toggle_mode = 1;
int flag_end_binds = 0;
int flag_console = 0;

volatile int nits;
volatile int pwm;
int nits_pret=0;


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

    CONFIG_BUTTONS();

    /*###################### Semaphore Creation #####################*/
    k_sem_init(&sem_1a, 0, 1);
    k_sem_init(&sem_ab, 0, 1);
    k_sem_init(&sem_bc, 0, 1);
    k_sem_init(&sem_cd, 0, 1);
    /*###############################################################*/
    
    /*####################### Thread Creation #######################*/

    thread_1_tid = k_thread_create(&thread_1_data, thread_1_stack,
        K_THREAD_STACK_SIZEOF(thread_1_stack), thread_1_code,
        NULL, NULL, NULL, thread_1_prio, 0, K_NO_WAIT);

    thread_A_tid = k_thread_create(&thread_A_data, thread_A_stack,
    K_THREAD_STACK_SIZEOF(thread_A_stack), thread_A_code,
    NULL, NULL, NULL, thread_A_prio, 0, K_NO_WAIT);    

    thread_B_tid = k_thread_create(&thread_B_data, thread_B_stack,
        K_THREAD_STACK_SIZEOF(thread_B_stack), thread_B_code,
        NULL, NULL, NULL, thread_B_prio, 0, K_NO_WAIT);
    
    thread_C_tid = k_thread_create(&thread_C_data, thread_C_stack,
        K_THREAD_STACK_SIZEOF(thread_C_stack), thread_C_code,
        NULL, NULL, NULL, thread_C_prio, 0, K_NO_WAIT);
    
    thread_D_tid = k_thread_create(&thread_D_data, thread_D_stack,
        K_THREAD_STACK_SIZEOF(thread_D_stack), thread_D_code,
        NULL, NULL, NULL, thread_D_prio, 0, K_NO_WAIT);


    /*##############################################################*/
    console_init();  
    printf("\n\x1b[2J\r");
    while(1) {
          MENU();
          choice = console_getchar();
          console_putchar(choice);
          if(choice == '1') {
            CHANGE_DATE();
          } else if(choice == '2') {
            SET_DATE_PWM();
          } else if(choice == '3') {
            SET_PWM();
          } else if(choice == '4') {
            printk("\n");
            printk("Hora Atual -> %d:%d:%d\n", hours, minutes, seconds);
          } else if(choice == '5') {
            printk("\n");
            printk("ACTUAL -> %d    //    LUMINOSITY -> %d    //    PWM -> %d \n",ref, nits,pwm);
          } else if(choice == '6') {
              printk("\n");
              printk("Hora Inicial -> %d:%d:%d\n", hours_init, minutes_init, seconds_init);
              printk("Hora Final -> %d:%d:%d\n", hours_finito, minutes_finito, seconds_finito);
          } else break;
        
    }
    return;
}
/* ########################################################################################################################################## */
/* ##################################                         THREAD IMPLEMENTATION                        ##################################*/
/* ########################################################################################################################################## */



/* ###################    THREAD 1    ######################*/

void thread_1_code(void *argA , void *argB, void *argC)
{
  int err=0;
  int count = 0;
  int i;
  int64_t fin_time=0, release_time = 0;
  int ret = 0;


  /*printk("Thread 1 Init\n\r");*/       

  release_time = k_uptime_get() + thread_1_period;

  while(1)
  {
    /*printk("Thread 1 Activated\n\r");*/

    if(flag_console == 0) clock();

    
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
        /*printk("aqui");*/
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


    fin_time = k_uptime_get();

    if (fin_time < release_time) 
    {
      k_msleep(release_time - fin_time);
      release_time += thread_1_period;
    }
  }
  timing_stop();
}


/* ###################    THREAD A    ######################*/

void thread_A_code(void *argA , void *argB, void *argC)
{
  int err=0;
  int count = 0;
  int i;
  int ret = 0;

  /*printk("Thread A Init\n\r");*/

  while(1)
  {
    k_sem_take(&sem_1a, K_FOREVER);
    /*printk("Thread A Activated\n\r");*/

    err = adc_sample();
    if (err) 
    {
      printk("adc_sample() failed with error code %d\n\r", err);
    } 
    else 
    {
      if (adc_sample_buffer[0] > 1023) 
      {
        /*printk("adc reading out of range\n\r");*/
      } 
      else 
      {
        var_ab = adc_sample_buffer[0];
        /*printk("adc reading  raw: %4u / %4u mV: \n\r", var_ab, (uint16_t)(3000 * var_ab /((float)1023)));*/
      }
    }

    k_sem_give(&sem_ab);
  }
}

/* ###################    THREAD B    ######################*/

void thread_B_code(void *argA , void *argB, void *argC)
{
  int err=0;
  uint16_t count = 0;
  int i;

  /*printk("Thread B Init\n\r");*/

  while(1)
  {

   k_sem_take(&sem_ab, K_FOREVER);
   /*printk("Thread B Activated\n\r");*/

   media = 0;
   media_2 = 0;
   count = 0;

     for (i = 0; i < WINDOW_SIZE - 1; i++) 
     {
       buffer[i] = buffer[i + 1];
       /*printk("adc reading %d: raw:%4u / %4u mV: \n\r", i, buffer[i], (uint16_t)(1000 * buffer[i] * ((float)3 / 1023))); */
     }
       buffer[WINDOW_SIZE - 1] = var_ab;
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
     var_bc = media_2 / count;
   } 
   else 
   {
     var_bc = 0;
   }
    
   /*printk("adc reading %d: raw:%4u / %4u mV: \n\r", count, var_bc, (uint16_t)(3000 * var_bc / ((float) 1023)));*/

   k_sem_give(&sem_bc);
  }
}


/* ###################    THREAD C    ######################*/

void thread_C_code(void *argA , void *argB, void *argC)
{
  int i;
  int ret = 0;
  int u;
  int flag = 0;

  /*printk("Thread C Init\n\r");*/

  while(1)
  {
    k_sem_take(&sem_bc , K_FOREVER);

    /*printk("Thread C Activated\n\r");*/

    ret = 0;
    flag = 1;

    nits = (uint16_t)(3000 * var_bc / ((float) 1023));
    nits = -0.0455*nits + 136.36; 
    
    if(flag){
      if(nits>ref) u--;
      else if(nits<ref) u++;

      if(u >= 100) u = 100;
      if(u<=0) u = 0;
    }

    /*printk("ref -> %d | nits -> %d | u -> %d",ref,nits,u);*/
    var_cd = 100 - u;
    pwm = u;
      
    k_sem_give(&sem_cd);
  }
}

void thread_D_code(void *argA , void *argB, void *argC)
{
  int i;
  int ret = 0;
  int pwm = 0;

  /*printk("Thread D Init\n\r");*/

  const struct device *gpio0_dev;

  gpio0_dev = device_get_binding(DT_LABEL(GPIO0_NID));

  if (gpio0_dev == NULL) {
    printk("Error: Failed to bind to GPIO0\n\r");
    return;
  } else {
    /*printk("Bind to GPIO0 successfull \n\r");*/
  }

  ret = gpio_pin_configure(gpio0_dev, BOARDLED_PIN, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    printk("gpio_pin_configure() failed with error %d\n\r", ret);
    return;
  }

  const struct device *pwm0_dev;

  pwm0_dev = device_get_binding(DT_LABEL(PWM0_NID));

  if (pwm0_dev == NULL) {
    /*printk("Error: Failed to bind to PWM0\n r");*/
    return;
  } else {
    /*printk("Bind to PWM0 successful\n\r");*/
  }

  unsigned int pwmPeriod_us = 100;                       

  while(1)
  {
    k_sem_take(&sem_cd , K_FOREVER);

    /*printk("Thread D Activated\n\r");*/

    ret = 0;

    ret = pwm_pin_set_usec(pwm0_dev, BOARDLED_PIN,
        pwmPeriod_us, (unsigned int)((pwmPeriod_us * var_cd) / 100), PWM_POLARITY_NORMAL);

    if (ret) 
    {
      printk("Error %d: failed to set pulse width\n", ret);
      return;
    }

  }
}
/* ########################################################################################################################################## */
/* ##################################                      THREAD IMPLEMENTATION FINITO                    ##################################*/
/* ########################################################################################################################################## */


void clock(){
  count += thread_1_period;

  if (count >= 1000){
     seconds++;
     count -= 1000;
  }
  if (seconds >= 60){
     minutes++;
     seconds -= 60;
  }
  if (minutes >= 60){
     hours++;
     minutes -= 60;
  }
  if (hours >= 24){
     days++;
     minutes -= 24;
  }
/*
  printk("seconds -> %d | minutes -> %d | hours -> %d\n",seconds,minutes,hours); */
}

void CONFIG_BUTTONS()
{
    int ret = 0;

    const struct device *gpio0_dev;
    gpio0_dev = device_get_binding(DT_LABEL(GPIO0_NID));
    ret = gpio_pin_configure(gpio0_dev, BOARDBUT1, GPIO_INPUT | GPIO_PULL_UP);
    ret = gpio_pin_configure(gpio0_dev, BOARDBUT2, GPIO_INPUT | GPIO_PULL_UP);
    ret = gpio_pin_configure(gpio0_dev, BOARDBUT3, GPIO_INPUT | GPIO_PULL_UP);
    ret = gpio_pin_configure(gpio0_dev, BOARDBUT4, GPIO_INPUT | GPIO_PULL_UP);

    if (ret < 0) {
        printk("Error %d: Failed to configure BUT 1 \n\r", ret);
	return;
    }

    ret = gpio_pin_interrupt_configure(gpio0_dev, BOARDBUT1, GPIO_INT_EDGE_TO_ACTIVE);
    ret = gpio_pin_interrupt_configure(gpio0_dev, BOARDBUT2, GPIO_INT_EDGE_TO_ACTIVE);
    ret = gpio_pin_interrupt_configure(gpio0_dev, BOARDBUT3, GPIO_INT_EDGE_TO_ACTIVE);
    ret = gpio_pin_interrupt_configure(gpio0_dev, BOARDBUT4, GPIO_INT_EDGE_TO_ACTIVE);

    if (ret != 0) {
	printk("Error %d: failed to configure interrupt on BUT1 pin \n\r", ret);
	return;
    }

    gpio_init_callback(&but1_cb_data, but1press_cbfunction, BIT(BOARDBUT1));
    gpio_add_callback(gpio0_dev, &but1_cb_data);

    gpio_init_callback(&but2_cb_data, but2press_cbfunction, BIT(BOARDBUT2));
    gpio_add_callback(gpio0_dev, &but2_cb_data);

    gpio_init_callback(&but3_cb_data, but3press_cbfunction, BIT(BOARDBUT3));
    gpio_add_callback(gpio0_dev, &but3_cb_data);

    gpio_init_callback(&but4_cb_data, but4press_cbfunction, BIT(BOARDBUT4));
    gpio_add_callback(gpio0_dev, &but4_cb_data);
}

void MENU()
{
  printk("Modo Automatico - SENSOR DE LUZ\n");
  printk("1 - Modificar data atual\n");
  printk("2 - Escolher periodo de ON/OFF\n");
  printk("3 - Escolher luminosidade\n");
  printk("4 - Verficar data atual\n");
  printk("5 - Verificar PWM atual\n");
  printk("Escolha ->  ");
}

void CHANGE_DATE () 
{
    flag_console = 1;
    hours = 0;
    minutes = 0;
    seconds = 0;
    printk("\n");
    printk("Coloque a hora (ex: 21): ");
    for (int i = 0; i <= 2; i++) {
      scan = console_getchar();
      console_putchar(scan);
      scan -= 48;
      if ((scan <= 9) && (scan >= 0)) {
        hours = hours * 10 + scan;
      } else
        break;
    }
    printk("\n");
    printk("Coloque os minutos (ex: 21): ");
    for (int i = 0; i <= 2; i++) {
      scan = console_getchar();
      console_putchar(scan);
      scan -= 48;
      if ((scan <= 9) && (scan >= 0)) {
        minutes = minutes * 10 + scan;
      } else
        break;
    }
    printk("\n");
    printk("Coloque os segundos(ex: 21): ");
    for (int i = 0; i <= 2; i++) {
      scan = console_getchar();
      console_putchar(scan);
      scan -= 48;
      if ((scan <= 9) && (scan >= 0)) {
        seconds = seconds * 10 + scan;
      } else
        break;
    }
    printk("\n");
    printk("Hora Atual -> %d:%d:%d\n", hours, minutes, seconds);
    flag_console = 0;
}

void SET_DATE_PWM () 
{
    hours_init = 0;
    minutes_init = 0;
    seconds_init = 0;
    scan = 0;
    printk("\n");
    printk("Coloque a hora de inicio(ex: 21): ");
    for (int i = 0; i <= 2; i++) {
      scan = console_getchar();
      console_putchar(scan);
      scan -= 48;
      if ((scan <= 9) && (scan >= 0)) {
        hours_init = hours_init * 10 + scan;
      } else
        break;
    }
    scan = 0;
    printk("\n");
    printk("Coloque os minutos de inicio (ex: 21): ");
    for (int i = 0; i <= 2; i++) {
      scan = console_getchar();
      console_putchar(scan);
      scan -= 48;
      if ((scan <= 9) && (scan >= 0)) {
        minutes_init = minutes_init * 10 + scan;
      } else
        break;
    }
    scan = 0;
    printk("\n");
    printk("Coloque os segundos de inicio (ex: 21): ");
    for (int i = 0; i <= 2; i++) {
      scan = console_getchar();
      console_putchar(scan);
      scan -= 48;
      if ((scan <= 9) && (scan >= 0)) {
        seconds_init = seconds_init * 10 + scan;
      } else
        break;
    }
    hours_finito = 0;
    minutes_finito = 0;
    seconds_finito = 0;
    scan = 0;
    printk("\n");
    printk("Coloque a hora de fim (ex: 21): ");
    for (int i = 0; i <= 2; i++) {
      scan = console_getchar();
      console_putchar(scan);
      scan -= 48;
      if ((scan <= 9) && (scan >= 0)) {
        hours_finito = hours_finito * 10 + scan;
      } else
        break;
    }
    printk("\n");
    printk("Coloque os minutos de fim (ex: 21): ");
    scan = 0;
    for (int i = 0; i <= 2; i++) {
      scan = console_getchar();
      console_putchar(scan);
      scan -= 48;
      if ((scan <= 9) && (scan >= 0)) {
        minutes_finito = minutes_finito * 10 + scan;
      } else
        break;
    }
    scan = 0;
    printk("\n");
    printk("Coloque os segundos de fim (ex: 21): ");
    for (int i = 0; i <= 2; i++) {
      scan = console_getchar();
      console_putchar(scan);
      scan -= 48;
      if ((scan <= 9) && (scan >= 0)) {
        seconds_finito = seconds_finito * 10 + scan;
      } else
        break;
    }

    printk("\n");
    printk("Hora Inicial -> %d:%d:%d\n", hours_init, minutes_init, seconds_init);
    printk("Hora Final -> %d:%d:%d\n", hours_finito, minutes_finito, seconds_finito);
}

void SET_PWM() {
    printk("\n");
    printk("Coloque a iluminusiçao pretendida em %(ex: 21): ");
    nits_pret = 0;
    for (int i = 0; i <= 2; i++) {
      scan = console_getchar();
      console_putchar(scan);
      scan -= 48;
      if ((scan <= 9) && (scan >= 0)) {
        nits_pret = nits_pret * 10 + scan;
      } else
        break;
    }

    printk("\n");
    printk("Luminosidade -> %d\n", nits_pret);
}

void CHECK_DATE() {
      ref = 0;
        if (hours_init < hours) {
          if (hours_finito > hours) {
            ref = nits_pret;
          } else if (hours_finito == hours) {
            if (minutes_finito > minutes) {
              ref = nits_pret;
            } else if (minutes_finito == minutes) {
              if (seconds_finito >= seconds) {
                ref = nits_pret;
              }
            }
          }
        } else if (hours_init == hours) {
          if (minutes_init < minutes) {
            if (hours_finito > hours) {
              ref = nits_pret;
            } else if (hours_finito == hours) {
              if (minutes_finito > minutes) {
                ref = nits_pret;
              } else if (minutes_finito == minutes) {
                if (seconds_finito >= seconds) {
                  ref = nits_pret;
                }
              }
            }
          } else if (minutes_init == minutes) {
            if (seconds_init < seconds) {
              if (hours_finito > hours) {
                ref = nits_pret;
              } else if (hours_finito == hours) {
                if (minutes_finito > minutes) {
                  ref = nits_pret;
                } else if (minutes_finito == minutes) {
                  if (seconds_finito >= seconds) {
                    ref = nits_pret;
                  }
                }
              }
            }
          }
        }
}