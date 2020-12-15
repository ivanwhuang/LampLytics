#include "WiFiEsp.h"
// Emulate Serial1 on pins 6/7 if not present 
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(10, 11); // RX, TX 
#endif

#include <DHT.h>;
#include "rgb_lcd.h"

#define FAN_MOTOR A0
#define LIGHT_SENSOR A1
#define BUTTON_PIN 3
#define RED_LIGHT 5
#define BLUE_LIGHT 6
#define ECHO_PIN 8
#define TRIG_PIN 9
#define DHTPIN 2 
#define DHTTYPE DHT22 

// System State Flags
#define SYS_OFF -1
#define SYS_STANDBY 0
#define SYS_ACTIVE 1

#define WIFI_MILLIS 30000

char ssid[] = "---------------"; // your network SSID (name) 
char pass[] = "---------------"; // your network password 
int status = WL_IDLE_STATUS; // the Wifi radio's status 
char server[] = "3.88.218.230";

//char var[] = "TEMP";
char get_request[200];
// Initialize the Ethernet client object 
WiFiEspClient client;

// Initialize the dht object
DHT dht(DHTPIN, DHTTYPE);
// humidity and temperature constants
float temp; 
char tempStr[6];

int minLightThresh = 0;
int maxLightThresh = 900;

int currentLightVolt = 0;
int lightThreshold = 5; 
int lightVolt = 0;
int light = 0;

// distance constants
long duration;
int distInch;

unsigned long deskTimestamp;
unsigned long deskTime = 0;
char deskTimeStr[6];

int fanVolt = 0;
float minHotThresh = 15.00;
float maxHotThresh = 20.00;

// lcd display constants
rgb_lcd lcd;

const int colorR = 120;
const int colorG = 250;
const int colorB = 0;

int sys_state;
unsigned long wifi_timer;

void setup()
{
  // initialize serial for debugging 
  Serial.begin(115200);
  // initialize serial for ESP module 
  Serial1.begin(115200);
  // initialize ESP module 
  WiFi.init(&Serial1);
  // check for the presence of the shield 
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); // don't continue
  while (true);
  }

  // attempt to connect to WiFi network 
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: "); 
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("You're connected to the network"); 
  printWifiStatus();

  pinMode(RED_LIGHT, OUTPUT);
  pinMode(BLUE_LIGHT, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUTTON_PIN, INPUT);

  dht.begin();

  lcd.begin(16,2);
  lcd.setRGB(colorR, colorG, colorB);

  lcd.print("Lamplytics");
  
  sys_state = SYS_OFF;   
  wifi_timer = millis() + WIFI_MILLIS;

}

void loop() {

  if (millis() > wifi_timer){

    Serial.println();
    if (!client.connected()){
      Serial.println("Starting connection to server...");
      client.connect(server, 5000); 
    }
    
    Serial.println("Connected to server");
  
    temp = dht.readTemperature();
    dtostrf(temp, 4, 2, tempStr);
    deskTime += (millis() - deskTimestamp); 
    deskTime = deskTime / 1000;
    itoa(deskTime, deskTimeStr, 10);
  
    // Make a HTTP request
    sprintf(get_request,"GET /newData?temp=%s&deskTime=%s HTTP/1.1\r\nHost: 3.88.218.230\r\n\r\n\r\n", tempStr, deskTimeStr);
    client.print(get_request);
    delay(500);
    while (client.available()) {
      char c = client.read();
      Serial.write(c); 
    }

    wifi_timer = millis() + WIFI_MILLIS;  
    deskTimestamp = millis();
    deskTime = 0;
  }
  // clears the trig pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);   

  // Read the echo pin, which returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  distInch = duration * 0.0133/2;

  // If object detected within 30 inches of ultrasound sensor
  if (distInch <= 30){
    if (sys_state == SYS_OFF){
      deskTimestamp = millis();
      sys_state = SYS_ACTIVE;
    }

    else if (sys_state == SYS_STANDBY){
      if (digitalRead(BUTTON_PIN) == HIGH){
        sys_state = SYS_ACTIVE;
      }
    }
    else {  // sys_state = SYS_ACTIVE
      light = analogRead(LIGHT_SENSOR); 
      temp = dht.readTemperature();

      if (light >= maxLightThresh){
        lightVolt = 0;  
      }
      else if (light < maxLightThresh){
        lightVolt = map(light, minLightThresh, maxLightThresh, 255, 0);
      }
      
      if (temp > maxHotThresh){
        fanVolt = 255;
      }
      else if (temp >= minHotThresh && temp <= maxHotThresh){
        fanVolt = map(temp, minHotThresh, maxHotThresh, 0, 255);
      }
      else{
        fanVolt = 0;
      }
  
      lcd.setCursor(0,0);
      lcd.print("light: " + String(light));
      lcd.setCursor(0,1);
      lcd.print("temp: " + String(temp) + " C");  
      
      if (abs(lightVolt - currentLightVolt) >= lightThreshold){
        currentLightVolt = lightVolt;
        analogWrite(RED_LIGHT, lightVolt);  
        analogWrite(BLUE_LIGHT, lightVolt);     
//        Serial.print("voltage supplied to LEDs: ");
//        Serial.println(lightVolt);
      } 
//      Serial.print(" %, Temp: ");
//      Serial.print(temp);
//      Serial.println(" Celsius");      
      analogWrite(FAN_MOTOR, fanVolt);   
      if (digitalRead(BUTTON_PIN) == HIGH){
        analogWrite(RED_LIGHT, 0);
        analogWrite(BLUE_LIGHT, 0);
        analogWrite(FAN_MOTOR, 0);
        currentLightVolt = 0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Lamplytics");  
        sys_state = SYS_STANDBY;         
      }             
    }
  }
  else{
    if (sys_state == SYS_ACTIVE){
      analogWrite(RED_LIGHT, 0);
      analogWrite(BLUE_LIGHT, 0);
      analogWrite(FAN_MOTOR, 0);
      currentLightVolt = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Lamplytics");
      deskTime += (millis() - deskTimestamp);
      sys_state = SYS_OFF; 
      temp = 0.0;        
    }
    if (sys_state == SYS_STANDBY){
      deskTime += (millis() - deskTimestamp);     
      sys_state = SYS_OFF; 
      temp = 0.0;           
    }
  }

  delay(200);
 }

void printWifiStatus() {
  // print the SSID of the network you're attached to 
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP(); 
  Serial.print("IP Address: "); 
  Serial.println(ip);
  // print the received signal strength 
  long rssi = WiFi.RSSI(); 
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
