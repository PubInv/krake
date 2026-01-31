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

### PlatformIO Core (CLI) Installation
The official [documentation](https://docs.platformio.org/en/latest/core/installation/shell-commands.html#piocore-install-shell-commands) 
has great guide on how to install the CLI tooling. [Here](https://docs.platformio.org/en/latest/core/userguide/index.html#piocore-userguide)
is PlatformIO's guide on how to use the CLI command `pio`. The full guide for PlatformIO Core installation is available [here](https://docs.platformio.org/en/latest/core/installation/index.html). 

As the documentation says, you **do not** have to follow all steps in the CLI guide if you have **also** installed the VS Code extension, PlatformIO IDE. It will be advised to follow the directions under the documentation for "Install Shell Commands" to be able to run the commands `pio` and `piodebuggdb` anywhere in the terminal. For the `Makefile`
mentioned below, it is required to install the shell commands for the `make` command to function correctly.

### Build
In the this directory, there is a "make" file. If you have make installed, you can run "make". There are two targets:

    1. run --- this does a compile and upload, and begins running a "monitor", which prints the output of the serial monitor and allows commands to be typed in, just as they are typically done in the Arduino IDE.
    2. monitor --- this does a reset and runs the monitor without doing a fully reset.

These commands are implemented on the command line as:

```
    make run
    make monitor
```

# Building LittleFS Filesystem
With PlatformIO, the creation of the `LittleFS` is a separate step from building the firmware image. However, it only has to be built if
there have been changes to the `/data` directory and it only has to be uploaded to the device if the the image has changed or the flash
pages containing the file system have been erased.

## PlatformIO IDE (VS Code Extension)
In the extension side panel, below the `General` section with the `Build` and `Upload` targets, there is another section, `Platform`. Under
the `Platform` section, there are the `Build Filesystem Image` and `Upload Filesystem Image`. Using the `Upload Filesystem Image` target will
build and upload the file system if the file system needs building.

## PlatformIO Core (CLI)
The full command is `pio run --target uploadfs` but the `Make` target `make uploadfs` can be used.

# Krake Installation
Installing/flashing a new firmware image requires the device, Krake, to be set into a state for it to allow a new firmware image. That can be accomplished with the following steps:
1. Connect Krake to power and to the computer
2. Press and hold the BOOT button.
3. While still holding BOOT, press and release the RESET button once.
4. Release the BOOT button.
5. The chip is now in boot mode. You can run your flasher and the port should respond.
