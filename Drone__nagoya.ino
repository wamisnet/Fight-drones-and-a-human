#include"Drone_IR.h"

#define THRESHOLD 40
#define IR 600 //500 is IR
#define AOE 300 //200 is ally. 300 is enemy.
#define DAMAGE 5 //1 is MIN. 9 is MAX.
#define IRMAX 1000

int data[] = {IR, IRMAX - IR, AOE, IRMAX - AOE, DAMAGE * 100 + 30, 100, 200, 300, 400, 500, 600, 700, 800, 900};
int damage_count;
int HP = 2000;

void setup() {
  Drone.setup(11, 13, 1);
  Drone.motor_setup(3);
  Drone.led_setup(5);
  Serial.begin(115200);
  pinMode(2,INPUT_PULLUP);
}

//int data[] = {800, 160, 100, 40, 50, 60, 70, 80, 90, 100, 100, 100, 100, 100, 100};
void loop() {
  irget();
 // irsend();
  delay(100);
 //Drone.motor_time(400);
  //Drone.led_color(random(250), random(250), 1);
}
void irget() {
  unsigned  long data2[255] = {0};
  int l;
  damage_count = 0;
  Serial.print(l = Drone.IR_get(data2));
  //Serial.println("個のデータがあります。");
  for (int i = 0; i < l; i++) {
    Serial.print(i);
    Serial.print(":");
    Serial.println(data2[i]);
  }

  int d = int(data2[4] / 100);
  if (data2[0] >= 600 - THRESHOLD && data2[0] <= 600 + THRESHOLD && data[1] >= 400 - THRESHOLD && data[1] <= 400 + THRESHOLD) {
    for (int i = 1; i <= d; i++) {
      if (data2[i + 4] >= i * 100 - THRESHOLD && data2[i + 4] <= i * 100 + THRESHOLD) {
        damage_count++;
      }
    }
    if (damage_count == d) {
      Serial.print("Got shot " );
      Drone.motor_time(400);
      if (data2[2] >= AOE - THRESHOLD && data2[2] <= AOE + THRESHOLD && data[3] >= 1000 - AOE - THRESHOLD && data[3] <= 1000 - AOE + THRESHOLD) {
        Serial.print("by ally. ");
      } else {
        Serial.print("by enemy. ");
      }
      /* ここから下の部分は、本当は上のelseのところに入れる。今はテストなので自分の赤外線を自分で食らうようにしてる。*/
      Serial.print("Damage ");
      Serial.println(d * 100);
      digitalWrite(5, HIGH);
      delay(10);
      digitalWrite(5, LOW);

      current_status(d * 100);
    }
  }
}
void irsend() {
  if(!digitalRead(2) == 1){
  Drone.led_color(random(250), random(250), 1);
  
  Drone.IR_send(data, sizeof(data) / sizeof(int));
  delay(100);
  }
}

void current_status(int damage) {
  HP -= damage;
  if (HP > 0) {
    Serial.print("HP:");
    Serial.println(HP);
    delay(5000);
  } else {
    Serial.println("GAME OVER!!");
    for (int i = 0 ; i < 10; i ++) {
      digitalWrite(5, HIGH);
      delay(100);
      digitalWrite(5, LOW);
      delay(100);
    }
    digitalWrite(5, HIGH);
    delay(5000);

    HP = 2000;
    digitalWrite(5, LOW);
  }
}

