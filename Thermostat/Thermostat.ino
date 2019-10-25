/*
  Name:		Thermostat.ino
  Created:	10/21/2019 5:19:47 AM
  Author:	fxrev
*/

// the setup function runs once when you press reset or power the board
#include "PubSub.h"
#include "Display.h"
#include "TempSensor.h"


#define ACRelay1 D5
#define ACRelay2 D6
//#define ACRelay3 D7
//#define ACRelay4 D8

#define BUTTON_PIN_1 D3
#define BUTTON_PIN_2 D4
#define BUTTON_PIN_3 D7


#define LED_PIN D8

static String deviceType = "HVAC";

static int setTemperature = 79;
static bool turnOn = false;
static bool autoAC = true;
static bool cool = true;
static char* statusCool = "STA: COOL";
static char* statusHeat = "STA: HEAT";
static int currentTemperature = 0;
static int currentHumidity = 0;

volatile byte interruptCounter = 0;

static unsigned long last_interrupt_up = 0;
static unsigned long last_interrupt_down = 0;
static unsigned long last_interrupt_select = 0;


std::string controllerID = "";

void ICACHE_RAM_ATTR handleUpButton();
void ICACHE_RAM_ATTR handleDownButton();
void ICACHE_RAM_ATTR handleSelButton();

void setup() {

  // Begin Serial on 115200
  // Remember to choose the correct Baudrate on the Serial monitor!
  // This is just for debugging purposes
  Serial.begin(9600);

  // Setup the first button with an internal pull-up :
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  attachInterrupt(BUTTON_PIN_1, handleUpButton, FALLING);


  // Setup the second button with an internal pull-up :
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  attachInterrupt(BUTTON_PIN_2, handleDownButton, FALLING);

  // Setup the second button with an internal pull-up :
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
  attachInterrupt(BUTTON_PIN_3, handleSelButton, FALLING);





  //Setup the Pins :
  pinMode(LED_PIN, OUTPUT);
  pinMode(ACRelay1, OUTPUT);
  pinMode(ACRelay2, OUTPUT);
  //  pinMode(ACRelay3, OUTPUT);
  //  pinMode(ACRelay4, OUTPUT);


  pubsubSetup();

  displaySetup();


  sensorSetup();


}
unsigned long counter;

void ICACHE_RAM_ATTR handleUpButton() {
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_up > 100)
  {
    setTemperature++;
  }
  last_interrupt_up = interrupt_time;
}

void ICACHE_RAM_ATTR handleDownButton() {
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_down > 100)
  {
    setTemperature--;
  }
  last_interrupt_down = interrupt_time;
}

void ICACHE_RAM_ATTR handleSelButton() {
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_down > 100)
  {
    cool ^= true;
  }
  last_interrupt_select = interrupt_time;
}


void loop() {
  pubsubLoop();

  uint intervalToSend = 10;
  counter++;

  if (updated) {
    handleCommand(Controllerid, Command, Data);
  }

  currentTemperature = (int)checkTemperatureFah();
  currentHumidity = (int)checkHumidity();


  showData();

  controlAC();


  if (counter % intervalToSend == 0) {
    sendData(deviceType+" "+ String(deviceId) + " temperature " + currentTemperature);
    sendData(deviceType+" "+String(deviceId) + " humidity " + currentHumidity);
    sendData(deviceType+" "+String(deviceId) + " setTemperature " + setTemperature);
    sendData(deviceType+" "+String(deviceId) + " cooling " + cool);
    sendData(deviceType+" "+String(deviceId) + " On " + turnOn);
  }


}

void showData() {
  
  auto tempMessage = "TMP: " + String(currentTemperature) + "F";
  auto humMessage = "HUM: " + String(currentHumidity) + "%";
  auto setTempMessage = "SET: " + String(setTemperature) + "F";
  
  clear();
  display.setCursor(0, 0);
  displayText((char*)tempMessage.c_str());
  displayText((char*)humMessage.c_str());
  displayText((char*)setTempMessage.c_str());
  if (cool)
    displayText((char*)statusCool);
  else
    displayText((char*)statusHeat);
  show();
}

void sendData(String str) {
  sendMessage(str);
}


void handleCommand(std::string id, std::string c, std::string dat) {
  String str = "";
  str += id.c_str();
  str += " ";
  str += c.c_str();
  str += " ";
  str += dat.c_str();
  Serial.println(str);

  if (strcmp(controllerID.c_str(), "") && strcmp(deviceId, id.c_str())) {
    controllerID = id;
    Serial.println("New Controller " + String(controllerID.c_str()));
  }
  if (!strcmp(controllerID.c_str(), "")) {
    auto com = (char*)c.c_str();
    int data = std::strtol((char*)dat.c_str(), nullptr, 10);
    if (compare(com, "setTemp")) {
      setTemperature = data;
    }
    else if (compare(com, "setAuto")) {
      if (data == 0)
        autoAC = false;
      else
        autoAC = true;

    }
    else if (compare(com, "setCool")) {
      if (data == 0)
        cool = false;
      else
        cool = true;

    }
    else if (compare(com, "setOn")) {
      if (data == 0)
        turnOn = false;
      else
        turnOn = true;

    }
  }

  updated = false;

}

void controlAC() {

  if (autoAC) {
    if (cool && setTemperature <= currentTemperature) {
      digitalWrite(ACRelay1, HIGH);
      turnOn = true;
    }
    else if (!cool && setTemperature > currentTemperature) {
      turnOn = true;
      digitalWrite(ACRelay2, HIGH);
    }
    else
    {
      turnOn = false;
      digitalWrite(ACRelay1, LOW);
      digitalWrite(ACRelay2, LOW);
    }

  }
  else {
    if (cool && turnOn) {
      digitalWrite(ACRelay1, HIGH);
      digitalWrite(ACRelay2, LOW);
    }
    else if (!cool && turnOn) {
      digitalWrite(ACRelay2, HIGH);
      digitalWrite(ACRelay1, LOW);
    }
    else {
      digitalWrite(ACRelay1, LOW);
      digitalWrite(ACRelay2, LOW);
    }
  }
}
