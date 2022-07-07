# Metal-detector-robot
A metal detector robot built on the basis of the **ATmega2560** microcontroller. 
Equipped with a bluetooth **HC-05** module, in case of metal detection, 
transmits the world coordinates (the robot receives them by GPS using **NEO-6M**) of the object via bluetooth to a receiver device, 
for example, a PC or Android device. The [rocker-bogie](https://en.wikipedia.org/wiki/Rocker-bogie) 
solution used as a mobile platform. The robot traverses the area represented by a rectangle on a plane 
using a [depth-first search](https://en.wikipedia.org/wiki/Depth-first_search) algorithm.
The rectangle is divided into cells, in each of which the robot looks in 4 directions and, 
depending on the readings of the distance sensor (**HC-SR04**), decides whether it is possible 
to go in one direction or another. Three **L298N** drivers are used to control the motors.
In order for the robot to turn exactly at a certain angle, a high-precision optical encoder 
installed on one of the motors.
<p float="left">
  <img src="https://github.com/Astronomax/metal-detector-robot/blob/main/photos/IMG_20220707_1.jpg?raw=true" alt="drawing" width="300"/>
  <img src="https://github.com/Astronomax/metal-detector-robot/blob/main/photos/IMG_20220707_4.jpg?raw=true" alt="drawing" width="300"/>
  <img src="https://upload.wikimedia.org/wikipedia/commons/5/55/Rocker_bogie.gif?raw=true" alt="drawing" width="300"/>
</p>

### Circuit diagram
<p float="left">
  <img src="https://github.com/Astronomax/metal-detector-robot/blob/main/full_scheme.jpg?raw=true" alt="drawing" width="350"/>
  <img src="https://github.com/Astronomax/metal-detector-robot/blob/main/md_scheme.jpg?raw=true" alt="drawing" width="350"/>
</p>

Such a simple representation is enough to understand how everything wired. Everything is powered in parallel. 
Specific pin numbers are not so important, but they can be viewed in the code if it is very necessary. 
The diagram of the metal detector was taken from this 
[website](http://dzlsevilgeniuslair.blogspot.com/2013/07/diy-arduino-based-metal-detector.html).
