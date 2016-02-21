## Judge-receiver ##

例如要遠端連到 `butler@a.a.a.a`，在 butler 的家目錄配置如下，權限部份請特別小心 sandbox 的執行檔，必須 `chmod +s sandbox`。

```
root@supermicro:/home/butler# ls -l
total 832
-rwxr-xr-x 1 butler butler    770 Jan 23 16:06 butler
-rw-r--r-- 1 root   root      305 Jan 23 16:06 config.h
-rw-r--r-- 1 root   root      274 Jan 23 16:06 GNUmakefile
-rw-rw-r-- 1 root   root        5 Feb 18 00:00 log
-rwsr-sr-x 1 root   root    18457 Jan 23 16:06 sandbox
-rw-r--r-- 1 root   root     3281 Jan 23 16:06 sandbox.c
drwxr-xr-x 2 root   root     4096 Sep 12 19:06 script
-rw-r--r-- 1 root   root      106 Jan 23 16:06 test.c
-rwxr-xr-x 1 butler butler 795864 Jan 23 16:06 tiger
-rw-r--r-- 1 root   root      449 Jan 23 16:06 tiger.c
```

```
root@supermicro:/home/butler# tree
.
├── butler
├── config.h
├── GNUmakefile
├── log
├── sandbox
├── sandbox.c
├── script
│   ├── fstab
│   ├── mount
│   └── umount
├── test.c
├── tiger
└── tiger.c
```

編輯一下 `.bashrc` 的環境，等下遠端過來操作沙盒時，需要直接當作指令執行 butler 家目錄下編譯好的執行檔

```
root@supermicro:/home/butler# vim .bashrc
```

在 `.bashrc` 加入

```
export PATH=$HOME:$PATH
```

更新一下剛剛的設定

```
root@supermicro:/home/butler# source .bashrc
```

測試是否有成功，確定 `echo $PATH` 中出現 `/home/butler`。

```
root@supermicro:/home/butler# su butler
butler@supermicro:~$ echo $PATH
/home/butler:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games
```


### 準備沙盒 ###

* 確定 server 上有 `cgroup` 功能，在某些舊 kernel 並沒有我們需要限制沙盒的功能 (目前部分版本的 CentOS 和 Debian 沒有支持)。

```
$ ls /sys/fs/cgroup/memory
```

確定 `memory.limit_in_bytes` 存在在清單中。(可以使用 `$ mount` 找到 `cgroup` 在哪個資料夾。)

* 在根目錄建造沙盒 `/sandbox`，資料夾架構如下：(特別注意到 `chmod +s sandbox` 的權限配置)

```
root@supermicro:/# ls -l
total 104
drwsrwsrwx  13 root root  4096 Feb 25  2015 sandbox
```

* 建立 sandbox 資料夾目錄，執行 `./script/prepare` 會創建空的資料夾，接著執行 `./script/mount` 將跟目錄下的這些資料夾 mount 到 sandbox 下。結果大致如下：

```
root@supermicro:/sandbox# tree -L 2
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

* 確認 `sandbox/sandbox/app`、`sandbox/sandbox/ws` 的權限配置如下，uid 與 gid 皆為 butler。如果權限不對，執行 `$ chown butler:butler app ws` 解決此問題。

```
root@supermicro:/sandbox/sandbox# ls -l
total 8
drwxr-xr-x 2 butler butler 4096 Feb 18 00:00 app
drwxrwxrwx 2 butler butler 4096 Feb 13 23:41 ws
```
