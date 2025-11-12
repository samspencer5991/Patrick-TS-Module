#ifndef MAIN_H
#define MAIN_H

#include "hardware_def.h"
#include "stdint.h"
#include "led_bar.h"

#define NUM_PRESETS 128

// Defines the hardware type of the toggle switch
typedef enum
{
	ToggleCentreMomentary, // Centre default momentary switch with A/C/B options
	ToggleCentreLatch,	   // Centre default latching switch with A/C/B options
	ToggleMomentary,	   // Momentary switch A/B options only
	ToggleLatch			   // Latching switch A/B options only
} ToggleSwitchType;

typedef struct
{
	PotMode mode;		// Control UI mode
	PotLedMode ledMode; // Control LED UI mode

} ContinuousPotConfig;

typedef struct
{
	ToggleSwitchType mode;

} ToggleSwitchConfig;

// Module specific settings which are not changed
typedef struct
{
	ContinuousPotConfig contPotConfigs[NUM_CONT_POTS];
	ToggleSwitchConfig toggleSwitchConfigs[NUM_TOGGLE_SWITCHES];
} ModuleSettings;

// Values for each preset
typedef struct
{
	uint32_t id;									 // Tracks changes from the editor
	uint16_t contPotValues[NUM_CONT_POTS];			 // Continuous pot values 0-1023
	uint8_t toggleSwitchValues[NUM_TOGGLE_SWITCHES]; // Toggle switch values 0
	char name[16];									 // Preset name
} Preset;

// Global settings which can be configured
typedef struct
{
	uint8_t bootState;							// Boot state flag handled by esp32_settings
	uint32_t profileId;							// Tracks changes from the editor
	uint8_t ledBrightness;						// Global LED brightness 0-255
	CHSV toggleColours[NUM_TOGGLE_SWITCHES][3]; // 24-bit RGB colours for A/B/C positions
	CHSV contPotColours[NUM_CONT_POTS][2];			// 24-bit RGB colours
	PotLedMode contPotLedModes[NUM_CONT_POTS];
} GlobalSettings;

extern GlobalSettings globalSettings;
extern Preset presets[NUM_PRESETS];

void assignGlobalLedBarSettings();
void presetUp();
void presetDown();
void goToPreset(uint16_t presetIndex);

#endif // MAIN_H