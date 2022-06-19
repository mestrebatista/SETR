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
/* ###################################                         UART VARIABLES                             ###################################*/
/* ########################################################################################################################################## */

#define FATAL_ERR -1 /* Fatal error return code, app terminates */

#define UART_NID DT_NODELABEL(uart0)    /* UART Node label, see dts */
#define RXBUF_SIZE 60                   /* RX buffer size */
#define TXBUF_SIZE 60                   /* TX buffer size */
#define RX_TIMEOUT 1000                  /* Inactivity period after the instant when last char was received that triggers an rx event (in us) */

/* Struct for UART configuration (if using default valuies is not needed) */
const struct uart_config uart_cfg = {
		.baudrate = 115200,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
};

/* UAR related variables */
const struct device *uart_dev;          /* Pointer to device struct */ 
static uint8_t rx_buf[RXBUF_SIZE];      /* RX buffer, to store received data */
static uint8_t rx_chars[RXBUF_SIZE];    /* chars actually received  */
volatile int uart_rx_rdy_flag;          /* Flag to signal main() that a message is available */

/* UART callback function prototype */
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data);



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
volatile int minutes = 0;
volatile int hours = 0;
volatile int days = 0;
uint8_t welcome_mesg[] = "UART demo: Type a few chars in a row and then pause for a little while ...\n\r"; 
uint8_t rep_mesg[TXBUF_SIZE];

