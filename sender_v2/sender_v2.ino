#include <mcp_can.h>
#include <SPI.h>
#include "CommandFunctions.h"


constexpr int can_cs_pin = 10;
constexpr int button_pin = A4;
constexpr int diode_pin = A5;

MCP_CAN CAN(can_cs_pin);
byte message[8];

void blick(unsigned long time) {
  digitalWrite(diode_pin, HIGH);
  delay(time);
  digitalWrite(diode_pin, LOW); 
  delay(time);
}

void setup() {  
  Serial.begin(115200);
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(diode_pin, OUTPUT);
  
  blick(1000);
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized Successfully!");
    blick(200);
  } else {
    Serial.println("Error Initializing MCP2515...");
    blick(700);
    for(;;);
  }
  CAN.setMode(MCP_NORMAL);

  Serial.println("=== CAN Servo Control CLI ===");
  Serial.println("Available commands:");
  Serial.println("  h, help - Show this help message");
  Serial.println("  r, reset - Reset all slave positions");
  Serial.println("  a, assign <newID> - Assign ID to a slave");
  Serial.println("  ss, servo <arduino_id> <servo_id> <angle> - Set servo position");
  Serial.println("  c, calibrate <arduino_id> <servo_id> <min_angle> <max_angle> - Calibrate servo");
  Serial.println("  s, status - Show system status");
  Serial.println("\nReady for commands...");
}

void loop() {
 if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    processCommand(input);
  }
}

void processCommand(String command) {
  // Convert to lowercase for case-insensitive comparison
  command.toLowerCase();
  
  // Split command into parts
  int spaceIndex = command.indexOf(' ');
  String cmd = (spaceIndex == -1) ? command : command.substring(0, spaceIndex);
  String params = (spaceIndex == -1) ? "" : command.substring(spaceIndex + 1);
  
  if (cmd == "r" || cmd == "reset") {
    Serial.println("Resetting all slave positions...");
    ResetUnknownPositions();
    Serial.println("Reset command sent.");
    
  } else if (cmd == "h" || cmd == "help") {
    Serial.println("=== CAN Servo Control CLI ===");
    Serial.println("Available commands:");
    Serial.println("  h, help - Show this help message");
    Serial.println("  r, reset - Reset all slave positions");
    Serial.println("  a, assign <newID> - Assign ID to a slave");
    Serial.println("  ss, servo <arduino_id> <servo_id> <angle> - Set servo position");
    Serial.println("  c, calibrate <arduino_id> <servo_id> <min_angle> <max_angle> - Calibrate servo");
    Serial.println("  s, status - Show system status");
    
  } else if (cmd == "a" || cmd == "assign") {
    if (params.length() > 0) {
      uint8_t newID = params.toInt();
      if (newID > 0 && newID <= 255) {
        Serial.print("Assigning ID ");
        Serial.print(newID);
        Serial.println(" to slave...");
        assignID(newID); // important line here <----------------------------
        Serial.println("ID assignment command sent.");
      } else {
        Serial.println("Error: Invalid ID. Must be between 1-255.");
      }
    } else {
      Serial.println("Error: Missing ID parameter. Usage: assign <newID>");
    }
    
  } else if (cmd == "ss" || cmd == "servo") {
    // Parse servo parameters
    int firstSpace = params.indexOf(' ');
    int secondSpace = params.indexOf(' ', firstSpace + 1);
    
    if (firstSpace != -1 && secondSpace != -1) {
      uint8_t arduino_id = params.substring(0, firstSpace).toInt();
      uint8_t servo_id = params.substring(firstSpace + 1, secondSpace).toInt();
      uint8_t angle = params.substring(secondSpace + 1).toInt();
      
      if (arduino_id > 0 && servo_id >= 0 && angle <= 180) {
        Serial.print("Setting servo ");
        Serial.print(servo_id);
        Serial.print(" on Arduino ");
        Serial.print(arduino_id);
        Serial.print(" to ");
        Serial.print(angle);
        Serial.println(" degrees...");
        SetServoPosition(arduino_id, servo_id, angle); // important line here <----------------------------
        Serial.println("Servo command sent.");
      } else {
        Serial.println("Error: Invalid parameters. Arduino ID must be >0, angle must be 0-180.");
      }
    } else {
      Serial.println("Error: Missing parameters. Usage: servo <arduino_id> <servo_id> <angle>");
    }
    
  } else if (cmd == "c" || cmd == "calibrate") {
    // Parse calibration parameters
    int spaces[3];
    int spaceCount = 0;
    int searchStart = 0;
    
    // Find all spaces
    while (spaceCount < 3 && searchStart < params.length()) {
      int spacePos = params.indexOf(' ', searchStart);
      if (spacePos == -1) break;
      spaces[spaceCount++] = spacePos;
      searchStart = spacePos + 1;
    }
    
    if (spaceCount == 3) {
      uint8_t arduino_id = params.substring(0, spaces[0]).toInt();
      uint8_t servo_id = params.substring(spaces[0] + 1, spaces[1]).toInt();
      uint8_t min_angle = params.substring(spaces[1] + 1, spaces[2]).toInt();
      uint8_t max_angle = params.substring(spaces[2] + 1).toInt();
      
      if (arduino_id > 0 && min_angle <= 180 && max_angle <= 180 && min_angle < max_angle) {
        Serial.print("Calibrating servo ");
        Serial.print(servo_id);
        Serial.print(" on Arduino ");
        Serial.print(arduino_id);
        Serial.print(" with range ");
        Serial.print(min_angle);
        Serial.print("-");
        Serial.print(max_angle);
        Serial.println(" degrees...");
        ServoCalibration(arduino_id, servo_id, min_angle, max_angle);
        Serial.println("Calibration command sent.");
      } else {
        Serial.println("Error: Invalid parameters. Check Arduino ID, angles (0-180), and min < max.");
      }
    } else {
      Serial.println("Error: Missing parameters. Usage: calibrate <arduino_id> <servo_id> <min_angle> <max_angle>");
    }
    
  } else if (cmd == "s" || cmd == "status") {
    Serial.println("=== System Status ===");
    Serial.println("CAN Bus: Active");
    Serial.println("Baud Rate: 500 kbps");
    Serial.println("Mode: Normal");
    Serial.println("Ready to send commands.");
    
  } else {
    Serial.print("Unknown command: ");
    Serial.println(cmd);
    Serial.println("Type 'help' for available commands.");
  }
}


