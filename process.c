#define _GNU_SOURCE
#include "process.h"
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/syscall.h>
#define GET_TIME 334

int process_cpu(int pid, int core)
{
	if (core > sizeof(cpu_set_t)) {
		fprintf(stderr, "Core index error.");
		return -1;
	}

	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);
		
	if (sched_setaffinity(pid, sizeof(mask), &mask) < 0) {
		perror("sched_setaffinity");
		exit(1);
	}

	return 0;
}

int process_create(struct process P)
{
	int pid = fork();

	if (pid < 0) {
		perror("fork");
		return -1;
	}

	/* child */
	if (pid == 0) { 
		unsigned long start_sec, start_nsec, end_sec, end_nsec;		
		char cmd[256];
		syscall(GET_TIME, &start_sec, &start_nsec);
		for (int i = 0; i < P.t_exec; i++) UNIT_TIME();
		syscall(GET_TIME, &end_sec, &end_nsec);
		sprintf(cmd, "echo \"[Project1] %d %lu.%09lu %lu.%09lu\n\" > /dev/kmsg",getpid(), start_sec, start_nsec, end_sec, end_nsec);
		system(cmd);
		exit(0);
	}
	
	/* parent */
	process_cpu(pid, CHILD_CPU);

	return pid;
}

int process_idle(int pid)
{
	struct sched_param param;
	param.sched_priority = 0;
	int ret = sched_setscheduler(pid, SCHED_IDLE, &param);
	
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}

	return ret;
}

int process_run(int pid)
{
	struct sched_param param;
	param.sched_priority = 0;
	int ret = sched_setscheduler(pid, SCHED_OTHER, &param);
	
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}

	return ret;
}
