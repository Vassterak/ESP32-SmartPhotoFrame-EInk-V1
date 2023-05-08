#include <Arduino.h>

#define LEDPIN GPIO_NUM_23

void setup()
{
  // put your setup code here, to run once:
  pinMode(LEDPIN, OUTPUT);

}

void loop()
{
  digitalWrite(LEDPIN, HIGH);
  delay(1000);
  digitalWrite(LEDPIN, LOW);
  delay(1000);
  // put your main code here, to run repeatedly:
}
