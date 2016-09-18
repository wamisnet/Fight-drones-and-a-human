#ifndef Drone_IR_h
#define Drone_IR_h
#include<Arduino.h>
#include <SoftwareSerial.h>

const int SW[] = { 6,7,8 };
const int ID[] = { 2,4,5 };
const int LED[] = { A0,A1,A2,A3,A4 };

class DroneParts {
  public:
	  void begin(int _motor = 3, int _mode = A5, int _id[3] = ID, int _sw[3] = SW, int _led[5] = LED),
		  setLed(int _led,bool _state),
		  setBar(int _bar, bool _state),
		  setMotor(int time),
		  wifiPrint(String s),
		  setId(),
		  loop(int _interval=5);
	  int listenSW(),
		  getId(),
		  readSW();
  private:
	  int motor, modeP,mode, *id, *sw, *led, storageLed,idChar;
	  int getStorage();
};
extern DroneParts Drone;
#endif


