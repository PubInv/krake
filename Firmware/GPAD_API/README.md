# Building

## PlatformIO IDE (Visual Studio Code Extension)
1. Install the Visual Studio Code (VS Code) text editor
    - [Website](https://code.visualstudio.com/)
2. Install the [PlatformIO](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) VS Code extension
    - The provided link has instructions on how install the extension in VS Code
3. The PlatformIO project needs to be opened
    - Click the PlatformIO logo/extension on the left tool bar
    - Under the `QUICK ACCESS > PIO Home` section, selection `Projects & Configuration`
    - At the top, select `Add Existing` and select this directory
4. To compile, select the PlatformIO logo/extension on the left again if it is not still selected
    - Under `esp32dev > General` select `Build` and the firmware image will be built
    - There are additional options for uploading the firmware to the device and monitoring

## PlatformIO from Command Line

Some users prefer not to use VSCode (those who prefer Emacs, Vim, subtext, etc.)

In the this directory, there is a "make" file. If you have make installed, you can run "make". There are two targets:

    1. run --- this does a compile and upload, and begins running a "monitor", which prints the output of the serial monitor and allows commands to be typed in, just as they are typically done in the Arduino IDE.
    2. monitor --- this does a reset and runs the monitor without doing a fully reset.

These commands are implemented on the command line as:

```
    make run
    make monitor
```

Remember, with our current hardware, to prepare the Krake to have firmware uploaded, you need to perform these actions:
1.      Connect Krake to power and to the computer
2.      Press and hold the BOOT button.
3.      While still holding BOOT, press and release the RESET button once.
4.      Release the BOOT button.
5.      The chip is now in boot mode. You can run your flasher and the port should respond.
