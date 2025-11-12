#include "device_api_handler.h"
#include "device_api_utility.h"
#include "device_api.h"
#include "ArduinoJson.h"
#include "math.h"
#include "main.h"
#include "esp32_settings.h"

// Transmit functions
void sendCheckResponse(uint8_t transport)
{
	// Allocate the JSON document
	JsonDocument doc;

	// Device information
	doc["deviceModel"] = "TS Module";
	doc["firmwareVersion"] = FW_VERSION;
	doc["hardwareVersion"] = HW_VERSION;
	char uidStr[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];
	pico_get_unique_board_id_string(uidStr, 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1);
	doc["uId"] = uidStr;
	doc["deviceName"] = "TS Module";
	doc["profileId"] = globalSettings.profileId;

	if (transport == USB_CDC_TRANSPORT)
	{
		serializeJson(doc, Serial);
		sendPacketTermination(USB_CDC_TRANSPORT);
	}
	else if (transport == MIDI_TRANSPORT)
	{
		CustomWriter writer;
		writer.transport = MIDI_TRANSPORT;
		serializeJson(doc, writer);
		writer.flush();
		sendPacketTermination(MIDI_TRANSPORT);
	}
	return;
}

void sendGlobalSettings(uint8_t transport)
{
	JsonDocument doc;
	doc["ledBrightness"] = globalSettings.ledBrightness;
	for(uint8_t i=0; i<NUM_CONT_POTS; i++)
	{
		doc["continuousPots"][i]["colours"][0]["hue"] = globalSettings.contPotColours[i][0].raw[0];
		doc["continuousPots"][i]["colours"][0]["sat"] = globalSettings.contPotColours[i][0].raw[1];
		doc["continuousPots"][i]["colours"][0]["val"] = globalSettings.contPotColours[i][0].raw[2];

		doc["continuousPots"][i]["colours"][1]["hue"] = globalSettings.contPotColours[i][1].raw[0];
		doc["continuousPots"][i]["colours"][1]["sat"] = globalSettings.contPotColours[i][1].raw[1];
		doc["continuousPots"][i]["colours"][1]["val"] = globalSettings.contPotColours[i][1].raw[2];

		if(globalSettings.contPotLedModes[i] == PotLedFill)
			doc["continuousPots"][i]["ledMode"] = "fill";
		else if(globalSettings.contPotLedModes[i] == PotLedDot)
			doc["continuousPots"][i]["ledMode"] = "dot";
		else if(globalSettings.contPotLedModes[i] == PotLedFillGradient)
			doc["continuousPots"][i]["ledMode"] = "fillGradient";
		else if(globalSettings.contPotLedModes[i] == PotLedDotGradient)
			doc["continuousPots"][i]["ledMode"] = "dotGradient";
	}
	for(uint8_t i=0; i<NUM_TOGGLE_SWITCHES; i++)
	{
		doc["toggleSwitches"][i]["colours"][0]["hue"] = globalSettings.toggleColours[i][0].raw[0];
		doc["toggleSwitches"][i]["colours"][0]["sat"] = globalSettings.toggleColours[i][0].raw[1];
		doc["toggleSwitches"][i]["colours"][0]["val"] = globalSettings.toggleColours[i][0].raw[2];

		doc["toggleSwitches"][i]["colours"][1]["hue"] = globalSettings.toggleColours[i][1].raw[0];
		doc["toggleSwitches"][i]["colours"][1]["sat"] = globalSettings.toggleColours[i][1].raw[1];
		doc["toggleSwitches"][i]["colours"][1]["val"] = globalSettings.toggleColours[i][1].raw[2];

		doc["toggleSwitches"][i]["colours"][2]["hue"] = globalSettings.toggleColours[i][2].raw[0];
		doc["toggleSwitches"][i]["colours"][2]["sat"] = globalSettings.toggleColours[i][2].raw[1];
		doc["toggleSwitches"][i]["colours"][2]["val"] = globalSettings.toggleColours[i][2].raw[2];
	}
	if (transport == USB_CDC_TRANSPORT)
	{
		serializeJson(doc, Serial);
		sendPacketTermination(USB_CDC_TRANSPORT);
	}
	else if (transport == MIDI_TRANSPORT)
	{
		CustomWriter writer;
		writer.transport = MIDI_TRANSPORT;
		serializeJson(doc, writer);
		writer.flush();
		sendPacketTermination(MIDI_TRANSPORT);
	}
}

void sendBankSettings(int bankNum, uint8_t transport)
{
	JsonDocument doc;
	for(uint8_t i=0; i<NUM_CONT_POTS; i++)
	{
		doc["continuousPots"][i]["value"] = presets[bankNum].contPotValues[i];
	}
	for(uint8_t i=0; i<NUM_TOGGLE_SWITCHES; i++)
	{
		doc["continuousPots"][i]["value"] = presets[bankNum].contPotValues[i];
	}
	
	if (transport == USB_CDC_TRANSPORT)
	{
		serializeJson(doc, Serial);
		sendPacketTermination(USB_CDC_TRANSPORT);
	}
	else if (transport == MIDI_TRANSPORT)
	{
		CustomWriter writer;
		writer.transport = MIDI_TRANSPORT;
		serializeJson(doc, writer);
		writer.flush();
		sendPacketTermination(MIDI_TRANSPORT);
	}
	return;
}

void sendBankId(int bankNum, uint8_t transport)
{
}

