/** @file main.h
 * @brief header support file 
 *
 * 
 * This file consists on the header for the main file.
 * \n It helps in the organization.
 * 
 * 
 * @author José Mestre Batista and Renato Rocha
 * @date 17 May 2022
 */

#ifndef _main_h
#define _main_h

/**
 * @brief 
 * The main function implements the core of the state machine used in the code.
 * \n The concept consistes on the states and the transitions that comprise them.
 * \n It is also focused on the inner states when a coin it's used.
 * 
 */
void main(void); 


void StateMachine();
/**
 * @brief Adds the value chosen to the credit of the user.
 *
 * @code
 * credit = credit + cach;
 * showMenu(0);
 * showSpace();
 * printk("Dinheiro adicionado : %d Centimos\n\r", cach);
 * printk("Dinheiro Atual : %d Centimos\n\r", credit); 
 * @endcode
 * 
 */

void addMoney(int cach);

/**
 * @brief Returns all the credit from the vending machine.
 * \n Returns the value of the credit returned.
 *
 * @code
 * 
 * showMenu(0);
 * showSpace();
 * printk("Dinheiro devolvido : %d Centimos\n\r", credit);
 * credit = 0;
 * printk("Dinheiro Atual : %d Centimos\n\r", credit); 
 * 
 * @endcode
 * 
 */
void resetMoney();

/**
 * @brief The purpose of this function is to rotate the selection of the highlighted product.
 * 
 * @code
 * 
 * if(flag == 1)
 * {
 *   choice = choice + 1;
 *   if(choice >= 4) choice = 1;
 * } else if (flag == 2)
 * {
 *   choice = choice - 1;
 *   if(choice <= 0) choice = 3;
 * }
 * showMenu(1);
 * @endcode
 * 
 */
void UpOrDown(int flag);

/**
 * @brief This function is in charge of the logic to 
 * identify and calculate if the credit is enough to buy the product chosen 
 * and inform the user according to that.  
 * 
 * 
 * Example of the logic for the Beer product:
 * @code
 *  if(choice == 1)
 *  {
 *   if(credit >= 150)
 *   {
 *      credit = credit - 150;
 *      showMenu();
 *      showSpace();
 *      printk("Produto Entregue (Beer)\n\r");
 *      printk("Dinheiro Descontado: 150 Centimos\n\r");
 *      printk("Dinheiro Atual : %d Centimos\n\r", credit); 
 *    } else 
 *    {
 *      showMenu();
 *      showSpace();
 *      printk("Custo do Produto : 150 Centimos\n\r");
 *      printk("Dinheiro que Falta: %d Centimos\n\r",(150-credit));
 *      printk("Dinheiro Atual : %d Centimos\n\r", credit); 
 *    }
 *   } 
 * @endcode
 * 
 * @param the result of the choice made and it's aditional information
 */
void Check();

/**
 * @brief Main function of the UI for the products. This function is called in all the others.
 * \n It's used so that the user can see and interact with the vending machine.
 * 
 * 
 * This is the product options presented for the user:
 * @verbatim 
    - Beer : 150 Centimos              <----
    - Tuna Sandwich : 100 Centimos 
    - Coffee Sandwich : 50 Centimos
  @endverbatim
 *	
 */
void showMenu(int flag);

/**
 * @brief Function only to print a separation space between the products and the actions.	
 */
void showSpace();


/**
 * @brief Function for the configuration of all the buttons.	
 */
void CONFIG_BUTTONS();


//
//functions for button pressed
//


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
/**
 * @brief This function only serves to update the respective flag
 * and to inform that the button 5 was pressed.
 */ 
void but5press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
/**
 * @brief This function only serves to update the respective flag
 * and to inform that the button 6 was pressed.
 */ 
void but6press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
/**
 * @brief This function only serves to update the respective flag
 * and to inform that the button 7 was pressed.
 */ 
void but7press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
/**
 * @brief This function only serves to update the respective flag
 * and to inform that the button 8 was pressed.
 */ 
void but8press_cbfunction(const struct device *dev, struct gpio_callback *cb, uint32_t pins);











