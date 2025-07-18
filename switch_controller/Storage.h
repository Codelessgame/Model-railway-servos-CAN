
#ifndef STORAGE_H
#define STORAGE_H

#include <EEPROM.h>


struct ArduinoConfig {
    uint8_t canId;
};

struct ServoConfig {
    uint8_t plusPosition;
    uint8_t minusPosition;
    uint8_t speed;
};


constexpr int ServoConfigBaseAddress = 32;
template<size_t servoCount>
struct Cache {
  public:
    ArduinoConfig arduinoConfig;
  
  private:
    byte reserved_[ServoConfigBaseAddress - sizeof(ArduinoConfig)];

  public:
    ServoConfig servoConfigs[servoCount];
};


template<size_t servoCount>
class Storage {
  private:
    Cache<servoCount> cache;

  public:
    void begin() {
      EEPROM.get(0, cache);
    }

    void save() {
      EEPROM.put(0, cache); 
    }


    const ArduinoConfig& getArduinoConfig() {
      Serial.println("Arduinoconfig accesed");
      return cache.arduinoConfig;
    }

    void setArduinoConfig(const ArduinoConfig& arduinoConfig) {
      cache.arduinoConfig = arduinoConfig;
    }

    const ServoConfig& getServoConfig(size_t index) {
      Serial.println("Servoconfig accesed");
      return cache.servoConfigs[index];
    }

    void setServoConfig(size_t index, const ServoConfig& servoConfig) {
      cache.servoConfigs[index] = servoConfig;
    }

};

#endif // STORAGE_H
