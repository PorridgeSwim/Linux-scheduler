-You Zhou yz3883, Aoxue Wei aw3389, Panyu Gao pg2676
-HW6
-	Description for each part

Part6: We added structures for freezer to the following files:
linux/include/asm-generic/vmlinux.lds.h
linux/include/linux/sched.h
linux/kernel/sched/core.c
linux/kernel/sched/sched.h
We added freezer's .c file as
kernel/sched/freezer.c
We also edited makefile to compile this
user/test/Makefile

Part7:
 SCH POL PSR %CPU  C     PID USER     CMD
 .
 .
 .
 0 TS    2  0.0  0    2646 debbie    \_ bash
 7 #7    1 43.5 43    2657 debbie        \_ python test.py
 7 #7    1 40.3 40    2658 debbie        \_ python test.py
 7 #7    1 40.3 40    2659 debbie        \_ python test.py

