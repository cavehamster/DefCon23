# Atmel AVR

The other processor on your badge is an Atmel AVR.  It handles the interface to most of the analog stuff: joysticks, buttons, battery...

Included here is Mike's code for the AVR as delivered at defcon, with my fixes applied to resolve a couple of bugs.  There's probably a cleaner way of organizing Ardunio code.

# Programming

To program your chip, open the .ino in the Arduino sketchbook, and use 'Arduino Duemilanove or Diecimila' as the board, with whatever port it show up as on your machine.  On your badge, slide the switch on the bottom of the badge to the right (as viewed from the front, away from the USB port).  You can then compile and upload the sketch to the chip.  If you get an error that the port cannot be opened, make sure you close the terminal in Propeller IDE so it's not blocking it.

-hamster