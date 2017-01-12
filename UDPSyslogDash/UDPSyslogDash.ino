#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <OLEDDisplay.h>
#include <OLEDDisplayFonts.h>
#include <OLEDDisplayUi.h>
#include <SH1106.h>
#include <SH1106Brzo.h>
#include <SH1106Spi.h>
#include <SH1106Wire.h>
#include <SSD1306.h>
#include <SSD1306Brzo.h>
#include <SSD1306Spi.h>
#include <SSD1306Wire.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "WiFiUDP.h"

/*  
 * Matt Deeds, 2016
 * This code uses the ESP8266 as a Syslog sever to collect info from your home wifi router 
 * about when your amazon dash button establishes a connection to it. This is useful if 
 * you want to push the dash button and trigger some hardware or software event on
 * the ESP8266. This could be used for other devices besides the dash button, all you need is
 * its MAC address. Your router also needs to be able to support the Syslog protical but most
 * routers do this. The ESP checks the packet reviced from the router with the MAC
 * addresses of devices it knows. You can send Syslogs from multiple routers that are in range.
 * 
 */

/////////////////
//MAC ADDRESSES//
/////////////////
byte macBat[12] = {53,48,70,53,68,65,68,48,50,55,65,67}; // MAC Address of the amazon batteries dash button 

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 5; // Thing's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read
#define FIRST   3
#define NUMPIXELS   1

/////////////
//Variables//
/////////////
const char AP_MI[] = "ESP1EB"; // set this to your wifi name
bool rec = false;
int delayval = 500; // delay for half a second

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;
byte packetBuffer[512];
unsigned int localPort = 514;
bool correct = false;

// Initalize display
SSD1306  display(0x3c, D3, D4);

// Initalize NeoPixels for a specific pin
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, FIRST, NEO_GRB + NEO_KHZ800);

// Initalize Server
WiFiServer server(80);

void setup() 
{
  initHardware();
  setupWiFi();
  server.begin();
  pixels.begin(); // This initializes the NeoPixel library.

  // Listen to packets on syslog port 514
  udp.begin(localPort);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.display();
}

void loop() 
{
  // Blink the onboard LED to make sure everything is looping and no hangups 
  digitalWrite(LED_BUILTIN, HIGH); // led off
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW); // led on
  pixels.setPixelColor(1, pixels.Color(0,150,0));
  pixels.show();
  delay(2000);

  // Check for Dash button
  // Call this for different for each device you are looking for
  rec = udpListen( macBat );
  
  // Set display text alignment and font  
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  
  // Display Local Ip
  String lip = WiFi.localIP().toString();
  display.drawString(64, 0, "Esp IP: " + lip);
  
  // Display Gateway IP (IP of the router)
  String gip = WiFi.gatewayIP().toString();
  String hName = WiFi.hostname(); 
  display.drawString(64, 15, "Gateway: " + gip);

  // Lights up led to green if macBat MAC is found
  check(0,150,0);

  // Send all text to display and then clear it as a refresh
  display.display();
  display.clear();
}

// Open udp port and listen for syslog info
bool udpListen( byte arMAC[]) {

  int noBytes = udp.parsePacket();
  String received_command = "";

  if ( noBytes ) {
    Serial.print(millis() / 1000);
    Serial.print(":Packet of ");
    Serial.print(noBytes);
    Serial.print(" received from ");
    Serial.print(udp.remoteIP());
    Serial.print(":");
    Serial.println(udp.remotePort());
    // We've received a packet, read the data from it
    udp.read(packetBuffer,noBytes); // read the packet into the buffer

    //byte BatteriesArray[512] = {60,49,49,48,62,70,114,105,32,78,111,118,32,49,52,32,49,51,58,52,51,58,51,54,32,50,48,49,52,32,68,45,76,105,110,107,32,83,121,115,116,101,109,115,32,68,73,82,45,54,53,53,32,83,121,115,116,101,109,32,76,111,103,58,32,69,83,80,49,69,66,58,32,87,105,114,101,108,101,115,115,32,115,121,115,116,101,109,32,119,105,116,104,32,77,65,67,32,97,100,100,114,101,115,115,32,53,48,70,53,68,65,68,48,50,55,65,67,32,97,115,115,111,99,105,97,116,101,100};

    // Chech if the macBat array is a subset of packet buffer
    // is the connection request comming from the right device?
    // if so, make the esp do something cool
    if ( isSubset(packetBuffer, arMAC, 512, 12) != 0 ){
      Serial.print("Device Found! Performing Action");
      Serial.println(" ");
      bool rec = true;
       
    } else {
      Serial.print("Device not found");
      Serial.println(" ");
      bool rec = false;
    }

    // display the packet contents in HEX or decimal in serial console
    // you can use this to get the MAC of whatever device you are trying to monitor 
    for (int i=1;i<=noBytes;i++)
    {
      //Serial.print(packetBuffer[i-1],HEX);
      Serial.print(packetBuffer[i-1]);
      received_command = received_command + char(packetBuffer[i - 1]);
      if (i % 32 == 0)
      {
        Serial.println();
      }
      else Serial.print(' ');
    } // end for
    Serial.println();
    return rec;
  }
}

// check if byte array of syslog info contains the byte array of dash button mac address
int isSubset(byte arr1[], byte arr2[], int m, int n)
{
    int i = 0;
    int j = 0;
    for (i=0; i<n; i++)
    {
        for (j = 0; j<m; j++)
        {
           if(arr2[i] == arr1[j])
              break;
        }

        /* If the above inner loop was not broken at all then
           arr2[i] is not present in arr1[] */
        if (j == m)
           return 0;
    }

    /* If we reach here then all elements of arr2[] 
      are present in arr1[] */
    return 1;
}

void check(int r, int g, int b){
  
  // Check if button press has been detected 
  if (rec == true ){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(1, pixels.Color(r,g,b));
    pixels.show();
  } else if (rec == false){
    pixels.setPixelColor(1, pixels.Color(0,0,0));
    pixels.show();
  }
}
// setup Wifi access point
void setupWiFi() {
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_MI, NULL);
}

// Initalize Hardware
void initHardware() {
  
  Serial.begin(115200);
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(LED_BUILTIN, OUTPUT);
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}

