```bash
mkdir build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=YES -G Ninja ..
mv compile_commands.json ..
ninja
```
