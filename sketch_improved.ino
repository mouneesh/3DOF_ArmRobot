// Suryadevara V S N S Mouneesh
// Student ID: M00951299


#include <AccelStepper.h> // Include the AccelStepper library for controlling stepper motors

// Define pin numbers for Motor 1 (Direction, Step, and Enable pins)
#define dirPin1 30    // Direction pin for motor 1
#define stepPin1 32   // Step pin for motor 1
#define enPin1 28     // Enable pin for motor 1

// Define pin numbers for Motor 2 (Direction, Step, and Enable pins)
#define dirPin2 40    // Direction pin for motor 2
#define stepPin2 36   // Step pin for motor 2
#define enPin2 38     // Enable pin for motor 2

// Define pin numbers for Motor 3 (Direction, Step, and Enable pins)
#define dirPin3 11    // Direction pin for motor 3
#define stepPin3 12   // Step pin for motor 3
#define enPin3 10     // Enable pin for motor 3

// Define Hall sensor pins
const int hallPin1 = 18;   // Hall sensor pin for motor 1
const int hallPin2 = 21;   // Hall sensor pin for motor 2
const int hallPin3 = 3;    // Hall sensor pin for motor 3

// Variables for Motor 1's positioning and movement
int countHPM1 = 0;         // Step count for homing motor 1
int stepsPerLineM1 = 533;  // Steps needed for one full line movement for motor 1
int homeValueM1 = 6;       // Home position value for motor 1
int currentPositionM1 = homeValueM1; // Current position of motor 1

// Variables for Motor 2's positioning and movement
int countHPM2 = 0;         // Step count for homing motor 2
int stepsPerLineM2 = 1167; // Steps needed for one full line movement for motor 2
int homeValueM2 = 3;       // Home position value for motor 2
int currentPositionM2 = homeValueM2; // Current position of motor 2

// Variables for Motor 3's positioning and movement
int countHPM3 = 0;         // Step count for homing motor 3
int stepsPerLineM3 = 437;  // Steps needed for one full line movement for motor 3
int homeValueM3 = 16;      // Home position value for motor 3
int currentPositionM3 = homeValueM3; // Current position of motor 3

// Flags to indicate whether each motor has reached the home position
bool homepositionM1 = false; 
bool homepositionM2 = false;
bool homepositionM3 = false;

// Create stepper motor objects for each motor using the AccelStepper library
AccelStepper stepper1(AccelStepper::DRIVER, stepPin1, dirPin1, enPin1);
AccelStepper stepper2(AccelStepper::DRIVER, stepPin2, dirPin2, enPin2);
AccelStepper stepper3(AccelStepper::DRIVER, stepPin3, dirPin3, enPin3);

// Variables for serial communication
String inputString = "";         // Holds the input string from serial
boolean stringComplete = false;  // Flag to indicate when a string is completely received

bool messagePrinted = false;     // Flag to ensure homing completion message is printed only once

void setup()
{
  // Start serial communication at 9600 baud rate
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Enter commands in the format: <motor_number>,<command>,<value>");
  Serial.println("Commands available: CW (Clockwise), CCW (Counterclockwise)");
  Serial.println("Examples:");
  Serial.println("1,CW,1000");  // Example: Move motor 1 clockwise by 1000 steps
  Serial.println("2,CCW,500");  // Example: Move motor 2 counterclockwise by 500 steps
  Serial.println("-----------------------------------------------------");

  // Configure Hall sensor pins as input with internal pull-up resistors for debouncing
  pinMode(hallPin1, INPUT_PULLUP);
  pinMode(hallPin2, INPUT_PULLUP);
  pinMode(hallPin3, INPUT_PULLUP);

  // Set motor enable pins as output
  pinMode(enPin1, OUTPUT);
  pinMode(enPin2, OUTPUT);
  pinMode(enPin3, OUTPUT);

  // Enable the stepper motor drivers (LOW enables the drivers)
  digitalWrite(enPin1, LOW);
  digitalWrite(enPin2, LOW);
  digitalWrite(enPin3, LOW);

  // Attach interrupts to Hall sensors to detect homing completion (trigger on falling signal)
  attachInterrupt(digitalPinToInterrupt(hallPin1), hallValueChangedH1, FALLING);
  attachInterrupt(digitalPinToInterrupt(hallPin2), hallValueChangedH2, FALLING);
  attachInterrupt(digitalPinToInterrupt(hallPin3), hallValueChangedH3, FALLING);

  // Set motor speed and acceleration for each motor
  stepper1.setMaxSpeed(2000);   
  stepper1.setAcceleration(1000);

  stepper2.setMaxSpeed(2000);   
  stepper2.setAcceleration(1000);

  stepper3.setMaxSpeed(4000);   
  stepper3.setAcceleration(1000);

  // Reserve memory for the input string (to avoid reallocations)
  inputString.reserve(200);
}

