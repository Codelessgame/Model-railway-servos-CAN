
#ifndef SWITCH_SERVO_H
#define SWITCH_SERVO_H

#include <Servo.h>
#include "Storage.h"

enum class ServoPosition : uint8_t {
  PLUS,
  MINUS
};

enum class ServoState : uint8_t {
  PLUS,
  MINUS,
  UNKNOWN
};

class SwitchServo {
  private:
    Servo servo;
    const ServoConfig* config;

    uint8_t currentPosition;
    uint8_t targetPosition;

    unsigned long lastUpdateTime;

  public:
    void begin(const int pin, const ServoConfig& config) {
      servo.attach(pin);
      this->config = &config;
      moveNow(this->config->plusPosition);
      lastUpdateTime = 0;
    }

    void update(unsigned long now) {
      if (lastUpdateTime > now) // Rollover protection
        lastUpdateTime = 0;

      if (currentPosition == targetPosition)
        return;

      if (lastUpdateTime + (unsigned long) config->speed > now)
        return;
        
      
      lastUpdateTime = now;

      if (currentPosition < targetPosition)
        currentPosition++;
      else
        currentPosition--;

      servo.write(currentPosition);
    }

    void moveSlow(ServoPosition position) {
      if (position == ServoPosition::PLUS)
        targetPosition = config->plusPosition;
      else
        targetPosition = config->minusPosition;
    }

    void moveNow(uint8_t target) {
      targetPosition = currentPosition = target;
      servo.write(target);
    }

    ServoState getState() {
      if (currentPosition == config->plusPosition)
        return ServoState::PLUS;
      else if (currentPosition == config->minusPosition)
        return ServoState::MINUS;
      else
        return ServoState::UNKNOWN;
    }
    void moveToStoredPosition(){
      ServoState state = getState();
      switch (state){
       case ServoState::PLUS:
            moveSlow(ServoPosition::PLUS);
            break;
        case ServoState::MINUS:
            moveSlow(ServoPosition::MINUS);
            break;
        default:
            Serial.println("error unknown servo state");
            break;
      }

    }
};





#endif // SWITCH_SERVO_H
