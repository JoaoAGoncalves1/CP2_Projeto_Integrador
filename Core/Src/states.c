/*
 * states.c
 *
 *  Created on: Mar 20, 2023
 *      Author: joaoa
 */
#include "states.h"

uint8_t delim_1[1], delim_2[] = " ", rx_data = 0, tx_data = 0, new_com = 0, rx_buffer[FIFO_LEN], tx_buffer[FIFO_LEN], val_com_flag = FALSE, desc_sampl_flag = FALSE;
uint8_t samp_start_flag = FALSE ,samp_end_flag = FALSE, samp_num, tx_flag = FALSE;
volatile uint16_t w_rx_bindex = 0, r_tx_bindex = 0,  r_rx_bindex = 0, w_tx_bindex = 0;
char *comando, *token, *aux_token, *memo_comando, *memo_valid_comando, *message, temp_arr[20], *memory_ptr; State_Machine this_state = 0;

void int_command(uint8_t flag_enable){

	char *com_arr[] = {"CS", "EN", "HW", "SW"};
 	if(((w_rx_bindex-r_rx_bindex) != 0) && (new_com >0) ){

 		if((*memo_comando != '$')&& (val_com_flag)){
			memo_valid_comando = (char*)malloc(strlen(memo_comando)+1);
			strcpy (memo_valid_comando,memo_comando);
		}

	comando = _my_strtok(&rx_buffer[r_rx_bindex&(FIFO_LEN-1)], delim_1[0]);
	memo_comando = (char*)malloc(strlen(comando)+1);//+1 para o caractere '\0'
	strcpy (memo_comando,comando);
	r_rx_bindex += (strlen(comando)+1);
	token = strtok(comando, delim_2);
	_push_message(memo_comando);
	append_char(delim_1[0]);

	switch(my_command(com_arr, token, 15)){
		case 0:
			Control_System(flag_enable);
			break;
		case 1:
			flag_enable = Enable_Pin_CMD(flag_enable);
			break;
		case 2:

			break;
		case 3:

			break;
		default:
			_push_message("Comando invalido!");
			val_com_flag = FALSE;
			break;
		}
		new_com--; //Limpa a flag de novo comando
		append_char(delim_1[0]);
		append_char(delim_1[0]);
		append_char('>');
		start_tx();
	}
 	if(tx_flag){
 		start_tx();
 		tx_flag = FALSE;

 	}
}
void Control_System(uint8_t flag_enable){
	//modo reset, manual, ensaio experimental, automático//
	strcpy (token, memo_comando);
	token = strtok(token, " ");
	if (token != NULL)
	token = strtok(NULL, " ");

	this_state = strtol(token, NULL, 16);
	_push_message("CS ");

	switch (this_state){
		case 0:	//RESET						//Neste estado as variaveis de controlo devemm ser resetadas e os pinos de enable desabilitados
			this_state = STATE_RESET;
			Reset_State();
			break;
		case 1: //MANUAL					//Neste estado os parâmetros devem ser inicializaddos em malha aberta
			this_state = STATE_MANUAL;
			Manual_State();					//inicializar perfifericos
			break;							//posição e velocidade	//acionar PWM
		case 2:
			this_state = STATE_EN_EXPERIM;
			Experimental_Mode();
			break;
		case 3:
			this_state = STATE_AUTOMATIC;
			Automatic_Mode();
			break;
		default:
			_push_message("Estado invalido");
		break;
	}
}
uint8_t Enable_Pin_CMD(uint8_t flag_enable){

	strcpy (token, memo_comando);
	token = strtok(token, " ");
	if (token != NULL)
	token = strtok(NULL, " ");

	this_state = strtol(token, NULL, 16);

	if (this_state == STATE_RESET){		//Disable
		flag_enable = 0;
		Disable_Pin();
	}
	else if (this_state == 1){	//Enable
		flag_enable = 1;
		Enable_Pin();
	}
	else
		_push_message("Estado invalido");
	return flag_enable;
}
void Reset_State(){
	if (this_state == STATE_RESET){
		_push_message("Reset_State\r");
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);

	}
	else
		_push_message("Estado invalido");
}
void Manual_State(){

	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);		//inicializa o timer 4 em PWM

}
void Experimental_Mode(){

}
void Automatic_Mode(){

}
void Disable_Pin(){
	this_state = STATE_RESET;
}
void Enable_Pin(){
	this_state = STATE_ENABLE;
}
/*
void Start_PWM(){
	static int duty_cycle = 10 000;
	TIM_OC_InitTypeDef sConfigOC = {0};
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = duty_cycle;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel (&htim4, &sConfigOC, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start (&htim4, TIM_CHANNEL_2);
}
void Change_PWM(char* token){
	static uint8_t duty_cycle = 10000;
	token =  strtok (token, " ");
	TIM_OC_InitTypeDef sConfigOC = {0};
	if (token == " "){ //comando enviado for para aumentar o PWM
		if (duty_cycle > 0){
			duty_cycle -= 500;		//corresponde a 5%
		}
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = duty_cycle;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		HAL_TIM_PWM_ConfigChannel (&htim4, &sConfigOC, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start (&htim4, TIM_CHANNEL_2);
	}
	else if (token == " "){ //comando enviado for para diminuir o PWM
		if (duty_cycle < 100){
			duty_cycle += 500;		//corresponde a 5%
		}
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = duty_cycle;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		HAL_TIM_PWM_ConfigChannel (&htim4, &sConfigOC, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start (&htim4, TIM_CHANNEL_2);
	}
	else
		_push_message("Valor de Duty Cycle inválido");
}
*/
uint8_t my_command(char *com_array[], char*token, uint8_t arr_len){
	uint8_t i = 0;
	while((strcmp(com_array[i], token))){
		if(i == (arr_len-1)){
			i = -1;
			break;
		}
		i++;
	}
	return i;
}
void _push_message(char *message){

	char *aux_ptr;
	uint8_t i = 0;
	aux_ptr = message;
	for (i=0; i<strlen(message); i++){
		 tx_buffer[w_tx_bindex&(FIFO_LEN-1)] = *aux_ptr;
		  aux_ptr++;
		  w_tx_bindex++;
	}

}
void append_char(char caract){
	tx_buffer[w_tx_bindex&(FIFO_LEN-1)] = caract;
	w_tx_bindex++;

}
void start_tx(){

	if (USART3->ISR & USART_ISR_TC)	{ // Transmission Complete (TC) É ATIVADA QUANDO A TRANSMISSÃO É CONCLUIDA - CASO NAO ESTEJA CONCLUIDA NAO SE DEVE INVOCAR NOVAMENTE HAL_UART_Transmit_IT()
		HAL_UART_Transmit_IT(&huart3, (uint8_t *)&tx_buffer[r_tx_bindex&(FIFO_LEN-1)],1);
		r_tx_bindex++;
	}

}
char* _my_strtok(char* str, char delimiter){
	uint8_t i = 0;
	for(i=0; str[i] != delimiter; i++){
		i&= (FIFO_LEN-1);
		temp_arr[i] = str[i];
	}
	temp_arr[i] = '\0';
	return temp_arr;
}
//-------------------------------------------Fim dos comandos--------------------------------------------//
/*
HAL_StatusTypeDef samp_init(void){
	MX_ADC2_Init();
	MX_TIM1_Init();
	HAL_TIM_Base_Start(&htim1);
	return HAL_OK;
}
HAL_StatusTypeDef samp_start(void){
	return HAL_ADC_Start_IT(&hadc2);
}
HAL_StatusTypeDef samp_stop(void){
	return HAL_ADC_Stop_IT(&hadc2);
}
HAL_StatusTypeDef samp_period_update(float frequency){

	__HAL_TIM_DISABLE(&htim1);
	htim1.Init.Period = 10800/frequency;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	__HAL_TIM_ENABLE(&htim1);
	return HAL_OK;
}
void samp_adcCh(uint8_t adc_ch){

	ADC_ChannelConfTypeDef sConfig = {0};
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	sConfig.Channel = adc_ch;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK){
		Error_Handler();
	}
	if(adc_ch<8){
		__HAL_RCC_GPIOA_CLK_ENABLE();
		GPIO_InitStruct.Pin = (1<<adc_ch);
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	}else if(adc_ch<10){

		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitStruct.Pin = (1 << (adc_ch-8));
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	}else if(adc_ch<16){

		__HAL_RCC_GPIOC_CLK_ENABLE();
		GPIO_InitStruct.Pin = (1 << (adc_ch-10));
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	}
}


*/