void loop()
{
  // Check if all motors have reached their home position
  if (homepositionM1 && homepositionM2 && homepositionM3) 
  {
    // Set all motor positions to 0 once homing is completed
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);

    // Print homing completion message once
    if (!messagePrinted) 
    {
      Serial.println("Homing Completed");
      Serial.println("-----------------------------------------------------");
      messagePrinted = true; // Avoid printing the message repeatedly
    }

    // Check if data is available from serial input
    if (Serial.available() > 0) 
    {
      String data = Serial.readStringUntil('\n'); // Read data until newline character

      // Parse the input string for motor number, command, and value
      int firstCommaIndex = data.indexOf(',');
      int secondCommaIndex = data.indexOf(',', firstCommaIndex + 1);
      String firstValue = data.substring(0, firstCommaIndex);
      String secondValue = data.substring(firstCommaIndex + 1, secondCommaIndex);
      String thirdValue = data.substring(secondCommaIndex + 1);

      // Convert the string values to integers
      int value1 = firstValue.toInt();
      int value2 = secondValue.toInt();
      int value3 = thirdValue.toInt();

      // Print the received values for debugging
      Serial.println("Got the Values");
      Serial.print("Value 1: ");
      Serial.println(value1);
      Serial.print("Value 2: ");
      Serial.println(value2);
      Serial.print("Value 3: ");
      Serial.println(value3);
      Serial.println("-----------------------------------------------------");

      // Move the motors to the specified positions
      moveToPositionM1(value1);
      moveToPositionM2(value2);
      moveToPositionM3(value3);
    }
    delay(100); // Small delay to avoid overloading the loop
  }
  else
  {
    // If motors haven't homed, keep running them until homed
    if(countHPM1 == 0)
    {
      stepper1.run(); // Run motor 1 until it reaches the home position
      homePositionM1(); // Check motor 1's homing status
    }
    if(countHPM2 == 0)
    {
      stepper2.run(); // Run motor 2 until it reaches the home position
      homePositionM2(); // Check motor 2's homing status
    }
    if(countHPM3 == 0)
    {
      stepper3.run(); // Run motor 3 until it reaches the home position
      homePositionM3(); // Check motor 3's homing status
    }

    // Process incoming serial commands for motor movement
    if (Serial.available() > 0) 
    {
      String data = Serial.readStringUntil('\n');  // Read serial data until newline character
      data.trim();  // Remove any leading/trailing spaces

      // Find comma positions to extract motor number, command, and value
      int firstComma = data.indexOf(',');
      int secondComma = data.indexOf(',', firstComma + 1);

      // Check for valid command format
      if (firstComma == -1) 
      {
        Serial.println("Invalid command format.");
        Serial.println("-----------------------------------------------------");
        return;
      }

      String motorStr = data.substring(0, firstComma);
      int motorNumber = motorStr.toInt();

      // Validate motor number (must be 1, 2, or 3)
      if (motorNumber < 1 || motorNumber > 3) 
      {
        Serial.println("Motor number must be 1, 2, or 3.");
        Serial.println("-----------------------------------------------------");
        return;
      }

      String commandStr;
      String valueStr;

      // Check if second comma exists, indicating correct format
      if (secondComma == -1) 
      {
        Serial.println("Invalid command format. Expected: <motor_number>,<command>,<value>");
        Serial.println("-----------------------------------------------------");
        return;
      } 
      else 
      {
        // Extract command and value
        commandStr = data.substring(firstComma + 1, secondComma);
        valueStr = data.substring(secondComma + 1);
      }

      commandStr.trim();
      valueStr.trim();

      // Process valid commands for motor movement (CW or CCW)
      if (commandStr.equalsIgnoreCase("CW") || commandStr.equalsIgnoreCase("CCW")) 
      {
        int steps = valueStr.toInt(); // Convert step count to integer
        if (steps <= 0) 
        {
          Serial.println("Number of steps must be positive.");
          Serial.println("-----------------------------------------------------");
          return;
        }

        // Determine direction based on command (1 for CW, -1 for CCW)
        int directionMultiplier = commandStr.equalsIgnoreCase("CW") ? 1 : -1;

        // Print the movement command details
        Serial.print("Moving Motor ");
        Serial.print(motorNumber);
        Serial.print(" ");
        Serial.print(commandStr);
        Serial.print(" by ");
        Serial.print(steps);
        Serial.println(" steps.");
        Serial.println("-----------------------------------------------------");

        // Execute movement based on motor number
        switch (motorNumber) 
        {
          case 1:
            stepper1.move(steps * directionMultiplier);
            break;
          case 2:
            stepper2.move(steps * directionMultiplier);
            break;
          case 3:
            stepper3.move(steps * directionMultiplier);
            break;
        }
      } 
      else 
      {
        // Handle unknown commands
        Serial.println("Unknown command. Valid commands are CW and CCW.");
        Serial.println("-----------------------------------------------------");
      }
    }
  }
}

