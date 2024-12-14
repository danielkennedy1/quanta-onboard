import send
import protocol
import struct
from datetime import datetime

# payload = b'\x73\xCF\x5D\x67\x00\x00\x00\x00\x4F\x8F\xCD\x41' + \
#           b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' + \
#           b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' + \
#           b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' + \
#           b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' + \
#           b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' + \
#           b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' + \
#           b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' + \
#           b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' + \
#           b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' + \
#           b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' + \
#           b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'


if __name__ == "__main__":
    # Construct the packet to get the system time
    packet = send.construct_packet(protocol.protocol_function_table["get_avg_temperature_for_minute"], b"")

    # # Send the packet to the ESP32
    payload = send.send_packet_to_esp32(packet)

    unix_timestamp: int = struct.unpack('<Q', payload[:8])[0]

    print(f"Unix Timestamp: {unix_timestamp}")

    timestamp = datetime.fromtimestamp(float(unix_timestamp))

    # Convert the Unix timestamp to a human-readable date
    timestamp_str = timestamp.strftime('%Y-%m-%d %H:%M:%S')
    

    # Extract the next 4 bytes as a float (big-endian)
    float_value = struct.unpack('<f', payload[8:12])[0]  # '>f' for big-endian 32-bit float

    # Pretty print the values
    print("Payload Analysis:")
    print(f"  Unix Timestamp: {unix_timestamp} ({timestamp_str} UTC)")
    print(f"  Float Value: {float_value}")
