
 NexStar serial port message test
 ================================

This program test NexStar astronomical telescope stand drive commands.

Ref(s):

  - https://www.paquettefamily.ca/nexstar/NexStar_AUX_Commands_10.pdf
  - https://www.nexstarsite.com/PCControl/ProgrammingNexStar.htm
  - https://www.nexstarsite.com/OddsNEnds/MCFirmwareUpgradeHowTo.htm
  - https://www.cloudynights.com/topic/556591-bluetooth-or-wifi-control-of-nexstar-8se-with-newer-usb-hand-control/
  - https://github.com/ABasharEter/CSerialPort/tree/master/CSerialPort
  - https://www.pololu.com/docs/0J73/15.6
  - https://stackoverflow.com/questions/1981459/using-threads-in-c-on-windows-simple-example
  - https://learn.microsoft.com/en-us/windows/win32/procthread/process-and-thread-functions

Application can build with CodeBlocks (Windows, MinGW system)
  - https://www.codeblocks.org/

Example command with com port:  COM20

  - nexstar.exe 20

Keyboard controls:
  - drive speed can control with cursor arrow keys from keyboard
  - "Esc" and '0' keys stop movement
  - Ctrl-C is application exit

