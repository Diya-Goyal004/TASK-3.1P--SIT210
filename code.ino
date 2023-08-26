#include <WiFiNINA.h>
// Include a header file that stores sensitive information like SSID and password
#include "secrets.h" 
// Include the ThingSpeak header file for using ThingSpeak API
#include "ThingSpeak.h" 
// Include the DHT sensor library
#include "DHT.h"

// Define the pin to which the DHT sensor is connected
#define DHTPIN 2 
// Define the type of DHT sensor being used (DHT22)
#define DHTTYPE DHT22

// Store the network SSID in a character array
char ssid[] = SECRET_SSID;  
// Store the network password in a character array 
char pass[] = SECRET_PASS;  
// Store the network key Index number (needed only for WEP networks) 
int keyIndex = 0; 
// Create a WiFi client object for network communication           
WiFiClient  client;

// Store ThingSpeak channel number
unsigned long myChannelNumber = SECRET_CH_ID;
// Store ThingSpeak write API key
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Create a DHT object with the defined pin and type
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  // Start serial communication at 9600 bps
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  // Initialize the DHT sensor
  dht.begin();
  // Initialize ThingSpeak library with the WiFi client
  ThingSpeak.begin(client);   
}

void loop() {
  // Read humidity from the DHT sensor
  float h = dht.readHumidity();
  // Read temperature in Celsius from the DHT sensor
  float t = dht.readTemperature();
  // Read temperature in Fahrenheit from the DHT sensor
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again)
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
  // Connect or reconnect to WiFi if not connected
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      // Connect to the WiFi network
      WiFi.begin(ssid, pass);  
      Serial.print(".");
      // Wait for connection
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // Set the fields with the sensor values
  ThingSpeak.setField(1,h);
 ThingSpeak.setField(2, t);
 ThingSpeak.setField(3, f);

  // figure out the status message
 
  
  // Write data to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  
  
  delay(20000); // Wait 20 seconds before updating the channel again
}
