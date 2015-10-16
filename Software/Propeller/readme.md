# Parallax Propeller

The main processor on your badge is the Parallax Propeller.  It's a cute little 8 core 32 bit microcontroller.

The code here was written in C using the [Parallax SimpleIDE](http://learn.parallax.com/propeller-c-set-simpleide).  Parallax also has a great tutorial there in getting started.

Why C?  The Propeller was designed for a special programming language called Spin, but I just like programming in C a little better.  Fortunately, there is a nice GCC for the Propeller that abstracts most of the strangeness of the Propeller from you and you can just get down and dirty with code.

What's new here?  My branch of the Parallax code is written in C instead of Spin.  It's a ground up re-write, and as such, is slightly different than what you got on your badge.  Currently, it doesn't have all the LED patterns you got at defcon just yet, but it supports the LCD backlight.  I plan to fully support the hardware on the badge once I've figured it all out, so, stay tuned!

#Programming

See the [Programming Notes](../../programmingnotes.md)

Basically, install the SimpleIDE above, open the .side file, under Tools->Properties->General set the Reset signal to RTS, slide the switch on the bottom of your badge to the left (towards the USB port), and select the proper port.  Then run with terminal if you'd like to see a debug spew, or run it out of RAM if you just want it run for a little while, or upload to EEPROM if you want to keep the code forever.

-hamster