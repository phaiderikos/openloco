directory Core

source /home/davide/workspace/svd-tools/gdb-svd.py

svd /home/davide/workspace/svd/STM32L4x6_v1r1.svd

disconnect

target remote localhost:3333
