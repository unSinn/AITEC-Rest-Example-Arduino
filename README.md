AITEC-Rest-Example-Arduino
==========================

Description
-----------

Arduino Sketch using the Arduino Rest Server (https://github.com/julioterra/Arduino_Rest_Server/wiki) Grove Light Sensor and Number Display.

### GET Value
You can read the Light Sensors value by HTTP GET the JSON containing the value:

   curl http://192.168.2.200/json/output_light
   
### POST Value
You can set the value on the Number Display by a HTTP POST:
   curl -i -X POST -d 'input_numberdisplay=1234' http://192.168.2.200/input_numberdisplay

Arduino Setup
-------------
On your Grove Shield connect the Light Sensor to Port A0 and the Number Display to Port D2.
