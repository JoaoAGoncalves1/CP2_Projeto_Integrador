/*
 * PWM_file.h
 *
 *  Created on: 24/03/2023
 *      Author: joaoa
 */

#ifndef INC_PWM_FILE_H_
#define INC_PWM_FILE_H_
#include "states.h"

int Start_PWM(int duty_cycle);
int Change_PWM(char* token, int duty_cycle);

#endif /* INC_PWM_FILE_H_ */
