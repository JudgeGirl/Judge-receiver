#undef NDEBUG
#define _GNU_SOURCE
#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "config.h"

static pid_t slave;
static bool TLE;

static inline unsigned int GetTickCount() {
	struct timespec tp;
	assert(clock_gettime(CLOCK_MONOTONIC_RAW, &tp) == 0);
	return tp.tv_sec*1000u + tp.tv_nsec/1000000;
}

static void handler(int sig) {
	TLE = true;
	kill(slave, SIGKILL);
}

static inline void dummy(int arg) {
}

static void do_master(int TL,long long ML) {
	dummy(system("cgdelete memory:/sandbox"));
	assert(system("cgcreate -t nobody:nogroup -g memory:/sandbox")==0);
	{
		FILE *fp = fopen(CGROUP_FS "memory/sandbox/memory.limit_in_bytes","w");
		assert(fp != NULL);
		fprintf(fp, "%lld\n", ML);
		assert(fclose(fp) == 0);
	}
	int status;
	assert(waitpid(slave,&status,WUNTRACED)==slave);
	assert(WIFSTOPPED(status)&&WSTOPSIG(status)==SIGSTOP);
	signal(SIGALRM,handler);
	unsigned int t = GetTickCount();
	alarm(TL);
	assert(kill(slave, SIGCONT)==0);
	assert(waitpid(slave,&status,0)==slave);
	alarm(0);
	t=GetTickCount()-t;
	dummy(system("killall -u nobody"));
	bool MLE = false;
	{
		FILE *fp = fopen(CGROUP_FS "memory/sandbox/memory.failcnt","r");
		assert(fp != NULL);
		if (getc(fp)!='0'||getc(fp)!='\n'||getc(fp)!=EOF) MLE = true;
		assert(fclose(fp)==0);
	}
	long long m;
	{
		FILE *fp = fopen(CGROUP_FS "memory/sandbox/memory.max_usage_in_bytes","r");
		assert(fp!=NULL);
		assert(fscanf(fp,"%lld",&m)==1);
		assert(fclose(fp)==0);
	}
	FILE *fp = fopen(SLAVE_LOG, "w");
	assert(system("echo \"OPEN SLAVE w\" > ~/log") == 0);
	assert(fp != NULL);
	assert(system("echo \":OK\n\" > ~/log") == 0);
	if (MLE) {
		fputs("MLE\n", fp);
	} else if (TLE) {
		fputs("TLE\n", fp);
	} else if (!WIFEXITED(status)||WEXITSTATUS(status)!=0) {
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

static void do_slave(char *argv[], int PL) {
	/* -- disassociate parts of the process execution context -- */
	assert(unshare(CLONE_FILES) == 0);
	assert(unshare(CLONE_FS) == 0);
	assert(unshare(CLONE_NEWIPC) == 0);
	assert(unshare(CLONE_NEWNET) == 0);
	assert(unshare(CLONE_NEWNS) == 0);
	assert(unshare(CLONE_NEWUTS) == 0);
	assert(unshare(CLONE_SYSVSEM) == 0);

	/* -- change working directory, then change root directory to protect -- */
	assert(chdir(CHROOT) == 0);
	assert(chroot(".") == 0);
	assert(chdir(CHDIR) == 0);

	assert(setgid(NOGROUP) == 0);
	assert(setuid(NOBODY) == 0);
	/* -- limit the maximum number of threads -- */	
	{
		struct rlimit rlim;
		rlim.rlim_max = PL;
		rlim.rlim_cur = rlim.rlim_max;
		assert(setrlimit(RLIMIT_NPROC, &rlim)==0);
	}
	/* -- set lower priority than the master process, but higher than other process -- */
	{
		struct sched_param param;
		param.sched_priority = sched_get_priority_max(SCHED_FIFO)-1;
		assert(sched_setscheduler(0, SCHED_FIFO, &param) == 0);
	}

	assert(raise(SIGSTOP) == 0);
	assert(execvp(argv[0], argv) != -1 && 0);
}

static void proc_help(char *argv[]) {
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT "\x1B[37m"
	fprintf(stderr, "Usage: %s [options] <command>\n", argv[0]);
	fprintf(stderr, "OPTIONS\n");
	fprintf(stderr, "   -time <integer>        Time limited in seconds\n");
	fprintf(stderr, "                          " KMAG "Default -time %d\n\n" KWHT, 10);
	fprintf(stderr, "   -mem <integer>         Memory Limited in bytes\n");
	fprintf(stderr, "                          " KMAG "Default -mem %d\n\n" KWHT, 2147483647);
	fprintf(stderr, "   -thread <integer>      Thread Limited\n");
	fprintf(stderr, "                          " KMAG "Default -thread %d\n\n" KWHT, NPROC_LIMIT);
	fflush(stderr);
	exit(1);
}

int main(int argc, char *argv[]) {
	if (argc <= 3)
		proc_help(argv);
	assert(argc > 3);
	int TL = 10, PL = NPROC_LIMIT;
	long long ML = 2147483647;
	int exec_argv_idx = 0;
	/* -- process option -- */
	{
		const char option[][16] = {"-time", "-mem", "-thread"};
		const int MAX_OPTION = sizeof(option) / sizeof(option[0]);
		for (int i = 1; i < argc; i++) {
			int has = 0;
			for (int j = 0; j < MAX_OPTION; j++) {
				if (strcmp(argv[i], option[j]))
					continue;
				has = 1;
				switch(j) {
					case 0:
						assert(sscanf(argv[++i], "%d", &TL) == 1 && "Time Limited Error");
						break;
					case 1:
						assert(sscanf(argv[++i], "%lld", &ML) == 1 && "Memory Limited Error");
						break;
					case 2:
						assert(sscanf(argv[++i], "%d", &PL) == 1 && "Thread Limited Error");
						break;
				}
			}
			if (has == 0) {
				exec_argv_idx = i;
				break;
			}
		}
	}
	/* -- check the config range --*/
	assert(TL >= 0	&& "Time Limited Format Error: <signed 32-bit integer>");
	assert(ML >= -1	&& "Memory Limited Foramt Error: <unsigned 64-bit integer>");
	assert(PL >= 1	&& "Thread Limited Foramt Error: <signed 32-bit integer>");
	/* -- check exec privilege -- */
	assert(setuid(0) == 0 && "Run with root privilege");
	assert(setgid(0) == 0 && "Run with root privilege");
	/* -- close swap area for all device -- */
	assert(system("swapoff -a") == 0);
	/* -- use the high priority to make it more stable -- */
	{
		struct sched_param param;
		param.sched_priority = sched_get_priority_max(SCHED_FIFO);
		assert(sched_setscheduler(0, SCHED_FIFO, &param) == 0);
	}

	slave = fork();
	if (slave == 0) {
		char **args = malloc((argc+1)*sizeof(char*));
		args[0] = "cgexec";
		args[1] = "-g";
		args[2] = "memory:/sandbox";
		for(int i = exec_argv_idx, j = 3; i <= argc; i++, j++) 
			args[j] = argv[i];
		do_slave(args, PL);
	}
	assert(slave != -1);
	do_master(TL, ML);
	return 0;
}