void sendCurrentBank(uint8_t transport)
{
	JsonDocument doc;

	if (transport == USB_CDC_TRANSPORT)
	{
		serializeJson(doc, Serial);
		sendPacketTermination(USB_CDC_TRANSPORT);
	}
	else if (transport == MIDI_TRANSPORT)
	{
		CustomWriter writer;
		writer.transport = MIDI_TRANSPORT;
		serializeJson(doc, writer);
		writer.flush();
		sendPacketTermination(MIDI_TRANSPORT);
	}
}

// Parsing functions
void parseGlobalSettings(char *appData, uint8_t transport)
{
	// Allocate the JSON document
	JsonDocument doc;

	// Deserialize the JSON document
	DeserializationError error = deserializeJson(doc, appData);
	// Test if parsing succeeds
	if (error)
	{
		Serial.printf("deserializeJson() failed: %s\n", error.c_str());
		Serial.write(appData, strlen(appData));
		return;
	}

	if(doc["continuousPots"].is<JsonArray>())
	{
		for(uint8_t i=0; i<NUM_CONT_POTS; i++)
		{
			globalSettings.contPotColours[i][0].raw[0] = doc["continuousPots"][i]["colours"][0]["hue"];
			globalSettings.contPotColours[i][0].raw[1] = doc["continuousPots"][i]["colours"][0]["sat"];
			globalSettings.contPotColours[i][0].raw[2] = doc["continuousPots"][i]["colours"][0]["val"];

			globalSettings.contPotColours[i][1].raw[0] = doc["continuousPots"][i]["colours"][1]["hue"];
			globalSettings.contPotColours[i][1].raw[1] = doc["continuousPots"][i]["colours"][1]["sat"];
			globalSettings.contPotColours[i][1].raw[2] = doc["continuousPots"][i]["colours"][1]["val"];

			if(strcmp(doc["continuousPots"][i]["ledMode"], "fill") == 0)
				globalSettings.contPotLedModes[i] = PotLedFill;
			else if(strcmp(doc["continuousPots"][i]["ledMode"], "dot") == 0)
				globalSettings.contPotLedModes[i] = PotLedDot;
			else if(strcmp(doc["continuousPots"][i]["ledMode"], "fillGradient") == 0)
				globalSettings.contPotLedModes[i] = PotLedFillGradient;
			else if(strcmp(doc["continuousPots"][i]["ledMode"], "dotGradient") == 0)
				globalSettings.contPotLedModes[i] = PotLedDotGradient;
		}
	}

	if(doc["toggleSwitches"].is<JsonArray>())
	{
		for(uint8_t i=0; i<NUM_TOGGLE_SWITCHES; i++)
		{
			globalSettings.toggleColours[i][0].raw[0] = doc["toggleSwitches"][i]["colours"][0]["hue"];
			globalSettings.toggleColours[i][0].raw[1] = doc["toggleSwitches"][i]["colours"][0]["sat"];
			globalSettings.toggleColours[i][0].raw[2] = doc["toggleSwitches"][i]["colours"][0]["val"];
		}
	}

	// Update the values to the ledBar items
	assignGlobalLedBarSettings();

	esp32Settings_SaveGlobalSettings();
}

void parseBankSettings(char *appData, uint16_t bankNum, uint8_t transport)
{
	// Allocate the JSON document
	JsonDocument doc;

	// Deserialize the JSON document
	DeserializationError error = deserializeJson(doc, appData);
	// Test if parsing succeeds
	if (error)
	{
		Serial.printf("deserializeJson() failed: %s\n", error.c_str());
		return;
	}
	esp32Settings_SavePresets();
}

void ctrlCommandHandler(char *appData, uint8_t transport)
{
	// Allocate the JSON document
	JsonDocument doc;

	// Deserialize the JSON document
	DeserializationError error = deserializeJson(doc, appData);
	// Test if parsing succeeds
	if (error)
	{
		Serial.write("deserializeJson() failed: ", strlen("deserializeJson() failed: "));
		Serial.write(error.c_str(), strlen(error.c_str()));
		Serial.write("\n", strlen("\n"));
		return;
	}

	if (doc[USB_COMMAND_STRING])
	{
		// If there is an array of commands
		uint8_t numCommands = doc[USB_COMMAND_STRING].size();
		for (uint16_t i = 0; i < numCommands; i++)
		{
			// Strings
			if (doc[USB_COMMAND_STRING][i].is<const char *>())
			{
				const char *command = doc[USB_COMMAND_STRING][i];
				// Prioritise the restart command
				if (strcmp(command, USB_RESTART_STRING) == 0)
				{
					esp32Settings_SoftwareReset();
				}
				else if (strcmp(command, USB_ENTER_BOOTLOADER_STRING) == 0)
				{
					reset_usb_boot(0, 0);
				}
				else if (strcmp(command, USB_BANK_UP_STRING) == 0)
				{
					presetUp();
				}
				else if (strcmp(command, USB_BANK_DOWN_STRING) == 0)
				{
					presetDown();
				}
				else if (strcmp(command, "savePresets") == 0)
				{
					esp32Settings_SavePresets();
				}
				else if (strcmp(command, USB_FACTORY_RESET_STRING) == 0)
				{
					esp32Settings_ResetAllSettings();
				}
				else
				{
					Serial.print("Unknown CTRL command");
				}
			}
			else
			{
				if (!doc[USB_COMMAND_STRING][i][USB_GO_TO_BANK_STRING].isNull())
				{
					uint16_t presetIndex = doc[USB_COMMAND_STRING][i][USB_GO_TO_BANK_STRING];
					if (presetIndex < NUM_PRESETS)
					{
						goToPreset(presetIndex);
					}
				}
			}
		}
	}
}