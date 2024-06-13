target extended-remote :3333

file build/software.elf

mon reset halt

source post-connect.gdb
