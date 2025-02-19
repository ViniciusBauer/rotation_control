# Integrated Project - Fan Control

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

- **/control_project/conversion.xlsx**  
  Excel file containing data or conversion parameters used during the project’s development.

- **/control_project/IHMBauer.pdf**  
  Document providing an overview or reference for the Human-Machine Interface (HMI) design.

- **/control_project/rotation_control.c**  
  The main C source code implementing the fan control. Key features include:
  - Configuration of timers, the AD converter, PWM, LCD, and keypad.
  - A state machine-based HMI for setpoint configuration and selection of PI or PID controllers.
  - A control routine that updates speed and duty cycle by integrating the discrete control equations.

- **/control_project/rotation_control.m**  
  A MATLAB/Octave script used for system analysis or simulation, supporting the design and validation of the controllers.

- **/control_project/rotation_control.pdf**  
  A detailed document outlining the methodology used, including:
  - The experimental procedure for plant identification.
  - Derivation and comparison of the continuous and discretized transfer functions.
  - Performance analyses of the PI and PID controllers, highlighting *Settling Time* and *Overshoot*.

## How to Use the Project

1. **Source Code:**  
   - Navigate to the `/control_project` folder and open `rotation_control.c` to review the complete control implementation.
   - Compile the code and program the microcontroller according to the instructions and comments within the file.

2. **Documentation and Scripts:**  
   - Refer to `rotation_control.pdf` for a comprehensive explanation of the experimental procedures, plant identification, and controller analysis.
   - Use `rotation_control.m` to replicate or extend the simulations and analyses performed in MATLAB/Octave.
   - Check `IHMbauer.pdf` for insights into the HMI design and implementation details.

3. **Data Files:**  
   - Examine `conversion.xlsx` if you need to review or modify the data/conversion parameters used in the project.
