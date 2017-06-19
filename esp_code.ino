// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>

const char* ssid     = "ssid";
const char* password = "wifipassword";

// Data wire is plugged into port 2 on the ESP8266
#define ONE_WIRE_BUS 2 // GPIO pin 2
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Arrays to hold device addresses. These will need to be changed depending on what addresses your sensors have.
byte insideThermometer[] = { 0x28, 0xFF, 0xC6, 0xB5, 0x70, 0x16, 0x05, 0x70 };
byte outsideThermometer[] = { 0x28, 0xFF, 0x81, 0x98, 0x74, 0x16, 0x04, 0xE5 };

void setup(void)
{
  // start serial port
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  pinMode(LED_BUILTIN, OUTPUT);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }

  // Start up the library
  sensors.begin();

  // set the resolution to 9 bit per device
  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
}


void loop(void)
{
  sensors.requestTemperatures();
  float insideTempC = sensors.getTempC(insideThermometer);
  float insideTempF = DallasTemperature::toFahrenheit(insideTempC);
  float outsideTempC = sensors.getTempC(outsideThermometer);
  float outsideTempF = DallasTemperature::toFahrenheit(outsideTempC);
  float deltaF = insideTempF - outsideTempF;
  float deltaC = insideTempC - outsideTempC;

  WiFiClient client;

  String PostData = "{\"insideTempC\": ";
  PostData += insideTempC;
  PostData += ",";
  PostData += "\"insideTempF\": ";
  PostData += insideTempF;
  PostData += ",";
  PostData += "\"outsideTempC\": ";
  PostData += outsideTempC;
  PostData += ",";
  PostData += "\"outsideTempF\": ";
  PostData += outsideTempF;
  PostData += ",";
  PostData += "\"deltaF\": ";
  PostData += deltaF;
  PostData += ",";
  PostData += "\"deltaC\": ";
  PostData += deltaC;
  PostData += "}";

  Serial.print(PostData);

  const uint16_t port = 8081; // Port Logstash is listening on
  const char* host = "192.168.1.18"; // IP address of Logstash server

  if (client.connect(host, port)) {
    digitalWrite(LED_BUILTIN, LOW); // This mixed with the lower HIGH will just flash the built in LED while uploading
    client.println("POST /posts HTTP/1.1");
    client.println("Host: 192.168.1.18");
    client.println("Cache-Control: no-cache");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);
    client.stop();
    digitalWrite(LED_BUILTIN, HIGH);
    }

  delay(60000); // Delay before taking next readin in milliseconds
}
