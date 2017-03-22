## MIC/Xeon Phi ##


### Library ###

```
sudo ln -s /usr/lib64/libcoi_host.so.0 /usr/lib/libcoi_host.so.0
sudo ln -s /tools/intel/parallel_studio_xe_2016/compilers_and_libraries_2016.3.210/linux/compiler/lib/intel64/libiomp5.so /usr/lib/libiomp5.so
sudo ln -s /tools/intel/parallel_studio_xe_2016/compilers_and_libraries_2016.3.210/linux/compiler/lib/intel64/liboffload.so.5 /usr/lib/liboffload.so.5
sudo ln -s /tools/intel/parallel_studio_xe_2016/compilers_and_libraries_2016.3.210/linux/compiler/lib/intel64/libimf.so /usr/lib/libimf.so
sudo ln -s /tools/intel/parallel_studio_xe_2016/compilers_and_libraries_2016.3.210/linux/compiler/lib/intel64/libsvml.so /usr/lib/libsvml.so
sudo ln -s /tools/intel/parallel_studio_xe_2016/compilers_and_libraries_2016.3.210/linux/compiler/lib/intel64/libintlc.so.5 /usr/lib/libintlc.so.5
sudo ln -s /tools/intel/parallel_studio_xe_2016/compilers_and_libraries_2016.3.210/linux/compiler/lib/intel64/libirng.so /usr/lib/libirng.so
```

### fstab ###


```
dev/mic/
dev/mic0
dev/mic1
lib64/
sys/class/mic/
sys/devices/
tools/
```

Use `chmod 660 /dev/mic*` to set access authority.

```
morris1028@Mermaid /dev> ls -la mic*
crw-rw-rw- 1 root research 247, 2 Jul 23 10:36 mic0
crw-rw-rw- 1 root research 247, 3 Jul 23 10:36 mic1

mic:
total 0
drwxr-xr-x  2 root research     80 Jul 23 10:36 ./
drwxr-xr-x 19 root root       4680 Jul 23 11:18 ../
crw-rw-rw-  1 root research 247, 0 Jul 23 10:36 ctrl
crw-rw-rw-  1 root research 247, 1 Jul 23 10:36 scif
```
