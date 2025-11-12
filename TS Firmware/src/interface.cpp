#include "interface.h"
#include "Arduino.h"
#include "hardware_def.h"
#include "mcp3008.h"
#include "continuous_pot.h"
#include "led_bar.h"
#include "main.h"

CRGB leds[NUM_LEDS];
LEDBar ledBars[NUM_CONT_POTS];
ContPot contPots[NUM_CONT_POTS];

void initLedBars();
void printPots();

volatile uint8_t handleToggle1Flag = 0;
volatile uint8_t handleToggle2Flag = 0;

void toggle1ISR();
void toggle2ISR();

void handleToggle1();
void handleToggle2();

void initInterface()
{
	Serial.println("Initialising Interface");
	FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(50);
	FastLED.show();

	// Continuous Pots
	ADC_SPI.setSCK(ADC_SPI_SCK_PIN);
	ADC_SPI.setRX(ADC_SPI_MISO_PIN);
	ADC_SPI.setTX(ADC_SPI_MOSI_PIN);
	ADC_SPI.begin();

	pinMode(ADC_SPI_CS_PIN, OUTPUT);

	digitalWrite(ADC_SPI_CS_PIN, HIGH);
	for(uint8_t i=0; i<NUM_CONT_POTS; i++)
	{
		contPot_init(&contPots[i]);
	}
	initLedBars();

	// Toggle Switches
	pinMode(TOGGLE1A_PIN, INPUT_PULLUP);
	pinMode(TOGGLE1B_PIN, INPUT_PULLUP);
	pinMode(TOGGLE2A_PIN, INPUT_PULLUP);
	pinMode(TOGGLE2B_PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(TOGGLE1A_PIN), toggle1ISR, CHANGE);
	attachInterrupt(digitalPinToInterrupt(TOGGLE1B_PIN), toggle1ISR, CHANGE);
	attachInterrupt(digitalPinToInterrupt(TOGGLE2A_PIN), toggle2ISR, CHANGE);
	attachInterrupt(digitalPinToInterrupt(TOGGLE2B_PIN), toggle2ISR, CHANGE);

	Serial.println("Interface Initialised");
}

uint8_t readPots()
{
	static uint16_t lastPotValues[NUM_CONT_POTS];
	static uint8_t lastPotDirections[NUM_CONT_POTS];
	int contResults[NUM_CONT_POTS*2];
	uint8_t updateFlag = 0;
	static uint8_t readIndex = 0;
	// Read raw ADC values
	contResults[0] = mcp3008_readADC(&ADC_SPI, ADC_SPI_CS_PIN, POT1A_ADC_CHANNEL, 0);
	contResults[1] = mcp3008_readADC(&ADC_SPI, ADC_SPI_CS_PIN, POT1B_ADC_CHANNEL, 0);
	contResults[2] = mcp3008_readADC(&ADC_SPI, ADC_SPI_CS_PIN, POT2A_ADC_CHANNEL, 0);
	contResults[3] = mcp3008_readADC(&ADC_SPI, ADC_SPI_CS_PIN, POT2B_ADC_CHANNEL, 0);
	contResults[4] = mcp3008_readADC(&ADC_SPI, ADC_SPI_CS_PIN, POT3A_ADC_CHANNEL, 0);
	contResults[5] = mcp3008_readADC(&ADC_SPI, ADC_SPI_CS_PIN, POT3B_ADC_CHANNEL, 0);


	// Filter and process results for the continuous pots
	for (uint8_t i = 0; i < NUM_CONT_POTS; i++)
	{
		// Process absolute values
		contPot_update(&contPots[i], contResults[i * 2], contResults[i * 2 + 1]);
		// Map filtered values to interface structure

		int valueDelta = contPots[i].pos - lastPotValues[i]; // Amount of change in the pot's absolute position
		// Apply min/max boundaries
		int tempPotValue = ledBars[i].value + valueDelta;
		if (abs(valueDelta) < POT_WRAP_THRESHOLD)
		{
			if (tempPotValue < 0)
				ledBars[i].value = 0;

			else if (tempPotValue > 1023)
				ledBars[i].value = 1023;

			else
				ledBars[i].value += valueDelta;
		}
		if (lastPotValues[i] != contPots[i].pos)
		{
			lastPotValues[i] = contPots[i].pos;
			updateFlag = 1;
		}
		lastPotDirections[i] = contPots[i].dir;
	}

#ifdef SERIAL_PRINT_POTS
	if (updateFlag)
		printPots();
#endif
	return updateFlag;
}

