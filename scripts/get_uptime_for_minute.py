import send
import protocol
import struct
from datetime import datetime

if __name__ == "__main__":
    packet = send.construct_packet(protocol.protocol_function_table["get_uptime_for_minute"], b"")

    payload = send.send_packet_to_esp32(packet)

    unix_timestamp: int = struct.unpack('<Q', payload[:8])[0]
    avg_temp = struct.unpack('<f', payload[8:12])[0]

    print(f"Unix Timestamp: {unix_timestamp}")

    timestamp = datetime.fromtimestamp(float(unix_timestamp))

    timestamp_str = timestamp.strftime('%Y-%m-%d %H:%M:%S')

    print("Payload Analysis:")
    print(f"  Unix Timestamp: {unix_timestamp} ({timestamp_str} UTC)")
    print(f"  Float Value: {avg_temp}")