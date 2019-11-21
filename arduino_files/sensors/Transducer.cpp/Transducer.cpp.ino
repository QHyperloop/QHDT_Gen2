#include "Arduino.h"
#include <Wire.h>
#include "Transducer.h"

Transducer::Transducer(int b1, int b2, int b4, int tmp, int trans){
    int bit1 = b1;
    int bit2 = b2;
    int bit4 = b4;
    int temp = tmp;
    int transducer = trans;
}

Transducer::initTran(){
  pinMode(transducer, INPUT);
  pinMode(bit1, OUTPUT);
  pinMode(bit2, OUTPUT);
  pinMode(bit4, OUTPUT);
  pinMode(temp, INPUT);

  
}

Transducer::runTran(){
  pressure = PRESSURE_SLOPE * ((float)analogRead(transducer) + BIAS) ;
  /*
    Serial.print("Pressure");
    Serial.print(":\t");
    Serial.print(pressure);
    Serial.print("psi");
  */
  Serial.print("\tpressure");
  //Serial.print("\ttemperatures:");
  for (int x = 0; x < 2; x++)
  {
    digitalWrite(bit4, x);
    for (int y = 0; y < 2; y++)
    {
      digitalWrite(bit2, y);
      for (int z = 0; z < 2; z++)
      {
        digitalWrite(bit1, z);
        float temperature = (float(analogRead(temp)) * 3.3 / 1024 - 0.5) * 1000 / 19.5;
        //Serial.print(temperature);
        //Serial.print("  ");

      }
    }
  }
  Serial.print("\ttemp");
  Serial.println();
}
