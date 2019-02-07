#!/usr/bin/python
import sys
import Adafruit_DHT
import time
import json
import RPi.GPIO as GPIO
import dht11

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.cleanup()


def RCtime (PiPin):
  measurement = 0
  # Discharge capacitor
  GPIO.setup(PiPin, GPIO.OUT)
  GPIO.output(PiPin, GPIO.LOW)

  time.sleep(0.1)
  GPIO.setup(PiPin, GPIO.IN)
  # Count loops until voltage across
  # capacitor reads high on GPIO
  while (GPIO.input(PiPin) == GPIO.LOW):
    measurement += 1

  return measurement


# read data using pin 14
instance = dht11.DHT11(pin = 14)
result = instance.read()
  
#humidity, temperature = Adafruit_DHT.read_retry(11, 4)
data = {'humidity':result.humidity, 'temperature':result.temperature, 'light':RCtime(17)}
print json.dumps(data)