# minigame-example
A stripped-down minigame pulled from the code I use for my minigames at https://joshpowlison.com/minigames

The goal is to help you learn how to get started creating web games with data shared between JS and C compiled to WASM.

## Compiling C to WASM (Windows)
1. Download and install LLVM (when installing, add LLVM to the system PATH for all users). To make it easy, go down to "Pre-Built Binaries" and and get the "Windows (64-bit)" version from this page: https://releases.llvm.org/download.html#11.0.1
	
2. In the console, in the same folder as the game, run `clang --target=wasm32 -Os -flto -nostdlib -std=c99 -Wl,--no-entry -Wl,--export-all -o script.wasm script.c` (You could replace the -std with a version of C++, and remove -Os, which does strong compression; these are just the settings I'm using right now)

If the compilation fails, you'll get errors explaining why. Otherwise, if it works, clear your browser cache and refresh the game page to see the updates.

## Looking for an engine?
The purpose of this repo is to help you learn how you could build a JS and C game engine and game yourself. This is not meant to develop out into a public game engine.

If you're looking for a game engine in C that exports to the web, consider [RAYLIB](https://www.raylib.com/).
