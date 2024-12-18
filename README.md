# IoT Device Real-Time System Project

## Overview
- LM173 Immersive Software Engineering
- CS4447 Computational Thinking
- Embedded & Real-Time Systems
- Instructor: Dr. Mark Burkley
- Daniel Kennedy 22340017


## Protocol

| Field          | Type     | Description                          | Byte(s)       |
|----------------|----------|--------------------------------------|---------------|
| `start_byte`   | `uint8_t`| Start byte                           | 0             |
| `function_flag`| `uint8_t`| Function selector                    | 1             |
| `payload_size` | `uint8_t`| Size of the payload in bytes         | 2             |
| `payload`      | `uint8_t[257]`| Payload for function parameters | 3 - 259       |

## Byte Layout

| Byte    | 0           | 1             | 2             | 3 - 258       |
|---------|-------------|---------------|---------------|---------------|
| Field   | `start_byte`| `function_flag`| `payload_size`| `payload`    |


## Functions
0x00 - Heartbeat