// Function to move motor 1 to a given position
void moveToPositionM1(int valueM1) 
{
  if (valueM1)
  {
    int testPositionM1 = valueM1;
    if (0 <= testPositionM1 && testPositionM1 <= 12)
    {
      if (currentPositionM1 < testPositionM1)
      {
        Serial.println("The Motor 1 Joint is Moving in: Clockwise Dir.");
        Serial.print("From the current position: ");
        Serial.println(currentPositionM1);
        int destinationM1 = testPositionM1 - currentPositionM1;
        destinationM1 = destinationM1 * stepsPerLineM1;
        Serial.print("The motor will move: ");
        Serial.print(destinationM1);
        Serial.println(" steps");
        Serial.println("-----------------------------------------------------");
        moveMotorM1(destinationM1);
        currentPositionM1 = testPositionM1;
        delay(2000);
      }
      if (currentPositionM1 > testPositionM1)
      {
        Serial.println("The Motor 1 Joint is Moving in: AntiClockwise Dir.");
        Serial.print("From the current position: ");
        Serial.println(currentPositionM1);
        int destinationM1 = currentPositionM1 - testPositionM1;
        destinationM1 = destinationM1 * stepsPerLineM1;
        Serial.print("The motor will move: ");
        Serial.print(destinationM1);
        Serial.println(" steps");
        Serial.println("-----------------------------------------------------");
        moveMotorM1(-destinationM1);
        currentPositionM1 = testPositionM1;
        delay(2000);
      }
    }
  }
}

// Function to move motor 2 to a given position
void moveToPositionM2(int valueM2) 
{
  if (valueM2)
  {
    int testPositionM2 = valueM2;
    if (0 <= testPositionM2 && testPositionM2 <= 12)
    {
      if (currentPositionM2 < testPositionM2)
      {
        Serial.println("The Motor 2 Joint is Moving in: Clockwise Dir.");
        Serial.print("From the current position: ");
        Serial.println(currentPositionM2);
        int destinationM2 = testPositionM2 - currentPositionM2;
        destinationM2 = destinationM2 * stepsPerLineM2;
        Serial.print("The motor will move: ");
        Serial.print(destinationM2);
        Serial.println(" steps");
        Serial.println("-----------------------------------------------------");
        moveMotorM2(-destinationM2);
        currentPositionM2 = testPositionM2;
        delay(2000);
      }
      if (currentPositionM2 > testPositionM2)
      {
        Serial.println("The Motor 2 Joint is Moving in: AntiClockwise Dir.");
        Serial.print("From the current position: ");
        Serial.println(currentPositionM2);
        int destinationM2 = currentPositionM2 - testPositionM2;
        destinationM2 = destinationM2 * stepsPerLineM2;
        Serial.print("The motor will move: ");
        Serial.print(destinationM2);
        Serial.println(" steps");
        Serial.println("-----------------------------------------------------");
        moveMotorM2(destinationM2);
        currentPositionM2 = testPositionM2;
        delay(2000);
      }
    }
  }
}

