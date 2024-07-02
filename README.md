# Embedded-System-Project

This project has been done for the embedded system course at UniGe using dsPIC33EP512MU810 MCU.

The main point of the project are the following.

## Firmware requirements

### Control System
- Control Loop Frequency: 1 kHz
- Motor PWM and IR Sensor Update Frequency: 1 kHz
- Initial State: “Wait for start”
   - PWM DC of all motors: 0
   - LED and indicators blink at 1 Hz

### Execution State
- Command Processing:
  - Commands are processed in order of arrival.
  - Message format: SPCCMD,x,t where x is the action and t is the duration in milliseconds.
  - Actions:
      - Forward motion
      - Clockwise rotation
      - Backward motion
  - Maximum queue size: 10 commands

### Error Handling
- If the queue is full, the robot sends an error message and discards the new command.
- Commands received in the “Wait for start” state are queued for execution once the system transitions to the “Execute” state.

### Motor Control
- PWM Signals:
   - Four PWM signals control the buggy’s motors.
   - PWM frequency: 10 kHz
- Wheel Movements:
   - Left wheels forward: left_pwm > 0
   - Left wheels backward: left_pwm < 0
   - Right wheels forward: right_pwm > 0
   - Right wheels backward: right_pwm < 0
 
### Battery
- Battery voltage is available on pin AN11 after a partitioning circuit.

![image](https://github.com/FrancescoRac/Embedded-System-Project/assets/93876265/2fb90ab5-6c92-4d8a-8f74-4125712fdea9)


### IR Sensor
- Mounting: The infrared sensor should be mounted on the Buggy Mirikub 1 or 2 (i.e., in front of the buggy).
- Signal Reading: The signal can be read on AN14/AN15.
- Enable Signal: The enable to the IR sensor must be given on the digital I/O on RB9/RA3.

### Data Logging/Command Interface through UART
- Installation: The UART to RS232 module should be installed on the Clicker Mikrobus 2.
- Signal Remapping:
   - TX signal: Remapped to RD0/RP64.
   - RX signal: Remapped to RD11/RP175.
- Messages to PC:
   - $MBATT,v_batt* where v_batt is the sensed battery voltage in Volts, at 1 Hz frequency. Use two digits, i.e., X.YZ.
   - $MDIST,distance* where distance is the sensed distance in cm, at 10 Hz frequency. Use an integer.
### UART Baud Rate
- Implementation: The chosen UART baud rate frame structure should never lose a message due to its implementation (i.e., proper dimensioning of buffers), even with full use of bandwidth.




