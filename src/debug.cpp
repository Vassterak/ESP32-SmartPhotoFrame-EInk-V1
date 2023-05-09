#include <Arduino.h>
#include "debug.h"

void Debug::printLine(const char *input)
{
    Serial.println(input);
}