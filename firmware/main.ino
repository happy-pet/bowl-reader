#define PRINTF_BUFFER_SIZE 128

int enablePin = D0;
int loadCellPin = A0;

int currentWeight = -100;//to ensure we trigger the check and update the value to the real value
int fastReading;
int slowReading;
int fastTolerancePriorToEvent = 8;
int slowTolerancePriorToEvent = 4;

#define LARGE_READINGS_LENGTH 500

int readingBuffer[LARGE_READINGS_LENGTH];

void Serial_printf(const char* fmt, ...) {
  char buff[PRINTF_BUFFER_SIZE];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buff, PRINTF_BUFFER_SIZE, fmt, args);
  va_end(args);
  Serial.println(buff);
}


void sortInts(int buffer[], int bufferLen){
  for(int i = 0; i <= bufferLen; i++){
    for(int j = 0; j <= bufferLen - i; j++){
      if(buffer[j] > buffer[j+1]){
        int temp = buffer[j];
        buffer[j] = buffer[j+1];
        buffer[j+1] = temp;
      }
    }
  }
}

int getReading(int readingsLen){
  if (readingsLen > sizeof(readingBuffer)) {
    Serial.println("readingsLen too large");
    return 1;
  }
  Serial.printf("Getting median reading of %d samples\n", readingsLen);

  digitalWrite(enablePin, HIGH);
  delay(150);// let the op amp circuit stabilize
  // do our reading
  for (int i = 0; i < readingsLen; i++) {
    delay(1);
    readingBuffer[i] = analogRead(loadCellPin);
  }
  digitalWrite(enablePin, LOW);
  sortInts(readingBuffer, readingsLen);
  int median = readingBuffer[(readingsLen / 2) - 1];
  Serial.printf("Median reading is %d\n", median);
  return median;
}

int getReading(bool isFast){
  if(isFast){
    return getReading(LARGE_READINGS_LENGTH / 20);
  } else {
    return getReading(LARGE_READINGS_LENGTH);
  }
}

int getHighlyAccurateReading(){
  int iterations = 0;
  int finalReading;
  bool isValid = false;
  while(!isValid && iterations < 10){
    int slowReading1 = getReading(false);
    delay(1000);//1 second of delay
    int slowReading2 = getReading(false);
    delay(1000);//1 second of delay
    int slowReading3 = getReading(false);
    isValid = true;
    isValid = isValid && !isOverToleranceOf(slowReading1,slowReading2,slowTolerancePriorToEvent);//all slow readings must be +-4 between each other
    isValid = isValid && !isOverToleranceOf(slowReading2,slowReading3,slowTolerancePriorToEvent);
    isValid = isValid && !isOverToleranceOf(slowReading1,slowReading3,slowTolerancePriorToEvent);
    finalReading = (slowReading1 + slowReading2 + slowReading3) / 3;
    iterations++;
  }
  if(isValid){
    return finalReading;
  } else {
    return -1;//unable to get stable reading
  }
}

bool isOverToleranceOf(int value1, int value2, int tolerance){
  return value1-value2<-tolerance || value1-value2>tolerance;
}

bool isFastReadingOverTolerance(int reading){
  return isOverToleranceOf(currentWeight,reading,fastTolerancePriorToEvent);
}

bool isSlowReadingOverTolerance(int reading){
  return isOverToleranceOf(currentWeight,reading,slowTolerancePriorToEvent);
}

void send(String message,int data){
  String eventMessage = message+","+String(data);
  Serial.printf("Publishing event: sendData %s\n", eventMessage.c_str());
  Particle.publish("sendData", eventMessage, 60, PRIVATE);
}


void setup() {
  pinMode(enablePin, OUTPUT);
  analogRead(loadCellPin);
  //pinMode(loadCellPin, AN_INPUT);//this isn't supposed to be used for analogRead, as stated in the docs

  digitalWrite(enablePin, LOW);

  Serial.begin(9600);
  Serial.println("Turning on");
  send("turnOn",0);//not needed, but maybe we want to log it, or send the current tare value back for some reason

  /* Serial.begin(9600);//another weird potential problem, if I remove the code for the serial connection I get an assertion failed error from te photon */
}

void loop() {
  fastReading = getReading(true);
  if(isFastReadingOverTolerance(fastReading)){
    send("checkingFastReadingAt",fastReading);
    slowReading = getHighlyAccurateReading();
    if(slowReading != -1){
      if(isSlowReadingOverTolerance(slowReading)){
        currentWeight = slowReading;
        send("weightUpdate",slowReading);
      }
    } else {
      send("fail",-1);//we tried 10 times to get a stable reading between 3 slow samples, and failed. I'd like to know if this happens
    }
  }
  delay(1000*15);
}
