#include <Wire.h>
#include <L298N.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define GPS
#define PIN_TRIG 10
#define PIN_ECHO 11
#define SET(x,y) (x |=(1<<y))

Adafruit_PCD8544 display = Adafruit_PCD8544(37, 39, 41, 43, 45);
//--------------------
static const uint32_t GPSBaud = 9600;
static char outstr[15];
TinyGPSPlus gps;
//--------------------
const float Sampl = 0.3; //длина обхода
const float Robot = 0.4; //длина робота 
//--------------------
bool md_perm = 1; //разрешение металлоискателю на измерение новой частоты
//--------------------
motor left[3] = {motor(9, 31, 29, 200), motor(4, 40, 42, 200), motor(8, 25, 27, 200)};
motor right[3] = {motor(6, 28, 30, 200), motor(5, 36, 38, 200), motor(7, 26, 24, 200)};
group rover(left, right, 3);
//--------------------
const float Length = 1.0;
const float Width = 1.0;
float res = 0;
//--------------------
const int clickForMeter = 3500;
const int clickFor90degLeft = 1350; 
const int clickFor90degRight = 1500; 
volatile long counter = 0;
//--------------------
unsigned long t0=0;         //предыдущее время
int t=0;                    //время на 1000 колебаний (определяет частоту)
int v0=0;  //эталонная частота
float f=0;   //разница между эталонной (v0) и текущей частотой (t)
float koef = 0.5;
unsigned long KalPast = 0; //время предыдущей калибровки
unsigned long FindPast = 0; //время предыдущего найденного предмета
//-------------------- 
const int INF = -30000;

void setup() {
 Serial.begin(9600);
 Serial1.begin(GPSBaud); //GPS
 Serial3.begin(9600); //Bluetooth
 //-------------------- 
 digitalWrite(12, HIGH); //кнопка
 TCCR5A=0;
 TCCR5B=0x07;
 SET(TIMSK5,OCF5A); //использовали макрос SET для прерывания по таймеру 5
 //-------------------- 
 pinMode(PIN_TRIG, OUTPUT);
 pinMode(PIN_ECHO, INPUT);
 //-------------------- 
 //датчик на колесе
 pinMode(2, INPUT_PULLUP); 
 pinMode(3, INPUT_PULLUP); 
 attachInterrupt(0, ai0, RISING);
 attachInterrupt(1, ai1, RISING);
 //--------------------
 display.begin();
 display.clearDisplay();
 display.display();
 display.setContrast(50);
 display.setTextSize(2);  // установка размера шрифта
 display.setTextColor(BLACK); // установка цвета текста
 //--------------------
 while(millis() <= 3000){
   v0 = t; 
 }
 #ifdef GPS
 while(true){
  while(Serial1.available())
      gps.encode(Serial1.read()); 
  if(gps.location.isValid())
    break;
 }
 #endif
 Serial1.end();
}

float absf(float f){
  return ((f < 0.0) ? -f : f);
}
//прерывания
SIGNAL(TIMER5_COMPA_vect)
{
  OCR5A+=1000;
  t=micros()-t0;
  t0+=t;
  if(md_perm) //можно пересчитывать частоту
  f=f*0.9+absf(t-v0)*0.1;
}

  void ai0() {
  if(digitalRead(3)==LOW) {
  counter--;
  }else{
  counter++;
  }
  }
   
  void ai1() {
  if(digitalRead(2)==LOW) {
  counter++;
  }else{
  counter--;
  }
  }
//прерывания
  
int lastCalibr = 0;
void md_calibr(){
  int phase = 300000; //5 минут
  if(millis() - lastCalibr >= phase){
    v0 = t;
    lastCalibr = millis();
  }
}

void disp(){
      display.clearDisplay();
      display.setCursor(0, 0); // установка позиции курсора
      dtostrf(gps.location.lat(),1, 6, outstr); 
      Serial3.println(outstr);
      Serial.println(outstr);
      dtostrf(gps.location.lat(),1, 4, outstr); 
      display.println(outstr);
      dtostrf(gps.location.lng(),1, 6, outstr); 
      Serial3.println(outstr);
      Serial.println(outstr);
      dtostrf(gps.location.lng(),1, 4, outstr); 
      display.println(outstr);
      display.display();
}

int lastFind = -INF;
void md_find(){
  Serial1.begin(GPSBaud);
  float limit = 14.0;
  int phase = 3000; //2.5 секунды
 
  if(f > limit && millis() - lastFind >= phase) //предмет обнаружен
  {
    md_perm = 0; //измерять частоту нельзя
    rover.Stop();
    delay(1000);
    md_perm = 1; //измерять частоту можно
    while(Serial1.available())
      gps.encode(Serial1.read()); 
    disp();
    md_calibr();
    lastFind = millis();
  }
  Serial1.end();
}

void Left()
{
counter = 0;

while(abs(counter) < clickFor90degLeft){ 
rover.Left();
md_find();
}

rover.Stop(); delay(400);
}

void Right()
{
counter = 0; 

while(abs(counter) < clickFor90degRight) {
  rover.Right();
  md_find();
}

rover.Stop(); delay(400);
}

bool dist()
{
  int border = 20; //контрольная длина, до которой можно приближаться к объекту (см)
  const float k = 0.0159; //коэффициент зависимости расстояния от времени
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  float duration = pulseIn(PIN_ECHO, HIGH) * k; 
  Serial.println(duration);
  return duration >= border;
}

bool turn = 0;

int pass(){
  const float h = 0.9, w = 0.4;
  
  rover.Stop(); delay(400);
  Right();
  
  counter = 0;
  while(abs(counter) < clickForMeter * w) {
    rover.Forward();
  }
  rover.Stop(); delay(400);
  
  Left();
  
  counter = 0;
   while(abs(counter) < clickForMeter * h) {
    rover.Forward();
  }
  rover.Stop(); delay(400);
  
  Left();
  
  counter = 0;
  while(abs(counter) < clickForMeter * w) {
    rover.Forward();
  }
  rover.Stop(); delay(400);
  
  Right();
  return clickForMeter * h;
}

void loop() {
  if(res < Width){
   counter = 0;
   
   while(abs(counter) < clickForMeter * Length) {
    rover.Forward();
    if(!dist()){
    int lastCtr = counter;
    counter = lastCtr + pass();
    }
    md_find();
   }
   
   rover.Stop(); delay(400);
   (turn == 0) ? Right() : Left();
   
   counter = 0;
   
   while(abs(counter) < clickForMeter * Sampl) {
   rover.Forward(); 
   if(!dist()){
   int lastCtr = counter;
   counter = lastCtr + pass();
   }
   md_find();
   }
   
   rover.Stop(); delay(400);
   (turn == 0) ? Right() : Left();
   turn = (turn == 0);
   res += Sampl;
  }
}
