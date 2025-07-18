#ifndef Command_functions_H
#define Command_functions_H


extern MCP_CAN CAN;
extern byte message[8];


enum class CommandType : uint8_t {
    SET_SERVO = 1,
    SETALL = 2,
    ASSIGN_ID = 3,
    SET_CALIBRATION = 4,

    ID_____ToAllSlaves______ = 0

};


void ResetUnknownPositions(){
  message[0] = static_cast<uint8_t>(CommandType::ID_____ToAllSlaves______);
  CAN.sendMsgBuf(static_cast<uint8_t>(CommandType::SETALL), CAN_STDID, 1, message); 
  memset(message, 0, sizeof(message)); // clears the message variable
}

void assignID(uint8_t newID){
  message[0] = static_cast<uint8_t>(CommandType::ID_____ToAllSlaves______);
  message[1] = newID;
  CAN.sendMsgBuf(static_cast<uint8_t>(CommandType::ASSIGN_ID), CAN_STDID, 2, message); 
  memset(message, 0, sizeof(message));
  
}

void SetServoPosition(uint8_t arduino_id, uint8_t servo_id, uint8_t ServoAngle){
  message[0] = arduino_id;
  message[1] = servo_id;
  message[2] = ServoAngle;

  CAN.sendMsgBuf(static_cast<uint8_t>(CommandType::SET_SERVO), CAN_STDID, 3, message);
  memset(message, 0, sizeof(message));
}

void ServoCalibration(uint8_t arduino_id, uint8_t servo_id, uint8_t min_angle, uint8_t max_angle){
  message[0] = arduino_id;
  message[1] = servo_id;
  message[2] = min_angle;
  message[3] = max_angle;
  CAN.sendMsgBuf(static_cast<uint8_t>(CommandType::SET_CALIBRATION), 0, 4, message);
  memset(message, 0, sizeof(message));
}



#endif Command_functions_H