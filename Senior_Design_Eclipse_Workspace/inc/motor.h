#ifndef MOTOR_H_
#define MOTOR_H_

#include "stm32746g_discovery.h"
#include "System_Variable_Settings.h"

class Motor
{

public:

	Motor(TIM_TypeDef *TIMX, const float counts_to_position,
		  const int encoder_bits, const float increment);
	void enable();
	void setDesiredPosition(float desiredPos);
	void setDuty(int16_t dutyInput);
	bool getDirection();
    float getPosition();
    float getDesiredPosition();
    float getPosError();
    bool increase();
    bool decrease();
    int32_t getCount(void);
    float getDuty(){return (float)duty_;}
    // TODO: create
    void disable()
    {
    	enable_ = false;
    	duty_ = 0;
    }
    bool enabled()
    {
    	return enable_;
    }

private:

	void Error_Handler(void);
	void setCount();

	int16_t duty_;
	TIM_HandleTypeDef TIM_HANDLE_;
	bool dir_;
	int16_t prescaler_;
	int16_t period_;
	float counts_to_position_;
	int32_t counter32_;
	int16_t overflows_;
	uint16_t counter16_;
	float desiredPos_;

	// Timer Output Compare Configuration Structure
	TIM_OC_InitTypeDef sMotorConfig;
	// Timer handler declaration
	TIM_HandleTypeDef Encoder_Handle;
	// Timer Encoder Configuration Structure declaration
	TIM_Encoder_InitTypeDef sEncoderConfig;
	// Bits to set correct multiplex pins
	GPIO_PinState encoder_bit_A;
	GPIO_PinState encoder_bit_B;
	//
	float increment_;
	bool enable_;
};

#endif /* MOTOR_H_ */
