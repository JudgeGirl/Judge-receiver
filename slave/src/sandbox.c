#undef NDEBUG
#define _GNU_SOURCE
#include<assert.h>
#include<signal.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sched.h>
#include<sys/resource.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include"config.h"

static pid_t slave;
static bool TLE;

static inline unsigned int GetTickCount() {
	struct timespec tp;
	assert(clock_gettime(CLOCK_MONOTONIC_RAW,&tp) == 0);
	return tp.tv_sec*1000u+tp.tv_nsec/1000000;
}

static void handler(int sig) {
	TLE=true;
	kill(slave,SIGKILL);
}

static inline void dummy(int arg) {
}

static void do_master(int TL,long long ML) {
	dummy(system("cgdelete memory:/sandbox"));
	assert(system("cgcreate -t nobody:nogroup -g memory:/sandbox")==0);
	FILE *fp=fopen(CGROUP_FS "memory/sandbox/memory.limit_in_bytes","w");
	assert(fp!=NULL);
	fprintf(fp,"%lld\n",ML);
	assert(fclose(fp)==0);
	int status;
	assert(waitpid(slave,&status,WUNTRACED)==slave);
	assert(WIFSTOPPED(status)&&WSTOPSIG(status)==SIGSTOP);
	signal(SIGALRM,handler);
	unsigned int t=GetTickCount();
	alarm(TL);
	assert(kill(slave,SIGCONT)==0);
	assert(waitpid(slave,&status,0)==slave);
	alarm(0);
	t=GetTickCount()-t;
	dummy(system("killall -u nobody"));
	bool MLE=false;
	fp=fopen(CGROUP_FS "memory/sandbox/memory.failcnt","r");
	assert(fp!=NULL);
	if(getc(fp)!='0'||getc(fp)!='\n'||getc(fp)!=EOF)MLE=true;
	assert(fclose(fp)==0);
	long long m;
	fp=fopen(CGROUP_FS "memory/sandbox/memory.max_usage_in_bytes","r");
	assert(fp!=NULL);
	assert(fscanf(fp,"%lld",&m)==1);
	assert(fclose(fp)==0);
	fp=fopen(SLAVE_LOG,"w");
	assert(system("echo \"OPEN SLAVE w\" > ~/log") == 0);
	assert(fp!=NULL);
	assert(system("echo \":OK\n\" > ~/log") == 0);
	if(MLE) {
		fputs("MLE\n",fp);
	} else if(TLE) {
		fputs("TLE\n",fp);
	} else if(!WIFEXITED(status)||WEXITSTATUS(status)!=0) {
		if (!WIFEXITED(status))
			assert(system("echo \"ERROR: 1\" > ~/log;") == 0);
		else 
			assert(system("echo \"ERROR: 2\" > ~/log;") == 0);
		fputs("RE\n",fp);
	} else {
		fputs("OK\n",fp);
	}
	fprintf(fp,"%u\n%lld\n",t,m);
	assert(fclose(fp)==0);
}

static void do_slave(char *argv[]) {
	assert(unshare(CLONE_FILES) == 0);
	assert(unshare(CLONE_FS) == 0);
	assert(unshare(CLONE_NEWIPC) == 0);
	assert(unshare(CLONE_NEWNET) == 0);
	assert(unshare(CLONE_NEWNS) == 0);
	assert(unshare(CLONE_NEWUTS) == 0);
	assert(unshare(CLONE_SYSVSEM) == 0);
	assert(chdir(CHROOT) == 0);
	assert(chroot(".") == 0);
	assert(chdir(CHDIR) == 0);
	assert(setgid(NOGROUP) == 0);
	assert(setuid(NOBODY) == 0);
	struct rlimit rlim;
	rlim.rlim_max=NPROC_LIMIT;
	rlim.rlim_cur=rlim.rlim_max;
	assert(setrlimit(RLIMIT_NPROC,&rlim) == 0);
	assert(raise(SIGSTOP) == 0);

	{
		struct sched_param param;
		param.sched_priority = sched_get_priority_max(SCHED_FIFO)-1;
		assert(sched_setscheduler(0, SCHED_FIFO, &param) == 0);
	}

	assert(execvp(argv[0],argv) != -1 && 0);
}

int main(int argc,char *argv[]) {
	assert(argc > 3);
	const int TL = atoi(argv[1]);
	assert(TL >= 0);
	const long long ML = atoll(argv[2]);
	assert(ML >= -1);
	assert(setuid(0) == 0);
	assert(setgid(0) == 0);
	assert(system("swapoff -a") == 0);

	{
		struct sched_param param;
		param.sched_priority = sched_get_priority_max(SCHED_FIFO);
		assert(sched_setscheduler(0, SCHED_FIFO, &param) == 0);
	}

	slave = fork();
	if (slave == 0) {
		char **args=malloc((argc+1)*sizeof(char*));
		args[0] = "cgexec";
		args[1] = "-g";
		args[2] = "memory:/sandbox";
		for(int i = 3; i <= argc; i++)
			args[i] = argv[i];
		do_slave(args);
	}
	assert(slave != -1);
	do_master(TL, ML);
	return 0;
}
