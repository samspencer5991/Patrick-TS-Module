#ifndef MIDI_HANDLING_H_
#define MIDI_HANDLING_H_
#include "stdint.h"

void midi_SendDeviceApiSysExString(const char* array, unsigned size, uint8_t containsFraming);


#endif // MIDI_HANDLING_H_