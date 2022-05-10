#define IRBitLength 10
#define bit0 560 // bit 0 space threshold (microseconds)
#define bit1 1120 // bit 1 space threshold (microseconds)

int frequency = 38000, IRLedPin = 7, address = 215;
int fanSpeed = -1e9;
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

int getFanSpeed() {
  int fanAnalog = analogRead(A0);
  return int(float(fanAnalog) / 1023 * 254);
}

void convertIntToBits(int bits[], int value, int i) {
  if(debug) {
    debugLog("Value: ", value);
  }
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

void loop() {
  int newFanSpeed = getFanSpeed();
  if(!(abs(newFanSpeed - fanSpeed) >= 20 && fanSpeed != -1e9))
    return;
  int bits[IRBitLength];
  for(int i = 0; i < IRBitLength; i++)
    bits[i] = 0;
  convertIntToBits(bits, newFanSpeed, 9);
  sendData(bits);
  fanSpeed = newFanSpeed;
  delay(500);
}
