#include"Drone_IR.h"
int ir_in, ir_out, motorp, led;
void Drone_IR::IR_send(int *data, int dataSize) {
  for (int cnt = 0; cnt <  dataSize; cnt++) {
    unsigned long len = data[cnt] * 10; // dataは10us単位でON/OFF時間を記録している
    unsigned long us = micros();
    do {
      digitalWrite(ir_out, 1 - (cnt & 1)); // cntが偶数なら赤外線ON、奇数ならOFFのまま
      delayMicroseconds(8);  // キャリア周波数38kHzでON/OFFするよう時間調整
      digitalWrite(ir_out, 0);
      delayMicroseconds(7);
    } while (long(us + len - micros()) > 0); // 送信時間に達するまでループ
  }
}
void Drone_IR::setup(int in, int out,bool in_order,bool out_order) { //inを赤外線入力（受信機）、outを赤外線出力（LED）とする
  //orderに0の時順にプラス
  int i = 0;
  pinMode(out, OUTPUT);
  i++;
  if (out_order != 0)i -= 2;
  pinMode(out + i, OUTPUT);
  digitalWrite(out + i, LOW);
  pinMode(in, INPUT);
  i=0;
  i++;
  if (in_order != 0)i -= 2;
  pinMode(in +i, OUTPUT);
  digitalWrite(in +i, LOW);
  i++;
  if (in_order != 0)i -= 2;
  pinMode(in + i, OUTPUT);
  digitalWrite(in + i, HIGH);
  ir_in = in;
  ir_out = out;
}
void Drone_IR::motor_setup(int pin) {
  pinMode(pin, OUTPUT);
  motorp = pin;
}
void Drone_IR::led_setup(int pin) {
  for (int i = 0; i < 4;i++)
  pinMode(pin+i, OUTPUT);
  digitalWrite(pin + 3, LOW);
  led = pin;
}
void Drone_IR::motor_time(int time) {
  digitalWrite(motorp, HIGH);
  delay(time);
  digitalWrite(motorp, LOW);
}
void Drone_IR::led_color(char green,char blue,bool red) {
  analogWrite(led, green);
  analogWrite(led+1, blue);
  digitalWrite(led+3,red);
}
unsigned long now = micros();
unsigned long lastStateChangedMicros = micros();
int state = HIGH_STATE;
int Drone_IR::IR_get(unsigned long IRbit[255]) {
  unsigned long ir;
  int i = 0;
  while ((ir = IR_get_long()) != 0) {
    if (i++ != 0) {
      IRbit[i - 2] = ir;
    }
  }
  if (i == 0)return false;
  else return i - 1;
}
unsigned long Drone_IR::IR_get_long() {
  unsigned long ir;
  if (state == LOW_STATE) {
    waitLow();
  } else {
    int ret = waitHigh();
    if (ret == 1) {
      Serial.print("\n");
      return 0;
    }
  }
  now = micros();
  ir = (now - lastStateChangedMicros) / 10;
  //Serial.print(ir);
  //Serial.print(",");
  lastStateChangedMicros = now;
  if (state == HIGH_STATE) {
    state = LOW_STATE;
  }else {
    state = HIGH_STATE;
  }
  return ir;
}
void Drone_IR::waitLow() {
  while (digitalRead(ir_in) == LOW);
}

int  Drone_IR::waitHigh() {
  unsigned long start = micros();
  while (digitalRead(ir_in) == HIGH) {
    if (micros() - start > 50000) {
      return 1;
    }
  }
  return 0;
}

Drone_IR Drone;

