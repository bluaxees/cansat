/*
 * main.c
 *
 *  Created on: 2021 Feb 19 17:27:11
 *  Author: newot
 */




#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)

/**

 * @brief main() - Application entry point
 *
 * <b>Details of function</b><br>
 * This routine is the application entry point. It is invoked by the device startup code. It is responsible for
 * invoking the APP initialization dispatcher routine - DAVE_Init() and hosting the place-holder for user application
 * code.
 */

// MAIN_CLOCK is set to 32 MHz
// Delay is approximately:
// (1/MAIN_CLOCK)*i
void delay(unsigned long int i) {
	while(i--) {}
}

int is_even_parity(uint32_t sensor_data){
	int count = 0;
	for (int i = 0; sensor_data > 0; i++){
		if (sensor_data % 2){
			count++;
		}
		sensor_data >>= 1;
	}
	if (count % 2){
		return 0;
	}
	else{
		return 1;
	}
}

void get_bitstream(uint32_t data_1, uint32_t data_2){

    int bitstream_1[21];
	for (int i = 0; i < 21; i++) {
	    if (data_1 == 0) {
	        bitstream_1[21-i-1] = 0;
	    }
		else {
		    bitstream_1[21-i-1] = data_1 % 2;
    		data_1 >>= 1;
		}
	}

	int bitstream_2[31];
	for (int i = 0; i < 31; i++) {
	    if (data_2 == 0) {
	        bitstream_2[31-i-1] = 0;
	    }
		else {
		    bitstream_2[31-i-1] = data_2 % 2;
    		data_2 >>= 1;
		}
	}

	for (int i = 0; i < 21; i++) {
	    if (bitstream_1[i] == 1) {
	        DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_0);
	        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_1);
	    }
	    else {
	        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_0);
	        DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_1);
	    }
		delay(110);
	}

	for (int i = 0; i < 31; i++) {
	    if (bitstream_2[i] == 1) {
	    	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_0);
	        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_1);
	    }
	    else {
	        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_0);
	        DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_1);
	    }
		delay(110);
	}
}

// results[0] = pin 2.2
// results[1] = pin 2.4
// results[2] = pin 2.3
XMC_VADC_RESULT_SIZE_t results[3];
int n = 3;

void ADC_Measurement_Handler() {

	// When n is 3, data from all 3 sensors have been read.
	// We can then get the bitstream.
	if (n==3) {
		n = 0;

		// Packet to be transmitted.
		// Packet is separated into two sections, one 21 bit
		// data and one 31 bit data.
		// The value here is still in decimal form and needs
		// to be converted to 1s and 0s.
		uint32_t data_1;
		uint32_t data_2;

		// Logic to compute the check bit set to even parity,
		// such that parity bit is set to 0 when the number
		// of 1s is even.
		if (is_even_parity(results[0])==1){
			data_1 = 0x1FC000 + (results[0] << 1);
		}
		else{
			data_1 = 0x1FC001 + (results[0] << 1);
		}
		if (is_even_parity(results[1])==1){
			data_2 = (results[1] << 20) + 0x0001;
		}
		else{
			data_2 = (results[1] << 20) + 0x40001;
		}
		if (is_even_parity(results[2])==1){
			data_2 = data_2 + (results[2] << 6);
		}
		else{
			data_2 = data_2 + (results[2] << 6) + 0x0020;
		}

		get_bitstream(data_1, data_2);
		delay(110);

		ADC_MEASUREMENT_StartConversion(&ADC_MEASUREMENT_0);
	}

	results[n] = ADC_MEASUREMENT_GetGlobalResult();
	n++;

}

int main(void) {
	DAVE_STATUS_t status;
	//PWM_STATUS_t status1;
	status = DAVE_Init();           /* Initialization of DAVE APPs  */

	if(status != DAVE_STATUS_SUCCESS) {
		/* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
		XMC_DEBUG("DAVE APPs initialization failed\n");
		while(1U);
	}

	ADC_MEASUREMENT_StartConversion(&ADC_MEASUREMENT_0);
	/* Placeholder for user application code. The while loop below can be replaced with user application code. */

	while(1U) {
		//more code here
	}

}
