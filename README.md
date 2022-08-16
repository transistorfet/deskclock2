
Deskclock 2
===========

###### *Started July 31, 2022*

A desktop companion device for displaying status and controlling things without interrupting flow.
This is a replacement to my previous deskclock project which was originally built in 2009 and later
revamped with an Arduino in 2012.  It has an LCD screen, buttons, a temperature sensor, an IR
receiver, and a USB connection to my desktop computer.  The computer is running
[Nerve](http://jabberwocky.ca/projects/nerve/), which can perform actions in response to the
buttons, and display information like the current time, current song playing on my computer, and
indoor and outdoor temperature.  The buttons are currently configured to control the music player on
my computer, as well as toggle some lights in my computer room.

![alt text](images/prototype.jpg "Prototype using AdaFruit NeoKeys and 2.8 inch LCD")

Above is the prototype, with the old deskclock to be replaced visible in the background

The prototype is built using an AdaFruit
[Feather STM32F405 Express](https://www.adafruit.com/product/4382) with two
[NeoKeys](https://www.adafruit.com/product/4980) and a
[2.8 inch LCD](https://www.adafruit.com/product/2090).

