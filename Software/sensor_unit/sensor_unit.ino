//
// Adresses for: 
//  - Barometer   0x77 (119)
//  - Screen      0x3C (60)
//


#include <Wire.h>
#include <Arduino.h>        
#include <SFE_BMP180.h>         // barometer
#include <dht.h>                // humidity sensor

dht humiditySensor;

#define ALTITUDE 182.0          // Altitude of Chicago, IL in meters
#define visibilitySensorPin 3 
#define humiditySensorPin 7


// WiFi
#include <SoftwareSerial.h>  //Including the software serial library
#define DEBUG true
SoftwareSerial WiFiModule(10,11);

// screen
#include <U8glib.h>
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // I2C / TWI

// barometer
SFE_BMP180 barometer;
// devices adresses
const int screenAddr = 60, barometerAddr = 119; 
double  temperature = -1, 
        pressure = -1,
        myAltitude = -1, 
        visibility = -1, 
        humidity = -1;
int refSeconds = 0;


void setup() {
  Serial.begin(19200);
  Wire.begin();
  setupBarometer();
  setupWifi();
  setupScreen();
}


void loop() {
  collectData();
  redrawScreen();
  
  if (wifiIsConnected()) { 
    generateAndSendMsgWifi();
  }

  // showDebugInfo();
  delay(500);
}


int getSecondsElapsed() {
  return (millis()/1000) - refSeconds;
}


void redrawScreen() {
  u8g.firstPage();
  do {
      updateScreen();
  } while ( u8g.nextPage());
}


void showDebugInfo() {
  Serial.println("---------");
  Serial.println(temperature);
  Serial.println(humidity);
  Serial.println(pressure);
  Serial.println(visibility);  
}


void setupScreen() {
  u8g.setFont(u8g_font_unifont);
  // Instructs the display to draw with a pixel on
  u8g.setColorIndex(1); 
}


bool wifiIsConnected() {
  String ip = getIP();

  Serial.println("#########################################################");
  Serial.println("#########################################################");
  Serial.println(ip);  
  Serial.println("#########################################################");
  
  //check wifi status if there is no substring 
  //means its not connected 
  if (ip.indexOf("192.168.") != -1) {
    Serial.println("Returning true...");
    return true;
  }
  else {
    Serial.println("Returning false...");
    return false;
  }
}


void updateScreen() {  

  char temp[10];
  
  int val = temperature - 4;
  sprintf(temp,"%d", val);
  u8g.drawStr( 0, 26, "Temperature: ");
  u8g.drawStr( 100, 26, temp);
  u8g.drawStr( 120, 26, "C");

  val = visibility / 10;
  sprintf(temp,"%d", val);
  u8g.drawStr( 0, 38, "Visibility: ");
  u8g.drawStr( 100, 38, temp);
  u8g.drawStr( 120, 38, "%");

  val = humidity;
  sprintf(temp,"%d", val);
  u8g.drawStr( 0, 49, "Humidity: ");
  u8g.drawStr( 100, 49, temp);
  u8g.drawStr( 120, 49, "%");

  val = pressure;
  sprintf(temp,"%d", val);
  u8g.drawStr( 0, 60, "Pressure: ");
  u8g.drawStr( 70, 60, temp);
  u8g.drawStr( 105, 60, "hPa");
}


//
// findDevices()
//
// Function for debug purposes
// This function is not neccessary for program to work
// It just verifies if devices are connected and found 
// by the microcontroller
//
void findDevices() {
  Wire.beginTransmission(screenAddr);
  bool screenFound = (Wire.endTransmission() == 0);
  //screenFound ? line_01 = "OK." : line_01 = "Failed.";

  Wire.beginTransmission(barometerAddr);
  bool barometerFound = (Wire.endTransmission() == 0);
}


void collectData() {
  checkHumidity();
  checkTemperature();
  checkPressure();
  checkAltitude();
  checkVisibility();
}


void checkTemperature() {

  char status = barometer.startTemperature();

  if(status != 0) {
    // wait for measurment to complete
    delay(status);
  
    double t;
    // retrieve temperature measurement
    status = barometer.getTemperature(temperature);
    if ( status != 0) {
      // intentionally empty, all OK
    }
    else {
      //displayMessage("Temperature measure/nfailed");
    }
  }
}


