# Notes on memory management

## c++ way
new + delete or smart pointers do not initialize godot::Object properly, so it is better to use godot memory allocator functions.

## memnew
To initialize GDExtension objects we must call ```memnew``` because of this
([link](https://github.com/godotengine/godot-cpp/issues/1057#issuecomment-1454665074))
```
GDExtension objects must indeed be instantiated using memnew which ends up calling the Wrapped::_postinitialize method as you identified.
```
memenew is THE way to initialize single instance of the C++ class that is inheretied from godot::Object

## memnew_arr
```memnew_arr``` does not call ```memnew```, therefore it can not be used to allocate memory for gdextension classes, but will work for anything else

## memalloc + memnew_placement
memalloc allows to allocate memory by size and then initialize it.
```
Node2D* nodes = (Node2D*)memalloc(sizeof(Node2D) * size);
memnew_placement(&nodes[0], Node2D());
```
This combination allows us to pre-allocate big chunk of memory and reuse it.
