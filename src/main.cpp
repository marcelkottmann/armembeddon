#include <Arduino.h>
#include "codon_rt_lib.h"

extern "C" int codon_main(int argc, char *argv[]);

void setup()
{
  Serial.begin(115200);
  delay(500); // wait for serial monitor to open
  Serial.println("-- Starting codon program --");
  int exitCode=codon_main(0, NULL);
  Serial.printf("-- codon program terminated with exit code %d --",exitCode);
}


void loop()
{
}

