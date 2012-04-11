USBPPM
=============

USBPPM is a firmware for AT90USB-Style Atmel AVRs.
The device firmware (which was developed on an AT90USBKey) takes rc-channel data via USB control- or interrupt-requests and produces a PPM signal on one pin.
The host side uses SDL to get data from a joystick or gamepad and updates the channel data via a USB interrupt request on the device side.

I developed this software to control my quadcopter without an expensive remote control.
It works, but it could be better. The usual gamepads just have crappy analog sticks which are just not as good as the ones used in a professional RC.

The PPM-code was taken from [Ulrich Radig][0].

The USB-code is provided by the [LUFA-Library by Dean Camera][1] 

Prerequisites
-----------
**Software:**

* AVR-GCC
* dfu-programmer
* libusb
* libSDL

**Hardware:**

* Gamepad
* A board with an AT90USB-AVR (e.g. AT90USBKey, TeensyUSB, etc.)
* A transmitter and receiver pair which transmit the PPM signal. I used the [Deltag][2] Tx-1 and Rx-32
* Your quadcopter or whatever you want to control

Build instructions
-----------
    git clone git://github.com/G33KatWork/USBPPM.git
    cd USBPPM
    git submodule update --init
    make -C host
    make -C device dfu

For the last step, of course, your AVR-Board needs to run a DFU-Bootloader.

Configuration
-----------
The host and device software has several configuration defines.

**Host:**

In main.c are several defines for the channel order, which axes are taken from your gamepad/joystick for what channel etc.
The host-configuration could be better, but I'm sure you'll get it. It's not much code.

**Device:**

In PPM.h are some defines for the PPM-Signal generation and the output-pin registers for the PPM-signal.
You can enable RS232-Debugging with 9600 baud/s 8,n,1 in Debug.h by uncommenting the DEBUG-define.

[0]: http://www.ulrichradig.de/home/index.php/avr/avr_-_rc
[1]: http://www.fourwalledcubicle.com/LUFA.php
[2]: http://deltang.co.uk/
