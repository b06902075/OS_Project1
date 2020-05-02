#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>

#define PARENT_CPU 0
#define CHILD_CPU 1

#define UNIT_TIME() { volatile unsigned long i; for(i = 0; i < 1000000UL; i++); }

struct process {
	char name[32];
	int t_ready;
	int t_exec;
	int t_RR;
	pid_t pid;
};

int process_cpu(int pid, int core);
int process_create(struct process P);
int process_idle(int pid);
int process_run(int pid);

#endif
