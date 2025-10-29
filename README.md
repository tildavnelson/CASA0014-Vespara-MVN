# Neith - Vespara's almost moon

Neith was a hypothetical moon of Venus that was reported by several astronomers between 1672 and 1770, but is now known not to exist. Sightings were likely caused by optical illusions, stars near Venus, or misidentified objects.
While prototyping the Internet of Things - I created Neith, an interactive Arduino Prototype controller for the LED light installation [Vespara](https://github.com/ucl-casa-ce/casa0014/tree/main/vespera), using an ultrasonic distance sensor.

# Contents of repository
- Source code for Neith as well as the version that lead up to its completion.
- List of components and tools
- Concept behind the design
- Instructions to reproduce

  # What does Neith do?
  
Neith is a hanging sphere that can be raised or lowered using a pulley and another sphere. Underneath sits an ultrasonic distance sensor that sends out a sound signal, waits for the echo, and measures how long it takes to return to calculate the distance to the ground below. The proximity of Neith to the ground controls the lights on Vespera, the closer to Vespara the warmer the closer to the ground the cooler.

 # What's Neith made from?
 
**Contents**
**Hardware:**
- Vespara light installation: Vespera is a WiFi-enabled luminaire that can be controlled via MQTT messages.
- Arduino MKR1010 microcontroller
- Ultrasonic Distance Sensor HC-SR04

**Software:**
- Arduino IDE

**Library:**
- SPI
- Adafruit_NeoPixel
- WiFiNINA
- PubSubClient
- arduino_secrets
- utility/wifi_drv

**Craft Materials**
- Papier mache spheres
- string
- hooks

**Overview**

This system enables the Vespera light installation to respond to movement and proximity using the ultrasonic distance sensor HC-SR04. 

The sensor continuously measures distance at determined intervals and my code translates this data into determined RGB values corresponding to set distances.

The RGB values are sent to Vespara via Wifi connecting to MQTT broker mqtt.cetools.org where it's published on a specific channel student/CASA0014/light/19 allowing the sharing of data to be wireless.

The Vespera light, powered by an Arduino MKR1010 is made up of a strip of 72 NeoPixel LEDs.

By lifting Neith further away from or dropping it closer to the ground the LED display results in a ripple animation that reflects the viewer’s interaction with the installation, the proximity of Neithe to the groun and to Vespara and the narrative of their dynamic.


**Concept**

 "Vespera is a Latin word for "evening", derived from vesper, which also refers to the evening star, the planet Venus as it appears in the twilight sky. The light represents the passage of time and the transition from day to night, the moment when a single point of light becomes visible against the fading horizon."

<img width="1912" height="1352" alt="Scanned from a Xerox Multifunction Printer" src="https://github.com/user-attachments/assets/55fe0fae-23e0-4b93-946a-060d5e1ae35b" />



I was inspired by the wording of this text in the making of my controller device for Vespara, taking from the planetary system, and the 'twilight sky'. I wanted to make a controller that would allow Vespara to be seen as a planet or a Sun. I also wanted the person interacting with the controller and Vespara to be in movement, physically involved in the light changes. Moving the planets and moons closer to the sun.
  
<img width="1039" height="666" alt="Screenshot 2025-10-28 at 19 51 38" src="https://github.com/user-attachments/assets/92517b06-123b-4c61-84cc-2ea55833f660" />


**Procedure & instructions to reproduce**

- Install libraries mentioned
- Create an arduino_secrets.h file to confidentially connect to wifi and MQTT.
- Set up the HC-SR04 Ultrasonic Distance Sensor and got it reliably measuring distances.
- Using: the sensor, 4 male to female wires and the micro USB cable.
  Follow this Youtube video for reference: https://www.youtube.com/watch?v=n-gJ00GTsNg&t=122s
  
- Connect the Arduino to the MQTT broker and subscribe to specific given channel.

- Wrote code to convert distance readings into RGB colour values, using 10 cm intervals across a total height range of 2 m.

- Group the LEDs so they could be controlled in sections rather than individually.

- Add a ripple effect animation that lights up each group in sequence to make the colour changes more dynamic.

- Continuously test and debug the setup, adjusting the animations and code as the project develops.

<img width="1754" height="1241" alt="Scanned from a Xerox Multifunction Printer (5)" src="https://github.com/user-attachments/assets/7c5ab8d5-a006-42bd-921c-8609aa08c542" />


