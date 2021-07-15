# Arduino Money Box

## [Click here](https://www.youtube.com/watch?v=M8Lv9_ZU4I0) for a video demonstration.

[money box](https://github.com/tobyjbrown/arduino-money-box/aerial-view.png)

This is a project built using the Arduino Nano, a series of photo-interrupter units, an LCD screen, a servo motor and some pushbuttons. The goal for this project was to take a simple 'money box' idea and build it from scratch using the tools I already had available to me (hence the shoebox and hardware mess!). 

The system utilises the 'EEPROM' library to store the user's pin number and balance across on/off power cycles. The pushbuttons work via a simple trick learned from Paul Mcwhorter's Youtube videos which, instead of using the buttons as pull-up or pull-down resistors, the buttons are connected to pins on the Arduino that, although having their mode set to digital inputs, are written to HIGH so that pressing down on the button connects that HIGH to ground, causing the input on that pin to read 0 instead of 1 (this trick is useful in its conservative use of jumper wires and connections). The program is always reading from the buttons and compares each read value to the one in the previous loop, changing the program state if there is a difference between the two (this is imperative to the buttons responding as expected to user input, irrespective of the duration the user holds the button down for).

I prototyped the loose system first using an Arduino Uno, before transferring it to the shoebox and soldering together the nano. In the future, I aim to incorporate CAD and 3D printing to produce more secure mechanical housings.

I would love to hear any feedback on how to improve the system.

Thank you for viewing my project.