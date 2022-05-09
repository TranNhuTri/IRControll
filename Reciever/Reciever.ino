#define IRBitLength 32
#define bit_0 500 // bit 1 space threshold (microseconds)
#define bit_1 1000 // bit 0 space threshold (microseconds)

int input = 8, addr = 215;
bool debug = true;

void setup() {
  Serial.begin(9600);
  pinMode(input, INPUT);
}

void debugLog(String title, int value) {
  Serial.print(title);
  Serial.println(value);
}

void readPulse(int pulses[]) {
  for(int i = 0; i < IRBitLength; i ++)
    pulses[i] = pulseIn(input, HIGH);
}

bool convertPulseToBits(int bits[], int pulses[]) {
  if(debug) {
    Serial.print("Pulses: ");
    for(int i = 0; i < IRBitLength; i++) {
      Serial.print(pulses[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  for(int i = 0; i < IRBitLength; i++) {
    if(pulses[i] == 0)
      return false;
    if(pulses[i] >= bit_1) 
      bits[i] = 1;
    else
      if(pulses[i] >= bit_0)
        bits[i] = 0;
  }
  
  if(debug) {
    Serial.print("Bits: ");
    for(int i = 0; i < IRBitLength; i++)
      Serial.print(bits[i]);
    Serial.println();
  }
  return true;
}

int convertBitsToInt(int bits[], int startIndex, int endIndex) {
  int tmp = 1, res = 0;
  for(int i = endIndex; i >= startIndex; i--) {
    res = res + tmp * bits[i];
    tmp = tmp * 2;
  }
  if(bits[startIndex] == 1 && endIndex - startIndex + 1 == 12) {
    res = -1 * (res - tmp / 2);
  }
  return res;
}

void getData(int& address, int& speedLeft, int& speedRight) {
  int pulses[IRBitLength], bits[IRBitLength];
  do {}
  while(pulseIn(input, LOW) < 9);
  
  readPulse(pulses);
  convertPulseToBits(bits, pulses);
  address = convertBitsToInt(bits, 0, 7);
  speedLeft = convertBitsToInt(bits, 8, 19);
  speedRight = convertBitsToInt(bits, 20, 31);
}

void loop() {
//  int value = digitalRead(input);
//  if(value == 0)
//    Serial.println("oke");
  int address, speedLeft, speedRight;
  getData(address, speedLeft, speedRight);
  if(address != addr)
    return;
  if(debug) {
    debugLog("Address: ", address);
    debugLog("Speed left: ", speedLeft);
    debugLog("Speed right: ", speedRight);
  }
}
