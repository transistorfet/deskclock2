
DeskClock 2
===========

###### *Started July 31, 2022*

A desktop companion device for displaying status and controlling things without interrupting flow.
This is a replacement to my previous deskclock project which was originally built in 2009 and later
revamped with an Arduino in 2012.  It has an LCD screen, buttons, a temperature sensor, an IR
receiver, and a USB connection to my desktop computer.  The controller is a 
[Adafruit Feather STM32F405 Express](https://learn.adafruit.com/adafruit-stm32f405-feather-express)
programmed using the Arudino framework.  The computer is running
[Nerve](http://jabberwocky.ca/projects/nerve/), which can perform actions in response to the
buttons, and display information like the current time, current song playing on my computer, and
indoor and outdoor temperature.  The buttons are currently configured to control the music player
on my computer, as well as toggle some lights in my computer room.


Custom PCB
----------

![alt text](images/board.jpg "Custom PCB in place")

The custom PCB assembled, showing the current time, current artist and song playing in xmms2, and
the indoor and outdoor temperatures.  It was designed in November 2022, and assembled in December.

![alt text](images/board-no-lcd.jpg "Custom PCB top view without the LCD showing the components")

![alt text](images/board-bottom.jpg "Custom PCB bottom view showing the Kali solderless keyboard button sockets")

On the bottom, you can see the Kali solderless keyboard button sockets, which are compatible with
CherryMX switches.  They were surprisingly easy to install, align, and solder, using a pre-made
KiCAD footprint for those sockets.  I also stuck some rubber feet to the bottom to clear the
sockets from touching the desk, and also keeping it in place when pressing the buttons.

![alt text](images/board-under-monitors.jpg "Custom PCB with parts assembled, showing the Python program's log messages on the monitor above")

The custom PCB with parts assembled, and the LCD floating. On the monitor above, you can see the
Python program's log messages showing it receiving the outdoor temperature and sending it to
DeskClock2, and querying the current song to be displayed.


Prototype
---------

![alt text](images/prototype.jpg "Prototype using AdaFruit NeoKeys and 2.8 inch LCD")

Above is the prototype, with the old deskclock to be replaced visible in the background

The prototype is built using an AdaFruit
[Feather STM32F405 Express](https://learn.adafruit.com/adafruit-stm32f405-feather-express) with two
[NeoKeys](https://www.adafruit.com/product/4980) and a
[2.8 inch LCD](https://www.adafruit.com/product/2090).


Programming with Arduino
------------------------

In the Arduino IDE:
- select board "Generic STM32F4 Serios"
- select board part number "Adafruit Feather STM32F405"
- select USB support "CDC (generic 'Serial' supersede USART)
- select upload method "STM32CubeProgrammer (DFU)"

