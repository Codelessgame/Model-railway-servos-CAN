

#include "Storage.h"
#include "SwitchServo.h"
#include "CommandProcessor.h"
#include "CanHandler.h"

#include "util.h"
#include <SPI.h>
#include <mcp_can.h>



constexpr int diodePin = A5;

constexpr int servoPins[] = { A4, A3, A2, A1, A0, 3, 4, 5, 6, 7, 8, 9 };
constexpr size_t servoCount = arrayLength(servoPins);

constexpr int canChipSelectPin = 10;


Storage<servoCount> storage;
SwitchServo servos[servoCount];

MCP_CAN canController(canChipSelectPin);
CanHandler canHandler(canController);

CommandProcessor<servoCount> commandProcessor(canHandler, servos, storage, servoCount);


void setup() {
  Serial.begin(115200);
  if (canController.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
        Serial.println("MCP2515 Initialized Successfully!");
    } else {
        Serial.println("Error Initializing MCP2515...");
        for(;;);
    }

  canController.setMode(MCP_NORMAL);


  storage.begin();
  for (size_t i = 0; i < servoCount; ++i)
    servos[i].begin(servoPins[i], storage.getServoConfig(i));


 Serial.println("=== CAN Slave ===");
}

void loop() {
  commandProcessor.processCommands();

  unsigned long now = millis();
  for (size_t i = 0; i < servoCount; ++i)
    servos[i].update(now);
}
