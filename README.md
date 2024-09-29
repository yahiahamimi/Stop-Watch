# Mini Project 2 - Stop Watch

This project is a simple timer that counts up in seconds, minutes and hours. The timer is implemented using interrupts and the AVR's timer/counter1 (TC1) module.

## Features:

- Counts up in seconds, minutes and hours
- Uses interrupts to update the timer at regular intervals
- Uses the AVR's timer/counter1 (TC1) module to generate the interrupts
- Displays the time in the format "HH:MM:SS"
- supports time stop and reset

## How it works:

1. The timer/counter1 (TC1) module is configured to generate an interrupt every second.
2. When the interrupt is triggered, the interrupt service routine (ISR) is called.
3. The ISR updates the timer by incrementing the seconds, minutes and hours as necessary.
4. The timer is then displayed on the LCD display in the format "HH:MM:SS".


