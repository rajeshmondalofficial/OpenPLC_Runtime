import serial

# Configure the serial port
port = "/dev/ttyAMA0"  # Replace with the correct port for your setup (e.g., COM3 on Windows)
baud_rate = 9600

# Initialize the serial connection
ser = serial.Serial(port, baud_rate, timeout=1)

try:
    # Data to send
    send_data = "Hello, UART Loopback!"

    # Write data to UART
    ser.write(send_data.encode('utf-8'))
    print(f"Sent: {send_data}")

    # Read the data back
    received_data = ser.read(len(send_data)).decode('utf-8')
    print(f"Received: {received_data}")

    # Verify loopback
    if send_data == received_data:
        print("Loopback successful!")
    else:
        print("Mismatch in loopback data.")

finally:
    # Close the serial connection
    ser.close()
