# EspDashButton
Esp8266 arduino project that uses the Amazon Dash button to trigger events on the esp8266. 

This code uses the ESP8266 as a Syslog sever to collect info from your home wifi router about when your amazon dash button establishes a connection to it. This is useful if you want to push the dash button and trigger some hardware or software event on the ESP8266. This could be used for other devices besides the dash button, all you need is its MAC address. Your router also needs to be able to support the Syslog protical but most routers do this. The ESP checks the packet reviced from the router with the MAC addresses of devices it knows. You can send Syslogs from multiple routers that are in range.

I created this project as a solution to Amazons newest version of the dash button not brodcasting ARP probes which were used in the first wave of dash button hacks. I also did this because I wanted some way for the dash button to communicate with the Esp8266 so I could easily use it with microcontroller projects. 
