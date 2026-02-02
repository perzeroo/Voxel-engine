# Voxel Engine
## Build
Navigate to the project root and run:
```bash
cmake -G "Ninja" -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=RelWithDebInfo -B build
```
then
```bash
cmake --build build
```
