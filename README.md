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
```
git clone https://github.com/PromZona/DwarfDungeon
cd DwarfDungeon
git submodule update --init
```
2) Get godot extension_api.json and place in DwarfDungeon directory
```
godot  --dump-extension-api
mv extension_api.json <path to DwarfDungeon directory>
```
3) compile and build
```
cmake --preset debug-clang
cd build
ninja install
```
> [!NOTE]
> Building godot-cpp for the first time takes some time 

4) Run godot project
