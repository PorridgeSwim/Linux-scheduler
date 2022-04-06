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
We used Use the ps command before and after this part, to verify that our addition to the kernel had no effect.

Part7:
 SCH POL PSR %CPU  C     PID USER     CMD
 .
 .
 .
 0 TS    2  0.0  0    2646 debbie    \_ bash
 7 #7    1 43.5 43    2657 debbie        \_ python test.py
 7 #7    1 40.3 40    2658 debbie        \_ python test.py
 7 #7    1 40.3 40    2659 debbie        \_ python test.py

We copied lots of code, deleted lots of code, add some codes. It can works at the end, but we still are not so
clear how it works.

part8:

Our solution works. In this part, we make Freezer the default scheduling policy. We modified 'linux/init/init_task.c', 'linux/kernel/kthread.c', 'linux/kernel/sched/core.c' to set the Freezer as the default policy.

Here is part of the `ps` command result. Most of processes' SCH have become '7'. The result shows 20 CPU-bound processes(not counting the four parent processes that forked them) running across 4 CPUs:

SCH POL PSR %CPU  C     PID USER     CMD
  7 #7    0  0.0  0       2 root     [kthreadd]
  7 #7    0  0.0  0       3 root      \_ [rcu_gp]
  7 #7    0  0.0  0       4 root      \_ [rcu_par_gp]
  7 #7    0  0.0  0       6 root      \_ [kworker/0:0H-events_highpri]
  7 #7    0  0.0  0       9 root      \_ [mm_percpu_wq]
  7 #7    0  0.0  0      10 root      \_ [rcu_tasks_rude_]
  7 #7    0  0.0  0      11 root      \_ [rcu_tasks_trace]
  7 #7    0  0.0  0      12 root      \_ [ksoftirqd/0]
  7 #7    0  0.0  0      13 root      \_ [rcu_sched]
  1 FF    0  0.0  0      14 root      \_ [migration/0]

...

  7 #7    0  0.1  0    1298 pg2676   xfce4-terminal
  7 #7    3  0.0  0    1303 pg2676    \_ bash
  7 #7    1  0.0  0   72255 pg2676    |   \_ ./program
  7 #7    1 19.9 19   72256 pg2676    |   |   \_ ./program
  7 #7    1 19.9 19   72257 pg2676    |   |   \_ ./program
  7 #7    1 19.8 19   72258 pg2676    |   |   \_ ./program
  7 #7    1 19.8 19   72259 pg2676    |   |   \_ ./program
  7 #7    1 19.9 19   72260 pg2676    |   |   \_ ./program
  7 #7    0  0.0  0   72261 pg2676    |   \_ ./program
  7 #7    0 19.9 19   72262 pg2676    |   |   \_ ./program
  7 #7    0 19.8 19   72263 pg2676    |   |   \_ ./program
  7 #7    0 19.8 19   72264 pg2676    |   |   \_ ./program
  7 #7    0 19.9 19   72265 pg2676    |   |   \_ ./program
  7 #7    0 19.9 19   72266 pg2676    |   |   \_ ./program
  7 #7    2  0.0  0   72267 pg2676    |   \_ ./program
  7 #7    2 20.0 20   72268 pg2676    |   |   \_ ./program
  7 #7    2 20.1 20   72269 pg2676    |   |   \_ ./program
  7 #7    2 20.0 20   72270 pg2676    |   |   \_ ./program
  7 #7    2 20.0 20   72271 pg2676    |   |   \_ ./program
  7 #7    2 19.9 19   72272 pg2676    |   |   \_ ./program
  7 #7    3  0.0  0   72277 pg2676    |   \_ ./program
  7 #7    3 19.9 19   72278 pg2676    |       \_ ./program
  7 #7    3 20.0 20   72279 pg2676    |       \_ ./program
  7 #7    3 19.8 19   72280 pg2676    |       \_ ./program
  7 #7    3 19.4 19   72281 pg2676    |       \_ ./program
  7 #7    3 19.5 19   72282 pg2676    |       \_ ./program
  7 #7    0  0.0  0    1317 pg2676    \_ [bash] <defunct>
  7 #7    3  0.0  0   72219 pg2676    \_ bash
...

