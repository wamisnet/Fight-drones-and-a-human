#include"Drone_IR.h"
int ir_in, ir_out, motorp, led, _HP;

//1秒は1000000マイクロ秒
//      157200
//データは3回繰り返す

//同じデータを送信する
//1000(500-1500)：0　2000(1500-2500)：1 データの間は500
//start bit : 1000,300,2000,300,1000,300,2000,300　1000と2000を繰り返す間は300 programとして300が4回続くところを検知し、1000，3000の繰り返しだったら次へ進む　データ数8//送信するデータを2000→3000に変更
//data bit (id):　7bitはデータ　最後の一ビットはパリティ　　　データ数8
//data bit (damage):　7bitはデータ　最後の一ビットはパリティ　データ数8
//stop bit : 3000,500,3000,500 二回同じデータを送信する 　　　データ数4　
//総データ数　28
//次のデータまで5000開ける

void Drone_IR::IRSend(int irSend[2]) {
  int data[100];
  int array = 0;
  for (int y = 0; y < 3; y++) {
    //startbit
    for (int i = 0; i < 4; i++) {
      data[array + i * 2] = 1000 + 1000 * (i % 2);
      data[array + i * 2 + 1] = 300;
    }
    array += 8;
    //databit
    for (int i = 0; i < 2 ; i++) {
      int paritybit;
      paritybit = 0;
      byte bits = (byte)irSend[i];
      for (int j = 0; j < 7; j++) {
        data[array + 6 - j] = 1000 + 1000 * (bits & 0x01); //一ビット毎にデータをコピーする
        bits >>= 1;
        if (data[array + 6 - j] == 1) paritybit++;
      }
      array += 8;
      data[array - 1] = 1000 + 1000 * (paritybit % 2)*2;
    }
    //Stopbit
    for (int i = 0; i < 2; i++) {
      data[array + i * 2] = 3000;
      data[array + i * 2 + 1] = 500;
    }
    array += 4;
    data[array++] = 4000; //次の通信との間隔を取る
  }
  for (int i = 0; i < array; i++) {
    Serial.print(data[i]); Serial.print(":");
  }
  Drone.IR_signal(data, array);
  Nefry.ndelay(100);
  Serial.println();
}

