# LCD-16x02-8bits

Programming the [MSP430FR2476](https://www.ti.com/lit/ds/symlink/msp430fr2475.pdf) to control a LCD 16x02 using parallel interface of 8 bits. Code Composer Version: 10.2.0.00009 was used.

![image](https://user-images.githubusercontent.com/58916022/171038644-ca2b143a-7a07-43d0-8c1e-b353a310770b.png)


## Pin configuration

The LaunchPad™ development kit used for development was the [LP-MSP430FR2476](https://www.ti.com/lit/ug/slau802/slau802.pdf) from Texas Instruments manufacurer. 

![image](https://user-images.githubusercontent.com/58916022/171039872-ea7854ae-ccd5-4e1a-9b89-9519344b6273.png)

| Pin number | GPIO | Connection |
| ----- | ----- | ----- |
| 27 | P3.1 | Enable |
| 34 | P4.1 | RS |
| Not connected | Not connected | RW |
| 45 | P5.2 | D0 |
| 44 | P5.1 | D1 |
| 43 | P5.0 | D2 |
| 42 | P4.7 | D3 |
| 22 | P6.0 | D4 |
| 24 | P3.3 | D5 |
| 40 | P6.1 | D6 |
| 41 | P6.2 | D7 |


