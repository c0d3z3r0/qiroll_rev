# Qiroll E-PRO Reverse Engineering

**Hardware:**

I got a defective Qiroll E-PRO (2023 version), so I decided to tear it apart and reverse engineer it.
The whole PCB is sealed with epoxy, but with massive violence I was able to reveal it.
First, I used a cordless drill with a 1 mm drill to "mill" the PCB out of the aluminum case. Don't ask... it was a massacre.

I didn't expect them to place components and vias 1 mm to the edge but... the board got damaged a bit more...
With a tiny flathead screwdriver and a hammer I chiseled off the epoxy. Of course various components got ripped off including some traces.
The crucial parts are still there and some parts (diodes, caps, resistors) can be guessed.

I will try to repair the board. Let's see...

**Firmware:**
Checking Github for some of the strings I found out they use VESC from the [VESC Project][VESCProject] (GH: https://github.com/vedderb/bldc)
*Of course* they violate the GPL license and don't provide sources............. but no worries... I'm gonna help them fix this! :-)

The firmware contains a string `org.vesc.410-QR`. Well, that's VESC HW410, "QR" (Qiroll) version, no? Yes!
After compiling and comparing hundreds of fw versions, I found the matching one: v4.00 (https://github.com/vedderb/bldc/tree/4.00)
I was unable to build a 100% matching image because I don't know what compiler version they used... Probably the Windows ARM GCC. Who knows, but IDA Pro + BinDiff helped a lot.

The image contains an EEPROM section with settings. I need to find a way to convert them... oh well, I thought I had seen USB pads on Testpoints, maybe I can connect it to VESC Tool? With the EEPROM settings read out, I might be able to run it with a newer VESC version.

## Pics or didn't happen!

- Sure! Look [here](/PICTURES.md)

## Current status

- PCB: reverse engineered the board and created a [schematic](/schematic), pdf [here](/schematic/qiroll.pdf)
- Firmware: dumped both uC - zero protection enabled; reverse engineering of ESP32 firmware WIP

## Some notes

- Motor
  - BLDC motor in a tube with ball-bearing on both sides
  - Motor attached on bearings' inner rings; outer tube probably run through a planetary gearbox
  - No way to disassemble without damage (bearings pressed-in, maybe glued)
  - no hall sensors

- Battery
  - H70 battery consists of 9 18650 cells (9S3P configuration) and a custom charge controller
  - Sino Wealth [SH367007][SH367007-ds] "Lithium Battery Pack Protection Chip" (and cell balancer)
  - STM8S003F3P6 for 4-LED capacity indicator; triggered connector pin 1 pulled high (100k resistor!)
  - I've added a button (100k ohms to 37V) to trigger it without having to attach the Qiroll

- Schematic
  - Weird fuse / MOSFET circuit... I don't get it... did they mess it up? Why is there no "global" fuse?

- Two microcontrollers
  - ESP32-C3 for button control and LED, [APM32F407RGT6][APM32F407RGT6-ds] (STM32 clone) for motor control
  - I would have used ESP32-S2/S3 (MCPWM, ULP) to reduce complexity and save power.....

- 3-phase sensorless controller
  - 3-Phase halfbridge gate controller [FD6288T][FD6288T-ds] (similiar to TI DRV8300DI)
  - no hall sensors
  - current sensors
  - temp sensor 10k NTC (103AT)

- ESP32 Firmware
  - Libraries and versions from strings:
  - ESP-IDF v4.4-beta1, commit a79dc75f0a -> https://github.com/espressif/esp-idf/commit/a79dc75f0a91f4b558dcefb7ae5096c71c63e2e9
  - heads/idf-release/v4.4-0-gd6934 not public? maybe modified https://github.com/espressif/arduino-esp32
  - arduino-lib-builder

- APM32/STM32 Firmware
  - String `org.vesc.410-QR` -> VESC HW410, "QR" (Qiroll) version, [VESC Project][VESCProject]
  - Apparently no customization besides that string
  - GPL violation, what else?! -.-
  - Comparing hundreds of fw versions, I found the matching one: v4.00 (https://github.com/vedderb/bldc/tree/4.00)

## ESP32 UART output

```
ESP-ROM:esp32c3-api1-20210207
Build:Feb  7 2021
rst:0x1 (POWERON),boot:0xe (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fcd6100,len:0x420
load:0x403ce000,len:0x90c
load:0x403d0000,len:0x236c
SHA-256 comparison failed:
Calculated: ccb0d00bac7e84e1d90a12e4f75f4ab6c5f7e71bb209afd5819c4c9557a6db71
Expected: c9cf160580940ec7801c73b16423824e72ad12055c732e83ce66332240af42a7
Attempting to boot anyway...
entry 0x403ce000
 ESC_DATA=0 Battery_Model=B60 AD_Start=4095 AD_Start=4095 ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection ESC_DATA=0 data protection
```

Note: checksum error is "normal" (bug in ardiuno/esp-idf framework)

## Dump ESP32 firmware via UART

```
$ esptool.py --after no_reset --port /dev/ttyUSB2 --baud 3000000 read_flash 0 ALL esp32c3.bin
esptool.py v4.7.0
Serial port /dev/ttyUSB2
Connecting....
Detecting chip type... ESP32-C3
Chip is ESP32-C3 (QFN32) (revision v0.4)
Features: WiFi, BLE, Embedded Flash 4MB (XMC)
Crystal is 40MHz
MAC: 60:55:f9:af:b6:c4
Uploading stub...
Running stub...
Stub running...
Changing baud rate to 3000000
Changed.
Detected flash size: 4MB
4194304 (100 %)
4194304 (100 %)
Read 4194304 bytes at 0x00000000 in 44.9 seconds (747.6 kbit/s)...
Staying in bootloader.

$ esptool.py --after no_reset --port /dev/ttyUSB2 --baud 3000000 verify_flash 0 esp32c3.bin
esptool.py v4.7.0
Serial port /dev/ttyUSB2
Connecting....
Detecting chip type... ESP32-C3
Chip is ESP32-C3 (QFN32) (revision v0.4)
Features: WiFi, BLE, Embedded Flash 4MB (XMC)
Crystal is 40MHz
MAC: 60:55:f9:af:b6:c4
Uploading stub...
Running stub...
Stub running...
Changing baud rate to 3000000
Changed.
Configuring flash size...
Verifying 0x400000 (4194304) bytes @ 0x00000000 in flash against esp32c3.bin...
-- verify OK (digest matched)
Staying in bootloader.
```

## Dump APM32 firmware via SWD

```
$ openocd -f interface/ft4232h_swd.cfg -f target/stm32f4x.cfg -c "init; reset halt; flash read_bank 0 apm32.bin 0 0x100000; exit"
Open On-Chip Debugger 0.12.0
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.org/doc/doxygen/bugs.html
Info : FTDI SWD mode enabled
swd
Info : clock speed 2000 kHz
Info : SWD DPIDR 0x2ba01477
Info : [stm32f4x.cpu] Cortex-M4 r0p1 processor detected
Info : [stm32f4x.cpu] target has 6 breakpoints, 4 watchpoints
Info : starting gdb server for stm32f4x.cpu on 3333
Info : Listening on port 3333 for gdb connections
[stm32f4x.cpu] halted due to debug-request, current mode: Thread
xPSR: 0x01000000 pc: 0x0800c000 msp: 0x20000800
Info : device id = 0x0009a413
Info : flash size = 1024 KiB
```

[SH367007-ds]: https://pdfcoffee.com/sinowealth-sh367008x-038xy-aad01-c160914zh-cn-en-pdf-free.html
[FD6288T-ds]: https://static.qingshow.net/fortiortech/file/1597746029372.pdf
[APM32F407RGT6-ds]: https://global.geehy.com/uploads/tool/APM32F405xG%20407xExG%20datasheet%20V1.7.pdf
[VESCProject]: https://vesc-project.com/
