#ifndef Drone_h
#define Drone_h
#include<Arduino.h>
#include <SoftwareSerial.h>
/*
int SWs[] = { 6,7,8 };
int IDs[] = { 2,4,5 };
int LEDs[] = { A0,A1,A2,A3,A4 };
*/
class DroneParts {
  public:
	  void begin(int _motor, int _mode, int* _id, int* _sw, int* _led),
	  //int _motor = 3, int _mode = A5, int* _id= IDs, int* _sw = SWs, int* _led= LEDs),
		  setLed(int _led,bool _state),
		  setBar(int _bar),
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
extern DroneParts DroneIO;

#endif


