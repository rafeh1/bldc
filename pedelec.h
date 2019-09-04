/*
 * pedelec.h
 *
 *  Created on: Sep 3, 2019
 *      Author: motorcontrol
 */

#ifndef PEDELEC_H_
#define PEDELEC_H_

void pedelec_tim_isr(void);
float pedeled_get_frecuency(void);
float pedeled_get_duty_cycle(void);
void pedelec_init(void);

#endif /* PEDELEC_H_ */
