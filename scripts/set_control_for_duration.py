import struct
import send
import protocol

if __name__ == "__main__":
    control = True
    duration = 15        # 4-byte integer

    payload = struct.pack('<?I', control, duration)
    # Construct the packet to set the temperature for a duration
    packet = send.construct_packet(protocol.protocol_function_table["set_control_for_duration"], payload)

    print(f"complete packet: {packet.hex()}")
    
    # Send the packet to the ESP32
    send.send_packet_to_esp32(packet)