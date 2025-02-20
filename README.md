# Dwarf Dungeon
A Game.

## Requirements
- cmake
- godot v4.3
- ninja
- clang++ compiler

> [!NOTE]
> Compiler and generator can be overwritten with CMakeUserPresets.json file if required.

## Build
1) Clone Repository and Submodules
```sh
git clone https://github.com/PromZona/DwarfDungeon
cd DwarfDungeon
git submodule update --init
```
2) Get godot extension_api.json and place in DwarfDungeon directory
```sh
godot  --dump-extension-api
mv extension_api.json <path to DwarfDungeon directory>
```
3) Configure, compile, install
```sh
cmake --preset debug-clang
cmake --build --preset debug-build --target install
```
or

If you want auto-detection of tools
```sh
cmake -S . -G <Your Generator:ninja/make> -B <build directory>
cd <build directory>
<ninja/make> install
```
> [!NOTE]
> Building godot-cpp for the first time takes some time 

4) Run godot project

### Web build
1) Install Emscripten

Download and install instructionsa are [here](https://emscripten.org/docs/getting_started/downloads.html)

2) Run cmake with emcmake

In DwarfDungeon root directory 
```sh
emcmake cmake --preset debug-web
emcmake cmake --build --preset web-build
```

3) Godot export

3.1) Download default web template from godot.

3.2) Set **Variant->Extension Support** to true

3.3) Set **Progressive Web App->Enabled** to true (optional?)

3.4) Press Export Project

