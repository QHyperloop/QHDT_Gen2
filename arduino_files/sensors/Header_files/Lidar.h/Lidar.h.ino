#include "Arduino.h"
#include <Wire.h>

class Lidar{
  public:
    Lidar(int trigger, int monit0r, int laserR, int laserL, unsigned long pulse);
    int lidar_trigger;
    int lidar_monitor;
    int laser_left;
    int laser_right;
    void initLidar();
    void runLidar();
    unsigned long pulseWidth;
};
