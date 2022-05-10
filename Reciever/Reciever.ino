#define IRBitLength 10
#define bit_0 500 // bit 1 space threshold (microseconds)
#define bit_1 1000 // bit 0 space threshold (microseconds)

int input = 8, addr = 215, fanPin = 9;
bool debug = true;

void setup() {
  Serial.begin(9600);
  pinMode(input, INPUT);
  pinMode(fanPin, OUTPUT);
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

int getData() {
  int bits[IRBitLength], pulses[IRBitLength];
  do{}
  while(pulseIn(input, LOW) < 9);
  
  readPulse(pulses);
  convertPulseToBits(bits, pulses);
  int data = convertBitsToInt(bits, 0, 9);
  return data;
}

void loop() {
//  int data = getData();
//  debugLog("Data: ", data);
  digitalWrite(fanPin, HIGH);
}
