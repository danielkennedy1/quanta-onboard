import socket

# ESP32 server IP and port
ESP32_IP = '192.168.184.91'  # Replace with your ESP32's IP address
PORT = 1234  # Port to which ESP32 is listening

# Custom protocol parameters
START_BYTE = 0x02
PAYLOAD = b'ABCDE'  # Payload to send
LENGTH = len(PAYLOAD)
CHECKSUM = 0  # We'll calculate the checksum based on the payload

# Function to calculate checksum (simple XOR)
def calculate_checksum(data):
    checksum = 0
    for byte in data:
        checksum ^= byte
    return checksum

# Create the protocol message
def create_protocol_message(payload):
    length = len(payload)
    checksum = calculate_checksum(payload)
    
    # Custom protocol format: [START_BYTE][LENGTH][PAYLOAD][CHECKSUM]
    message = bytes([START_BYTE, length]) + payload + bytes([checksum])
    return message

# Send message to ESP32
def send_message_to_esp32(message):
    try:
        # Create a socket and connect to the ESP32 server
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((ESP32_IP, PORT))
            print(f"Connected to {ESP32_IP}:{PORT}")
            
            # Send the message
            s.sendall(message)
            print(f"Sent: {message.hex()}")

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    # Create the custom protocol message
    message = create_protocol_message(PAYLOAD)

    # Send the message to the ESP32
    send_message_to_esp32(message)
