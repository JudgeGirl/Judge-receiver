## Judge-receiver ##

For example, receiver `maplewing@140.112.xxx.xxx`

```
+ /home/maplewing
|
+-+- script
| |
| +--- fstab, mount, umount
|
+-+- slave
  |
  +--- butler, config.h, GUNmakefile, ...
```

### Prepare ###

1. make sure `cgroup` work on server.


```
$ cd /sys/fs/cgroup/memory
```

check `memory.limit_in_bytes` (use `$ mount` get path of cgroup)

2. make dirctionary `/sandbox`, below file structure

```
sandbox/
├── bin
├── dev
├── etc
├── lib
├── lib64
├── opt
├── proc
├── sandbox/
|   ├── app
|   └── ws
├── sys
├── tmp
└── usr

```

3. mount environment run `./mount` in script.

4. make sure `sandbox/sandbox/app sandbox/sandbox/ws` with your account `butler`

```
morris1028@supermicro:/sandbox/sandbox$ ls -la
total 16
drwxr-xr-x  4 root      root      4096 Feb 25  2015 .
drwxrwxrwx 13 root      root      4096 Feb 25  2015 ..
drwxr-xr-x  2 butler    butler 4096 Oct 19 17:30 app
drwxrwxrwx  2 butler    butler 4096 Sep  7 02:43 ws
```

if not, use `$ chown butler:butler ws` and `$ chown butler:butler app` fixed this problem.

5. make sure `/sandbox` with `chmod +s sandbox`.

