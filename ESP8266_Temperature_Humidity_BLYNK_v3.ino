/************************************************************************************

 *  My Channel: https://www.youtube.com/c/TeachMeSomething  Visit for More Project Videos
 *  
 *  *********************************************************************************
 *  Preferences--> Aditional boards Manager URLs : 
 *  For ESP32:
 *  https://dl.espressif.com/dl/package_esp32_index.json
 *  
 *  *********************************************************************************/
#define BLYNK_TEMPLATE_ID "templ"
#define BLYNK_DEVICE_NAME "name"
#define BLYNK_AUTH_TOKEN "auth"


#define BLYNK_PRINT Serial
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "ssid";
char pass[] = "pass";

BlynkTimer timer;

#define DHTPIN 5 
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

// Data wire is plugged TO GPIO 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Number of temperature devices found
int numberOfDevices;

float *temperatures;

// We'll use this variable to store a found device address
DeviceAddress tempDeviceAddress; 


void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  float temp1 = temperatures[0];
  float temp2 = temperatures[1];

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, h);
    Blynk.virtualWrite(V2, temp1);
    Blynk.virtualWrite(V3, temp2);
    Serial.print("Temperature : ");
    Serial.print(t);
    Serial.print("    Humidity : ");
    Serial.println(h);
    Serial.print("    temp1 : ");
    Serial.println(temp1);
    Serial.print("    temp2 : ");
    Serial.println(temp2);
}
void setup()
{   
  Serial.begin(115200);
  // Start up the library
  sensors.begin();
  
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timer.setInterval(100L, sendSensor);

  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();

  // Allocate memory for temperatures (float) of all devices on the wire.
  temperatures = (float *)malloc(sizeof(float) * numberOfDevices);
  if(temperatures == NULL) {
    Serial.println("Memory allocation failed!");
    while(1); // Endless loop. Not a good idea in production code!
  }
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  } 
 }

void loop()
{
  Blynk.run();
  timer.run();
    sensors.requestTemperatures(); // Send the command to get temperatures
  
  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices; i++) {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)) {
      temperatures[i] = sensors.getTempC(tempDeviceAddress);
      
      // Output the device ID
      Serial.print("Temperature for device: ");
      Serial.println(i,DEC);
      Serial.print("Temp C: ");
      Serial.print(temperatures[i]);
    }
  }
  
  delay(5000);
 }

 // function to print a device address
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}
