import requests
import time
import datetime
import serial

TOKEN = "BBUS-zjuPmVgxisCTzVOcQ1wUjwNDxaPcsI"
DEVICE_LABEL = "my_pc"
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

ser = serial.Serial('COM5', 115200)  # Replace 'COM4' with the appropriate COM port

def get_request(label):
    variable_data = {}
    url = "http://industrial.api.ubidots.com"
    url = "{}/api/v1.6/devices/{}".format(url, DEVICE_LABEL)
    headers = {"X-Auth-Token": TOKEN, "Content-Type": "application/json"}
    try:
        variable_url = f"{url}/{label}/values/?page_size=1"
        response = requests.get(variable_url, headers=headers)
        if response.status_code == 200:
            response_json = response.json()
            timestamp = timestamp_conversion(response_json['results'][0]['timestamp'])
            variable_data['timestamp'] = timestamp
            variable_data['value'] = response_json['results'][0]['value']
            variable_data['context'] = response_json['results'][0]['context']
            return variable_data
        else:
            print(f"Error: Unable to fetch data for variable {label}")
            return None
    except Exception as e:
        print("Exception:", e)
        return None

def timestamp_conversion(time):
    # Unix timestamp (example)
    timestamp = time / 1000  # Assuming the timestamp is in milliseconds, so dividing by 1000 to convert to seconds

    # Convert Unix timestamp to datetime object
    dt_object = datetime.datetime.fromtimestamp(timestamp)

    # Extract date and hour
    date = dt_object.strftime('%d-%m-%Y')  # Format: YYYY-MM-DD
    hour = dt_object.strftime('%H:%M')  # Format: HH:MM:SS
    
    return (date, hour)

def print_to_serial(variable_data):
    message = f"{variable_data['timestamp'][0], variable_data['timestamp'][1]}, {variable_data['value']}"
    ser.write(message.encode())
    print(message)

def main():
    for label in VARIABLE_LABELS:
        variable_data = get_request(label)
        if variable_data:
            print(variable_data)
            print_to_serial(variable_data)
    time.sleep(60)

if __name__ == '__main__':
    while (True):
        main()
        time.sleep(1)