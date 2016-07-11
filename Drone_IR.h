#ifndef Drone_IR_h
#define Drone_IR_h
#include<Arduino.h>
#include<Nefry.h>
#include<Esp.h>
#define LOW_STATE 0
#define HIGH_STATE 1
#define DATALEN 28
class Drone_IR {
  public:
    void setup(int, int, bool in_order = 0, bool out_order = 0),
         motor_setup(int pin),
         led_setup(int pin),
         motorTime(int time),
         led_color(char green, char blue, bool red),
         setHP(int hp),
         hitHP(int damage),
         webPrint(),
         IRSend(int irSend[2]);//赤外線の送信に使う　
    //irSend [0] :userID
    //irSend [1] :ダメ―ジ
    bool IRGet();//赤外線を受信したいときに使う、赤外線を取得したときにTrue
    int getID(),
        getHP(),
        getDamage();

  private:
    unsigned long IR_get_long();
    void IR_signal(int *, int),
         waitLow();
    int waitHigh(),
        IR_get(int data[255]),
        startpoint(int arraylen, int* array, int sp),
        databit(int *dataArray, int sp),
        startbit(int arraylen, int* array, int sp),
        binary(int decimal);
    bool range(int _range, int _source, int _data),
         stopbit(int *array, int sp);
};
extern Drone_IR Drone;
#endif