void checkPressure() {
  // parameter in range 0 - 3,
  // 0 -> highest response, 3 -> longest wait
  char status = barometer.startPressure(3);
  if (status != 0){
    delay(status);    
    status = barometer.getPressure(pressure, temperature);
    if (status != 0) {
      // intentionally empty, all OK
    }
    else {
      //displayMessage("Pressure measure/nfailed");
    }
  }
}


void checkAltitude() {
  double baselinePressure = barometer.sealevel(pressure, ALTITUDE);
  myAltitude = barometer.altitude(pressure, baselinePressure);
}


void checkVisibility() {
  visibility = analogRead(visibilitySensorPin);
  Serial.println(analogRead(visibilitySensorPin));
}


void checkHumidity() {
  int chk = humiditySensor.read11(humiditySensorPin);
  humidity = (float)humiditySensor.humidity;
}


String getResponse(String command, String line, const int timeout) {
  String response = "";
  WiFiModule.print(command);
  long int time = millis();
  while((time + timeout) > millis()) {
    while(WiFiModule.available()) {
      char c = WiFiModule.read();
      response += c;
      if(c == '\n') {
        break;
      }
    }
    if(response.indexOf(line) != -1) {
      Serial.print(response);
      return response;
    }
  }
}


void generateAndSendMsgWifi() {

  String msg = String("?said=1&tmp=" + String(temperature-4) + "&prs=" 
    + String(pressure) + "&alt=" + String(myAltitude) + "&vis=" 
    + String(visibility) + "&hmd=" + String(humidity));

  sendMessage("upsa.php", msg);
}


String sendData(String command, const int timeout, boolean debug) {
  String response = getResponse(command, "OK", timeout);

  return response;
}


boolean connectToWifi(String SSID, String password) {
  sendData("AT+CWMODE=1\r\n",1000,false);
  String query = String("AT+CWJAP=\""+ SSID +"\",\"" + password + "\"\r\n");
  String response = sendData(query, 10000, false);
  if(response.length() > 0) {
    return true;
  } else {
    return false;
  }
}


String getIP() {
  String response = getResponse("AT+CIFSR\r\n", "OK", 1000);
  if(response.indexOf("STAIP")) {
    int ipBegin = response.indexOf("\"") + 1;
    int ipEnd = response.indexOf("\"", ipBegin);
    return response.substring(ipBegin, ipEnd);
  } else {
    return "";
  }
}


boolean sendMessage(String file, String params) {
  String httpHeader = String("GET /" + file + params + " HTTP/1.1\r\nHost: smarthome.local\r\nConnection: alive\r\n\r\n");
  sendData(String("AT+CIPSEND=" + String(httpHeader.length()) + "\r\n"), 2000, false);
  Serial.println(params);
  while(WiFiModule.available()) {
    WiFiModule.read();
  }
  String response = getResponse(httpHeader, "SEND OK", 10000);
}


String getMessage(String file, String params) {

  String httpHeader = String("GET /" + file + params + " HTTP/1.1\r\nHost: smarthome.local\r\nConnection: alive\r\n\r\n");
  sendData(String("AT+CIPSEND=" + String(httpHeader.length()) + "\r\n"), 2000, false);
  while(WiFiModule.available()) {
    WiFiModule.read();
  }
  String response = getResponse(httpHeader, "SEND OK", 10000);

  long int some = millis();
  String res ="";
  while((some + 5000) > millis()) {
    while(WiFiModule.available()) {
      char c = WiFiModule.read();
      res += c;
    }
    if(res.indexOf("SHCE") != -1) {

      return res.substring(res.indexOf("SHCS") + 5, res.length() - 5);
    }
  }
}


void setupWifi() {
  Serial.begin(19200);
  WiFiModule.begin(19200);
  while(WiFiModule.available()) {
      Serial.write(WiFiModule.read());
  }
  boolean connection = connectToWifi("SmartHome", "smarthome");
  if(connection) {
      String response = sendData("AT+CIPSTART=\"TCP\",\"smarthome.local\",80\r\n", 2000, false);
      Serial.print("Connected\n");
  }
}

