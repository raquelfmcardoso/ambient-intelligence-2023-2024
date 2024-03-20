import time
import datetime
import serial
import threading

TOKEN = "BBUS-6r8wUc13PXf3brHoIIKQbZzlk5BEP5"
DEVICE_LABEL = "smart_pill_organizer"
VARIABLE_LABELS = [
    "medication-1",
    "medication-2",
    "medication-3",
    "medication1-hour",
    "medication1-minute",
    "medication2-hour",
    "medication2-minute",
    "medication3-hour",
    "medication3-minute"
]

ser = serial.Serial('COM4', 115200)  # Replace 'COM4' with the appropriate COM port
ser_lock = threading.Lock()

def get_request(label):
    message = f"Error: Unable to fetch data for variable {label}."
    with ser_lock:
        ser.write(message.encode())
        print(message)
    return None

def print_to_serial(label, message):
    msg = f"{message}\n"
    with ser_lock:
        ser.write(msg.encode())
        print(msg)

def read_from_serial():
    while True:
        with ser_lock:
            if ser.in_waiting > 0:
                received_data = ser.readline().decode().strip()
                print("Received from Arduino:", received_data)

def main():
    if datetime.datetime.now().second == 0:
        for label in VARIABLE_LABELS:
            variable_data = get_request(label)
            if variable_data:
                print(variable_data)
                print_to_serial(label, variable_data)

if __name__ == '__main__':
    read_thread = threading.Thread(target=read_from_serial)
    read_thread.start()
    while True:
        main()
        time.sleep(1)