# SD2019
Home Automation System

Use the following commands to control the devices

# Thermostat
ControllerID DeviceID Command Data

1234 1234 setCool 1/0
1234 1234 setAuto 1/0
1234 1234 setOn 1/0
1234 1234 setTemp 60-90

The Thermostat will broadcast data in similar form
DeviceType DeviceID DataType Data/Status

HVAC 1234 Cooling 1/0
HVAC 1234 On 1/0
HVAC 1234 setTemperature integer-fahrenheit
HVAC 1234 Humidity integer-percentage
HVAC 1234 Temperature integer-fahrenheit
  
  
