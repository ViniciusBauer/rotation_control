# Fan Control

This repository gathers the materials developed for the Integrated Project of the Control Theory 2 and Microcontrollers course (2023.2), carried by students Jonas and Vinícius Bauer.
The project involves implementing a discrete control for a fan using a microcontroller, applying both PI and PID controllers.

## Project Description

To obtain the transfer function of the plant (fan), an experimental procedure was carried out by applying a step input with a sampling time of 100 ms. The measurement of the fan speed allowed for the identification of the system dynamics and the determination of a first-order transfer function, which was then discretized using the ZOH method.

Two controllers were designed:
- **PI Controller (Proportional-Integral):**  
  Characterized by an adjustable zero and a fixed pole at Z = 1, it exhibited a *Settling Time* of 2.73 seconds and an *Overshoot* of 0.942%.
- **PID Controller (Proportional-Integral-Derivative):**  
  With two adjustable zeros and two fixed poles (at Z = 1 and Z = 0), it achieved a *Settling Time* of 5.39 seconds and an *Overshoot* of 0.478%.

Difference equations were derived to integrate the controllers into the microcontroller, ensuring the implementation of the discrete fan control.

## Validation and Simulation

The controllers were validated through simulations in Simulink. A set of blocks was used to model the plant and replicate the controllers' behavior. The simulations allowed a comparison between the model obtained with Sisotool and the code-based implementation (using code blocks in Simulink), highlighting the compatibility between the control effort and the established reference.

## Repository Structure

- **/src/rotation_control.c**  
  Contains the C source code that implements the fan control. Key features include:
  - Configuration of timers, the AD converter, PWM, LCD, and keypad.
  - Implementation of a Human-Machine Interface (HMI) based on a state machine, which allows for setting the setpoint and selecting between the PI and PID controllers.
  - A control routine that updates the speed parameters and duty cycle by integrating the discrete control equations.

- **/docs/rotation_control.pdf**  
  A detailed document outlining the methodology used, including:
  - The experimental procedure for plant identification.
  - Derivation and comparison of the continuous and discretized transfer functions.
  - Analyses of the performances of the PI and PID controllers, along with their respective *Settling Time* and *Overshoot*.

- **/docs/IHMbauer.pdf**  
  An illustrative diagram of the state machine implemented in the HMI, showing the various screens and user interactions.

- **/docs/simulink_blocks.png**  
  Images of the blocks used in Simulink for plant simulation and controller validation.

## How to Use the Project

1. **Source Code:**  
   - Navigate to the `/src` folder and open the `rotation_control.c` file to review the complete control implementation.
   - Compile the code and program the microcontroller according to the instructions and comments provided within the file.

2. **Documentation:**  
   - Refer to the `rotation_control.pdf` file in the `/docs` folder to gain a detailed understanding of the experimental procedures, plant identification, and controller analysis.
   - View the diagrams `IHMbauer.pdf` and `rotation_control.pdf` to understand the structure of the HMI and the modeling performed in Simulink.
