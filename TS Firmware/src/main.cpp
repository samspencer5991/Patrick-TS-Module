#include <Arduino.h>
#include "interface.h"
#include "hardware_def.h"
#include "esp32_settings.h"
#include "main.h"
#include "device_api.h"

GlobalSettings globalSettings;
Preset presets[NUM_PRESETS];
char deviceApiBuffer[8192];
volatile uint8_t updateLedsFlag = 0;

void defaultGlobalSettingsAssignment();
void defaultPresetsAssignment();

void setup()
{
	Serial.begin(115200);
	delay(1000);
	Serial.print("TubeScreamer Module Firmware Starting...\n");

	// Assign global and preset settings and boot the file system
	esp32Settings_AssignDefaultGlobalSettings(defaultGlobalSettingsAssignment);
	esp32Settings_AssignDefaultPresetSettings(defaultPresetsAssignment);
	uint8_t bootFlag = esp32Settings_BootCheck(&globalSettings, sizeof(GlobalSettings), presets, sizeof(Preset),
												NUM_PRESETS, &globalSettings.bootState);


	initInterface();
	//testLedBars();
}


void loop()
{
	updateLedsFlag = readPots();
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
	if(Serial.available())
	{
		deviceApi_Handler(deviceApiBuffer, 0);
	}
}

void defaultGlobalSettingsAssignment()
{
	// Standard global settings
	globalSettings.ledBrightness = 100;

	// Pots
	globalSettings.contPotColours[0][0] = CSV_NEON_PINK;
	globalSettings.contPotColours[0][1] = CSV_NEON_LIGHT_BLUE;
	globalSettings.contPotLedModes[0] = PotLedFillGradient;

	globalSettings.contPotColours[1][0] = CSV_NEON_PINK;
	globalSettings.contPotColours[1][1] = CSV_NEON_LIGHT_BLUE;
	globalSettings.contPotLedModes[0] = PotLedFillGradient;

	globalSettings.contPotColours[2][0] = CSV_NEON_PINK;
	globalSettings.contPotColours[2][1] = CSV_NEON_LIGHT_BLUE;
	globalSettings.contPotLedModes[0] = PotLedFillGradient;

	// Toggle switch colours
	globalSettings.toggleColours[0][0] = CSV_NEON_RED;
	globalSettings.toggleColours[0][1] = CSV_NEON_PINK;
	globalSettings.toggleColours[0][2] = CSV_NEON_SEAFOAM;

	globalSettings.toggleColours[1][0] = CSV_NEON_RED;
	globalSettings.toggleColours[1][1] = CSV_NEON_PINK;
	globalSettings.toggleColours[1][2] = CSV_NEON_SEAFOAM;

}

void defaultPresetsAssignment()
{
	for(uint8_t i=0; i<NUM_PRESETS; i++)
	{
		for(uint8_t j=0; j<NUM_CONT_POTS; j++)
		{
			presets[i].contPotValues[j] = 512;
		}
		for(uint8_t k=0; k<NUM_TOGGLE_SWITCHES; k++)
		{
			presets[i].toggleSwitchValues[k] = 0;
		}
		sprintf(presets[i].name, "Preset %d", i+1);
	}
}

void assignGlobalLedBarSettings()
{
	for(uint8_t i=0; i<NUM_CONT_POTS; i++)
	{
		ledBars[i].colours[0] = CHSV(	globalSettings.contPotColours[i][0].raw[0],
										globalSettings.contPotColours[i][0].raw[1],
										globalSettings.contPotColours[i][0].raw[2]);

		ledBars[i].colours[1] = CHSV(	globalSettings.contPotColours[i][1].raw[0],
										globalSettings.contPotColours[i][1].raw[1],
										globalSettings.contPotColours[i][1].raw[2]);

		ledBars[i].ledMode = globalSettings.contPotLedModes[i];
		ledBar_Update(&ledBars[i]);
	}
	FastLED.show();
}

void presetUp()
{

}

void presetDown()
{

}

void goToPreset(uint16_t presetIndex)
{

}