# Metal-detector-robot
<img src="https://upload.wikimedia.org/wikipedia/commons/5/55/Rocker_bogie.gif?raw=true" alt="drawing" width="300"/>

A metal detector robot built on the basis of the **ATmega2560** microcontroller. 
Equipped with a bluetooth **HC-05** module, in case of metal detection, 
transmits the world coordinates (robot receives them by GPS using **NEO-6M**) of the object via bluetooth to a receiver device, 
for example, a PC or Android device. The [rocker-bogie](https://en.wikipedia.org/wiki/Rocker-bogie) 
solution used as a mobile platform. The robot traverses the area represented by a rectangle on a plane 
using a [depth-first search](https://en.wikipedia.org/wiki/Depth-first_search) algorithm.
The rectangle is divided into cells, in each of which the robot looks in 4 directions and, 
depending on the readings of the distance sensor (**HC-SR04**), decides whether it is possible 
to go in one direction or another. Three **L298N** drivers used to control the motors.
In order for the robot to turn exactly at a certain angle, a high-precision optical encoder 
installed on one of the motors.
<p float="left">
  <img src="https://github.com/Astronomax/metal-detector-robot/blob/main/photos/IMG_20220707_1.jpg?raw=true" alt="drawing" width="300"/>
  <img src="https://github.com/Astronomax/metal-detector-robot/blob/main/photos/IMG_20220707_4.jpg?raw=true" alt="drawing" width="300"/>
</p>

### Circuit diagram
<p float="left">
  <img src="https://github.com/Astronomax/metal-detector-robot/blob/main/full_scheme.jpg?raw=true" alt="drawing" width="300"/>
  <img src="https://github.com/Astronomax/metal-detector-robot/blob/main/md_scheme.jpg?raw=true" alt="drawing" width="300"/>
</p>

Such a simple representation is enough to understand how everything wired. Everything powered in parallel. 
Specific pin numbers are not so important, but they can be viewed in the code if it is very necessary. 
The diagram of the metal detector taken from this 
[website](http://dzlsevilgeniuslair.blogspot.com/2013/07/diy-arduino-based-metal-detector.html).
There is also an optical encoder, and a display for displaying information about the objects found, 
but they are not shown in the diagram.
<p float="left">
  <img src="https://github.com/Astronomax/metal-detector-robot/blob/main/photos/IMG_20220707_6.jpg?raw=true" alt="drawing" width="350"/>
  <img src="https://github.com/Astronomax/metal-detector-robot/blob/main/photos/IMG_20220707_15.jpg?raw=true" alt="drawing" width="350"/>
</p>

### Dependencies
- [TinyGPS++](https://github.com/mikalhart/TinyGPSPlus) - NEO6M GPS data parser 
- [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library) - Display for duplicating information
- [Adafruit_PCD8544](https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library) - Display for duplicating information 
