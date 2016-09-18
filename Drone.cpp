#include"Drone.h"

SoftwareSerial ESPURAT(10, 9);//RX,TX

int DroneParts::getStorage(){
	return storageLed;
}

//ID
void DroneParts::setId(){
	idChar = 0;
	for (int i = 0; i < 3; i++) {
		if (digitalRead(id[i]) == LOW) {
			idChar++;
		}
		idChar << 1;
	}
	Serial.print("ID : ");
	Serial.println(idChar);
}
int DroneParts::getId(){	
	return idChar;
}

//Motor
void DroneParts::setMotor(int time){
	digitalWrite(motor, HIGH);
	delay(time);
	digitalWrite(motor, LOW);
}

//SW
int DroneParts::listenSW() {
	int sws = readSW();
	if (getStorage() >= sws) {
		setBar(getStorage() - sws);
		return sws;
	}
	return 0;
}
int DroneParts::readSW() {
	for (int i = 0; i < 3; i++) {
		if (digitalRead(sw[i]) == LOW) {
			return ++i;
		}
	}
	return 0;
}

//leds
void DroneParts::setBar(int _bar, bool _state) {
	for (int i =0; i < _bar; i++) {
		setLed(i, state);
	}
	storageLed = _bar;
}
void DroneParts::setLed(int _led, bool _state) {
	ditalWrite(led[_led], _state);
}

//WiFi
void DroneParts::wifiPrint(String s){
	ESPURAT.println(s);
}

//setup
void DroneParts::begin(int _motor, int _modeP, int _id[3], int _sw[3], int _led[5]) {
	motor = _motor;
	modeP = _modeP;
	id = _id;
	sw = _sw;
	led = _led;
	storageLed = 0;
	ESPURAT.begin(115200);
	pinMode(motor, OUTPUT);
	pinMode(modeP, INPUT_PULLUP);
	for (int i = 0; i < 3; i++) {
		pinMode(id[i], INPUT_PULLUP);
	}
	for (int i = 0; i < 3; i++) {
		pinMode(sw[i], INPUT_PULLUP);
	}
	for (int i = 0; i < 5; i++) {
		pinMode(led[i], OUTPUT);
	}
	mode = digitalRead(modeP);

}
//loop
void DroneParts::loop(int _interval){
	if (mode == LOW) {//赤外線受信モード
		unsigned long beforetime = millis();
		while (millis() - beforetime < _interval) {
			if (millis() < beforetime)break;
			//受信処理
			delay(1);
		}
		setBar(getStorage()++);
	}
}






DroneParts Drone;

