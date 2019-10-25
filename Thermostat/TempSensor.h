#include <DHTesp.h>
#define TempSensorPin D0

DHTesp dht;

void sensorSetup() {
  dht.setup(TempSensorPin, DHTesp::DHT22);
}

float checkTemperatureCel() {
  delay(dht.getMinimumSamplingPeriod());

  float temperature = dht.getTemperature();
  return temperature;

}

float checkTemperatureFah() {
  delay(dht.getMinimumSamplingPeriod());

  float temperature = dht.getTemperature();
  return dht.toFahrenheit(temperature);

}

float checkHumidity() {
  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  return humidity;

}
