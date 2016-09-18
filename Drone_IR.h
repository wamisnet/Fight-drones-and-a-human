#ifndef Drone_IR_h
#define Drone_IR_h
#include<Arduino.h>
#define LOW_STATE 0
#define HIGH_STATE 1
#define DATALEN 28
class Drone_IR {
  public:
	void setup(int _ID, int _hp, int _irIn=12, int _irOut=11),
         addHP(int hp),
         IRSend(int _damage);//赤外線の送信に使う　irSend [0] :userID		irSend [1] :ダメ―ジ
    bool IRGet();//赤外線を受信したいときに使う、赤外線を取得したときにTrue
	int getHP(),
		getDamageID(),
		getDamageHP();

  private:
	  int ir_in, ir_out, hp, id, idamage, state = HIGH_STATE, damageId;
    unsigned long IRRaw();
    void IR_signal(int *, int),
		 hitHP(int damage),
         waitLow();
    int waitHigh(),
		IRPacket(int data[500]),
        startpoint(int arraylen, int* array, int sp),
        databit(int *dataArray, int sp),
        startbit(int arraylen, int* array, int sp),
        binary(int decimal);
    bool range(int _range, int _source, int _data),
         stopbit(int *array, int sp);
};
extern Drone_IR DroneIR;
#endif


