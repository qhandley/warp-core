<p align="center">
<a href="https://osuaiaa.com/hybrid-rocket"><img src="imgs/hybrid2020_patch.png" width="255" height="255" alt="OSU Hybrid Rocket 2020 Mission Patch"></a><br/>
<b>Real-time embedded launch control application for AVR</b><br/>
</p>

The **Warp Core** is an embedded control application designed to facilitate communication between mission control and the launch pad via a socketed TCP connection. It prioritizes reliability and versatility to deliver a robust interface to the grounded rocket to manage pre-flight remote oxidizer filling and ignition timing with oxidizer injection - unique challenges with a hybrid rocket. The application is designed to run on a custom high-performance AVR embedded controller with supporting hardware, all designed and built for a senior [ECE capstone project](https://sites.google.com/oregonstate.edu/ece44x201932/home). The application is built on the Atmel AVR port of [FreeRTOS](https://www.freertos.org/).

## Setup and build
To build this project, clone the repository and run: 
```
git clone --depth 1 https://github.com/qhandley/warp-core.git
cd warp-core/FreeRTOS/Demo/AVR_ATmega1284p_WinAVR/
make
```
All compilation has been tested for gcc version 5.4.0.

## Hardware
Here is the [schematic](https://drive.google.com/file/d/11jHHyDcYjQdqKJOvxmM9LvJWHHDFpkBz/view?usp=sharing) for the embedded controller based on the ATmega1284p and WIZnet W5500 internet offload chip. Further details of the system can be found on the capstone project link above.

## FAQ / Troubleshoot
* *How do I program my AVR target?*

During development we primarily used AVRDUDE for programming on-chip flash and fuse/lock bits and USBasp for in-circuit programming.

* *How would I port this project to a different microcontroller?*

First, check out the [supported processor families](https://www.freertos.org/RTOS_ports.html) to see if there already exists a FreeRTOS kernel port for your target microcontroller. Use those examples as templates to configure the kernel for the target system.

* *My microcontroller keeps resetting, what gives?*

This is not an uncommon issue when developing with an RTOS, but could be for a host of reasons. The largest culprit is likely a stack overflow as a result of improper memory management. Check the *FreeRTOSConfig.h* and carefully consider the size of the heap and its implementation. Also check whether or not the watchdog timer is enabled/disabled; it may be beneficial to check the MCU's status register (MCUSR for AVR) to get a general idea of the reset source. The best advice is to make small, iterative changes so that tracking bugs is relatively easy.

## Contributors
<a href="https://github.com/tprihoda">
  <img src="https://avatars1.githubusercontent.com/u/46504109?s=400&v=4" width="100" height="100" alt="Thomas Prihoda"></a>
</a>
<a href="https://github.com/qhandley">
  <img src="https://avatars0.githubusercontent.com/u/50973949?s=400&v=4" width="100" height="100" alt="Quinn Handley"></a>
</a>
