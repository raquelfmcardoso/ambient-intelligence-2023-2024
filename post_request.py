import serial
import time
import requests
import math
import random

TOKEN = "BBUS-zjuPmVgxisCTzVOcQ1wUjwNDxaPcsI"
DEVICE_LABEL = "smart_pill_organizer"
VARIABLE_LABEL_1 = "medication 1"
VARIABLE_LABEL_2 = "medication 2"
VARIABLE_LABEL_3 = "medication 3"

# Open serial connection to Arduino
ser = serial.Serial('COM4', 115200)  # Replace 'COM4' with the appropriate COM port
print("Reading data from serial port.....");
time.sleep(2)
ser.reset_input_buffer() # Delete any stale data.

def build_payload(variable_1, variable_2, variable_3, data):
    value_1 = int(data[0])
    value_2 = int(data[1])
    value_3 = int(data[2])

    payload = {variable_1: value_1, variable_2: value_2, variable_3: value_3}
    return payload


def post_request(payload):
    url = "http://industrial.api.ubidots.com"
    url = "{}/api/v1.6/devices/{}".format(url, DEVICE_LABEL)
    headers = {"X-Auth-Token": TOKEN, "Content-Type": "application/json"}

    status = 400
    attempts = 0
    while status >= 400 and attempts <= 5:
        req = requests.post(url=url, headers=headers, json=payload)
        status = req.status_code
        attempts += 1
        time.sleep(1)

    if status >= 400:
        print("[ERROR] Could not send data after 5 attempts, please check \
            your token credentials and internet connection")
        return False

    print("Device ID updated...wait for data transfer")
    return True


def main():
    line = ser.readline() 
    if line:
        rxdata = line.decode()  
        tmp = rxdata.replace(':', ' ').replace('/', ' ').split()

        payload = build_payload(VARIABLE_LABEL_1, VARIABLE_LABEL_2, VARIABLE_LABEL_3, tmp)
        print(tmp[0],tmp[1],tmp[2])
        del tmp

        print("Data transfer started")
        post_request(payload)
        print("Data Successfully sent to cloud")


if __name__ == '__main__':
    while (True):
        main()
        time.sleep(1)
