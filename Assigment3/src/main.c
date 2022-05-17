/*
 * Paulo Pedreiras, 2022/02
 * Simple Digital Output example
 * 
 * Toggles periodically LED1, which is internally connected to P0.13 
 *
 * Base documentation:
 *        
 *      HW info:
 *          https://infocenter.nordicsemi.com/topic/struct_nrf52/struct/nrf52840.html
 *          Section: nRF52840 Product Specification -> Peripherals -> GPIO / GPIOTE
 * 
 *          Board specific HW info can be found in the nRF52840_DK_User_Guide_20201203. I/O pins available at pg 27
 *
 *      Peripherals:
 *          https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/reference/peripherals/gpio.html 
 * 
 * 
 */

/** @file main.c
 * @brief Main file with the creation of the state machine, UI and all the logic.
 * 
 * This file contains the main function with the treatment of the state machine.
 * It also contains the functions for the user interface.
 * 
 * @author José Mestre Batista and Renato Rocha
 * @date 17 May 2022
 */


#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <sys/printk.h>
#include <sys/__assert.h>
#include <string.h>
#include <timing/timing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Refer to dts file */
#define GPIO0_NID DT_NODELABEL(gpio0)

#define BOARDBUT1 0xb /* Pin at which BUT1 is connected. Addressing is direct (i.e., pin number) */
#define BOARDBUT2 0xc
#define BOARDBUT3 0x18
#define BOARDBUT4 0x19
#define BOARDBUT5 0x3
#define BOARDBUT6 0x4
#define BOARDBUT7 0x1c
#define BOARDBUT8 0x1d

#define BLINKPERIOD_MS 500 /* Blink period in ms*/ 

/* Int related declarations */
static struct gpio_callback but1_cb_data; /* Callback structure */
static struct gpio_callback but2_cb_data; /* Callback structure */
static struct gpio_callback but3_cb_data; /* Callback structure */
static struct gpio_callback but4_cb_data; /* Callback structure */
static struct gpio_callback but5_cb_data; /* Callback structure */
static struct gpio_callback but6_cb_data; /* Callback structure */
static struct gpio_callback but7_cb_data; /* Callback structure */
static struct gpio_callback but8_cb_data; /* Callback structure */

// Variables that determine which button was selected
volatile int dcToggleFlag1 = 0; /* Flag to signal a BUT1 press */
volatile int dcToggleFlag2 = 0; /* Flag to signal a BUT1 press */
volatile int dcToggleFlag3 = 0; /* Flag to signal a BUT1 press */
volatile int dcToggleFlag4 = 0; /* Flag to signal a BUT1 press */
volatile int dcToggleFlag5 = 0; /* Flag to signal a BUT1 press */
volatile int dcToggleFlag6 = 0; /* Flag to signal a BUT1 press */
volatile int dcToggleFlag7 = 0; /* Flag to signal a BUT1 press */
volatile int dcToggleFlag8 = 0; /* Flag to signal a BUT1 press */


void but1press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{    
    /* Inform that button was hit*/
    printk("But1 pressed at %d\n\r", k_cycle_get_32());
    
    /* Update Flag*/
    dcToggleFlag1 = 1;
}

void but2press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{    
    /* Inform that button was hit*/
    printk("But2 pressed at %d\n\r", k_cycle_get_32());
    
    /* Update Flag*/
    dcToggleFlag2 = 1;
}

void but3press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{    
    /* Inform that button was hit*/
    printk("But3 pressed at %d\n\r", k_cycle_get_32());
    
    /* Update Flag*/
    dcToggleFlag3 = 1;
}

void but4press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{    
    /* Inform that button was hit*/
    printk("But4 pressed at %d\n\r", k_cycle_get_32());
    
    /* Update Flag*/
    dcToggleFlag4 = 1;
}

void but5press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{    
    /* Inform that button was hit*/
    printk("But5 pressed at %d\n\r", k_cycle_get_32());
    
    /* Update Flag*/
    dcToggleFlag5 = 1;
}

void but6press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{    
    /* Inform that button was hit*/
    printk("But6 pressed at %d\n\r", k_cycle_get_32());
    
    /* Update Flag*/
    dcToggleFlag6 = 1;
}

void but7press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{    
    /* Inform that button was hit*/
    printk("But7 pressed at %d\n\r", k_cycle_get_32());
    
    /* Update Flag*/
    dcToggleFlag7 = 1;
}

void but8press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{    
    /* Inform that button was hit*/
    printk("But8 pressed at %d\n\r", k_cycle_get_32());
    
    /* Update Flag*/
    dcToggleFlag8 = 1;
}


const struct device *gpio0_dev;         /* Pointer to GPIO device structure */\

