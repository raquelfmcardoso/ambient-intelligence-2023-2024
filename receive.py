import serial
import time
import requests
import math
import random

TOKEN = "BBUS-zjuPmVgxisCTzVOcQ1wUjwNDxaPcsI"
DEVICE_LABEL = "My_PC"
VARIABLE_LABEL_1 = "hours"
VARIABLE_LABEL_2 = "minutes"
VARIABLE_LABEL_3 = "day"
VARIABLE_LABEL_4 = "month"
VARIABLE_LABEL_5 = "year"
VARIABLE_LABEL_6 = "medication 1"
VARIABLE_LABEL_7 = "medication 2"
VARIABLE_LABEL_8 = "medication 3"
VARIABLE_LABEL_9 = "medication 4"

# Open serial connection to Arduino
ser = serial.Serial('COM4', 115200)  # Replace 'COM4' with the appropriate COM port
print("Reading data from serial port.....");
time.sleep(2)
ser.reset_input_buffer() # Delete any stale data.

def build_payload(variable_1, variable_2, variable_3, variable_4, variable_5, variable_6, variable_7, variable_8, variable_9, data):
    # Creates six random values for sending data
    value_1 = int(data[0])
    value_2 = int(data[1])
    value_3 = int(data[2])
    value_4 = int(data[3])
    value_5 = int(data[4])
    value_6 = int(data[5])
    value_7 = int(data[6])
    value_8 = int(data[7])
    value_9 = int(data[8])

    payload = {variable_1: value_1, variable_2: value_2, variable_3: value_3, variable_4: value_4, variable_5: value_5, variable_6: value_6, variable_7: value_7, variable_8: value_8, variable_9: value_9}

    return payload


def post_request(payload):
    # Creates the headers for the HTTP requests
    url = "http://industrial.api.ubidots.com"
    url = "{}/api/v1.6/devices/{}".format(url, DEVICE_LABEL)
    headers = {"X-Auth-Token": TOKEN, "Content-Type": "application/json"}

    # Makes the HTTP requests
    status = 400
    attempts = 0
    while status >= 400 and attempts <= 5:
        req = requests.post(url=url, headers=headers, json=payload)
        status = req.status_code
        attempts += 1
        time.sleep(1)

    # Processes results
    #print(req.status_code, req.json())
    if status >= 400:
        print("[ERROR] Could not send data after 5 attempts, please check \
            your token credentials and internet connection")
        return False

    print("Device ID updated...wait for data transfer")
    return True


def main():
# Reading all bytes available bytes till EOL
    line = ser.readline() 
    if line:
        # Converting Byte Strings into unicode strings
        rxdata = line.decode()  
        # Converting Unicode String into integer
        #num = int(rxdata) 
        #print(num)
        tmp = rxdata.replace(':', ' ').replace('/', ' ').split()
        
        #send collected data to cloud as a payload
        payload = build_payload(VARIABLE_LABEL_1, VARIABLE_LABEL_2, VARIABLE_LABEL_3, VARIABLE_LABEL_4, VARIABLE_LABEL_5, VARIABLE_LABEL_6, VARIABLE_LABEL_7, VARIABLE_LABEL_8, VARIABLE_LABEL_9, tmp)
        print(tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6],tmp[7],tmp[8])
        del tmp #clear received data

        print("Data transfer started")
        post_request(payload)
        print("Data Successfully sent to cloud")


if __name__ == '__main__':
    while (True):
        main()
        time.sleep(1)
