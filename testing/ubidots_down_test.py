import requests
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
    variable_data = {}
    url = "http://industrial.api.ubidots.com"
    url = "{}/api/v1.6/devices/{}".format(url, DEVICE_LABEL)
    headers = {"X-Auth-Token": TOKEN, "Content-Type": "application/json"}
    
    message = "Error: Unable to fetch data for variable 1."
    return message

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
    for label in VARIABLE_LABELS:
        message = get_request(label)
        print(message)
        print_to_serial(label, message)
            
    time.sleep(60)

if __name__ == '__main__':
    read_thread = threading.Thread(target=read_from_serial)
    read_thread.start()
    while True:
        main()
        time.sleep(1)