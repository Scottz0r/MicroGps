# MicroGps

NMEA GPS module for embedded devices. This project contains a C++ class that collects NMEA formatted GPS messages.

Items in the `scottz0r::_detail` namespace are not for public consumption are may be changed in the future!

## Example Arduino Sketch

```c++
#include <MicroGps.h>

using namespace scottz0r;

MicroGps gps;

void setup()
{
    Serial.begin(57600);
    Serial1.begin(9600); // GPS Serial device.
}

void loop()
{
    while(Serial1.available())
    {
        char c = Serial1.read();
        if(gps.process(c) && gps.good())
        {
            if(gps.message_type() == MicroGps::MessageType::GPGGA)
            {
                // Do something with gps.position_data()
                handle_gpgga();
            }
        }
    }
}

void handle_gpgga()
{
    Serial.print("GPGGA-> ");
    if(gps.position_data().fix_quality == 0)
    {
        Serial.println("No fix (quality is 0)");
        return;
    }

    Serial.print("Lat: ");
    Serial.print(gps.position_data().latitude);
    
    Serial.print(", Lon: ");
    Serial.print(gps.position_data().longitude);

    Serial.print(", #Sat: ");
    Serial.print(gps.position_data().number_satellites);

    Serial.print(", HDOP: ");
    Serial.print(gps.position_data().horizontal_dilution);

    Serial.print(", Alt: ");
    Serial.print(gps.position_data().altitude_msl);

    Serial.print(", Geoid: ");
    Serial.print(gps.position_data().geoid_height);

    Serial.println();
}

```
