---
title: "ittyPDA"
description: "ittyPDA is an itty-bitty PDA/cyberdeck thingy. It's smaller than a Nintendo DS but has a real keyboard and a full color IPS LCD display. Built with a completely custom PCB housing an STM32 microcontroller running my custom OS.\n\n\nIt consists of two custom PCBs sandwiched together, one being the mainboard that has the keyboard matrix and microcontroller, and the secondary only handling power circuitry and battery charging. This is done to get around JLCPCB PCBA restrictions and to save space to make the device as thin as possible.\n\n\n\nMainly made for notetaking/writing, but am also planning on implementing a journal, a simple E-Reader, and some games likes Wordle and Tetris."
created_at: "2026-06-23"
---

# 2026-08-04: Working on the home screen !!

The home screen has an input box where you type the name of a program to open it. I implemented an autocomplete feature that finds the program which name begins with what you're typing, and then writes out that complete query (but with a darkened colour).

If you hit enter it will open the current autocompleted program.

This is me just having written "n", and the rest of the letters "[n]otes" are written in a darker color (maybe hard to tell on the picture).

![image](https://cdn.hackclub.com/019fd000-b38c-79f7-849b-db1a06ea51cd/image.png)

It also works to then hit enter and the notes app opens! cool !!

So now the OS is working as an early proof-of-concept! Theres a homescreen, from which you can launch any app by its name, and from any app you can press the super key (windows key equivalent) to return home!

# 2026-08-03: Preparing to order PCB

**Total time spent: 0.2 hours**

So, finally getting around to the dreadful task of writing a BOM :noooo:
I've used some resources from blueprint regarding the format and looking at other projects from there to see how they have formatted theirs. I actually put two seperate boms, one specifically for the components, mostly listing all them with their prices and whatnot, and the other bom is just for the larger aspects like PCB fabrication, assembly, and the total cost of parts.

This also forced me to finally go through JLCPCBs order process, which is really slow and i kept making mistakes and having to return to the start :sob:. I had previously thought it would cost about $70 to order, but because of taxes,customs, and shipping, it lands at about $135 :///
![image](https://cdn.hackclub.com/019fcabe-3d5b-710d-b1fc-88195445abfd/image.png)

Throughout the project ive done a lot of cost-saving measures, which I am now very glad to have done as i cant imageine how expensive this wouldve been if I had been more lax with using extended components, or even if I had dared to do double-sided assembly.

# 2026-08-01: Added hinge and screen enclosure

Still working on the case. Really hate this,.

Also wanted to see how it would look sliced in bambu labs studio, so I made a version of the case .blend file with only the actual printed parts, spaced them out, and then applied all modifiers and triangulated:

![image](https://cdn.hackclub.com/019fbf40-e0d7-7314-8028-d8a02736baf4/image.png)

I could only fit the bottom part of the case on a single build plate, but it seems to slice fine! :yay:
![image](https://cdn.hackclub.com/019fbf42-a7b3-72be-ad22-95a563198306/image.png)

# 2026-07-31: Work on the case :>

![image](https://cdn.hackclub.com/019fbb3a-9e82-79ca-8ab2-695110bfc270/image.png)

Started working on the case! Tried freeCAD but god am i bad with CAD software and at least for the first MVP I decided to jsut use blender :pf:

It should be fine I think and its much faster and easier for me.

# 2026-07-31: Rework battery charge sensing

Previously I was using ADC to read the voltage of the battery to estimate the battery level, but after being advised by [another hackclubber](https://github.com/espcaa) I replaced it with a dedicated battery sense IC. It's way more accurate so :yay:

I had to add yet another connector now between the two boards as the IC uses I2C for comms, while previously I only needed 1 line for voltage reading, but it was fine. The hardest part was finding any exposed pins of the microcontroller as the PCB is getting very cramped, and finding pins that were part of a hardware I2C port.

This is the schematic for the secondary board now:
![image](https://cdn.hackclub.com/019fbb07-5982-737f-998a-59125c80475a/image.png)

I think the PCB is probably ready to order now, so, yay!! Will do tomorrow maybe? its like 4 am :3

# 2026-07-27: Added battery charge sensing to PCB and wrote UI for battery indicator :3

Firstly, I added another connection between the secondary and main PCBs. This one carries the direct voltage of the battery, i.e. not regulated to exactly 3.3V. The value is connected to the one of microcontrollers analogue read pins. I also replaced the buck-boost converter with a fixed output linear regulator since its both cheaper and I dont want to discharge the battery under 3.3V anyways. The microcontroller can still notice this drop and handle shutoff, and the screen will immediately shut off from the voltage drop.

![image](https://cdn.hackclub.com/019fa5fe-e232-7555-9f93-f95b3473edf1/image.png)

Then I also worked on the UI for the battery indicator!

initially i was going for something like this:
![image](https://cdn.hackclub.com/019fa588-44dc-7d0d-949c-f0bc48f9d455/image.png)
^pcb status <3

but i messed up the code and accidentally made the bars diagonal which i thought looked really cool and i landed on this design:

![video](https://cdn.hackclub.com/019fa587-9c3b-7abf-8273-bf301c9eb8e0/VID_20260727_232726~2.mp4)
^ i had to film the video with a super low iso because otherwise it would be too bright for my phone to make out the details

# 2026-07-23: Working on UI designer tool, + adding battery percent

So, the last couple days I've been at Open Sauce!! Literally incredible but wont get into that here. During those days I worked on a UI designer tool, which lets you make a design in some markup language (json atm) / editor maybe in the future, and it generates C code which draws that to the screen. The reason why this is useful is that it sucks writing UI code for this thing, since the microcontroller cant hold the entire screen buffer in memory, so it has to be streamed. This UI builder handles the streaming aspects, making elements draw in the correct streaming chunk and whatnot. Its not perfect yet but am getting there!

I also had the idea to probably include a battery percent meter thingy that the device can read. I would use the microcontroller's analogue-to-digital converter to read the voltage of the unregulated battery line (which would need to be added, currently the MC only gets the fixed 3.3v). The voltage doesnt directly convert to battery percentage, but I followed a graph I found online for 3.7v lithium batteries to get a rough estimate of battery charge from the voltage. My graph is simplified and dumbed down to work with pure integer logic, no floats, and looks like this in geogebra:

![image](https://cdn.hackclub.com/019f90bd-3180-70e4-a8d3-9b22e60ff45e/image.png)

The device would also need a voltage divider since the ADC cant handle the full 4.2V. For testing with my breadboard devboard i just used a potentiometer hooked up between ground and 3.3v.

# 2026-07-16: Improved system scalability !!

Since ittyOS will have several different programs that can run, I have spent a bit of time configuring the project so that I can easily add new programs. Firstly, I have split the `ittyOS/` directory to `ittyOS/Core/` and `ittyOS/Programs/`. The programs directory contains the different programs that you will be able to run, each program having its own directory. There's also a root-level `programs.h` file that just includes all the programs. This way, by simply including `programs.h`, you actually include every program.

![image](https://cdn.hackclub.com/019f6c72-cfc4-723b-af42-65f3f9c46dd1/image.png)

I hate CMakeLists, and try to avoid touching it as much as possible. For the longest time, I have been manually adding each new source (.c) file to the `target_sources` in CMakeLists, but just found out you can use globs to include multiple files automatically, which is really cool. Also using globs, i configured it to automatically include every subdirectory of `ittyOS/programs/`.

The way a program is represented in memory is just a struct with its string name, and two function references for its `init()` and `update()` functions. C doesn't have classes, which I would normally use in this situation, but emulating classes with structs is *fine*. It is a bit annoying having to make sure that even the program's private methods are named uniquely, i.e., two different programs can't locally define a function called `init()`, I have to name them like `homeInit()` and `journalInit()`.

I also spent several hours on the image rendering, optimizing it further. Previously, even when rendering a horizontally cropped image, it would read the entire row data, and then I would use memset to reorder the data so that only the cropped image remained. Now I just read the data that is actually shown, which simplifies logic and probably is faster, depending on how wide the cropped image is.

I really want to make a barebones UI toolkit for this thing, just a simple webapp or something that can generate the C code to draw a specific screen, with references to image files and whatnot. Since drawing to the entire screen will always require the data to be streamed, because of the microcontrollers RAM constraints, manually writing the code to generate a specific screen is quite tedious. You manually have to write some sort of callback, which will modify the display buffer before its rendered by adding the different elements, depending on the current buffer's Y position. Its really something I should automate with some app that can generate the code instead.

# 2026-07-12: Improved image streaming capabilities

![image](https://cdn.hackclub.com/019f5827-1c52-7389-9731-24108707ae3c/image.png)

So I decided to make a general function for streaming file data. It streams the file in chunks and calls a callback on each chunk of data. This allowed me to add some new image drawing functions. Firstly, I made a function to draw an image with text overlayed. It isnt possible to seperately render text over an image without overwriting the background behind the text, so this will probably be very useful later on.

So far the image drawing functions have only drawn fullscreen images of exactly 480x320, so I also added a function to draw any sized image, at a specific X and Y location. I also went a bit further adding support for cropping the part of image thats rendered, which is what you see in the image. The cropping took several hours to get working correctly, since it makes the streaming of data is a lot more complex and needed new capabilities added to the file streamer function.

# 2026-07-12: quick note!!

so, i just did something which makes development of ittyOS sooo much easier !! i currently only have an STM32 devboard to develop on, not the physical ittypda PCB. so i have had two different repositories, one firmware for this devboard, and one for the real ittypda. every now and then i copy the changes i have made in the devboard firmware to the ittypda firmware. this takes a lot of time and is a real pain. 

the solution? i have one shared ittyOS, which is semi-portable. then i have two different directories for the different firmwares, one called ittypda, one called devboard. i have managed to configure cmake to then load the ittyOS source files and it works !! it was a struggle configuring my LSP to recognize which cmakelists to use but i got there in the end!! now all the ittyOS code is shared, and only the low level configuration / hardware setup is different between the two boards. so cool!!

![image](https://cdn.hackclub.com/019f5416-e6e8-7fa7-840e-a242605fb09e/image.png)

# 2026-07-11: Added ÅÄÖ to font

Since I am going to be using this device, and I am from sweden, I kinda want the "åäö" keys. I'm not sure I will be sticking with this font I'm currently using, but figured I'd add these letters to it anyway just for fun. I found this font part of a display library online, and the data is stored in C code, and looks like this: 

![image](https://cdn.hackclub.com/019f53ab-e03a-7627-b91c-8c80163a881e/image.png)
(and so on, for houndreds of lines)

By reading the code I figured out that this array is read 26 values at a time, corresponding to the height of a single character. Each value is a 16 bit integer, where each bit corresponds to a single pixel, whether it should be on or off. So each value is actually an entire horizontal row of pixels. 

I made a simple rust CLI program ([cfontdecoder](https://github.com/ingobeans/cfontdecoder)) that would parse and dump this entire font file to a directory of PNGs, and then made it able to parse new PNGs and turn them into new entries. I did this for the six characters, åäö and uppercase ÅÄÖ. I also did this process twice, for two different font sizes.

![image](https://cdn.hackclub.com/019f53b1-46d8-7f94-b2ec-82c112ebcade/image.png)

I have for now mapped åäö to bytes \x80, \x81, \x82 and continued for uppercase. I will probably implement basic utf-8 mapping later, so that utf8 text files can be read, but for now this was the easiest way to test the new font.

# 2026-07-11: Inventing an image file format and getting my devboard to render it!

**Total time spent: 3.0 hours**

So, to try out the microSD card slot that I jsut got working, I decided to try to get the display to display an image from the SD card. Since the microcontroller doesnt have enough ram to load an entire image to memory to show to the display, it has to be streamed, i.e. loading small chunks of the file at a time, and sending them off to the display. This actually proved to be quite difficult, especially as someone who is new to C, but I got there in the end. I have a File object from the FatFs library, which i "seek" in, i.e. move the read pointer, in a loop, each time reading a small chunk, drawing that to the display, then seeking again a bit further. This only works however, if the image file data can be **directly** parsed by the screen, which means we cant use anything like PNG or JPG, we need raw color data. I spent like 20 minutes looking for a tool to convert a PNG to a 16 bit color depth BMP image, which is a fileformat most similar to what I need. The problem was that I couldnt find a single one that worked!! Some of the online ones claimed to be 16 bit color depth, but when reading the Hex code of the file output, it clearly stated itself to be 24bit color.

So i decided to invent a file format. This is probably overkill but since I couldnt find any alternative this is what i did. I call it IBI, (.ibi file extension) short for Itty Bitmap Image (or Itty-Bitty-Image). It starts with 4 bytes being the magic numbers, which spells out "ibi!". Then its followed by 4 bytes saying the width of the image, and then 4 saying the height. The rest is just raw 16bit color data which can be sent directly to the display. Yay! I made a CLI tool in rust called [imgToIbi](https://github.com/ingobeans/imgtoibi) which can convert a PNG/JPG/WEBP/ICO/BMP file to an IBI file, or vice-versa.

The result of all this?:
![image](https://cdn.hackclub.com/019f5238-9338-789c-a315-d2df9f32fc3a/image.png)

^woaw,,, so cool*

*note: this is just the UI mockup image rendered to the screen. it is not interactive yet. but this is the same way i will render the wallpaper to the screen, or any other image.

# 2026-07-10: Spent the entire day debugging those random crashes of my devboard

**Total time spent: 2.0 hours**

Two journals ago, I had gotten the SD card reader working, but mentioned the devboard crashed like 9/10 times with completely random errors. Sometimes the processor was just executing garbage data, sometimes the instruction pointer was misaligned to the actual program's instructions, often by exactly 1 byte, so it would execute like the instruction between instructions, very weird. 

Take this for example:
![image](https://cdn.hackclub.com/019f4da9-6b22-7108-bad9-c7c21f74cc9b/image.png)
This is a dissasembly of a function, not really relevant, problem however is that the instruction pointer is at `0x8006B5B`. LITERALLY BETWEEN THE TWO LAST LINES OF CODE. Of course it crashes because it doesnt recognize the isntruction, since it is between two instructions. 

Turns out, after like 7 hours of debugging, I just had to turn down the clockspeed of the CPU. It defaults to the maximum of 84mhz, but I guess this devboard isnt designed well enough to handle that level of speed. Turning it down to 72mhz makes everything work. This is so incredibly frustrating and also relieving. I only logged 2 hours for this, since it was maybe 2 hours of actual progress, but it was really my entire day spent in those horrible STM32Cube programs, VScode, and my dissasembler of choice - binary ninja <3.

Anyways, got it to also read the contents of a file on the sd card. Its cool being able to format a drive, add a file with some text contents, and then having the devboard able to read that properly.

![image](https://cdn.hackclub.com/019f4da3-2567-75de-b3f9-b1657e7b0315/image.png)

# 2026-07-10: Replaced voltage regulator with buck-boost converter & added test pads

So, I realised the battery probably can drop below 3.3 volts. The microcontroller should be fine with a lower voltage, but the display's datasheet states that it requires either 3.3 or 5 volts. To remedy this, you can use a buck-boost converter that I learned about today ! Basically a buck-boost converter is a combination of a buck converter, a circuit which decreases voltage, and a boost converter, a circuit which increases voltage. Combined, it turns into this magical black box, which takes an input range of 1.8V - 5.5V and outputs a fixed 3.3V. It can both increase and decrease the voltage, to hit the target 3.3 volts.

Also added some test pads for different parts of the power circuitry, 5V, 3.3V, GND and one for the LiPo battery connection. Theres 3.3V and GND test pads for both the mainboard and the secondary PCB.

![image](https://cdn.hackclub.com/019f4bf4-60fb-73d0-8cd5-794378344c0b/image.png)

# 2026-07-09: Trying to get the SD card reader working

**Total time spent: 3.0 hours**

So, the display I'm using comes with a builtin microSD card slot that can be used over the same SPI interface. I have spent basically the entire day today and a few hours last night working on getting this working. I did kinda get it working, see this:

![image](https://cdn.hackclub.com/019f4842-25f8-7879-89b6-b34bfcc97fc0/image.png)

It basically just mounts the SD card, lists all the dirs and files (only one dir called "System Volume Information", and a 12 byte file called "test.txt"), then unmounts. I got a library for handling most of this. I had to replace the standard printf function though, since I wanted it to print to the screen. I'm quite pride of this code since ive barely every worked with C before but still got it working very well.

The problem: it almost never works. Its roughly like a 1 in 10 that it will work at all. THIS has been driving me COMPLETELY insane the last day. And i dont mean that the SD card fails to read or something, i mean the entire processor crashing. Its completely random, but more often than not, the processor will attempt to run an illegal instruction, jump to some bad address, or try to write misaligned data. I have 0 clue why this happens, its always a different type of crash, and always involving random memory addresses and stuff. I thought it might have to do with some buffer overflow in my printf text buffer which could maybe overwrite program code with text data, especially sicne I once saw it try to jump to 0xa, which is the `\n` characters hex code, but this wasnt the case. A lot of the times it will fail in the low level setup of the timers of the chip. Like it will fail before even reaching the first line of my code. Its very frustrating to debug a chip that sometimes dies during the setup of the system clock with a compltetly nonsensical error.

Anyways, hope to find cause soon. Sanity is weak

# 2026-07-08: Working on display drivers </3

So, Display arrived. Yay. Since I have an STM32 devboard lying around, I decided to try to hook it up to the display, and save future me a lot of time. This, has embarassingly taken 2 entire days to get working. I gave up like 3 different times and have tried like 4 different libraries from github. The official datasheets and material for this thing were abysmal. Even just getting it to turn on to display anything took SO long, but then eventually, I got this:

![image](https://cdn.hackclub.com/019f4406-456e-7922-9261-0b50823f2f17/image.png)

The relief I got when I saw that was insane. Even though its just garbage data I assumed I'd probably be able to get it working. This took maybe an hour more before I got it working correctly. That random colored noise junk data is actually just the data that exists in the display drivers screen buffer upon initialization. You have to manually override it with something else. The reason why that wasnt working was that I was trying to use DMA, but hadnt configured it in the STM32CubeMX app, i.e. the program that you use to configure your STM32 boards, like set up their pins, and it generates code configuration. DMA is basically as I understand it a fast way of sharing memory to a peripheral through SPI. I can also use the display without DMA, but refreshing it takes almost an entire second, whereas with DMA its instant.

Which finally leaves me at, this:

![image](https://cdn.hackclub.com/019f4409-4b3b-7a86-bb4a-66bb488e4788/image.png)

This part has been really frustrating to work on, but now that Im through I can start working on much more fun things! I havent used the microSD card slot yet, so want to get that working so that I can display an image from the SD card onto the screen. I'll keep working on the firmware!! I havent ordered the PCB yet (keep procrastinating) but IG I will do it tomorrow (?).

# 2026-07-05: Start working on firmware!

I dont think I'm completely done with the physical PCB yet, but since I had nothing better to do today I decided to try to start working on the firmware! For testing purposes, I had already a few days ago set up a basic C project for use with the STM32 toolchain. It had been suprisingly complex to get this working with VScode, and while I could've used the STM32 IDE, I just found it clunky and with odd keybinds, and I'd much much rather use vscode which I use for everything else. I just didnt want to feel tied to some super old proprietary software.

![image](https://cdn.hackclub.com/019f3550-0232-734c-81a8-be9b446b642b/image.png)

Getting the clangd language server to work was really difficult. After hours of trial and error I came up with the solution which requires both a .vscode/settings.json and .clangd file for configuration of clangd. I also need to use some preprocessing in a header file to fix one final warning, but now it thankfully does work!

I set up the basic framework for the operating system. When you create an STM32 project it naturally already comes with a main.c file, but since this is filled with lots of low level setup and config of the board, I decided to move everything custom to a seperate file, system.c. Getting this file to be included by CMake was a whole 'nother story but I got there in the end. I added some code for defining the keyboard matrix. I had to make a struct to define a GPIO pin, which contains both the pin index as well as the IO bus its part of (to read the state of an IO pin, you need both these values). I also made a sort of translation map from a row/column to a character. This also updates depending on shift/alt states. Since "åäö" arent part of the regular ascii character set, I gave them the special codes of \x80,\x81 and \x82 (plus 3 for their uppercase counterparts). I will probably think of a better way of handling this in the future but for now I just needed some way to represent them.

Thats basically where Ive got so far. Not a lot of impressive coding, but a whole lot of project configuration and boilerplate.

Oh right, I'm also considering adding an LED to the board to be used for debug purposes. Aside from the display, the board has no simple way of outputing data. For debug purposes, having some way of it communicating would be a potential life saver, which is why I'm considering adding an LED.

# 2026-07-04: Added silkscreen art to board, and fixed minor issues with PCB

Adding the silkscreen art to the board was suprisingly annoying. You cant just drag and drop the PNGs or anything, you have to convert your assets one by one into footprints. That also means you have to write out the exact target dimensions of the artwork in millimeters, you cant scale it or anything once placed on the board. Also, if you made a mistake, you have to reload the footprint library, which the only way I found to do was by disabling it, applying, then going back to settings and enabling it again.

Anyway, got the 6 images on there eventually !

![image](https://cdn.hackclub.com/019f2e1a-5af1-7311-b80d-ff273138a3f8/image.png)



I also added labels for some headers. I realised I probably want to move the BOOT0 button again, and in doing so, I unknowingly screwed up the ground planes and created an island which led to some super strange errors that took me ages to figure out. Had to open up a previous version of the board to have a side by side comparison / spot the difference.

I also adjusted the datalines for USB so their lengths matches. I had kept pushing this off, since I wanted to do this last when I know I wont have to reroute anything.

Finally I also updated the mockup with the new PCB version and everything looks great!!
![image](https://cdn.hackclub.com/019f2e19-cba8-73f2-9288-fc8914b20c97/image.png)

# 2026-07-04: Drew some art for the silkscreen

**Total time spent: 0.3 hours**

I really want the PCB to have some fun artwork so heres what I came up with so far :3

![image](https://cdn.hackclub.com/019f2e13-2f87-7237-86d8-cc5a9360d3a1/image.png)

Since I'm not manually placing the components, theres also really no need for labels for any part (aside from the header pins), so I get even more free space to work with !!

I havent put this on the PCB yet, just drawn the assets and overlayed them on a screenshot of the PCB

# 2026-07-03: Upgraded microcontroller !!

So, 96KB of RAM is not a lot, which @louis kindly pointed out to me. Driving the display will be a bit of a pain to get working later on, and will require significant optimizations, which I'm all for, but yeah, 96KB is really cutting it close, and louis suggested I upgrade to the STM32F411RET6, which had slightly more RAM with almost the same pinout.

It was pretty easy replacing the microcontroller with the new one, the only difference was that the new chip had the SPI interface's SCK wire on another pin, so I had to reroute that. I had also already been using this new pin for IO with the keyboard, so had to move those pins around too, but all in all, it went pretty quickly. 

When reading the new chips datasheet, I realised I had made some mistakes in the original design. Firstly, I had mixed up the USB's data+ and data- lines, which wouldve caused the USB to just not work at all, afaik. I had also had some completely incorrect capacitor values, which maybe wouldve been fine, but correcting it is even better.

I was also getting some strange errors from kicad, and eventually realised there was a problem with the edge cuts. There was an invisibly tiny gap somewhere, which I never found, but thankfully kicad has a tool to repair the edge cuts and fill in those gaps.

I also moved the BOOT0 button so it isnt positioned so weirdly anymore, and exported the board as a 3D model to check that everything looks good with the new version of the board.

![image](https://cdn.hackclub.com/019f29fe-4156-7205-8c8a-58414148d296/image.png) 

I really need to work on getting some cute silkscreen art tomorrow

# 2026-07-02: Drew some UI mockups :3

Not tracking any time for this. Just wanted to get a rough idea of what the software is going to look like. Took a lot of inspo from the old MacOS versions (like versions 7-9).

![image](https://cdn.hackclub.com/019f24ae-efb5-7425-9eae-1792788bbb44/image.png)

The left window image shows the homepage. The input box at the bottom is a search bar for apps. I imagine you'll also be able to enter math expressions like `72*3` and have them computed. 

The right image shows an example program running. When you launch an app, it will always take up the full screen. You cant have multiple programs running or anything like that.

right, also, i got the icons from [melonking's pixel sea](https://melonking.net/melon?z=/frames/pixelsea). (some are slightly modified)

Here is what the mockup render looks like with this home screen:
![image](https://cdn.hackclub.com/019f24b8-2b31-7fa4-aa37-15846acd7fe0/image.png)

# 2026-07-02: Ground planes finished !!

So, I just spent like two hours adding a ground plane to my PCB and resolving every single DRC error. From the beginning I had over 60 errors :sob:

This was mostly just a matter of connecting all ground and connections I had missed. I also had to fix a ton of footprints and tiny clearence issues, which really adds up over time. Some of the ground pads of the microcontroller were really hard to reach and I had to move around a lot of traces to reach them. Praying I wont have to ever touch those traces again lmao. It's a mess <3

![image](https://cdn.hackclub.com/019f2436-9d1a-7213-af8b-6da173ee6d53/image.png)

I also got some errors since the PCB actually consists of two different boards that snap off, which I could only ignore as they aren't actually relevant.

Next, I want to add a lot of some super cute artwork to the silkscreen. This might seem unnecessary but it is very integral to the PCB !!!!!!!!!! Right, I should also probably move the BOOT0 button (at the very top of the PCB) as it just kinda looks out of place otherwise haha

# 2026-07-01: Routed secondary PCB

This project has the electronics split on two PCBs, one mainboard that has the microcontroller, keyboard, etc, and one secondary board that just handles the power circuitry. Today I finished routing this secondary board :>

![image](https://cdn.hackclub.com/019f1e8a-95db-727a-ab5a-7169b42a4cc3/image.png)
^ secondary board PCB

This board was super easy to route as its so few lines over so much space.

I also remembered that data signal lines and power lines preferably shouldn't be right next to each other, so I rerouted some old traces where the USB data lines were running right by the 3.3V power supply line. I also had to fix some footprints that were messed up.

I also got stuck for a bit on this one footprint that wouldnt let me route anything from its pads. It was super frustrating!!! Eventually I realised the pads had a very large clearence value, which meant that the standard wire traces were too wide to pass this clearence. I just used some half width traces to break out the two wires I needed on these pads.

Anyway!! This project is coming along nicely!! I haven't routed any ground yet, I've heard it's good to save that for last and use ground planes, so thats what I'm gonna to (next time perhaps?).

Thanks for reading <3

# 2026-06-30: Traced routes!!

Today I finally started creating all the traces for the PCB. I was scared that I wouldn't have enough space and that this entire project would be doomed but thankfully it wasn't all that hard. This is my first time working with PCBs but yeah it went quite smoothly. Here's what the PCB looks like currently!!:
![image](https://cdn.hackclub.com/019f198b-698b-7c0b-a0d5-812e0fd1ecdd/image.png)

I reworked the way the secondary PCB will connect to the mainboard. Previously, it was intended for that part to connect with soldered wires, and itself to be oriented upright. Now, by the suggestion of Hekinav, I will instead use 5 through holes to connect the two boards. The secondary board will be mounted upside down against the mainboard, and through each of the holes (that line up between the two boards), I will use a little wire or something conductive and solder it there. This way the two boards will be sandwiched together nicely with a more reliable connection. Problem is though that this requires a lot more precision and alignment, made even more annoying by the fact that the secondary board will be flipped.

I also moved the connection to the bottom of the mainboard since I had a lot of free space there that I didn't know I would have until I did the routing.


I also exported the PCB as a 3D model and imported it in blender, along with a 3D model for the screen that I found online, to see how everything would fit, and that the connector between the two PCBs lined up properly.

![video](https://cdn.hackclub.com/019f198a-65cc-792f-8a89-6072fc0f99ab/20260630-1712-28.4099814.mp4)

# 2026-06-29: Layouted entire board :3

![image](https://cdn.hackclub.com/019f1584-9f8a-7481-8e60-5a69bc6e117d/image.png)

This is the rough layout of the board! Still havent done a single wire yet, so will probably have to move stuff around later. The bottom part, connected by mouse bites, will be broken off and then connected to the mainboard through those 5 solder pads that are present both on the secondary board and the main board.

Will attach like this:

![image](https://cdn.hackclub.com/019f1589-a74a-7f33-b3eb-18dc23dc13be/image.png)

I also left that gap on the top of the mainboard, through which I will route the wires connecting the power circuitry board. 

I guess I will have to actually route this thing next time (:sob:)

# 2026-06-29: Layout front side of PCB

Finally got around to adding the diodes to the keyboard :>
Problem is I had to redo the entire switch matrix since I hadnt aligned it to any grid the first time. This time I measured what the gap should be between each key and aligned them to that grid. Then I set snapping to half of that grid and placed the diodes:

![image](https://cdn.hackclub.com/019f149e-0387-74c3-95cf-5db04ffe245d/image.png)

The thing is though, the power circuitry of the device, I.e. the USB C port, the power switch, battery charging and voltage regulator ICs, etc, should all be placed on the bottom of the PCB. This way I can save a lot of space by having only the switches and other low profile component on the top side, and on the bottom I can have some larger components like the USB-C port. However, JLCPCB's PCBA basic service cant place components on the bottom side. My idea to get around this is to basically have two boards on the same PCB, with some thin bridge that can be physically snapped off. Then, wires can be soldered between the two parts to connect them. Illustration of what I mean:

![image](https://cdn.hackclub.com/019f14a4-757d-7220-82ec-be431c739827/image.png)

I haven't yet implemented this, but I'm thinking it would maybe work. Unless I have any better ideas this is what I'm going with. It also has the benefit of not needing to worry about routing collisions between the keyboard matrix and the power circuitry.

# 2026-06-27: PCB TIMEEEE!!!!

![image](https://cdn.hackclub.com/019f0be5-32ab-7702-900e-f4bd60a38d2a/image.png)

This project is rapidly approaching semi-finalization so I started working on the PCB layout !! Theres probably more to add to the schematic but for now I wanted to get a feel for how the PCB will look, and to get most of the work of laying it out done now.

First I had to assign footprints to all the parts which hadnt gotten ones yet, mostly capacitors and resistors. I also replaced a toggle switch which I had for the BOOT0 pin with a regualr push button.

Then I started working on the PCB. I know the rough size the PCB should be, about 10x6 centimeters, so I created a board outline of that size. Then I placed all the switches in the layout defined by the schematic. (I still havent placed out their respective diodes yet, just wanted to get their layout out first, and its almost 4 am).
![image](https://cdn.hackclub.com/019f0bdd-bd41-7f74-8928-4e067c57af4d/image.png)

Next, I placed out the rough position of the different larger components, such as the USB-C connector, the microcontroller, and the display connector.

Then I started going through all the capacitors and resistors, and one by one pairing them up with their respective components. The decouplers for the microcontroller I made sure to place as close to the physical pins as possible to reduce interference or something? Its something every datasheet and guide tells you to do, seems important. Anyways, I also placed the charging circuitry by the USB C.

Finally I added the BOOT0 button and the debugging headers. I still havent wired anything yet, wanted to get the layout done first, and I havent added the diodes yet, but other than that the PCB is coming together nicely. I am however a little concerned regarding whether I'll actually be able to fit all the wiring, especially for the keyboard matrix. If not, i'll have to make the board larger or find smaller footprints. Its not exactly the end of the world, but might mean I'll have to change some things around later on.

![image](https://cdn.hackclub.com/019f0be3-c1b1-71b4-8e14-bcf6ff76a7f7/image.png)

# 2026-06-26: Added Charge status LED :3

I added a yellow LED that glows while the device is charging! It stops when fully charged. I had already placed a symbol for an LED and Resistor but hadnt assigned any values or connected them properly.

The LiPo charging IC i use has a pin called CHG, which is low when the battery is actively being charged. I knew I wanted to have a status LED so I had added one when i first added the battery charger, but I realised now it was wired completely incorrectly, which i now corrected.

I chose a yellow LED since a green one feels more like "charge complete", and a red one misleadingly seems like it indicates low battery, while yellow feels more like "something is happening, but it isnt finished yet". I guess I couldve added different LEDs for different states or an RGB led, but i honestly find that unnecessary complexity. Now i can just use the builtin pin and nothing more which feels kinda clean. Also, I kind of have a pet peeve against devices that have LEDs that are always on (both my monitors in front of me right now have a bit of cardboard taped over their power LEDs), so i dont want more LEDs than necessary.

I found a suitable component on LCSC and assigned the footprint. However, I have no clue what value to give the resistor. I dont want the LED to be full brightness (probably?? unless max brightness is pretty dim), since again, i dont really want something being annoying by shining too brightly. Im not good enough with electronics to have a sort of intuition for LED brightness. I looked at some other open source projects using similar battery chargers (literally google searched for `BQ24040 site:github.com`) and they seemed to be like 1.5k or 1k, and since I'm using a yellow LED which i think is generally a bit dimmer than red ones, I opted to go with 1k. Might have a look at this again later but I think it will be fine.

![image](https://cdn.hackclub.com/019f069e-66db-7135-90a7-6c09b52dbfe7/image.png)

oo also i actually added a real readme.md to my repo :3

# 2026-06-26: Connected keyboard matrix + added power slide switch

(I'm using lapse for the first time so I think the time should be automatically logged to hackatime so im setting 0 hours for this journal to not double track)

I started by connecting the keyboard matrix to the microcontroller. The entire purpose of this kind of matrix is to reduce the amount of required GPIO pins, since almost no board has 57 IO pins, like I would need for this board if i didnt use a matrix. Instead I only need 17, 12 for the columns, and 5 for the rows.

This went quickly as there really wasnt any complexity to it. The only reason I hadnt done it sooner was because I didnt know which IO pins would be free (things like USB and SPI have specific IO pins that I cant freely move). Here is a view of the pins I use for the keyboard matrix:

![image](https://cdn.hackclub.com/019f04c3-a21c-79bf-8e7a-3d640b054d5b/image.png) 

![image](https://cdn.hackclub.com/019f04c4-61cb-7e46-b34b-dad0e5338e6f/image.png)

I also decided to add a power switch. I dont know whether I'll actually expose this on the final product, but i figured i'd add it for testing at the very least. Basically its just between the power output from the voltage regulator and the 3v3 line to allow the connection to be broken. I chose to use a slide switch since theyre nice for power switches and are clearly toggled on or off.

![image](https://cdn.hackclub.com/019f04c6-13a6-7de4-8dd5-43927c6dd7f4/image.png)

This also meant i didnt really need the Reset button I had previously placed, since you can just switch off and on the power instead, so I also removed that. Cool!

# 2026-06-26: Added connections for display port and debug headers

**Total time spent: 1.0 hours**

The ittyPDA will connect to the display using a 14 Pin FPC connector which I've now added and wired up on the schematic. The display actually has like great documentation so I know what each pin is for and I've connected them to the microcontroller. When developing for STM32 boards there's a software called STM32CubeMX which basically is to handle configuration of the boards, to set up things like USB or I2C connections. This is naturally just for the firmware (software side), but is really useful rn because I know the display uses SPI, so I can check where the SPI interfaces of my microcontroller are and wire them up accordingly.

My microcontroller has 3 Spi interfaces, and technically I think you can use any GPIO pins for SPI through software but using the builtin hardware SPI controllers is probably better. I chose to use the SPI3 interface, since its pins were located at the top of the microcontroller which I think will be good when routing the PCB (since the keyboard matrix will use a lot of the pins at the bottom and left side).

![image](https://cdn.hackclub.com/019f0453-62d8-7099-81d1-1b6b6ce854d3/image.png)

^schematic of the FPC connector. I've labeled all the pins according to the display's datasheet, except for the four pins used for the optional touch module that I dont have. I'm also unsure whether the connector should be flipped upside down and I will look into that later (im lazy).

Right, the display also has a microSD slot, which can also be read through this display interface, which is awesome!! I was going to add a microSD slot regardless, and I was amazed when I saw that the display I was gonna use already had one builtin that could be accessed through the same SPI connection.

I also added 4 debug headers:
![image](https://cdn.hackclub.com/019f0456-553d-7c99-b094-cbfd3f059165/image.png)

This is for flashing the device with firmware, but can also be used for like debugging memory and stuff. The downside is that you need a special ST-Link debugger to interface with them. I thankfully already randomly have one, and it looks like this:

![image](https://cdn.hackclub.com/019f045a-2254-73e2-9a7b-c9b0bf7dd761/image.png)
^actual image i took of mine

I believe you can also flash firmware to the device through USB if the BOOT0 button is held down, but I'm not entirely sure if this works out of the box and having this as a backup is a very obvious move. It also supplies power itself, so in case my final PCB has a broken USB port or power delivery circuit, this would still work to power and program the device.

Next, I will wire up the keyboard matrix I think :3

# 2026-06-25: Battery charging circuit!!!

**Total time spent: 2.0 hours**

I've now implemented the battery charging / power delivery circuit of the device! It consists of two parts, the actual LiPo charging IC, and the voltage regulator. The LiPo charger I use is the BQ24040, and it handles all the different phases and logic of charging a LiPo battery. This chip receives power from the USB-C port. It also has a status LED which lights up during charging, but is off when the device is fully powered.

The output current of the battery charging IC is then into the voltage regulator. This is because the battery is made to output 3.7V while the STM32 microcontroller only can handle up to 3.6V, though I intend to power it at 3.3V which is the general standard for these kinds of ICs.

The battery charging IC output is also of course connected to the battery's positive pole, through a JST PH 2.0mm connector. This is the connector that the battery I'm planning on using has.

![image](https://cdn.hackclub.com/019f0156-1831-7b8b-9e3e-371be748832b/image.png)

I was kind of scared of doing this part, since I know LiPo batteries can, explode. But I feel at least pretty comfortable since so much is handled by the premade IC. I struggled a bit with finding the right values for resistors and capacitors and stuff but generally the datasheets were pretty good, props to you TI. I also found [this](https://alpha.vovkey.com/calc.php?utm_source=Voltage-Regulator-Calculator.php) website useful for sanity checking resistor values for the linear voltage regulator. And also [this](https://www.youtube.com/watch?v=wy2Sr5DGSmc) youtube video was a goldmine for implementing this kind of thing !!!

# 2026-06-25: created a cool render :3

not a real journal entry, not tracking any time, just wanted to show this cool render i made :3

completely unnecessary but i was bored !!
![image](https://cdn.hackclub.com/019f00d8-8aec-717b-b781-c747138b1dfe/render.png)

^ the screen in the render has the size of the real thing, and the keyboard will probably look somewhat like that.

# 2026-06-25: Working on USB-C connectivity

**Total time spent: 2.0 hours**

This journal I worked on adding a USB-C connector. This doesnt seem that difficult but I quickly got sidetracked. What I know I want from the USB-C port is:
1. Power delivery (& battery charging)
2. Data transfer, you should be able to access the device's filesystem from a computer

But then I got to thinking, perhaps I should use USB OTG so the device can act both as host and client, so you could perhaps plug in a keyboard, and the device would then switch to host mode to accept that input device. When investigating this I discovered that my chosen IC, the STM32F103RGT6, cant do USB OTG, so I looked at which STM32 chips could, and discovered the STM32F401RET6. It supported USB OTG, and also had much better specs, and also was like $3 cheaper than the previous board???? I actually decided to switch to this IC altogether. I dont know whether I will implement USB OTG, but having the option to do so later down the road, as well as it being cheaper with better specs, it was a clear winner. 

I want to emphasize that I don't *need* this new board for USB-C altogether (only if I want OTG), I just happened down a rabbit hole while researching boards with USB OTG and saw that it had better specs and was cheaper.

This meant I had to redo some circuitry but it wasn't too bad. I wired the USB connector according to the old Blueprint Devboard guide, for now.

![image](https://cdn.hackclub.com/019f006d-5118-7212-8436-1b90c4d13371/image.png)

^ the VBUS of the USB isnt connected anywhere yet, will get to that once I start implementing the charging circuits


I also added a crystal oscillator, which is required for USB data transfer. 

![image](https://cdn.hackclub.com/019f006d-8d18-748c-96ce-4d06fbdd8232/image.png)

# 2026-06-25: POWER !!!

**Total time spent: 2.0 hours**

Hiya!! Today I have chosen the chip for the device: the STM32F103RGT6 !! It's in the "mainstream" tree of STM32 chips, which I seems good since I just want a balanced chip that is thoroughly documented. Initially I was looking at a chip under their "performance" family, but reading the datasheet I could tell it would be a lot harder to work with since it had a lot more features and just seemed a bit overkill.

I also spent some time looking at how I would supply power to the device. Of course it will have a battery, but initially I didn't know whether to use a premade battery charging module (like you can find on aliexpress, looks like this: ![image](https://cdn.hackclub.com/019eff4c-da7d-738b-bdbd-c2c17d5d695f/image.png))
, or to make my own and have it also part of the mainboard. I have decided to try implement the battery charging myself on the mainboard. This is because I want to be able to also do data transfer over the USB-C port. It also is more space efficient, so yay. Thankfully, after researching a bit about LiPo chargers, I realised that there are ICs made to basically handle the entire charging process, which will make everything easier, and is also comforting so I dont need to worry about my battery blowing up. I will probably use the Texas Instruments BQ24040 or a related chip. The process of charging a LiPo battery is suprisingly involved with different charging stages and more.

Since I finally chose the STM32F103RGT6, I started adding it to the schematic. I added decouplers and switches for the BOOT0 and NRST pins, but thats all I did so far.
![image](https://cdn.hackclub.com/019eff50-6580-7781-9650-80866fef55e5/image.png)

I haven't connected the VBAT power pin yet, and I'll get to that once I add the charging circuitry. As I understand it, it is basically an optional backup power source to keep the Real Time Clock running. So I can cut power to the regular power pins to turn off the device, but still supply a tiny bit of current to the VBAT pin to make sure the device keeps the time even after shut off. This might be wrong, just how I understood it, but seems very useful !!

Will work on USB-C or charging circuit next!!

# 2026-06-23: Project start!!

**Total time spent: 4.0 hours**

Hiya! I am designing a CyberDeck / PDA device ! I know it won't be perfect but it sure as heck will be whimsical! It would have the form factor of roughly a smaller Nintendo DS, with a full keyboard. Its designed to be used for notetaking as well as potentially games and being an e-reader. 
I made both a physical and 3D mockup of the idea, the physical one I mainly used to see whether such a small keyboard would be comfortable to use. The way I want to design the device is with a single mainboard PCB that both connects the keys of the keyboard as well as houses the IC chip. I will probably use a STM32 chip, since theyre cheap, power efficient, and I happen to own a STM32 debugger/flasher. It will also have some sort of rechargable battery (probably a LiPo). Having the keyboard and the IC be the same PCB does have the potential downside that if either part breaks, the entire board has to be reordered, but I think given the size constraints and complexity it would be a lot easier still to have it as one.
![image](https://cdn.hackclub.com/019ef58c-6fb7-730d-8dc3-c8c65ad13680/pipapda.png)

I spent a lot of the time of this journal researching parts to ensure that the hardware for project would be easily accesible and cheap. Finding a suitable display was the hardest, I really wanted an OLED but finding an OLED of roughly 3.5-4 inches was really difficult, so instead I will probably use a small 3.5" LCD display that I found on amazon for a really good price. The display also has a FPC connector, so I'll probably add one of those to the mainboard so the display can easily be connected with a single FPC cable.
I started also researching how a keyboard matrix works, following some hackclub guides and a youtube series. I knew the PCB would have to be ordered assembled, as given the size constraints I wouldnt be able to manually solder it, so I picked the electronics parts from JLCPCB's parts, to make future assembly easier (and cheaper since I can get parts they have in the "basic" category). I had to figure out how to import JLCPCB parts into kicad (solution: a python tool called easyeda2kicad), and then designed this simple board:
![image](https://cdn.hackclub.com/019ef58e-de55-71e3-85fd-06dca39df24c/pcbkeyb.png)

I chose to make the keyboard 12x5 (12 columns and 5 rows), since it has the minimum amount of keys I need. I am also from sweden so I'll also use a swedish keyboard layout.

Thats where I am right now! I've never completed a project like this before so I'm learning a lot but that also means I'm progressing quite slowly, but I really intend on finishing this project, no matter what! Wish me luck!! <3

