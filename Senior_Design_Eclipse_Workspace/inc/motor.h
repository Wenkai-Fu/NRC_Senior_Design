#ifndef MOTOR_H_
#define MOTOR_H_

#include "stm32746g_discovery.h"
#include "System_Variable_Settings.h"

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
	Motor(TIM_TypeDef *TIMX, const float counts_to_position, const int encoder_bits = 0);
	void motorInit(motor_id_t id);
	void setEnable(motor_id_t id, bool enable);
	void setEnable();
	bool getEnable(motor_id_t id);
	void setDuty(motor_id_t id, int16_t dutyInput);
	void setDuty(int16_t dutyInput);
	void setDirection(bool direction);
	bool getDirection(void);
	void Error_Handler(void);

	// encoder things
	int32_t getCount(void);
	void setCount(int32_t count32);
    //void enableEncoder(encoder_id_t encode);
//	void Error_Handler(void);
    float getPosition();
    float getDesiredPosition();
    void setDesiredPosition(float desiredPos);
    float getPosError();

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


	// encoder
	int16_t overflows_;
	uint16_t prev_counter_;

	/* Timer handler declaration */
	TIM_HandleTypeDef Encoder_Handle;
	/* Timer Encoder Configuration Structure declaration */
	TIM_Encoder_InitTypeDef sEncoderConfig;

	GPIO_PinState encoder_bit_A;
	GPIO_PinState encoder_bit_B;

	float desiredPos_;

};


#endif /* MOTOR_H_ */
