import send
import protocol

if __name__ == "__main__":
    # Construct the packet to get the system time
    packet = send.construct_packet(protocol.protocol_function_table["get_system_time"], b"")

    # Send the packet to the ESP32
    send.send_packet_to_esp32(packet)
