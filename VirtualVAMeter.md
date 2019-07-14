# VirtualVAMeter

Implements a full featured dual voltmeter and amperemeter as a virtual instrument. For complete documentation see `MegunoLink VirtualVAMeter R2.docx`

## Block Diagram

![Figure 1: Virtual VA Meter Block Diagram](https://i.imgur.com/M5QoHnj.jpg)

## Prototype

![Figure 2: Picture of the first prototype](https://i.imgur.com/GrRcgsv.jpg)

## Sample Screen

![Figure 3: Landing Screen](https://i.imgur.com/10qdUPB.jpg)

## Code Organization

### File Dependencies

![Fig 4: Dependencies](https://i.imgur.com/4cr3Oy8.jpg)

| FILE      	| Allocates	| Depends on...	| Depends on...	| Depends on...	|
|:-------:		|-----		|:-------------:|:----------:	|:----------:	|
| VirtualVAMeter| BUF buf, PROF prf, Ticker tic	| cliHandlers	| mscSupport	| Globals	|
| cliHandlers	| TCP tcp, MGN mgn, RinTEST rin	| mscSupport	| Globals		| - |
| mscSupport	| OLED oled, ADS14ads, DIIR qvolts, SCAN scan | Globals       | - | - |
| Globals		| CPU cpu, CLI cli, EXE exe, EEP eep, GLOBALS | - | - | - |

### Class Dependencies

| CLASS 		| Defined in... | Allocated in... 		|
|:------:		|:---------:	|:--------------:		|
| BUF buf		| bufClass.h	| VirtualVAMeters.cpp	|  
| PROF prf		| ticClass.h	| VirtualVAMeters.cpp	|
| Ticker tic	| esp8266		| VirtualVAMeters.cpp	|
| TCP tcp		| tcpClass		| cliHandlers.cpp		|
| MGN mgn		| mgnClass		| cliHandlers.cpp		|
| OLED oled		| oledClass.h	| mscSupport.cpp		|
| ADS15 ads		| ads15Class.h	| mscSupport.cpp		|
| DIIR qvolts12,qamps | filtClass.h	| mscSupport.cpp	|
| SCAN scan		| mscSupport.h	| mscSupport.cpp	|
| CPU cpu		| cpuClass.h	| globals.cpp		|
| CLI cli		| cliClass.h	| globals.cpp		|
| EXE exe		| cpuClass.h	| globals.cpp		|
| EEP eep		| eepClass.h	| globals.cpp		|
| GLOBALS myp	| global.h 		| globals.cpp		|

