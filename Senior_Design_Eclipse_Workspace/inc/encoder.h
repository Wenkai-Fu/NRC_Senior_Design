#ifndef ENCODER_H_
#define ENCODER_H_

#include "stm32746g_discovery.h"
#include "System_Variable_Settings.h"

typedef enum
{
	Azimuthal_Encoder,
	Claw_Encoder,
	Vertical_Encoder
}encoder_id_t;

class Encoder
{
	public:

		Encoder(void);
		int32_t getCount(void);
		void setCount(int32_t count32);
		bool getDirection(void);
		void enableEncoder(encoder_id_t encode);
		void Error_Handler(void);
		float getPosition(encoder_id_t encode);
		float getDesiredPosition(encoder_id_t encode);
		void setDesiredPosition(encoder_id_t encode, float posDesired);
		float getPosError();
		void setPosError(encoder_id_t encode, float posError);

	private:
		encoder_id_t encoder_id_;
		uint16_t prev_counter_;
		int16_t overflows_;
		float pos_;
		float azimPos_;
		float vertPos_;
		float clawPos_;
		float desiredAzimPos_;
		float desiredVertPos_;
		float desiredClawPos_;
		float posError_;
};



#endif /* ENCODER_H_ */
