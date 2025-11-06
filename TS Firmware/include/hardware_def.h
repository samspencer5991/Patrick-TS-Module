#ifndef HARDWARE_DEF_H
#define HARDWARE_DEF_H
#include "Arduino.h"

//------------- Pin Definitions -------------//
#define MODULE_STATUS_PIN		3
#define MODULE_TX_PIN			4
#define MODULE_RX_PIN			5
#define MODULE_SERIAL 			Serial2

#define LED_DATA_PIN			17

#define ADC_SPI_SCK_PIN			26
#define ADC_SPI_MISO_PIN		24
#define ADC_SPI_MOSI_PIN		27
#define ADC_SPI_CS_PIN			25

// Not used in the current prototype
#define POT1_SWITCH_PIN			0
#define POT2_SWITCH_PIN			1
#define POT3_SWITCH_PIN			2

#define TOGGLE1A_PIN			29
#define TOGGLE1B_PIN			28
#define TOGGLE2A_PIN			19
#define TOGGLE2B_PIN			18


//-------------- ADC Channels --------------//
#define ADC_SPI					SPI1
#define POT1A_ADC_CHANNEL		6
#define POT1B_ADC_CHANNEL		7
#define POT2A_ADC_CHANNEL		0
#define POT2B_ADC_CHANNEL		1
#define POT3A_ADC_CHANNEL		3
#define POT3B_ADC_CHANNEL		5

#define NUM_CONT_POTS			3
#define NUM_TOGGLE_SWITCHES		2


//------------------ LEDs ------------------//
#define NUM_LEDS				54
#define NUM_LEDS_PER_RING	 	16
#define POT1_LED_INDEX_START	16
#define POT2_LED_INDEX_START	0
#define POT3_LED_INDEX_START	32
#define	TOGGLE1_LEFT_LED		48
#define TOGGLE1_MID_LED			49
#define TOGGLE1_RIGHT_LED		50
#define	TOGGLE2_LEFT_LED		51
#define TOGGLE2_MID_LED			52
#define TOGGLE2_RIGHT_LED		53











#endif // HARDWARE_DEF_H //