int ref = 50;


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
    CONFIG_UART();

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
  int toggle_mode = 1;


  printk("Thread 1 Init\n\r");

  release_time = k_uptime_get() + thread_1_period;

  while(1)
  {
    printk("Thread 1 Activated\n\r");

    clock();

    if (uart_rx_rdy_flag == 1) {
      uart_rx_rdy_flag = 0;

      sprintf(rep_mesg, "You typed [%s]\n\r", rx_chars);

      err = uart_tx(uart_dev, rep_mesg, strlen(rep_mesg), SYS_FOREVER_MS);
      if (err) {
        printk("uart_tx() error. Error code:%d\n\r", err);
        return;
      }
    }

    if(toggle_mode) 
    {
        if(dcToggleFlag1 == 1) 
        {
           toggle_mode = 0;
           dcToggleFlag1 = 0;
        }
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
          dcToggleFlag3 = 0;
       }
       if(dcToggleFlag4 == 1) 
       {
          ref+=10;
          dcToggleFlag4 = 0;
       }
       if(ref>= 100) ref = 100;
       if(ref<= 0) ref = 0;
       printk("ref -> %d",ref);
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

  printk("Thread A Init\n\r");

  /*release_time = k_uptime_get() + thread_A_period; */

  while(1)
  {
    k_sem_take(&sem_1a, K_FOREVER);
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
        var_ab = adc_sample_buffer[0];
        printk("adc reading  raw: %4u / %4u mV: \n\r", var_ab, (uint16_t)(3000 * var_ab /((float)1023)));
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

  printk("Thread B Init\n\r");

  while(1)
  {

   k_sem_take(&sem_ab, K_FOREVER);
   printk("Thread B Activated\n\r");

   media = 0;
   media_2 = 0;
   count = 0;

   if(var_ab <= 900)
   {
     for (i = 0; i < WINDOW_SIZE - 1; i++) 
     {
       buffer[i] = buffer[i + 1];
       /*printk("adc reading %d: raw:%4u / %4u mV: \n\r", i, buffer[i], (uint16_t)(1000 * buffer[i] * ((float)3 / 1023))); */
     }
       buffer[WINDOW_SIZE - 1] = var_ab;
       /*printk("adc reading %d: raw:%4u / %4u mV: \n\r", 9, buffer[9], (uint16_t)(1000 * buffer[9] * ((float)3 / 1023)));*/
   }

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

   printk("adc reading %d: raw:%4u / %4u mV: \n\r", count, var_bc, (uint16_t)(3000 * var_bc / ((float) 1023)));

   k_sem_give(&sem_bc);
  }
}


/* ###################    THREAD C    ######################*/

void thread_C_code(void *argA , void *argB, void *argC)
{
  int i;
  int ret = 0;
  int pwm = 0;
  int nits = 0;
  int u;
  int flag = 0;

  printk("Thread C Init\n\r");

  while(1)
  {
    k_sem_take(&sem_bc , K_FOREVER);

    printk("Thread C Activated\n\r");

    ret = 0;
    flag = 1;

    nits = (uint16_t)(3000 * var_bc / ((float) 1023));
    nits = 100-((nits-900)/16);
    if(nits>=100 || nits <= 0) flag = 0;
    if(nits>=100) nits = 100;
    if(nits<= 0) nits = 0;
    printf("nits -> %d %\n",nits);

    /*
    pwm = (uint16_t)(1000 * var_bc * ((float)3 / 1023));
    if(pwm > 2800) pwm = 0;
    else if(pwm<800) pwm = 100;
    else pwm = -0.05*pwm+145; */
    if(flag){
      if(nits>ref) u--;
      else if(nits<ref) u++;

      if(u >= 100) u = 100;
      if(u<=0) u = 0;
    }

    printk("ref -> %d | nits -> %d | u -> %d",ref,nits,u);
    var_cd = 50;
      
    k_sem_give(&sem_cd);
  }
}

void thread_D_code(void *argA , void *argB, void *argC)
{
  int i;
  int ret = 0;
  int pwm = 0;

  printk("Thread D Init\n\r");

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

  unsigned int pwmPeriod_us = 25000;                       

  while(1)
  {
    k_sem_take(&sem_cd , K_FOREVER);

    printk("Thread D Activated\n\r");

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

  printk("seconds -> %d | minutes -> %d | hours -> %d\n",seconds,minutes,hours);
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


void CONFIG_UART()
{
int err=0; /* Generic error variable */


    /* Bind to UART */
    uart_dev= device_get_binding(DT_LABEL(UART_NID));
    if (uart_dev == NULL) {
        printk("device_get_binding() error for device %s!\n\r", DT_LABEL(UART_NID));
        return;
    }
    else {
        printk("UART binding successful\n\r");
    }

    /* Configure UART */
    err = uart_configure(uart_dev, &uart_cfg);
    if (err == -ENOSYS) { /* If invalid configuration */
        printk("uart_configure() error. Invalid configuration\n\r");
        return; 
    }

    /* Register callback */
    err = uart_callback_set(uart_dev, uart_cb, NULL);
    if (err) {
        printk("uart_callback_set() error. Error code:%d\n\r",err);
        return;
    }
		
    /* Enable data reception */
    err =  uart_rx_enable(uart_dev ,rx_buf,sizeof(rx_buf),RX_TIMEOUT);
    if (err) {
        printk("uart_rx_enable() error. Error code:%d\n\r",err);
        return;
    }

    /* Send a welcome message */ 
    /* Last arg is timeout. Only relevant if flow controll is used */
    err = uart_tx(uart_dev, welcome_mesg, sizeof(welcome_mesg), SYS_FOREVER_MS);
    if (err) {
        printk("uart_tx() error. Error code:%d\n\r",err);
        return;
    }
}

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    int err;

    switch (evt->type) {
	
        case UART_TX_DONE:
		printk("UART_TX_DONE event \n\r");
                break;

	case UART_TX_ABORTED:
		printk("UART_TX_ABORTED event \n\r");
		break;
		
	case UART_RX_RDY:
		printk("UART_RX_RDY event \n\r");
                /* Just copy data to a buffer. Usually it is preferable to use e.g. a FIFO to communicate with a task that shall process the messages*/
                memcpy(rx_chars,&(rx_buf[evt->data.rx.offset]),evt->data.rx.len); 
                rx_chars[evt->data.rx.len]=0; /* Terminate the string */
                uart_rx_rdy_flag = 1;
		break;

	case UART_RX_BUF_REQUEST:
		printk("UART_RX_BUF_REQUEST event \n\r");
		break;

	case UART_RX_BUF_RELEASED:
		printk("UART_RX_BUF_RELEASED event \n\r");
		break;
		
	case UART_RX_DISABLED: 
                /* When the RX_BUFF becomes full RX is is disabled automaticaly.  */
                /* It must be re-enabled manually for continuous reception */
                printk("UART_RX_DISABLED event \n\r");
		err =  uart_rx_enable(uart_dev ,rx_buf,sizeof(rx_buf),RX_TIMEOUT);
                if (err) {
                    printk("uart_rx_enable() error. Error code:%d\n\r",err);
                    exit(FATAL_ERR);                
                }
		break;

	case UART_RX_STOPPED:
		printk("UART_RX_STOPPED event \n\r");
		break;
		
	default:
                printk("UART: unknown event \n\r");
		break;
    }

}