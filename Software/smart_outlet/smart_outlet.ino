#include <U8glib.h>
#include "SoftwareSerial.h"


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // I2C / TWI

#define DEBUG true
#define RELAY 3

SoftwareSerial WiFiModule(10, 11);

String getResponse(String command, String line, const int timeout) {
  String response = "";
  WiFiModule.print(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (WiFiModule.available()) {
      char c = WiFiModule.read();
      response += c;
      if (c == '\n') {
        break;
      }
    }
    if (response.indexOf(line) != -1) {
      //Serial.print(response);
      return response;
    }
  }
}

String sendData(String command, const int timeout, boolean debug) {
  String response = getResponse(command, "OK", timeout);

  return response;
}

boolean conntectToWifi(String SSID, String password) {
  sendData("AT+CWMODE=1\r\n", 1000, false);
  String query = String("AT+CWJAP=\"" + SSID + "\",\"" + password + "\"\r\n");
  String response = sendData(query, 10000, false);
  if (response.length() > 0) {
    return true;
  } else {
    return false;
  }
}

//get ip from network 
String getIP() {
  String response = getResponse("AT+CIFSR\r\n", "OK", 1000);
  if (response.indexOf("STAIP")) {
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
  while (WiFiModule.available()) {
    WiFiModule.read();
  }
  String response = getResponse(httpHeader, "SEND OK", 10000);
}

String getMessage(String file, String params) {

  String httpHeader = String("GET /" + file + params + " HTTP/1.1\r\nHost: smarthome.local\r\nConnection: alive\r\n\r\n");
  sendData(String("AT+CIPSEND=" + String(httpHeader.length()) + "\r\n"), 2000, false);
  while (WiFiModule.available()) {
    WiFiModule.read();
  }
  String response = getResponse(httpHeader, "SEND OK", 10000);

  long int some = millis();
  String res = "";
  while ((some + 5000) > millis()) {
    while (WiFiModule.available()) {
      char c = WiFiModule.read();
      res += c;
    }
    if (res.indexOf("SHCE") != -1) {

      return res.substring(res.indexOf("SHCS") + 5, res.length() - 5);
    }
  }
}

//setup loop for evry device 
void setup() {
  Serial.begin(19200);
  WiFiModule.begin(19200);
  u8g.setFont(u8g_font_unifont);
  u8g.setColorIndex(1); // Instructs the display to draw with a pixel on.
  pinMode(RELAY, OUTPUT);
  tryConnect();

}


void loop() {
  String contain = "192.168";
  String test = getIP();

  //if connection was lost try to reconnect to the network 
  if (test.indexOf(contain) != -1) {
    Serial.println("Connection OK");
  }
  else {
    tryConnect();
  }

  //display on the screen 
  u8g.firstPage();
  String command = getMessage("adruino_power.php", "");

  //get variable from network if was turned on or off 
  int switchState = atoi(command.substring(8, command.length()).c_str());
  Serial.print(switchState);


  if (switchState) {
    //update info on the screen
    do {
      printScreen(switchState);
    } while ( u8g.nextPage() );

    //turn of relay
    digitalWrite(RELAY, HIGH);

  } else {
    //update info on the screen
    do {
      printScreen(switchState);
    } while ( u8g.nextPage() );


    //change turn off relay
    digitalWrite(RELAY, LOW);

  }

  //print in serial monitor 
  Serial.println("\n test " + test);
  Serial.println ("\n" + getIP());
  
  delay(500);

}

//connect to wifi and print in serial monitor status 
void tryConnect() {
  while (WiFiModule.available()) {
    Serial.write(WiFiModule.read());
  }
  boolean connection = conntectToWifi("SmartHome", "smarthome");
  if (connection) {
    String response = sendData("AT+CIPSTART=\"TCP\",\"smarthome.local\",80\r\n", 2000, false);
    Serial.print("Connected\n");
  }
}

//print info on the screen about wifi 
//ip, status and if relay is on or not 
void printScreen(int switchState) {

  //obtain IP adress 
  String wifi = getIP();

  //check if switch is turned on or off 
  if (switchState == 1) {
    u8g.drawStr( 0, 10, "Relay: ON");
  }
  else {
    u8g.drawStr( 0, 10, "Relay: OFF");
  }

  //check wifi status if there is no substring 
  //means its not connected 
  if (wifi.indexOf("192.168.") != -1) {
    u8g.drawStr( 0, 28, "WiFi: Connected");
    u8g.drawStr( 0, 42, wifi.c_str());
    u8g.drawStr( 0, 58, "Status: OK");
  }

  //disconnected 
  else {
    u8g.drawStr( 0, 28, "WiFi: NO WiFi");
    u8g.drawStr( 0, 42, wifi.c_str());
    u8g.drawStr( 0, 58, "Status: N/A");
  }
}
//end print screen function 

//------------------------------------------------------------------



