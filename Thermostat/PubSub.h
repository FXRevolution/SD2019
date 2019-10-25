
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <string.h>

// WiFi
// Make sure to update this for your own WiFi network!
const char* ssid = "PiNetwork";
const char* wifi_password = "SD2019FALL";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* mqtt_server = "192.168.0.10";
const char* mqtt_username = "username";
const char* mqtt_password = "password";
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char* deviceId = "1234";
const char* clientId = "1234";

const char* mqtt_sub_topic = "HVAC_Control";
const char* mqtt_pub_topic = "HVAC_Data";

const int mqtt_limit = 20;


bool updated;
char* Controllerid;
char* Command;
char* Data;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



char* convertBytes(uint8_t* payload, uint len) {
  std::string str = "";
  for (int i = 0; i < len; i++)
  {
    str += (char)payload[i];
  }

  return (char*)str.c_str();
}

void clearstr(char * str, int len) {
  for (int i = 0; i < len; i++)
    str[i] = '\0';
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  char* message = convertBytes(payload, length);
  Serial.println(message);

  std::string cont, com, dat;
  auto mess = std::string(message);

  int from, to;
  int i = 0;

  from = i;
  while (message[i] != ' ')
    ++i;
  to = i;
  cont = mess.substr(from, to);
  Controllerid = (char*)cont.c_str();
  ++i;


  from = i;
  while (message[i] != ' ')
    ++i;
  to = i;
  com = mess.substr(from, to);
  Command = (char*)com.c_str();
  ++i;


  from = i;
  while (message[i] != ' ' && message[i] != '\0')
    ++i;
  to = i;
  dat = mess.substr(from, to);
  Data = (char*)dat.c_str();
  ++i;



  //  if ()  {
  //    Serial.println("Bad Command Message!");
  //    return;
  //  }
  //  else
  Serial.println(Controllerid);
  Serial.println(Command);
  Serial.println(Data);
  updated = true;

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // Attempt to connect
    if (client.connect(clientId, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_pub_topic, "hello world");
      // ... and resubscribe
      client.subscribe(mqtt_sub_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void sendMessage(String str) {
  char* message = (char*)str.c_str();

  if (client.publish(mqtt_pub_topic, message)) {
    Serial.println("Message sent!");
  }
}

bool compare(char* one, char* two) {
  int i = 0;
  while (one[i] != '\0' && two[i] != '\0') {
    if (one[i] != two[i])
      return false;
      ++i;
  }
  return true;
}

void pubsubSetup() {
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void pubsubLoop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
