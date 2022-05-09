#define IRBitLength 32
#define bit0 560 // bit 0 space threshold (microseconds)
#define bit1 1120 // bit 1 space threshold (microseconds)

int frequency = 38000, IRLedPin = 7, address = 215;
int input1 = A0, input2 = A1, prevValue1 = -1e9, prevValue2 = -1e9;
bool debug = true;

void setup() {
  Serial.begin(9600);
  pinMode(IRLedPin, OUTPUT);
}

void debugLog(String title, int value) {
  Serial.print(title);
  Serial.println(value);
}

void IRLedOn() {
  tone(IRLedPin, frequency);
}

void IRLedOff() {
  noTone(IRLedPin);
}

void space(int microseconds) {
  IRLedOff();
  delayMicroseconds(microseconds);
}

void sendData(int bits[]) {
  IRLedOn();
  delay(9);
  space(4.5 * 1e3);
  
  if(debug) {
    Serial.print("Bits: ");
    for(int i = 0; i < IRBitLength; i++) {
      Serial.print(bits[i]);
    }
    Serial.println();
  }
  for(int i = 0; i < IRBitLength; i++) {
    IRLedOn();
    delayMicroseconds(560);
    if(bits[i] == 1) {
      space(bit1);
    }
    else {
      space(bit0);
    }
  }
  IRLedOn();
  delayMicroseconds(560);
  IRLedOff();
}

bool isMiddleValue(int value) { // 0 -> 1023 => middle value in range (511 512) + 20 => (490 533)
  if(value >= 490 && value <= 533)
    return true;
  return false;
}

bool checkIfDataChange() {
  int potent1Value = analogRead(input1), potent2Value = analogRead(input2);
  if((potent1Value >= prevValue1 - 20 && potent1Value <= prevValue1 + 20 && prevValue1 != -1e9) 
      && (potent2Value >= prevValue2 - 20 && potent2Value <= prevValue2 + 20 && prevValue2 != -1e9)) {
     return false;
  }
  return true;
}

void getData(int& speedLeft, int& speedRight) {
  int potent1Value = analogRead(input1), potent2Value = analogRead(input2);
  
  if(debug) {
    debugLog("Potentiometer 1 value: ", potent1Value);
    debugLog("Potentiometer 2 value: ", potent2Value);
  }
  if(isMiddleValue(potent1Value))
    speedLeft = speedRight = 0;
  else {
    int initSpeed = int(float(potent1Value - 512)/512 * 255);
    if(debug)
      debugLog("Init speed: ", initSpeed);

    speedLeft = speedRight = initSpeed;
    if(!isMiddleValue(potent2Value)) {
      int value = int(float(potent2Value - 512)/512 * initSpeed);

      if(debug)
        debugLog("Value: ", value);
      
      if(value < 0)
        speedLeft = speedLeft + (speedLeft > 0 ? value : -1 * value);
      else
        speedRight = speedRight + (speedRight > 0 ? -1 * value : value);
    }
  }

  if(debug) {
    debugLog("Motor left: ", speedLeft);
    debugLog("Motor right: ", speedRight);
  }

  prevValue1 = potent1Value;
  prevValue2 = potent2Value;
}

void convertIntToBits(int bits[], int value, int i) {
  if(value < 0) {
    bits[i - 11] = 1;
    value = -value;
  }
  
  while(value > 0) {
    bits[i] = value % 2;
    value = value / 2;
    i -= 1;
  }
}

void convertDataToBits(int bits[], int address, int speedLeft, int speedRight) {
  int i = IRBitLength - 1;
  convertIntToBits(bits, speedRight, i);
  i = 19;
  convertIntToBits(bits, speedLeft, i);
  i = 7;
  convertIntToBits(bits, address, i);
}

void loop() {
  if(!checkIfDataChange())
    return;
  int speedLeft = 0, speedRight = 0;
  getData(speedLeft, speedRight);
  int bits[IRBitLength];
  for(int i = 0; i < IRBitLength; i++)
    bits[i] = 0;
  convertDataToBits(bits, address, speedLeft, speedRight);
  sendData(bits);
  delay(500);
  
//  int data = analogRead(A0);
//  Serial.print("Data: ");
//  Serial.println(data);
//  sendData(data);
//  delay(1000);
}
