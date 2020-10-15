# zephyrdemo
Zephyr example application

## Some Instructions

### Critical Notes
This repo must live under the same directory that you created the zephyr workspace in. See the getting started guide: https://docs.zephyrproject.org/latest/getting_started/index.html

At this time, only the qemu_x86_64 target works. qemu_x86 and the ARM qemu boards do not work for some reason. The timers appear not to work correctly. Did not debug in-depth.

### Build and Run
```
west build -b qemu_x86_64 .
west build -t run
```
