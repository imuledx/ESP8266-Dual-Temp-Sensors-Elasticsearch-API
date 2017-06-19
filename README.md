# ESP8266-HVAC-Elasticsearch
Really this is just code to run two 

My set up is as follows: 
* Working ElasticSearch cluster
* Logstash listening for HTTP input from the ESP. Logstash basic conf file is in repo.
* Flashed ESP-01 using Arduino IDE
* Needs OneWire.h and Dallas Temperature.h


This can be used to have a single ESP-01 module connect to your wifi and report temperatures in F and C for two DS18B20 temperature sensors. I use one sensor at my HVAC intake and the otehr at the output. This will allow me to compare the two temperatures and make sure the system is performing right.