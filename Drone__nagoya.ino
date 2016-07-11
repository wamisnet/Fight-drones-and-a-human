#include"Drone_IR.h"
#include <Nefry.h>
//データをインターネットに送信します
#include <Nefry_Milkcocoa.h>
Nefry_Milkcocoa *milkcocoa;
char *datastore;

//Nefry pin connection
//D0   :D1:D2:D3   :D4  :D5
//IRout:  :  :motor:IRin:

//Nefry LED State
//Green :何の処理もしていない(0,255,0)
//Red   :Err(255,0,0)
//Blue系:処理中 ---+---オレンジ（255,0,255）:赤外線送信処理中
//                 +---黄緑(0,255,255)　　　:赤外線受信処理中

int HP, MODE; //体力
int userdata[2];
void hpControl(int id, int damage);

void setup() {
  Drone.setup(D4, D0);
  milkcocoa->print();//Setup ModuleにAPPIDの表示をする
  milkcocoa = Nefry_Milkcocoa::begin();
  datastore = Nefry.getConfStr(3);
  for (int i = 0; i < 2; i++)
    userdata[i] = Nefry.getConfValue(i);
  MODE = Nefry.getConfValue(2);
  Drone.setHP(Nefry.getConfValue(3));
  Drone.webPrint();
  Drone.motor_setup(D3);
  Drone.motorTime(2000);//起動完了したら振動する
}

void loop() {
  if (MODE == 1) {
    //赤外線送信モード
    if (Nefry.push_SW()) {
      Nefry.setLed(255, 0, 255);
      Drone.IRSend(userdata);
      Nefry.setLed(0, 255, 0);
    }
  } else {
    //赤外線受信モード
    if (Drone.IRGet()) {
      Nefry.print(Drone.getID());
      Nefry.print(" : ");
      Nefry.println(Drone.getDamage());
      hpControl(Drone.getID(), Drone.getDamage());
    }
  }
  Nefry.ndelay(10);
}
void hpControl(int id, int damage) {
  Nefry.setLed(0, 0, 255);
  Drone.motorTime(1000);
  DataElement elem = DataElement();
  Drone.hitHP(damage);
  elem.setValue("ID", Drone.getID());
  elem.setValue("HP", Drone.getHP());
  if ( milkcocoa->loop(10000)) {
    Nefry.println( "Milkcocoa Connect OK" );
    milkcocoa->push(datastore, &elem);
  } else {
    Nefry.println("Milkcocoa Connect NG" );
    Nefry.setLed(255, 0, 0);
    Nefry.ndelay(1000);
    return;
  }
  Nefry.ndelay(1000);
  Nefry.setLed(0, 255, 0);
}
