# Stepper Motor Control with AccelStepper Library

This project uses the **AccelStepper** library to control three stepper motors, including homing functionality and serial input commands for movement. The motors are controlled using an Arduino and receive commands through the Serial interface, allowing for precise control of each motor's direction and steps.

## Features

- Control up to **three stepper motors** using an Arduino.
- Homing functionality for all three motors, utilizing Hall effect sensors to detect the home position.
- Supports both **Clockwise (CW)** and **Counterclockwise (CCW)** commands for motor movement.
- Flexible serial input system, allowing the user to send commands to control motor movement.
- Example serial commands:
  - `1,CW,1000`: Moves motor 1 clockwise by 1000 steps.
  - `2,CCW,500`: Moves motor 2 counterclockwise by 500 steps.

## Hardware Requirements

- **Arduino** (Mega )
- **Three Stepper Motors**
- **Three Motor Drivers** 
- **Hall effect sensors** for homing detection
  
## Pin Configuration

The following pin numbers are used in the code for motor control and Hall effect sensors:

| Motor  | Direction Pin | Step Pin | Enable Pin | Hall Sensor Pin |
|--------|---------------|----------|------------|-----------------|
| Motor 1| 30            | 32       | 28         | 18              |
| Motor 2| 40            | 36       | 38         | 21              |
| Motor 3| 11            | 12       | 10         | 3               |

Make sure to connect your motors and sensors to these pins on your Arduino board.

## Serial Communication Format

Commands are sent to the Arduino over serial using the format:

Where:
- `motor_number` is 1, 2, or 3 (for each motor).
- `direction` is either `CW` (Clockwise) or `CCW` (Counterclockwise).
- `steps` is the number of steps the motor should move.

### Example Commands
- `1,CW,1000`: Move motor 1 clockwise by 1000 steps.
- `2,CCW,500`: Move motor 2 counterclockwise by 500 steps.
- `3,CW,1500`: Move motor 3 clockwise by 1500 steps.

## Software Setup

1. Download and install the **AccelStepper** library from the Arduino Library Manager.
2. Copy the code from the provided `.ino` file into your Arduino IDE.
3. Upload the code to your Arduino board.
4. Open the Serial Monitor at **9600 baud** and send commands using the format `<motor_number>,<direction>,<steps>`.

## Functions

### Homing
Each motor homes to its initial position using Hall effect sensors. When the sensor is triggered, the motor stops and sets its current position to zero.

### Move to Position
- `moveToPositionM1(int value)`: Move motor 1 to a specific position.
- `moveToPositionM2(int value)`: Move motor 2 to a specific position.
- `moveToPositionM3(int value)`: Move motor 3 to a specific position.

### Motor Movement
- `moveMotorM1(int steps)`: Move motor 1 by a certain number of steps.
- `moveMotorM2(int steps)`: Move motor 2 by a certain number of steps.
- `moveMotorM3(int steps)`: Move motor 3 by a certain number of steps.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.


