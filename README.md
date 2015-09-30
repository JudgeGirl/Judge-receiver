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

1. make sure `cgroup` work on server. (CentOS not work, Ubuntu OK.)
2. mount environment run `./mount` in script.
