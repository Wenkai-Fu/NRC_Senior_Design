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
    uint32_t get_raw_count();

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

    void set_zero();

    // limit switch operation
    void set_bot_ls(bool condition) {bot_ls = condition;}
    bool get_bot_ls() {return bot_ls;}

    void set_top_ls(bool condition) {top_ls = condition;}
    bool get_top_ls() {return top_ls;}

    void set_claw_ls(bool condition) {claw_ls = condition;}
    bool get_claw_ls() {return claw_ls;}

    int get_id() { return id;}

private:

	void Error_Handler(void);
	void setCount();

	int16_t duty_;
	TIM_HandleTypeDef TIM_HANDLE_;
	bool dir_;
	int16_t prescaler_;
	int16_t period_;
	float counts_to_position_;

	int16_t overflows_;
	uint32_t cnt;
	uint32_t old_cnt;

	float desiredPos_; // desired position

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

	// limit switches. z motor use bot and top. claw motor use claw.
	bool bot_ls, top_ls, claw_ls;

	int id;
};

#endif /* MOTOR_H_ */
