## README ##

Online Judge - OpenCL

This script is written to solve Memory Leak with OpenCL temporarily.

### Install ###

```
root $ mv memusage.py ~
```

```
$ crontab -e
```

detect each 15 seconds

```
# m h  dom mon dow   command
* * * * * ./memusage.py >mem.log
* * * * * sleep 15; ./memusage.py >mem.log
* * * * * sleep 30; ./memusage.py >mem.log
* * * * * sleep 45; ./memusage.py >mem.log
```

### fstab ###

```
/etc/OpenCL
```
