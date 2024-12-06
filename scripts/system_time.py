import socket

# ESP32 server IP and port
ESP32_IP = '192.168.0.174'  # Replace with your ESP32's IP address
PORT = 1234  # Port to which ESP32 is listening

# Custom protocol parameters
START_BYTE = 0x02
FUNCTION_ID = 0x01
PAYLOAD = b''  # Payload to send
LENGTH = len(PAYLOAD)

# Create the protocol message
def create_packet(function_id, payload):
    length = len(payload)
    
    # Custom protocol format: [START_BYTE][LENGTH][PAYLOAD][CHECKSUM]
    message = bytes([START_BYTE, function_id, length]) + payload
    return message

# Send message to ESP32
def send_packet_to_esp32(packet):
    try:
        # Create a socket and connect to the ESP32 server
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((ESP32_IP, PORT))
            print(f"Connected to {ESP32_IP}:{PORT}")
            
            # Send the message
            s.sendall(packet)
            print(f"Sent: {packet.hex()}")

            # Receive the response
            response = s.recv(260)
            print("Received response")
            print(f"start_byte: {response[0]:02X}")
            print(f"function_id: {response[1]:02X}")
            print(f"length: {response[2]}")
            print(f"payload: {response[3:-1].decode()}")

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    # Create the custom protocol message
    message = create_packet(FUNCTION_ID, PAYLOAD)

    # Send the message to the ESP32
    send_packet_to_esp32(message)
