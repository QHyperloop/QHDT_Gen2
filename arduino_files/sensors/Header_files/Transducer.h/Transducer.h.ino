#include "Arduino.h"
#include <Wire.h>

class Transducer{
  public:
    Transducer(int b1, int b2, int b4, int tmp, int trans);
    void initTran();
    void runTran();
    int bit1;
    int bit2;
    int bit4;
    int temp;
    int transducer;

  private:
   const float PRESSURE_SLOPE = 0.126;
   const float BIAS = -0.5 * 1024 / 5;

  
};
