# Ilo Musi Game Template

This is a game template designed to be used as a starter code for developing new games for Ilo Musi game console.

It also demonstrates the basic usage of the system library of Ilo Musi.

# Gameplay

It's a very simple game that shows a coin in the center. The player can control the movement of coin by using the left/right/up/down keys. Movement of the coin consumes MP. MP gets replenished over time. There's a text showing the MP at the bottom-left corner.

The coin's animation can be enabled by key 1 and disabled by key 2.

A looping background music is played throughout the game. When either key 1 or key 2 is pressed, a coin sound effect would be played. In addition, the current game state would be saved, which will be loaded upon the next time the game is loaded.


# Building and Running

1. Install the cross compiler `arm-none-eabi-` to your machine.
2. Clone this repository to download the scripts that's used for creating files of a custom graphical image format and packing game ROM: [https://github.com/ilo-musi/scripts](https://github.com/ilo-musi/scripts)
3. Modify `GAME_NAME`, `ROM_PACKER` and `IMAGE_CONVERTER` in Makefile
4. Run `make all` to build the game binary
5. Run `make pack` to pack the game binary and other assets like graphical images into the game ROM in ILM format
6. Copy `./bin/<game name>.ilm` to a microSD card
7. Insert that microSD card to Ilo Musi.
8. Power up Ilo Musi. Select the game with the menu to start it.

# Debugging

1. Install openocd and gdb
2. Build the game. See the section above. This has to be performed again if any resources other than resource #0 of the ROM is modified.
3. Rename the ROM file to "DEBUGRES.IMG" and put it into the root directory of the microSD card to be inserted into the game console. This has to be performed again if any resources other than resource #0 of the ROM is modified.
4. Run `make run` for linux (and possibly also for Mac OS X), `make run-windows-x86` or `make run-windows-x64` for Windows
5. Run gdb. And run the following commands inside gdb:

```
target remote localhost:3333
set remote hardware-breakpoint-limit 4
set remote hardware-watchpoint-limit 2
```

6. Now you can use gdb to debug the game as if it's a usual program running on the host machine, except that special care has to be taken when you're pausing and resuming the program. To pause the program while it's running, use `monitor halt`. Do not use gdb interrupt (CTRL+C). To resume the program, if you're expecting to pause at the next breakpoint, use `continue`. If you're expecting to pause by running `monitor halt` after resuming, use `monitor resume`.

Remarks: ddd is a decent GUI for debugging games made for this game console. It works well on linux. You could even create a custom button that run the gdb commands I've shown you above with ddd. Unfortunately, I'm yet to find any decent front-end for debugging games on Windows.

# About the Generated Game Rom and Assets

Each game ROM contains up to 256 resources. Each resource can contain arbitrary binary data. The 0th resource contains the game's binary. Other resources can contain graphical image, music, saved data, another game binary, etc.

Custom formats are used for game assets, including graphical image (or animation) and music. In addition, the game ROM itself is also a custom format. [Python scripts for generating files of these custom formats are available in this repository](https://github.com/ilo-musi/scripts).

# License

This repository contains code written by third party, which is located in the external/ directory. Currently the only third party code is the startup code available for download on STMicroelectronics' website, which is released under 3-clause BSD license.

Other than third party code, all material in this repository are public domain.

In long run, the third party code should be rewritten so that this game template would be fully public domain.