/* Variables for Assigment */
int credit = 0;
int choice = 1;
int state = 0;

/* Define States*/

#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define S4 4

/* Main function */
void main(void) 
{
    
    /* Bind to GPIO 0 */
    gpio0_dev = device_get_binding(DT_LABEL(GPIO0_NID));
    if (gpio0_dev == NULL) 
    {
        printk("Failed to bind to GPIO0\n\r");        
	return;
    }
    else 
    {
        printk("Bind to GPIO0 successfull \n\r");        
    }

    CONFIG_BUTTONS();

    showMenu(1);

    StateMachine();                       
        
    return;
} 

void StateMachine()
{
    while(1)
    {
      switch(state)
      {
        case S0:

          if((dcToggleFlag1 == 1) || (dcToggleFlag2 == 1) || (dcToggleFlag3 == 1) || (dcToggleFlag4 == 1) )
          {
            state = S1;
          }

          if(dcToggleFlag6 == 1)
          {
            state = S2;
          }

          if((dcToggleFlag5 == 1) || (dcToggleFlag7 == 1))
          {
            state = S3;
          }

          if(dcToggleFlag8 == 1)
          {
            state = S4;
          }
          break;
        case S1:
          if(dcToggleFlag1 == 1) 
          {
            addMoney(10);
            dcToggleFlag1 = 0;
          }
          if(dcToggleFlag2 == 1) 
          {
            addMoney(20);
            dcToggleFlag2 = 0;
          }
          if(dcToggleFlag3 == 1) 
          {
            addMoney(50);
            dcToggleFlag3 = 0;
          }
          if(dcToggleFlag4 == 1) 
          {
            addMoney(100);
            dcToggleFlag4 = 0;
          }
          state = S0;
          break;
        case S2: 
          resetMoney();
          dcToggleFlag6 = 0;
          state = S0;
        
        case S3:
          if(dcToggleFlag5 == 1)
          {
            UpOrDown(2);
            dcToggleFlag5 = 0;
          }
          if(dcToggleFlag7 == 1)
          {
            UpOrDown(1);
            dcToggleFlag7 = 0;
          }
          state = S0;
          break;
         case S4:
          Check();
          dcToggleFlag8 = 0; 
          state = S0;
          break;
         default: 
          state = S0; 
          break;    
      }
    }   
    return;
}

void addMoney(int cach)
{
  credit = credit + cach;
  showMenu(0);
  showSpace();
  printk("Dinheiro adicionado : %d Centimos\n\r", cach);
  printk("Dinheiro Atual : %d Centimos\n\r", credit); 
  return;
}

void resetMoney()
{
  showMenu(0);
  showSpace();
  printk("Dinheiro devolvido : %d Centimos\n\r", credit);
  credit = 0;
  printk("Dinheiro Atual : %d Centimos\n\r", credit);
  return; 
}

void UpOrDown(int flag)
{
  if(flag == 1)
  {
    choice = choice + 1;
    if(choice >= 4) choice = 1;
  } else if (flag == 2)
  {
    choice = choice - 1;
    if(choice <= 0) choice = 3;
  }
  showMenu(1);
  return;
}

void Check()
{
  if(choice == 1)
  {
    if(credit >= 150)
    {
      credit = credit - 150;
      showMenu();
      showSpace();
      printk("Produto Entregue (Beer)\n\r");
      printk("Dinheiro Descontado: 150 Centimos\n\r");
      printk("Dinheiro Atual : %d Centimos\n\r", credit); 
    } else 
    {
      showMenu();
      showSpace();
      printk("Custo do Produto : 150 Centimos\n\r");
      printk("Dinheiro que Falta: %d Centimos\n\r",(150-credit));
      printk("Dinheiro Atual : %d Centimos\n\r", credit); 
    }
  } else if (choice == 2)
  {
    if(credit >= 100)
    {
      credit = credit - 100;
      showMenu();
      showSpace();
      printk("Produto Entregue (Tuna Sandwich)\n\r");
      printk("Dinheiro Descontado: 100 Centimos\n\r");
      printk("Dinheiro Atual : %d Centimos\n\r", credit); 
    } else 
    {
      showMenu();
      showSpace();
      printk("Custo do Produto : 100 Centimos\n\r");
      printk("Dinheiro que Falta: %d Centimos\n\r",(100-credit));
      printk("Dinheiro Atual : %d Centimos\n\r", credit); 
    }
  } else if (choice == 3)
  {
    if(credit >= 50)
    {
      credit = credit - 50;
      showMenu();
      showSpace();
      printk("Produto Entregue (Coffee)\n\r");
      printk("Dinheiro Descontado: 50 Centimos\n\r");
      printk("Dinheiro Atual : %d Centimos\n\r", credit); 
    } else 
    {
      showMenu();
      showSpace();
      printk("Custo do Produto : 50 Centimos\n\r");
      printk("Dinheiro que Falta: %d Centimos\n\r",(50-credit));
      printk("Dinheiro Atual : %d Centimos\n\r", credit); 
    }
  }
  return;
}

