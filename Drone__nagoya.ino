#include"Drone_IR.h"
#include"Drone.h"

//Nefry pin connection
//D0   :D1:D2:D3   :D4  :D5
//IRout:  :  :motor:IRin:

//Nefry LED State
//Green :何の処理もしていない(0,255,0)
//Red   :Err(255,0,0)
//Blue系:処理中 ---+---オレンジ（255,0,255）:赤外線送信処理中
//                 +---黄緑(0,255,255)　　　:赤外線受信処理中

int HP, MODE, autosend, senddelay; //体力
int userdata[2];
void hpControl(int id, int damage);

#define SW 1
void setup() {
 // DroneIR.setup(D4, D0);
  //DroneIR.motor_setup(D3);
  //Drone.motorTime(500);//起動完了したら振動する
}

void loop() {
  
  if (MODE == 1) {
    //赤外線送信モード
    if (digitalRead(1)|| autosend == 1) {
      DroneIR.IRSend(100);
    }
  } else {
    //赤外線受信モード
    if (DroneIR.IRGet()) {
      Serial.print(DroneIR.getDamageID());
      Serial.print(" : ");
      Serial.println(DroneIR.getDamageHP());
      delay(50);
      //hpControl(Drone.getID(), Drone.getDamage());
    }
  }
  delay(10);
}
void hpControl(int id, int damage) {
  //Drone.motorTime(500);
  //Drone.hitHP(damage);

  //elem.setValue("ID", ID);
  //elem.setValue("attackerID", Drone.getID());
  //elem.setValue("HP", Drone.getHP());

  if (DroneIR.getHP() <= 0) { //体力がなくなったとき
    Serial.println("HP残りなし");
    while (1) {
      DroneIO.setMotor(100);
      delay(50);
    }
  }
  Serial.print("HP:");
  Serial.println(DroneIR.getHP());
  delay(1000);
}
