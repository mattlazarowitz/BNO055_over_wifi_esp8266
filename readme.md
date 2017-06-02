This is a really simple program to use a BNO055 orientation sensor from Adafruit on an ESP8266 using the Arduino framework and send the sensor data as text over wifi.
This is in an early stage now and is here to ensure this sketch does not get lost. There i much room for improvement in the code.
The ultimate goal is to create a wireless orientation sensor.
Changing the server type so the data can be sent raw is the first thing I wish to change.
Using generic UDP over a regular socket also seems like something I should do.
Any further ambitions for this project can wait until I get that far and have a client program to read the data.

This is based largely on example code from AdaFruit and code found in this post on the ESP8266 forums: http://www.esp8266.com/viewtopic.php?p=36531