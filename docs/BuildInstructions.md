# Build instructions

> **Disclaimer:** Follow these instruction at your own risk.
>
> Also check your motherboard's manual for the available headers, since these might differ.

## Soldering and board installation

![Schematic](./Assets/schematic.png)

1. Cut PCB to the right size if neccessary.
2. Drill two mounting holes into PCB to fit the board at the end of the case.
3. Solder the componentes onto PCB (daughterboard).
4. Screw Arduino with shield and spacers into case (saw spacers to the right length in neccessary).
5. Install daughterboard.
6. Wire daughterboard to Arduino.

![Daughterboard](./Assets/daughterboard.png)

## Installing software

1. Format SD card to FAT16 with [SDcard formatter (recommended)](https://www.sdcard.org/downloads/formatter/)
2. Store website (content of `web/src/`) on SD card.
3. Put SD card into ethernet shield.
4. Connect Arduino to a powerd on PC via USB and the shield to a router via ethernet.
5. Open router configuration, set a static IP range and choose a free IP address.
6. Set the IP address in the `server.ino` file and save.
7. Open Arduino IDE and upload `server.ino` to Arduino.
8. Test if the website can be reached by typing the IP into the webbrowser.
9. Unplug all the cables from the Arduino for the next steps.

## Installing into PC

1. Turn off Arduino and PC and detach from power.
2. Install USB internal header on the PC's motherboard. See your motherboard manual for a free USB header.
3. Wire cables between your PC and Arduino like following and close the case.
4. Connect Arduino to the USB header.
5. Connect the shield to ethernet to router.
6. Restore power to PC again. Arduino should turn on.
7. Open webbrowser on phone to open the website and turn on PC by pressing the power on button.
8. PC should start.

![Front panel IO](./Assets/frontpanelIO.png)
