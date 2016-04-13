/*
 * encoder.h
 *
 *  Created on: Feb 29, 2016
 *      Author: smith
 */

#ifndef ENCODER_H_
#define ENCODER_H_



class Encoder
{
	public:
		Encoder(void);

		int32_t read(void);

		int16_t direction(void);

		void set(int32_t count32);

		void Error_Handler(void);

	private:

		uint16_t prev_counter_;
		int16_t overflows_;

};



#endif /* ENCODER_H_ */
