# Split flap counter project

This project started as a joke, to display the "number of consecutive days without incident", as we were blowing up chips regularly in the lab.

The counter consists of 2 digit wheels capable of displaying the numbers 00 through 99. (Given the frequency of incidents, a 3rd wheel wasn't considered).
Each day, the counter increments by 1. When a "happy little accident" occurs, simply insert a chip into the bin to cause the counter to reset in a satisfying motion.

This project was developped in a couple days, mixing 3D printing, electronics and minimal code.

## Hardware
![Full assembly iso view](/split-flap_counter/Mechanical/Pictures/Full_iso.png "Freecad view of the full assembly")
### Mechanicals

The hardware was designed using Freecad 0.9 for the mechanical part. 
It uses multimaterial printing for the digit flaps, and regular PLA prints for the rest. The 2 wheels consist of mirrored parts facing each other, the assembly providing overall support of the drums.
The base structure was inspired by [Thomas H 3D](https://www.printables.com/model/69603-split-flap-counter-fully-printable) project, his being a single digit purely manual version. The flaps were customized for multi-material printing by [0Celta](https://www.printables.com/model/133058-multi-material-flaps-for-split-flap-counter).  

A ready made printable of the structure can be found [here](/Mechanical/3mF_exports_toPrint/full_plate.3mF).  
The individual printable parts are located in [/Mechanical/3mF_exports_toPrint/](/Mechanical/3mF_exports_toPrint/):
- 2x **Drum**
- 2x **Drum_end**
- 2x **Frame** (1 normal, 1 mirrored)
- 1x **JointKey**
- 1x **JointShaft**
- 2x **Flaps** (sets of 5 dual sided flaps)  

![Half iso view](/split-flap_counter/Mechanical/Pictures/Half_iso.png "Freecad view of the half assembly with flaps")

Additionnal hardware required:
- 2x **28-BYJ-48** 5V stepper motors
- 4x **M2x10** screws for the limit switches
- 4x **M3x8** screws for the motor mount
- 2x limit switches: mine were salvaged from an old industrial grade printer, I don't have the reference, they are similar-ish to Alps SSCTL10400.  

2 ventilation valve stepper motors are used, which are dirt cheap and run of 5V directly. They have a resolution of approx. 2048 steps per turn (in full step mode, not exact !) which is plenty for our application. One drawback is that they are quite slow: I found 5RPM to be a reliable value, i.e. ~12s to complete a revolution. Sliding lever microswitches provide zeroing on each wheel.  

### Assembly

![Half exploded iso view](/split-flap_counter/Mechanical/Pictures/Half_exploded_iso.png "Exploded view of the half assembly")

### Electronics

Kicad 8.0 was used for the electronics (schematic and placement), although the circuit was manually assembled using dead-bug technique later on, because of it's simplicity. Nonetheless, a [schematic](/split-flap_counter/Electronics/split_flap_driver/schematic.pdf) and example [layout](/split-flap_counter/Electronics/split_flap_driver/) are available for reference.  

The project is powered by a 5V supply (>500mA required), which could be a generic USB wall charger.
The steppers are controlled using an ULN2003 Darlignton driver + an extra NPN since I required 8 channels for the 2x4 stepper coils. The buttons & limit switches inputs were filtered since the code is interrupt driven and without any debouncing. A hidden button allows manual incrementation for testing purposes.  

The 4MHz crystal oscillator frequency can be dialed-in using a trimmer cap to within tens of seconds per day of accuracy, which is sufficient for rough day keeping.


### Software

The software was developped in plain low level C and compiled using AVR-GCC and Avrdude. The MCU runs at 4MHz, and the program occupies ~ 500 bytes of the 2kB of available program space.  

Fuse settings: `lfuse:w:0xfd:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m`  

Resource allocation:
- TIMER0: triggers at ~400hz, used for stepper state machine
- TIMER1: triggers every 8s, used for timekeeping (daily trigger)
- INT0: "zero" limit switch for the unit wheel. Configured as falling edge interrupt to reset position.
- INT1: "zero" limit switch for the tens wheel. Configured as falling edge interrupt to reset position.

The zeroing routine ensure a least half a turn of each wheel, even if the wheel was already diplaying a zero, for your enjoyment ☺.  
Upon zeroing, the time is also reset such that the display will increment 24 hours after the last reset.
