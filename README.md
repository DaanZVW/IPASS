# IPASS MCBELIB Snake controller
This is an project for the HU HBO-ICT-TI in the Netherlands in Utrecht.
The library contains a game (snake) and code for a display and controller.

This project was made by Daan Zimmerman van Woesik in 2019

___           
## Where is MCBELIB used for?
MCBELIB stands for Motion Controlled Bluetooth Enabled LIBrary, this means that the controller, which is used, contains a bluetooth module (**HC-05**) and a gyroscope (**GY-521**). The game will be controlled via the gyroscope and the data will be send over via bluetooth. This will then be used on the display so that you can play snake anywhere in a rather big radius.

### How to use it:
MCBELIB is quite easy to use. When you get the controller and display unit without an program on it, No problem.
Though there are some things for you to install, **Hwlib** is a library you need to install.

Go to this github page (https://github.com/wovo/installers) to download **Hwlib** and **Bmptk** which are both needed to use MCBELIB

After the installation of Hwlib and Bmptk you can upload the controller code to the controller and the display code to the display.
And that is it! You should be all up and running playing snake. Keep in mind that the connection might be considerd slow.

If you want to know more about MCBELIB, than acces the directory where the Display an Controller code lies and than do:

**doxygen configDoxy**

This will create a documentation for you to explore and find out more about MCBELIB's capabilitys.

### What do you need:

* Arduino Due
* Arduino Nano
* 1/2 GY521 chips (2 chips will enable you to play it without bluetooth)
* 2 HC05 bluetooth chips
