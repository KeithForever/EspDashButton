# EspDashButton
Esp8266 arduino project that uses the Amazon Dash button to trigger events on the esp8266. 

This code uses the ESP8266 as a Syslog sever to collect info from your home wifi router about when your amazon dash button establishes a connection to it. This is useful if you want to push the dash button and trigger some hardware or software event on the ESP8266. This could be used for other devices besides the dash button, all you need is its MAC address. Your router also needs to be able to support the Syslog protical but most routers do this. The ESP checks the packet reviced from the router with the MAC addresses of devices it knows. You can send Syslogs from multiple routers that are in range.

I created this project as a solution to Amazons newest version of the dash button not brodcasting ARP probes which were used in the first wave of dash button hacks. I also did this because I wanted some way for the dash button to communicate with the Esp8266 so I could easily use it with microcontroller projects. 

# Setup
Step one:
Setup a wireless router that is not connected to the internet. This will allow the Dash button to connect to it but will prevent the Dash button from buying any amazon products.
  
Step two: 
Use the amazon app on your phone to connect the dash button to the wireless network you just setup. When you get to the last step when it asks you to select a product, dont select a product. You can if you really want to since the router isnt connected to the internet. I just do this just to make sure I dont get amazon products shipped to my door randomly. 
  
Step three: 
Flash the arduino file to the esp8266. Edit the code so it contains your dash button mac adress and the name and password for your wireless network you setup in step one. I have a small i2c display connected to the esp so I can see what's going on.
  
Step four:
Setup syslog on your router from step one. For this you will need the IP address of your esp8266. Once this is setup your esp8266 should be alerted when your Dash button is online. I realize comparing mac addresses probably isnt the best way to do this, it was just the easiest to do at the time.
