#include <Arduino.h>
#include "interface.h"
#include "hardware_def.h"

void setup()
{
	Serial.begin(115200);
	Serial.print("TubeScreamer Module Firmware Starting...\n");
	initInterface();
	//testLedBars();
}


void loop()
{
	uint8_t updateLedsFlag = readPots();
	for(uint8_t i=0; i<NUM_CONT_POTS; i++)
	{
		//ledBars[i].value = contPots[i].pos;
		ledBar_Update(&ledBars[i]);
	}
	if(updateLedsFlag)
		FastLED.show();
	
	if(handleToggle1Flag)
	{
		handleToggle1();
	}
	if(handleToggle2Flag)
	{
		handleToggle2();
	}
	delay(10);
}