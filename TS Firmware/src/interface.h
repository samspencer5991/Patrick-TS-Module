#ifndef INTERFACE_H
#define INTERFACE_H

#include "led_bar.h"
#include "continuous_pot.h"

extern CRGB leds[];
extern LEDBar ledBars[];
extern ContPot contPots[];

extern volatile uint8_t handleToggle1Flag;
extern volatile uint8_t handleToggle2Flag;

void initInterface();
uint8_t readPots();
void testLedBars();

void handleToggle1();
void handleToggle2();

#endif // INTERFACE_H