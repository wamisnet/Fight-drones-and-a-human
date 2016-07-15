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

int HP, MODE, ID, autosend, senddelay; //体力
int userdata[2];
void hpControl(int id, int damage);

void setup() {
  Drone.setup(D4, D0);
  milkcocoa->print();//Setup ModuleにAPPIDの表示をする
  milkcocoa = Nefry_Milkcocoa::begin("appleioms482c", "LGBKAPNALIFMHAJG", "IVhLdPYfAWPFCBAfkNYGOhCYTNOCYCLHOBRJkFVD");
  // datastore = Nefry.getConfStr(3);
  for (int i = 0; i < 2; i++)
    userdata[i] = Nefry.getConfValue(i);
  MODE =  Nefry.getConfValue(2);
  autosend = Nefry.getConfValue(4);
  senddelay = Nefry.getConfValue(5);
  Drone.setHP(Nefry.getConfValue(3));
  Drone.webPrint();
  Drone.motor_setup(D3);
  Drone.motorTime(500);//起動完了したら振動する
}

void loop() {
  Nefry.setLed(0,255,0);
  if (MODE == 1) {
    //赤外線送信モード
    if (Nefry.push_SW() || autosend == 1) {
      Nefry.setLed(255, 0, 255);
      Drone.IRSend(userdata);
      Nefry.ndelay(senddelay);
      Nefry.setLed(0, 255, 0);
    }
  } else {
    //赤外線受信モード
    if (Drone.IRGet()) {
      Nefry.print(Drone.getID());
      Nefry.print(" : ");
      Nefry.println(Drone.getDamage());
      Nefry.ndelay(50);
      hpControl(Drone.getID(), Drone.getDamage());
    }
  }
  Nefry.ndelay(10);
}
void hpControl(int id, int damage) {
  Nefry.setLed(0, 0, 255);
  Drone.motorTime(500);
  DataElement elem = DataElement();
  Drone.hitHP(damage);

  elem.setValue("ID", ID);
  elem.setValue("attackerID", Drone.getID());
  elem.setValue("HP", Drone.getHP());
  if ( milkcocoa->loop(10000)) {
    Nefry.println( "Milkcocoa Connect OK" );
    //milkcocoa->push(datastore, &elem);
    milkcocoa->push("HP", &elem);
  } else {
    Nefry.println("Milkcocoa Connect NG" );
    Nefry.setLed(255, 0, 0);
    Nefry.ndelay(1000);
    
    return;
  }
  if (Drone.getHP() <= 0) { //体力がなくなったとき
    Nefry.println("HP残りなし");
    while (1) {
      Nefry.setLed(255, 0, 0);
      Drone.motorTime(100);
      Nefry.ndelay(50);
    }
  }
  Nefry.print("HP:");
  Nefry.println(Drone.getHP());
  Nefry.ndelay(1000);
  Nefry.setLed(0, 255, 0);
}
