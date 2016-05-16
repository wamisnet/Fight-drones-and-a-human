#ifndef Drone_IR_h
#define Drone_IR_h
#include<Arduino.h>
#define LOW_STATE 0
#define HIGH_STATE 1
class Drone_IR {
  public:
    void setup(int, int,bool in_order=0,bool out_order=0), 
		motor_setup(int pin),
		led_setup(int pin),
		motor_time(int time), 
		led_color(char green, char blue, bool red);
    void IR_send(int *, int);
    int IR_get(unsigned long data[255]);
    unsigned long IR_get_long();
  private:
    void waitLow();
    int waitHigh();
};
extern Drone_IR Drone;
#endif
