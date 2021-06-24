#ifndef L298N_H
#define L298N_H

#include "Arduino.h"

class L298N_motor {
public:
	L298N_motor() = default;
	L298N_motor(const uint8_t ENA, const uint8_t IN1, const uint8_t IN2);
  uint8_t get_ENA() const;
  uint8_t get_IN1() const;
  uint8_t get_IN2() const;
  
	void reverse();
	void forward() const;
	void back() const;
	void stop() const;
	void set_speed(const uint8_t speed) const;
  
private:
	uint8_t ENA, IN1, IN2;
};
#endif
