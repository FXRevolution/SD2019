
import paho.mqtt.client as mqtt
import time
import random

# Don't forget to change the variables for the MQTT broker!
mqtt_username = "username"
mqtt_password = "password"
mqtt_topic = "HVAC_Control"
mqtt_broker_ip = "192.168.0.13"

client = mqtt.Client("Python")
# Set the username and password for the MQTT client
client.username_pw_set(mqtt_username, mqtt_password)

Connected = False
# These functions handle what happens when the MQTT client connects
# to the broker, and what happens then the topic receives a message
def on_connect(client, userdata, flags, rc):
    # rc is the error code returned when connecting to the broker
    if rc == 0:
 
        print("Connected to broker")
 
        global Connected                #Use global variable
        Connected = True                #Signal connection 
 
    else:
 
        print("Connection failed")
    
    # Once the client has connected to the broker, subscribe to the topic
    client.subscribe(mqtt_topic)
    
def on_message(client, userdata, msg):
    # This function is called everytime the topic is published to.
    # If you want to check each message, and do something depending on
    # the content, the code to do this should be run in this function
    
    print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    
    # The message itself is stored in the msg variable
    # and details about who sent it are stored in userdata

# Here, we are telling the client which functions are to be run
# on connecting, and on receiving a message
client.on_connect = on_connect
client.on_message = on_message

# Once everything has been set up, we can (finally) connect to the broker
# 1883 is the listener port that the MQTT broker is using
client.connect(mqtt_broker_ip, 1883)

# Once we have told the client to connect, let the client object run itself
client.loop_start()

while Connected != True:
    time.sleep(10)
    print("not connected")

random.seed()

# example of how to publish a message
try:
    while (Connected == True):
        time.sleep(5)
        value = "HVAC 1234 temperature " + str(random.randint(0,100))
        client.publish("HVAC_Data", value)
        value = "HVAC 1234 humidity " + str(random.randint(0,100))
        client.publish("HVAC_Data", value)
        value = "HVAC 1234 cooling " + str(random.randint(0,1))
        client.publish("HVAC_Data", value)
        value = "HVAC 1234 on " + str(random.randint(0,1))
        client.publish("HVAC_Data", value)
        value = "HVAC 1234 setTemperature " + str(random.randint(0,100))
        client.publish("HVAC_Data", value)
        
except KeyboardInterrupt:
    client.disconnect()
    client.loop_stop()
        