void Drone_IR::IR_signal(int *data, int dataSize) {
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
void Drone_IR::setup(int in, int out, bool in_order, bool out_order) { //inを赤外線入力（受信機）、outを赤外線出力（LED）とする
  //orderに0の時順にプラス
  //int i = 0;
  pinMode(out, OUTPUT);
  /*
    i++;
    if (out_order != 0)i -= 2;
    pinMode(out + i, OUTPUT);
    digitalWrite(out + i, LOW);*/
  pinMode(in, INPUT);
  /*  i=0;
    i++;
    if (in_order != 0)i -= 2;
    pinMode(in +i, OUTPUT);
    digitalWrite(in +i, LOW);
    i++;
    if (in_order != 0)i -= 2;
    pinMode(in + i, OUTPUT);
    digitalWrite(in + i, HIGH);*/
  ir_in = in;
  ir_out = out;
}
void Drone_IR::motor_setup(int pin) {
  pinMode(pin, OUTPUT);
  motorp = pin;
}
void Drone_IR::led_setup(int pin) {
  for (int i = 0; i < 4; i++)
    pinMode(pin + i, OUTPUT);
  digitalWrite(pin + 3, LOW);
  led = pin;
}
void Drone_IR::motorTime(int time) {
  digitalWrite(motorp, HIGH);
  delay(time);
  digitalWrite(motorp, LOW);
}
void Drone_IR::led_color(char green, char blue, bool red) {
  analogWrite(led, green);
  analogWrite(led + 1, blue);
  digitalWrite(led + 3, red);
}
unsigned long now = micros();
unsigned long lastStateChangedMicros = micros();
int state = HIGH_STATE, iID, idamage;
int Drone_IR::getID() {
  return iID;
}
int Drone_IR::getDamage() {
  return idamage;
}

void Drone_IR::setHP(int hp) {
  _HP = hp;
}
void Drone_IR::hitHP(int damage) {
  _HP -= damage;
}
int Drone_IR::getHP() {
  return _HP;
}
void Drone_IR::webPrint() {
  Nefry.setConfHtml("ID", 10);
  Nefry.setConfHtml("Damage", 11);
  Nefry.setConfHtml("Mode(1:send", 12);
  Nefry.setConfHtml("HP", 13);
  Nefry.setConfHtml("AutoSend==1",14);
  Nefry.setConfHtml("SendDelay",15);
  for (int i = 10; i < 14; i++)
    Nefry.setConfHtmlPrint(1, i);
}
bool Drone_IR::IRGet() {
  int array[500] = {0};
  int l;
  Serial.println(l = Drone.IR_get(array));
  Nefry.ndelay(10);
  if (l == 0)return false;
  for (int i = 0; i < l; i++) {
    Serial.print(i);
    Serial.print(":");
    Serial.println(array[i]);
  }

  int sp;
  sp=0;
  do {
    sp = startbit(1000, array, sp);
    if (sp != -1) {
      sp += 7;
      Serial.println(iID = databit(array, sp));
      sp += 8;
      Serial.println(idamage = databit(array, sp));
      sp += 8;
      Serial.print("SP:");Serial.println(sp-DATALEN+4);
      if (stopbit(array, sp))return true;
    } else {
      Serial.println("err");
      return false;
    }
    sp++;
  } while (l - DATALEN >= sp);
  return false;
}
int Drone_IR::IR_get(int IRbit[500]) {
  unsigned long ir;
  int i = 0;
  while ((ir = IR_get_long()) != 0) {
    if (i++ != 0) {
      IRbit[i - 2] = (int) ir;
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
  } else {
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

//databit

int parity = 0;
int Drone_IR::databit(int *dataArray, int sp) {

  int ID = 0;
  for (int i = 6, j = 1; i >= 0; i--, j *= 2) {
    ID += j * binary(dataArray[sp + i]);

  }
  if (parity % 2 == binary(dataArray[sp + 7])) {
    Serial.println("DataBitOk");
    return ID;
  }
  else {
    Serial.println("DataBitErr");
    return -1;
  }

}
bool Drone_IR::range(int _range, int _source, int _data) {//range データの±いくつ許容するか　souce 比較元（正しいデータ）　data 比較されるデータ（受信データ）
  if (_data >= ( _source - _range / 2) && _data <= (_source + _range / 2))return true;
  else return false;
}

int Drone_IR::binary(int decimal) {
  if (range(1500, 1000, decimal))return 0;
  if (range(1500, 3000, decimal)) {
    parity++;
    return 1;
  } else return -1;
}
//startbit
int Drone_IR::startbit(int arraylen, int* array, int sp) {
  int rgArray[] = {1000, 2000}, i;
  if (arraylen - DATALEN  < sp)return -1;
  for (int c = sp; c < arraylen - DATALEN; c++) {
    sp = startpoint(arraylen, array, c);
    if (sp == -1)return -1;
    for (i = 0; i < 4; i++) {
      if (!range(1500, rgArray[i % 2], array[sp - 1 + i * 2]))break;
    }
    if (i >= 4) return sp;
  }
}
int Drone_IR::startpoint(int arraylen, int* array, int sp) { //arraylen 配列長さ　array 配列のポインタ
  for (int i = sp; i < arraylen - DATALEN; i++) {
    int j;
    for (j = 0; j < 4; j++) {
      if (!range(550, 300, array[i + j * 2]))break; //データが正しくないとき
    }
    if (j >= 4)return i;
  }
  return -1;
}
//stopbit
bool Drone_IR::stopbit(int *array, int sp) {
  const int rgArray[] = {3000, 500, 3000, 500};
  int i;
  for (i = 0; i < 4; i++) {
    if (!range(2000, rgArray[i], array[sp + i]))break;
  }
  if (i >= 4) {
    Serial.println("StopBitOk");
    return true;
  }else {
    Serial.println("StopBitErr");
    return false;
  }
}

Drone_IR Drone;

