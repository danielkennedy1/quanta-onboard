# IoT Device Real-Time System Project

## Overview
- LM173 Immersive Software Engineering
- CS4447 Computational Thinking
- Embedded & Real-Time Systems
- Instructor: Dr. Mark Burkley
- Daniel Kennedy 22340017


## Project Description
The goal is to design and implement a functional IoT device on the Firebeetle-ESP32 platform (or any compatible hardware that supports FreeRTOS). 

## Requirements
- **Framework and OS**: Use the ESP-IDF framework and the FreeRTOS operating system.
- **Programming Language**: The primary implementation should be in C or C++, but Rust or other compiled languages may also be used.
- **Real-Time Features**: Demonstrate real-time data acquisition and control capabilities.
- **Threaded Design**:
  - Separate threads must handle control/data acquisition and networked communication.
  - Threads should operate at different priorities.
  - Implement a bi-directional queuing mechanism for data transfer between threads.
- **Networked Communication**:
  - Communicate with an external data aggregation agent (e.g., on a laptop) using a proprietary, byte-aligned protocol.
  - Support communication over USB, WiFi, Bluetooth, or other protocols available to the agent and embedded device.
- **Robustness**:
  - The real-time control functionality must remain operational even if the agent is disconnected.
  - Implement data buffering to handle agent disconnects and reconnects.
- **Control Application**:
  - Include a real-time control feature such as:
    - Simulated heating or air-conditioning control.
    - LED or LCD display animations.
    - Motor positioning control.
    - Other relevant real-time control applications.
  - The specific feature will be chosen by the student.

## TODO
- [ ] Select a real-time control application.
- [ ] Set up development environment with ESP-IDF and FreeRTOS.
- [ ] Develop a real-time data acquisition and control system.
  - [ ] Implement a thread for control/data acquisition.
  - [ ] Ensure the control system operates independently of agent connection.
- [ ] Develop a networked data aggregation agent.
  - [ ] Design a proprietary, byte-aligned communication protocol.
  - [ ] Implement connectivity over USB, WiFi, Bluetooth, or other protocols.
- [ ] Implement bi-directional queuing between threads.
- [ ] Test for robustness under agent disconnect/reconnect scenarios.
- [ ] Modularize and document code for readability and maintainability.
- [ ] Prepare for project demonstration and code walkthrough.

