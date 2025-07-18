

#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H


#include "CanHandler.h"
#include "SwitchServo.h"  
#include <Servo.h> // Or your custom servo class

// Add public access specifier
enum class CommandType : uint8_t {
    SET_SERVO = 1,
    SETALL = 2,
    ASSIGN_ID = 3,
    SET_CALIBRATION = 4,
    ID_____ToAllSlaves______ = 0

};

template<size_t servoCount>
class CommandProcessor{

    public:
        CommandProcessor(CanHandler& canHandler , SwitchServo* servos, Storage<servoCount>& storage, int numberofservos )
            : m_canHandler(canHandler), m_servos(servos), m_storage(storage), m_servoCount(numberofservos)  {}
            void processCommands() {
                long id;
                byte len;
                byte buf[8];

                if (m_canHandler.receive(&id, &len, buf)) {
                    // Handle all command types
                     uint8_t myId = m_storage.getArduinoConfig().canId;
                    if (buf[0] == 0 || buf[0] == myId){

                        Serial.print("case: ");
                        switch(CommandType(id)) {
                            
                            case CommandType::SET_SERVO: {

                                if(len >= 2) {
                                    uint8_t servoIndex = buf[0];
                                    uint8_t position = buf[1];
                                    if(servoIndex < m_servoCount) {
                                        m_servos[servoIndex].moveNow(position);
                                    }
                                }
                                Serial.println("SET_SERVO");
                                break;
                            }
                            case CommandType::SETALL: {
                                for(size_t i = 0; i < m_servoCount; ++i) {
                                    m_servos[i].moveToStoredPosition();
                                }
                                Serial.println("SETALL");
                                break;
                            }

                            case CommandType::ASSIGN_ID: {
                                Serial.println("ASSIGN_ID");
                                Serial.print("ID: ");

                                ArduinoConfig newConfig;
                                Serial.println(buf[1]);
                                newConfig.canId = buf[1];
                                m_storage.setArduinoConfig(newConfig);
                                break;
                            }

                            case CommandType::SET_CALIBRATION: {
                                if(len >= 4) {
                                    uint8_t servoIndex = buf[1];
                                    uint8_t minAngle = buf[2];
                                    uint8_t maxAngle = buf[3];
                                    
                                    if(servoIndex < m_servoCount) {
                                        ServoConfig newConfig;
                                        newConfig.minusPosition = minAngle;
                                        newConfig.plusPosition = maxAngle;
                                        newConfig.speed = m_storage.getServoConfig(servoIndex).speed; // Keep existing speed
                                        
                                        m_storage.setServoConfig(servoIndex, newConfig);
                                        m_storage.save(); // Save to EEPROM
                                    }
                                }
                                Serial.println("SET_CAlIBRATION");
                                break;
                            }
                            // Add other command cases
                        }
                    } else{
                        Serial.println("Not matching Id");
                    }
                }
            }
    private:
        CanHandler& m_canHandler;
        SwitchServo* m_servos;
        Storage<servoCount>& m_storage;
        int m_servoCount;
};



#endif COMMAND_PROCESSOR_H