void showMenu(int flag)
{
  printk("\n\x1b[2J\r");
  if(choice == 1)
  {
    printk("Products : \n\r");
    printk("   - Beer : 150 Centimos              <---- \n\r");
    printk("   - Tuna Sandwich : 100 Centimos \n\r");
    printk("   - Coffee Sandwich : 50 Centimos \n\r");
  } else if(choice == 2)
  {
    printk("Products : \n\r");
    printk("   - Beer : 150 Centimos \n\r");
    printk("   - Tuna Sandwich : 100 Centimos     <---- \n\r");
    printk("   - Coffee Sandwich : 50 Centimos \n\r");
  } else if(choice == 3)
  {
    printk("Products : \n\r");
    printk("   - Beer : 150 Centimos \n\r");
    printk("   - Tuna Sandwich : 100 Centimos \n\r");
    printk("   - Coffee Sandwich : 50 Centimos    <---- \n\r");
  }
  if(flag == 1)
  {
    showSpace();
    printk("Dinheiro Atual : %d Centimos\n\r", credit); 
  }
  return;
}

void showSpace()
{
  printk("\n\r");
  printk("----------------------------------------------------\n\r");
  printk("\n\r");
  return;
}

/*Configure Buttons*/

void CONFIG_BUTTONS()
{
    int ret = 0;
    ret = gpio_pin_configure(gpio0_dev, BOARDBUT1, GPIO_INPUT | GPIO_PULL_UP);
    ret = gpio_pin_configure(gpio0_dev, BOARDBUT2, GPIO_INPUT | GPIO_PULL_UP);
    ret = gpio_pin_configure(gpio0_dev, BOARDBUT3, GPIO_INPUT | GPIO_PULL_UP);
    ret = gpio_pin_configure(gpio0_dev, BOARDBUT4, GPIO_INPUT | GPIO_PULL_UP);

    ret = gpio_pin_configure(gpio0_dev, BOARDBUT5, GPIO_INPUT | GPIO_PULL_UP);
    ret = gpio_pin_configure(gpio0_dev, BOARDBUT6, GPIO_INPUT | GPIO_PULL_UP);
    ret = gpio_pin_configure(gpio0_dev, BOARDBUT7, GPIO_INPUT | GPIO_PULL_UP);
    ret = gpio_pin_configure(gpio0_dev, BOARDBUT8, GPIO_INPUT | GPIO_PULL_UP);

    if (ret < 0) {
        printk("Error %d: Failed to configure BUT 1 \n\r", ret);
	return;
    }

    ret = gpio_pin_interrupt_configure(gpio0_dev, BOARDBUT1, GPIO_INT_EDGE_TO_ACTIVE);
    ret = gpio_pin_interrupt_configure(gpio0_dev, BOARDBUT2, GPIO_INT_EDGE_TO_ACTIVE);
    ret = gpio_pin_interrupt_configure(gpio0_dev, BOARDBUT3, GPIO_INT_EDGE_TO_ACTIVE);
    ret = gpio_pin_interrupt_configure(gpio0_dev, BOARDBUT4, GPIO_INT_EDGE_TO_ACTIVE);

    ret = gpio_pin_interrupt_configure(gpio0_dev, BOARDBUT5, GPIO_INT_EDGE_TO_ACTIVE);
    ret = gpio_pin_interrupt_configure(gpio0_dev, BOARDBUT6, GPIO_INT_EDGE_TO_ACTIVE);
    ret = gpio_pin_interrupt_configure(gpio0_dev, BOARDBUT7, GPIO_INT_EDGE_TO_ACTIVE);
    ret = gpio_pin_interrupt_configure(gpio0_dev, BOARDBUT8, GPIO_INT_EDGE_TO_ACTIVE);
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

    gpio_init_callback(&but5_cb_data, but5press_cbfunction, BIT(BOARDBUT5));
    gpio_add_callback(gpio0_dev, &but5_cb_data);

    gpio_init_callback(&but6_cb_data, but6press_cbfunction, BIT(BOARDBUT6));
    gpio_add_callback(gpio0_dev, &but6_cb_data);

    gpio_init_callback(&but7_cb_data, but7press_cbfunction, BIT(BOARDBUT7));
    gpio_add_callback(gpio0_dev, &but7_cb_data);

    gpio_init_callback(&but8_cb_data, but8press_cbfunction, BIT(BOARDBUT8));
    gpio_add_callback(gpio0_dev, &but8_cb_data);
}

