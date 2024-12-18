import socket

# ESP32 server IP and port
ESP32_IP = '192.168.0.106'  # Replace with your ESP32's IP address
PORT = 1234  # Port to which ESP32 is listening

START_BYTE = 0x02

# Send message to ESP32
def send_packet_to_esp32(packet: bytes, expext_response = True) -> None | bytes:
    try:
        # Create a socket and connect to the ESP32 server
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((ESP32_IP, PORT))
            print(f"Connected to {ESP32_IP}:{PORT}")
            
            # Send the message
            s.sendall(packet)
            print(f"Sent: {packet.hex()}")

            # Receive the response
            if expext_response:
                response = s.recv(260)
                print("Received response")
                print(f"start_byte: {response[0]:02X}")
                print(f"function_id: {response[1]:02X}")
                print(f"length: {response[2]}")
                payload = response[3:-1]
                print(f"payload (hex): {' '.join(f'{byte:02X}' for byte in payload)}")
                try:
                    print(f"payload (ascii): {payload.decode('ascii')}")
                except UnicodeDecodeError:
                    print("payload (ascii): <non-ascii data>")
                return payload

    except Exception as e:
        print(f"Error: {e}")

def construct_packet(function_id, payload, start_byte = START_BYTE) -> bytes:
    length = len(payload)
    message = bytes([start_byte, function_id, length]) + payload
    return message