void printPots()
{
	char str[20];
#ifdef SERIAL_PRINT_POTS
	for (uint8_t i = 0; i < NUM_CONT_POTS; i++)
	{
		sprintf(str, "%d Pos: %4d %d  ", i + 1, ledBars[i].value, contPots[i].dir);
		Serial.print(str);
	}
	Serial.println();
#endif
}

void initLedBars()
{
	ledBar_Init(&ledBars[0], POT1_LED_INDEX_START, NUM_LEDS_PER_RING, PotNormal,
				PotWrap, globalSettings.contPotLedModes[0], 1, globalSettings.contPotColours[0][0], globalSettings.contPotColours[0][1]);
	ledBar_Init(&ledBars[1], POT2_LED_INDEX_START, NUM_LEDS_PER_RING, PotNormal,
				PotWrap, globalSettings.contPotLedModes[1], 1, globalSettings.contPotColours[1][0], globalSettings.contPotColours[1][1]);
	ledBar_Init(&ledBars[2], POT3_LED_INDEX_START, NUM_LEDS_PER_RING, PotCentred,
				PotWrap, globalSettings.contPotLedModes[2], 1, globalSettings.contPotColours[2][0], globalSettings.contPotColours[2][1]);
}

void toggle1ISR()
{
	handleToggle1Flag = 1;
}

void toggle2ISR()
{
	handleToggle2Flag = 1;
}

void handleToggle1()
{
	delay(20);
	// Read the states of both pins to determine the toggle position
	bool stateA = digitalRead(TOGGLE1A_PIN);
	bool stateB = digitalRead(TOGGLE1B_PIN);
	if(stateA && !stateB)
	{
		Serial.println("Toggle 1: Left Position");
		leds[TOGGLE1_LEFT_LED] = globalSettings.toggleColours[0][0];
		leds[TOGGLE1_MID_LED] = 0;
		leds[TOGGLE1_RIGHT_LED] = 0;
	}
	else if(!stateA && stateB)
	{
		Serial.println("Toggle 1: Right Position");
		leds[TOGGLE1_LEFT_LED] = 0;
		leds[TOGGLE1_MID_LED] = 0;
		leds[TOGGLE1_RIGHT_LED] = globalSettings.toggleColours[0][2];
	}
	else if(stateA && stateB)
	{
		Serial.println("Toggle 1: Middle Position");
		leds[TOGGLE1_LEFT_LED] = 0;
		leds[TOGGLE1_MID_LED] = globalSettings.toggleColours[0][1];
		leds[TOGGLE1_RIGHT_LED] = 0;
	}
	else
	{
		Serial.println("Toggle 1: Invalid Position");
	}
	FastLED.show();
	handleToggle1Flag = 0;
}

void handleToggle2()
{
	delay(20);
	// Read the states of both pins to determine the toggle position
	bool stateA = digitalRead(TOGGLE2A_PIN);
	bool stateB = digitalRead(TOGGLE2B_PIN);
	if(stateA && !stateB)
	{
		Serial.println("Toggle 2: Left Position");
		leds[TOGGLE2_LEFT_LED] = globalSettings.toggleColours[1][0];
		leds[TOGGLE2_MID_LED] = 0;
		leds[TOGGLE2_RIGHT_LED] = 0;
	}
	else if(!stateA && stateB)
	{
		Serial.println("Toggle 2: Right Position");
		leds[TOGGLE2_LEFT_LED] = 0;
		leds[TOGGLE2_MID_LED] = 0;
		leds[TOGGLE2_RIGHT_LED] = globalSettings.toggleColours[1][2];
	}
	else if(stateA && stateB)
	{
		Serial.println("Toggle 2: Middle Position");
		Serial.println("Toggle 1: Middle Position");
		leds[TOGGLE2_LEFT_LED] = 0;
		leds[TOGGLE2_MID_LED] = globalSettings.toggleColours[1][1];
		leds[TOGGLE2_RIGHT_LED] = 0;
	}
	else
	{
		Serial.println("Toggle 2: Invalid Position");
	}
	FastLED.show();
	handleToggle2Flag = 0;
}


void testLedBars()
{
	fill_gradient(leds, 0, csvNeonPink, NUM_LEDS-1, csvNeonLightBlue, SHORTEST_HUES);
	FastLED.show();
}