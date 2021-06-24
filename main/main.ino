//author:  astronomax

#include "Wire.h"
#include "L298N.h"
#include "Thread.h"
#include "TinyGPS++.h"
#include "Adafruit_GFX.h"
#include "Adafruit_PCD8544.h"
#include "util/delay.h"
#include "bitset.h"

#define PIN_TRIG 10
#define PIN_ECHO 11
#define GPS_BAUD 9600
#define BLUETOOTH_BAUD 9600
#define CLICKS_PER_METER 3500
#define CLICKS_PER_LEFT_ROTATION 1350
#define CLICKS_PER_RIGHT_ROTATION 1540
#define COL_NUMBER 4
#define ROW_NUMBER 4
#define CALIBRATION_DURATION 3000
#define HIGH_FREQUENCY 14.0
#define COIL_DIAMETER 0.3
#define INITIAL_SPEED 200
#define DISPLAY_CONTRAST 50
#define DISPLAY_TEXT_SIZE 2
#define SET(x, y) (x |= (1 << y))

template <typename T>
inline void my_swap(T &a, T &b) {
  T _tmp(a);
  a = b;
  b = _tmp;
}

template <typename T, const unsigned int N>
class chassis {
  public:
    chassis(const T *_left, const T *_right) {
      this->_left = new T[N];
      this->_right = new T[N];
      for (int i = 0; i < N; i++)
        this->_left[i] = _left[i];
      for (int i = 0; i < N; i++)
        this->_right[i] = _right[i];
    }

    chassis(const chassis &another) : chassis(another._left, another._right) {}

    chassis(chassis &&another) {
      my_swap(_left, another._left);
      my_swap(_right, another._right);
    }

    chassis& operator=(chassis another) {
      my_swap(_left, another._left);
      my_swap(_right, another._right);
      return *this;
    }

    ~chassis() {
      delete [] _left;
      delete [] _right;
    }

    T* get_left_motors() const {
      return _left;
    }

    T* get_right_motors() const {
      return _right;
    }

    void forward() const {
      for (int i = 0; i < N; i++)
        _left[i].forward();
      for (int i = 0; i < N; i++)
        _right[i].forward();
    }

    void back() const {
      for (int i = 0; i < N; i++)
        _left[i].back();
      for (int i = 0; i < N; i++)
        _right[i].back();
    }

    void left() const {
      for (int i = 0; i < N; i++)
        _left[i].back();
      for (int i = 0; i < N; i++)
        _right[i].forward();
    }
    
    void right() const {
      for (int i = 0; i < N; i++)
        _left[i].forward();
      for (int i = 0; i < N; i++)
        _right[i].back();
    }

    void stop() const {
      for (int i = 0; i < N; i++)
        _left[i].stop();
      for (int i = 0; i < N; i++)
        _right[i].stop();
    }

    void set_speed(uint8_t speed) const {
      for (int i = 0; i < N; i++)
        _left[i].set_speed(speed);
      for (int i = 0; i < N; i++)
        _right[i].set_speed(speed);
    }

  private:
    T *_left = nullptr, *_right = nullptr;
};

TinyGPSPlus gps;
Adafruit_PCD8544 _display(37, 39, 41, 43, 45);
L298N_motor left[3] = {{9, 31, 29}, {4, 40, 42}, {8, 25, 27}};
L298N_motor right[3] = {{6, 28, 30}, {5, 36, 38}, {7, 26, 24}};
chassis<L298N_motor, 3> _chassis(left, right);
bitset<COL_NUMBER> used[ROW_NUMBER];
volatile long encoder = 0, v0 = 0, t = 0;
volatile float f = 0; //разница между эталонной (v0) и текущей частотой (t)

inline long dist_to_clicks(float distance) {
  return CLICKS_PER_METER * distance;
}

