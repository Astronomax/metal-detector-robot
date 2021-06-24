#include "L298N.h"

template <typename T>
inline void my_swap(T &a, T &b) {
  T _tmp(a); 
  a = b; 
  b = _tmp;
}

L298N_motor::L298N_motor(const uint8_t ENA, const uint8_t IN1, const uint8_t IN2) 
  : ENA(ENA), IN1(IN1), IN2(IN2) {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

uint8_t L298N_motor::get_ENA() const {
  return ENA;
}

uint8_t L298N_motor::get_IN1() const {
  return IN1;
}

uint8_t L298N_motor::get_IN2() const {
  return IN2;
}

void L298N_motor::reverse() {
  my_swap(IN1, IN2);
}

void L298N_motor::forward() const {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void L298N_motor::back() const {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}

void L298N_motor::stop() const {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void L298N_motor::set_speed(const uint8_t speed) const {
  analogWrite(ENA, speed);
}
