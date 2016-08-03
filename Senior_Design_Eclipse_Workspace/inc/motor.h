#ifndef MOTOR_H_
#define MOTOR_H_

#include "stm32746g_discovery.h"

typedef enum
{
	Azimuthal_Motor,
	Claw_Motor,
	Vertical_Motor
}motor_id_t;

class Motor
{
public:

	Motor();
	Motor(TIM_TypeDef *TIMX, const float counts_to_position);
	void motorInit(motor_id_t id);

	void setEnable(motor_id_t id, bool enable);
	bool getEnable(motor_id_t id);

	void setDuty(motor_id_t id, int16_t dutyInput);
	int16_t getDuty(motor_id_t id);

	void setDirection(bool direction);
	bool getDirection(void);

	void Error_Handler(void);

private:
	motor_id_t motor_id_;
	int16_t duty_;
	TIM_HandleTypeDef TIM_HANDLE_;
	bool dir_;
	int16_t prescaler_;
	int16_t period_;
	bool enableA_;
	bool enableV_;
	bool enableC_;

	float counts_to_position_;
	// Timer Output Compare Configuration Structure
	TIM_OC_InitTypeDef sMotorConfig;

};


#endif /* MOTOR_H_ */
