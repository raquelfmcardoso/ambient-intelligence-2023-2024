import serial

# Open serial connection to Arduino
ser = serial.Serial('COM4', 115200)  # Replace 'COM4' with the appropriate COM port

# Open file for writing
file_path = 'data.txt'
with open(file_path, 'w') as file:
    while True:
        # Read data from Arduino
        line = ser.readline().decode().strip()  # Decode bytes to string and remove whitespace
        
        # Write data to file
        file.write(line + '\n')
        file.flush()  # Ensure data is written immediately
        
        print("Data written:", line)
