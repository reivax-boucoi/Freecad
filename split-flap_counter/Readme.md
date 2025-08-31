# Split flap counter project

This project started as a joke, to display the "number of consecutive days without incident", as we were blowing up chips regularly in the lab.

The counter consists of 2 digit wheels capable of displaying the numbers 00 through 99. (Given the frequency of incidents, a 3rd wheel wasn't considered).
Each day, the counter increments by 1. When a "happy little accident" occurs, simply insert a chip into the bin to cause the counter to reset in a satisfying motion.

This project was developped in a couple days, mixing 3D printing, electronics and minimal code.

## Hardware
![Full assembly iso view](/split-flap_counter/Mechanical/Pictures/Full_iso.png "Freecad view of the full assembly")
### Mechanicals

The hardware was designed using Freecad for the mechanical part.

It uses multimaterial printing for the digit flaps, and regular PLA prints for the rest. The 2 wheels consist of mirrored parts facing each other, the assembly providing overall support of the drums.
2 stepper motors are used: 28BYJ-48, which are dirt cheap and run of 5V directly. They have a resolution of 2048 steps per turn (in full step mode) and are driven at ~5RPM, i.e. ~12s to complete a revolution. Sliding lever microswitches provide zeroing on each wheel.  

The printable parts are located in *"split-flap_counter/Mechanical/3mF_exports_toPrint"* :
- 2x Drum
- 2x Drum_end
- 2x Frame (1 normal, 1 mirrored)
- 1x JointKey
- 1x JointShfat
- 2x sets of 5 dual sided flaps  

Additionnal hardware required:
- 2x 28-BYJ-48 5V stepper motors
- 4x M2x10 screws for the limit switches
- 4x M3x8 screws for the motor mount
- 2x limit switches: mine were salvaged from an old industrial grade printer, I don't have the reference, they are similar to Alps SSCTL10400.

### Electronics

Kicad 8.0 was used for the electronics (schematic and placement), although the circuit was manually assembled using dead-bug technique later on,because of it's simplicity.
The project is powered by a 5V supply (>500mA required), which could be ageneric USB wall charger.
The steppers are controlled using an ULN2003 Darlignton driver + an extra NPN since I reuqired 8 channels for the 2x4 stepper leads.

### Software

The software was developped in plain low level C and compiled using AVR-GCC and Avrdude. The MCU runs at 4MHz, and the program occupies ~ 500bytes of the 2kb of available program space.  
Resource allocation:
- TIMER0: triggers at ~400hz, used for stepper state machine
- TIMER1: triggers every 8s, used for timekeeping (daily trigger)
- INT0: "zero" limit switch for the unit wheel. Configured as falling edge interrupt to reset position.
- INT1: "zero" limit switch for the tens wheel. Configured as falling edge interrupt to reset position.
