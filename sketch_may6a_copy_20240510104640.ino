#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

const int distanceSensorTrigPin = 52;
const int distanceSensorEchoPin = 53;
const int highDistWarningLEDPin = 44;

const int thermistorPin = A0;  

const float seriesResistor = 10000; 

int colorSensorFrequency = 0;
const unsigned long communicationBaudRate = 9600;

void setup() {
  setupDistanceSensor();
  setupColorSensor();
  Serial.begin(communicationBaudRate);
}

void loop() {
  readDistanceSensor();
  readRGBColorSensors();
  readTemperatureSensor();
  delay(5000); 
}

void setupDistanceSensor() {
  pinMode(distanceSensorTrigPin, OUTPUT); 
  pinMode(distanceSensorEchoPin, INPUT);  
  pinMode(highDistWarningLEDPin, OUTPUT); 
}

void setupColorSensor() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  // Set frequency scale to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}

void readDistanceSensor() {
  const long warningDistance = 18;
  long duration = measurePulseDuration();
  long distance = calculateDistance(duration);

  printToSerial("Distance", distance);

  handleDistanceWarningLED(distance, warningDistance);
}

long measurePulseDuration() {
  digitalWrite(distanceSensorTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(distanceSensorTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(distanceSensorTrigPin, LOW);

  return pulseIn(distanceSensorEchoPin, HIGH);
}

long calculateDistance(long duration) {
  const int speedOfSoundInCmPerMs = 0.034 / 2;
  return duration * speedOfSoundInCmPerMs; 
}

void handleDistanceWarningLED(long distance, long threshold) {
  digitalWrite(highDistWarningLEDPin, distance < threshold ? HIGH : LOW);
}

void readRGBColorSensors() {
  readRedColorSensor();
  readGreenColorSensor();
  readBlueColorSensor();
}

void readRedColorSensor() {
  // Read red
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  colorSensorFrequency = pulseIn(sensorOut, LOW);
  colorSensorFrequency = map(colorSensorFrequency, 40, 500, 255, 0);
  printToSerial("R", colorSensorFrequency);
  delay(100);
}

void readGreenColorSensor() {
  // Read green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  colorSensorFrequency = pulseIn(sensorOut, LOW);
  colorSensorFrequency = map(colorSensorFrequency, 40, 610, 255, 0);
  printToSerial("G", colorSensorFrequency);
  delay(100);
}

void readBlueColorSensor() {
  // Read blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  colorSensorFrequency = pulseIn(sensorOut, LOW);
  colorSensorFrequency = map(colorSensorFrequency, 15, 180, 255, 0);
  printToSerial("B", colorSensorFrequency);
  delay(100);
}

void readTemperatureSensor() {
  int adcValue = analogRead(thermistorPin);
  float resistance = (1023.0 / adcValue - 1) * seriesResistor;

  float temperatureC = 1.0 / (log(resistance / 10000) / 3950 + 1 / (25 + 273.15)) - 273.15;  // Convert to Celsius
  temperatureC = temperatureC + 12; // Calibration
  float temperatureF = temperatureC * 9.0 / 5.0 + 32;  // Convert Celsius to Fahrenheit

  printToSerial("Temperature", temperatureF);
}

void printToSerial(const char* label, float value) {
  Serial.print(label);
  Serial.print(": ");
  Serial.println(value);
}
