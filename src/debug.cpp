#include <Arduino.h>
#include "debug.h"

void Debug::printLine(const char *input)
{
    Serial.println(input);
}
void Debug::printLine(const char input)
{
    Serial.println(input);
}

void Debug::print(const char *input)
{
    Serial.print(input);
}
void Debug::print(const char input)
{
    Serial.print(input);
}