// Function to move motor 3 to a given position
void moveToPositionM3(int valueM3) 
{
  if (valueM3)
  {
    int testPositionM3 = valueM3;
    if (0 <= testPositionM3 && testPositionM3 < 34)
    {
      if (currentPositionM3 < testPositionM3)
      {
        Serial.println("The Motor 3 Joint is Moving in: Anti-Clockwise Dir.");
        Serial.print("From the current position: ");
        Serial.println(currentPositionM3);
        int destinationM3 = testPositionM3 - currentPositionM3;
        destinationM3 = destinationM3 * stepsPerLineM3;
        Serial.print("The motor will move: ");
        Serial.print(destinationM3);
        Serial.println(" steps");
        Serial.println("-----------------------------------------------------");
        moveMotorM3(destinationM3);
        currentPositionM3 = testPositionM3;
        delay(2000);
      }
      if (currentPositionM3 > testPositionM3)
      {
        Serial.println("The Motor 3 Joint is Moving in: Clockwise Dir.");
        Serial.print("From the current position: ");
        Serial.println(currentPositionM3);
        int destinationM3 = currentPositionM3 - testPositionM3;
        destinationM3 = destinationM3 * stepsPerLineM3;
        Serial.print("The motor will move: "); 
        Serial.print(destinationM3);
        Serial.println(" steps");
        Serial.println("-----------------------------------------------------");
        moveMotorM3(-destinationM3);
        currentPositionM3 = testPositionM3;
        delay(2000);
      }
    }
  }
}

// Functions to move each motor by a specified number of steps
void moveMotorM1(int steps_to_move)
{
  stepper1.setMaxSpeed(2200);   // Set max speed for motor 1
  stepper1.setAcceleration(1000); // Set acceleration for motor 1
  stepper1.setSpeed(4000);       // Set speed for motor 1
  stepper1.moveTo(steps_to_move); // Move motor 1 to the target position
  stepper1.runToPosition();      // Block until the motor reaches the position
}

void moveMotorM2(int steps_to_move)
{
  stepper2.setMaxSpeed(2200);   // Set max speed for motor 2
  stepper2.setAcceleration(1000); // Set acceleration for motor 2
  stepper2.setSpeed(4000);       // Set speed for motor 2
  stepper2.moveTo(steps_to_move); // Move motor 2 to the target position
  stepper2.runToPosition();      // Block until the motor reaches the position
}

void moveMotorM3(int steps_to_move)
{
  stepper3.setMaxSpeed(4400);   // Set max speed for motor 3
  stepper3.setAcceleration(1000); // Set acceleration for motor 3
  stepper3.setSpeed(6000);       // Set speed for motor 3
  stepper3.moveTo(steps_to_move); // Move motor 3 to the target position
  stepper3.runToPosition();      // Block until the motor reaches the position
}

// Homing functions for each motor to stop movement when the home position is reached
void homePositionM1()
{
  if(homepositionM1 == true && countHPM1 == 0 )
  {    
    stepper1.setSpeed(0); // Stop motor 1 when homed
    countHPM1 = +1;
  }
}

void hallValueChangedH1()
{
  homepositionM1 = true; // Mark motor 1 as homed when Hall sensor is triggered
}

void homePositionM2()
{
  if(homepositionM2 == true && countHPM2 == 0 )
  {    
    stepper2.setSpeed(0); // Stop motor 2 when homed
    countHPM2 = +1;
  }
}

void hallValueChangedH2()
{
  homepositionM2 = true; // Mark motor 2 as homed when Hall sensor is triggered
}

void homePositionM3()
{
  if(homepositionM3 == true && countHPM3 == 0 )
  {    
    stepper3.setSpeed(0); // Stop motor 3 when homed
    countHPM3 = +1;
  }
}

void hallValueChangedH3()
{
  homepositionM3 = true; // Mark motor 3 as homed when Hall sensor is triggered
}
