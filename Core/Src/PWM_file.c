/*
 * PWM_file.c
 *
 *  Created on: 24/03/2023
 *      Author: joaoa
 */
#include "PWM_file.h"


int Start_PWM(int duty_cycle){
	TIM_OC_InitTypeDef sConfigOC = {0};
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = duty_cycle;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel (&htim4, &sConfigOC, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start (&htim4, TIM_CHANNEL_2);
	return duty_cycle;
}
int Change_PWM (char* token, int duty_cycle){

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

	return duty_cycle;
}
