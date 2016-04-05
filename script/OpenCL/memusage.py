#!/usr/bin/env python3
import os
import sys
import subprocess

cmd = "free -m | awk 'NR==2{printf \"%.2f\",$3*100/$2 }'"
out = subprocess.check_output(cmd, shell=True)
mem_usage = float(out)

if mem_usage >= 90:
    print('WARNING: Memory Usage too High')
    os.system('reboot')
else:
    print('Memory Usage SAFE %f%%' % (mem_usage))