void display_location() {
  while (Serial1.available())
    gps.encode(Serial1.read());

  float _lat = gps.location.lat(), _lng = gps.location.lng();

  char outstr[10];
  dtostrf(_lat, 1, 6, outstr);
  Serial3.println(outstr);
  dtostrf(_lng, 1, 6, outstr);
  Serial3.println(outstr);

  _display.clearDisplay();
  _display.setCursor(0, 0);
  dtostrf(_lat, 1, 4, outstr);
  _display.println(outstr);
  dtostrf(_lng, 1, 4, outstr);
  _display.println(outstr);
  _display.display();
}

bool distance_check() {
  digitalWrite(PIN_TRIG, LOW);
  _delay_us(5);
  digitalWrite(PIN_TRIG, HIGH);
  _delay_us(10);
  digitalWrite(PIN_TRIG, LOW);

  static const float koef = 0.000159; //коэффициент зависимости расстояния от времени
  float distance = koef * pulseIn(PIN_ECHO, HIGH);
  return (distance >= COIL_DIAMETER);
}

bool md_check() {
  if (f > HIGH_FREQUENCY) {
    _chassis.stop();
    _delay_ms(1000);
    display_location();
    _delay_ms(2000);
    return true;
  }
  return false;
}

void turn_left() {
  encoder = 0;
  while (abs(encoder) < CLICKS_PER_LEFT_ROTATION)
    _chassis.left();
  _chassis.stop();
  _delay_ms(600);
}

void turn_right() {
  encoder = 0;
  while (abs(encoder) < CLICKS_PER_RIGHT_ROTATION)
    _chassis.right();
  _chassis.stop();
  _delay_ms(600);
}

void forward() {
  encoder = 0;
  while (abs(encoder) < dist_to_clicks(COIL_DIAMETER)) 
    _chassis.forward();
  _chassis.stop();
  _delay_ms(600);
}

void back() {
  encoder = 0;
  while (abs(encoder) < dist_to_clicks(COIL_DIAMETER)) 
    _chassis.back();
  _chassis.stop();
  _delay_ms(600);
}

inline bool inside(int row, int col) {
  return row >= 0 && row < ROW_NUMBER && col >= 0 && col < COL_NUMBER;
}

void dfs(int row, int col, int rotation) {
  static const int drow[4] = {1, 0, -1, 0}, dcol[4] = {0, 1, 0, -1};
    
  md_check();
  used[row].set_bit(col, true);
  for(int i = 0; i < 4; i++) {
    int ind = (rotation + i) % 4;
    int rto = row + drow[ind], cto = col + dcol[ind];
    if(inside(rto, cto) && !used[rto].get_bit(cto) && distance_check()) {
      forward();
      dfs(rto, cto, ind);
    }
    turn_right();
  }
  back();
}

SIGNAL(TIMER5_COMPA_vect) {
  OCR5A += 1000;
  static unsigned long t0 = 0; //предыдущее время
  t = micros() - t0;
  t0 += t;
  f = 0.9 * f + 0.1 * fabs(t - v0);
}

void setup() {
  TCCR5A = 0;
  TCCR5B = 0x07;
  SET(TIMSK5, OCF5A);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  Serial1.begin(GPS_BAUD);
  Serial3.begin(BLUETOOTH_BAUD);

  _display.begin();
  _display.clearDisplay();
  _display.display();
  _display.setContrast(DISPLAY_CONTRAST);
  _display.setTextSize(DISPLAY_TEXT_SIZE);
  _display.setTextColor(BLACK);

  _chassis.set_speed(INITIAL_SPEED);

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  attachInterrupt(0, []() {
    encoder += ((digitalRead(3) == LOW) ? -1 : 1);
  }, RISING);
  attachInterrupt(1, []() {
    encoder += ((digitalRead(2) == LOW) ? 1 : -1);
  }, RISING);

  _delay_ms(CALIBRATION_DURATION);
  v0 = t; f = 0;

  /*while (!gps.location.isValid())
    while (Serial1.available())
      gps.encode(Serial1.read());*/

  dfs(0, 0, 0);
}

void loop() {
}
