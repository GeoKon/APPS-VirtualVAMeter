#include <cpuClass.h>
#include "MegunoLink.h"

float x = 0;

CPU cpu;
void setup() 
{
 cpu.init();  // Serial.begin(9600);
}

void loop() 
{
  TimePlot SinePlot("Sin"), CosinePlot("Cos");

  SinePlot.SendData("Sine", sin(x));
  CosinePlot.SendData("Cosine", cos(x));
  x = x + 0.1;

  delay(100);
}
