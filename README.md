# ittyPDA
<img src="render/render.png" width=800 height=235>

hi! this is my little thing im working on.

ittyPDA is an itty-bitty PDA/cyberdeck thingy. its smaller than a nintendo ds but has a real keyboard and a full color IPS LCD display, powered by an STM32 microcontroller.

it's mainly intended for note-taking/writing, playing silly little videogames, and just being a very hackable little computer.

this repo contains both the hardware designs, under the `ittypda/` subdirectory, as well as the operating system under `ittyOS/`. 3D case files are under `case/`.

## specs & parts

* microprocessor: [STM32F411RET6](https://www.st.com/en/microcontrollers-microprocessors/stm32f411re.html)
    * Up to 100 MHz
    * 512 Kb flash
    * 128 Kb RAM
* display: [3.5" IPS display (no touch version)](https://www.lcdwiki.com/3.5inch_IPS_SPI_Module_ST7796)
* storage: SD card slot included on display
* charging/data transfer possible over USB-C
* battery: 2200mAh LiPo - [this is the one I use, but really any battery with a similar size and connector will work](https://www.amazon.se/-/en/gp/product/B0D7VT93JX?smid=A2YUU8D7JQVZCY&th=1)
* on-board charging circuitry powered by a [BQ24040DSQR](https://www.ti.com/product/BQ24040)
    * also battery level sensing

<img src="readme/pcb3d.png" width=250><img src="readme/pcb.png" width=250>
## download symbols/footprints

to open the kicad project, youll need to download some footprints. my parts are sourced from JLCPCB's parts / LCSC, and you can use [`easyeda2kicad`](https://github.com/uPesy/easyeda2kicad.py) to download LCSC parts to kicad.

the command you need to run is: 

```bash
easyeda2kicad --full --lcsc_id C318884 C191023 C22435642 C13738 C81080 C49023767 C19273152 C165948 C295747 C20416998 C2682616
```

which will download footprints/symbols/3d models for the switches, diodes, etc.

## project status (important !!)

project is in a very early WIP state currently! the PCB is the only truly *finished* part so far.

the case is just a minimum-viable-product for now, and the firmware mostly acts as a proof-of-concept regarding graphics streaming and input handling from the keyboard matrix.

thus the project is mostly about the creation of the PCB, with the case and firmware mostly just acting as beta features used to test the PCB itself.

these features will be developed in the coming months though, and when i have guaranteed that the core PCB does work! this is a project i intend to gradually work on for a long time !

## schematic / PCB

the schematic and PCB are designed in KiCad, files for which are found in `ittypda/`.

the PCB is split in two seperate parts. this is done to achieve a budget 4-layer board, and to get around the fact that JLCPCB's economy PCBA service cant assemble parts on the bottomside of the board.

the mainboard houses the microcontroller, the keypad, and display connector.

the secondary board houses the usb-c port, battery charging ic, voltage regulator, etc.

the two boards mount together like a PCB-sandwich and connect through 7 soldered pads.

showcase how the two boards are mounted:

<img src="readme/pcbshowcase.gif" width=400>

<img src=readme/schematic.png width=400>

## firmware - ittyOS

ittyOS is currently in an early proof of concept state, but will have many different apps later on.

<img src=readme/home.png width=200>

implemented:
* display drivers
* reading from microSD card
* streaming image data from microSD to screen
* input from keyboard
* program switching
* basic home screen
* basic notes app

todo:
* improve text input
* usb file transfer
* more apps!
    * wordle
    * journal
* different themes?

### compile ittyOS

the `../firmware` folder contains the low level hardware setup for different boards that can run ittyOS, while `ittyOS/` contains the shared code and the actual system, which is then linked to from the firmware.

you'll need to generate the firmware code from the STM32 project file, since the low-level drivers for these boards are very large and therefore gitignored. you'll need STM32CubeMX. then just open the project file for the firmware you want to build, and hit generate code. 

you'll also have to tell your LSP which CMakeLists you're using.

then, to build, navigate to the firmware you want to build's directory.

1. `cmake --preset Debug` (you only have to run this once)
2. `cmake --build --preset Debug`
3. `STM32_Programmer_CLI -c port=swd -w build/Debug/<DEVBOARD OR ITTYPDA HERE>.elf -v -rst`

## case

the case files are located under `case/`.

the case is designed in,.. blender. its mostly a MVP for a case, and when i get the phyiscal PCB, this and the firmware will be the major focus for the project moving forwards.

most parts are designed to friction-fit, though i wont know how well this works in practice, but will find out soon !

![alt text](readme/case1.png)![alt text](readme/case2.png)

## BOMs

### Final bom (bom.csv)

|Name                         |Cost ($)                              |Source                                                                                           |
|-----------------------------|--------------------------------------|-------------------------------------------------------------------------------------------------|
|PCB                          |13.43                                 |JLCPCB                                                                                           |
|PCB Assembly Fees            |48.21                                 |JLCPCB                                                                                           |
|PCB Assembly Components      |20.55                                 |JLCPCB                                                                                           |
|PCB Shipping                 |24.07                                 | DHL Express                                                                                     |
|Customs & taxes              | 26.56                                |                                                                                                 |
|Total                        |132.77                                |                                                                                                 |


### Parts bom (parts-bom.csv)

|Part Name                    |Description                           |Link                                                                                             |Quantity                                                            |Total Price ($)                                                        |
|-----------------------------|--------------------------------------|-------------------------------------------------------------------------------------------------|--------------------------------------------------------------------|-----------------------------------------------------------------------|
|CL05B104KO5NNNC              |100nF 0402 Ceramic Capacitors         |https://jlcpcb.com/partdetail/1877-CL05B104KO5NNNC/C1525                                         |16                                                                  |0.0864                                                                 |
|CL05A475MP5NRNC              |10V 0402 Ceramic Capacitors           |https://jlcpcb.com/partdetail/24469-CL05A475MP5NRNC/C23733                                       |4                                                                   |0.0828                                                                 |
|CL05C100JB5NNNC              |10pF 0402 Ceramic Capacitors          |https://jlcpcb.com/partdetail/33914-CL05C100JB5NNNC/C32949                                       |4                                                                   |0.0212                                                                 |
|CL05A105KA5NQNC              |1uF 0402 Ceramic Capacitors           |https://jlcpcb.com/partdetail/53938-CL05A105KA5NQNC/C52923                                       |8                                                                   |0.1272                                                                 |
|CL05A225MQ5NSNC              |2.2uF 0402 Ceramic Capacitors         |https://jlcpcb.com/partdetail/13164-CL05A225MQ5NSNC/C12530                                       |2                                                                   |0.0128                                                                 |
|S2B-PH-SM4-TB(LF)(SN)        |PH JST Connector 2mm                  |https://jlcpcb.com/partdetail/JST-S2B_PH_SM4_TB_LF_SN/C295747                                    |2                                                                   |0.427                                                                  |
|1N5819WS                     |Schottky Diodes                       |https://jlcpcb.com/partdetail/GuangdongHottech-1N5819WS/C191023                                  |114                                                                 |1.5504                                                                 |
|05A20L14P                    |FPC Connector                         |https://jlcpcb.com/partdetail/HanElectricity-05A20L14P/C22435642                                 |2                                                                   |0.1404                                                                 |
|TYPE-C-31-M-12               |USB-C Connectors                      |https://jlcpcb.com/partdetail/Korean_HropartsElec-TYPE_C_31_M12/C165948                          |2                                                                   |0.3686                                                                 |
|YLED0603Y                    |Yellow 0603 LED                       |https://jlcpcb.com/partdetail/YONGYUTAI-YLED0603Y/C19273152                                      |20                                                                  |0.13                                                                   |
|0402WGF1002TCE               |10k ohm 0402 Resistor                 |https://jlcpcb.com/partdetail/26487-0402WGF1002TCE/C25744                                        |2                                                                   |0.023                                                                  |
|0402WGF5101TCE               |5.1k ohm 0402 Resistor                |https://jlcpcb.com/partdetail/26648-0402WGF5101TCE/C25905                                        |4                                                                   |0.0268                                                                 |
|0402WGF5100TCE               |510 ohm 0402 Resistor                 |https://jlcpcb.com/partdetail/25866-0402WGF5100TCE/C25123                                        |2                                                                   |0.0122                                                                 |
|0402WGF1001TCE               |1k ohm 0402 Resistor                  |https://jlcpcb.com/partdetail/12256-0402WGF1001TCE/C11702                                        |4                                                                   |0.0392                                                                 |
|0402WGF100JTCE               |10 ohm 0402 Resistor                  |https://jlcpcb.com/partdetail/25820-0402WGF100JTCE/C25077                                        |6                                                                   |0.0342                                                                 |
|TS-1187A-B-A-B               |Tactile Switches                      |https://jlcpcb.com/partdetail/XKBConnection-TS_1187A_B_AB/C318884                                |116                                                                 |2.3664                                                                 |
|MST-12D18G4 SPDT             | Slide Switch                         |https://jlcpcb.com/partdetail/C49023767                                                          |2                                                                   |0.1654                                                                 |
|MAX17048G+T10                |Battery Sense IC MAX17048GT10         |https://jlcpcb.com/partdetail/2777647-MAX17048GT10/C2682616                                      |2                                                                   |4.3212                                                                 |
|BQ24040DSQR                  |Lithium Battery Charge IC BQ24040DSQR |https://jlcpcb.com/partdetail/TexasInstruments-BQ24040DSQR/C81080                                |5                                                                   |2.243                                                                  |
|TLV76133DCYR                 |3.3V Fixed Voltage Regulators - Linear|https://jlcpcb.com/partdetail/TexasInstruments-TLV76133DCYR/C7527500                             |2                                                                   |0.438                                                                  |
|STM32F411RET6                |Microcontroller                       |https://jlcpcb.com/partdetail/STMicroelectronics-STM32F411RET6/C94355                            |2                                                                   |7.7452                                                                 |
|X322516MLB4SI                |16MHz 9pF Crystal Oscillator          |https://jlcpcb.com/partdetail/YXC_CrystalOscillators-X322516MLB4SI/C13738                        |2                                                                   |0.1856                                                                 |


<hr>

<sup>no ai usage</sup>

<sup>made with <3</sup>
