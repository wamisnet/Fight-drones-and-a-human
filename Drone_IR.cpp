#include"Drone_IR.h"

//1秒は1000000マイクロ秒
//      157200
//データは3回繰り返す

//それ以上の送信は送信機や受信機のクラッシュを引き起こすため
//今回はここまでとする。

//同じデータを送信する
//1000(500-1500)：0　2000(1500-2500)：1 データの間は500
//start bit : 1000,300,2000,300,1000,300,2000,300　1000と2000を繰り返す間は300 programとして300が4回続くところを検知し、1000，3000の繰り返しだったら次へ進む　データ数8//送信するデータを2000→3000に変更
//data bit (id):　7bitはデータ　最後の一ビットはパリティ　　　データ数8
//data bit (damage):　7bitはデータ　最後の一ビットはパリティ　データ数8
//stop bit : 3000,500,3000,500 二回同じデータを送信する 　　　データ数4　
//総データ数　28
//次のデータまで5000開ける

void Drone_IR::begin(int _ID, int _hp, int _irIn, int _irOut) { //inを赤外線入力（受信機）、outを赤外線出力（LED）とする
	pinMode(_irOut, OUTPUT);
  digitalWrite(_irOut,LOW);
	pinMode(_irIn, INPUT);
	ir_in = _irIn;
	ir_out = _irOut;
	hp = _hp;
	id = _ID;
}
//HP管理
void Drone_IR::addHP(int _hp) {
	hp += _hp;
}
void Drone_IR::hitHP(int damage) {
	hp -= damage;
}
int Drone_IR::getHP() {
	return hp;
}

//ダメージ管理
int Drone_IR::getDamageID(){
	return damageId;
}
int Drone_IR::getDamageHP(){
	return idamage;
}

//送信部
void Drone_IR::IRSend(int _damage) {
  int data[100], irSend[2];
  irSend[0] = id;
  irSend[1] = _damage;
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
        data[array + 6 - j] = 1000 + 1000 * (bits & 0x01)*2; //一ビット毎にデータをコピーする
        bits >>= 1;
        if (data[array + 6 - j] == 3000) paritybit++;
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
  
  IR_signal(data, array);
  delay(100);
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
//送信部ここまで

//受信部
unsigned long now = micros();
unsigned long lastStateChangedMicros = micros();
bool Drone_IR::IRGet() {
  int array[500] = {0};
  int l;
  Serial.println(l = DroneIR.IRPacket(array));
  delay(10);
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
    //Serial.print("startSP:");Serial.println(sp);
	if (sp != -1) {
		sp += 7;
		// Serial.print("SP:");Serial.println(sp);
		Serial.println(damageId = databit(array, sp));
		if (damageId == -1) {
			sp -= 6;
			continue;
		}
		sp += 8;
		// Serial.print("damageSP:");Serial.println(sp);
		Serial.println(idamage = databit(array, sp));
		if (idamage == -1) {
			sp -= 14;
			continue;
		}
		sp += 8;
		// Serial.print("TopSP:");Serial.println(sp-DATALEN+4);
		if (stopbit(array, sp)) {
			hitHP(idamage);
			return true;
		}
    } else {
      Serial.println("err");
      return false;
    }
    sp++;
  } while (l - DATALEN >= sp);
  return false;
}
int Drone_IR::IRPacket(int IRbit[500]) {
  unsigned long ir;
  int i = 0;
  while ((ir = IRRaw()) != 0) {
    if (i++ != 0) {
      IRbit[i - 2] = (int) ir;
    }
  }
  if (i == 0)return false;
  else return i - 1;
}
unsigned long Drone_IR::IRRaw() {
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
      if (!range(300, 300, array[i + j * 2]))break; //データが正しくないとき
    }
    if (j >= 4)return i;
  }
  return -1;
}
//databit
int Drone_IR::databit(int *dataArray, int sp) {
	int parity = 0, ID = 0;
	for (int i = 6, j = 1; i >= 0; i--, j *= 2) {
		int now = binary(dataArray[sp + i]);
		if (now == -1)return -1;
		if (now == 1)parity++;
		ID += j * now;
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
//受信用etc
bool Drone_IR::range(int _range, int _source, int _data) {//range データの±いくつ許容するか　souce 比較元（正しいデータ）　data 比較されるデータ（受信データ）
	if (_data >= (_source - _range / 2) && _data <= (_source + _range / 2))return true;
	else return false;
}
int Drone_IR::binary(int decimal) {
	if (range(1500, 1000, decimal))return 0;
	if (range(1500, 3000, decimal))return 1;
	else return -1;
}
//受信部ここまで

Drone_IR DroneIR;

