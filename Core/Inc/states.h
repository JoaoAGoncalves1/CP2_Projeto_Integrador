/*
 * states.h
 *
 *  Created on: 22/03/2023
 *      Author: joaoa
 */

#ifndef INC_STATES_H_
#define INC_STATES_H_
#define FIFO_LEN 512
#define TRUE 1
#define FALSE 0

#include "main.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RCC_base 0x40023800
#define GPIOA_base 0x40020000
#define GPIOxEN 1

//------------------------------------variaveis------------------------------------//

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart3;

extern uint8_t delim_1[1], delim_2[], rx_data, tx_data, new_com, rx_buffer[FIFO_LEN], tx_buffer[FIFO_LEN], val_com_flag, desc_sampl_flag;
extern uint8_t samp_start_flag,samp_end_flag, samp_num, tx_flag;
extern volatile uint16_t w_rx_bindex, r_tx_bindex,  r_rx_bindex, w_tx_bindex;
extern char *comando, *token, *aux_token, *memo_comando, *memo_valid_comando, *message, temp_arr[20], *memory_ptr;

//------------------------------------Maq.Estados------------------------------------//

enum State_Machine_{
	STATE_RESET = 0,
	STATE_MANUAL = 1,
	STATE_EN_EXPERIM = 2,
	STATE_AUTOMATIC = 3,
	STATE_ENABLE = 4,
	STATE_SP_POS_CAL = 5,
	STATE_MONIT = 6,
}typedef State_Machine;
//------------------------------------funções------------------------------------//
void int_command(uint8_t flag_enable);
//CS
void Control_System(uint8_t flag_enable);
void Reset_State();
void Manual_State();
void Experimental_Mode();
void Automatic_Mode();
//EN
void Disable_Pin();
void Enable_Pin();
uint8_t Enable_Pin_CMD(uint8_t flag_enable);
//Outros
void Help();
void Version();
uint8_t my_command(char *com_array[], char*token, uint8_t arr_len);
char* _my_strtok(char* str, char delimiter);
void _push_message(char *message);
void append_char(char caract);
void start_tx(void);

#endif /* INC_STATES_H_ */
