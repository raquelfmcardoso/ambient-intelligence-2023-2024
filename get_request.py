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
    try:
        variable_url = f"{url}/{label}/values/?page_size=2"
        response = requests.get(variable_url, headers=headers)
        if response.status_code == 200:
            response_json = response.json()
            results = response_json.get('results', [])
            if len(results) >= 1:
                timestamp = timestamp_conversion(results[0]['timestamp'])
                variable_data['timestamp'] = timestamp
                variable_data['value'] = results[0]['value']
                if len(results) == 1:
                    variable_data['last_value'] = variable_data['value']
                else:
                    variable_data['last_value'] = results[1]['value']
            return variable_data
        else:
            message = f"Error: Unable to fetch data for variable {label}."
            with ser_lock:
                ser.write(message.encode())
                print(message)
            return None
    except Exception as e:
        exception = f"Error: {e}"
        with ser_lock:
            ser.write(exception.encode())
            print(exception)
        return None

def timestamp_conversion(time):
    timestamp = time / 1000
    dt_object = datetime.datetime.fromtimestamp(timestamp)
    date = dt_object.strftime('%d-%m-%Y')  # Format: DD-MM-YYYY
    hour = dt_object.strftime('%H:%M')  # Format: HH:MM
    return (date, hour)

def print_to_serial(label, variable_data):
    message = f"{label},{variable_data['timestamp'][1]},{variable_data['value']},{variable_data['last_value']}\n"
    with ser_lock:
        ser.write(message.encode())
        print(message)

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