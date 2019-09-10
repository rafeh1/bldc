/*
 * pedelec.h
 *
 *  Created on: Sep 3, 2019
 *      Author: motorcontrol
 */

#ifndef PEDELEC_H_
#define PEDELEC_H_

#include "stdbool.h"
#include "stdint.h"

void pedelec_tim_isr(void);
float pedeled_get_frecuency(void);
float pedeled_get_duty_cycle(void);
bool pedelec_get_pulse_detected_flag(void);
void pedelec_set_pulse_detected_flag(bool value);
void pedelec_init(void);
float pedelec_get_rpm(float frecuency, uint8_t magnets);

#endif /* PEDELEC_H_ */
