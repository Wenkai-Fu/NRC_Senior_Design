/*
 * pwm.h
 *
 *  Created on: Feb 25, 2016
 *      Author: smith
 */

#ifndef MOTOR_H_
#define MOTOR_H_

typedef enum
{
	Azimuthal_Motor,
	Claw_Motor,
	Vertical_Motor
}motor_id_t;

class Motor
{
public:

	Motor(motor_id_t id);

	int32_t read(void);

	void start(void);

	void stop(void);

	void dutyCycle(int16_t duty);

	void Error_Handler(void);

private:
	motor_id_t motor_id_;
	int16_t duty_;
	TIM_HandleTypeDef TIM_HANDLE_;
};


#endif /* MOTOR_H_ */
