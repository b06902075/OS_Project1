#define _GNU_SOURCE
#include "process.h"
#include "scheduler.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>

static int t_last;
static int t_now;
static int running_P;
static int finish_N;

int cmp(const void *a, const void *b) {
	return ((struct process *)a)->t_ready - ((struct process *)b)->t_ready;
}

int next_process(struct process *Ps, int N, int policy)
{
	/* non-preemptive */
	if (running_P != -1 && (policy == SJF || policy == FIFO))
		return running_P;

	int next_P = -1;

	switch(policy) {
	case FIFO:
		for(int i = 0; i < N; i++) {
			if(Ps[i].pid == -1 || Ps[i].t_exec == 0) continue;
			if(next_P == -1 || Ps[i].t_ready < Ps[next_P].t_ready) next_P = i;
		}
		
		break;
	case RR:
		if (running_P == -1) {
			for (int i = 0; i < N; i++) {
				if (Ps[i].pid != -1 && Ps[i].t_exec > 0){
					next_P = i;
					break;
				}
			}
		}
		else if ((t_now - t_last) % 500 == 0)  {
			next_P = (running_P + 1) % N;
			while (Ps[next_P].pid == -1 || Ps[next_P].t_exec == 0)
				next_P = (next_P + 1) % N;
		}
		else next_P = running_P;
		
		break;
	
	case SJF: case PSJF:
		for (int i = 0; i < N; i++) {
			if (Ps[i].pid == -1 || Ps[i].t_exec == 0) continue;
			if (next_P == -1 || Ps[i].t_exec < Ps[next_P].t_exec) next_P = i;
		}
		
		break;
	}

	return next_P;
}

int scheduling(struct process *Ps, int N, int policy)
{
	qsort(Ps, N, sizeof(struct process), cmp);

	for (int i = 0; i < N; i++) Ps[i].pid = -1;

	process_cpu(getpid(), PARENT_CPU);
	process_run(getpid());
	
	t_now = 0;
	running_P = -1;
	finish_N = 0;
	
	while(1) {
		if (running_P != -1 && Ps[running_P].t_exec == 0) {
			waitpid(Ps[running_P].pid, NULL, 0);
			printf("%s %d\n", Ps[running_P].name, Ps[running_P].pid);
			fflush(stdout);
			running_P = -1;
			finish_N++;
			
			if (finish_N == N) break;
		}

		for (int i = 0; i < N; i++) {
			if (Ps[i].t_ready == t_now) {
				Ps[i].pid = process_create(Ps[i]);
				process_idle(Ps[i].pid);
			}

		}

		int next_P = next_process(Ps, N, policy);
		/* context switch */
		if (next_P != -1 && running_P != next_P) {
			process_run(Ps[next_P].pid);
			process_idle(Ps[running_P].pid);
			running_P = next_P;
			t_last = t_now;
		}

		UNIT_TIME();
		if (running_P != -1) Ps[running_P].t_exec--;
		t_now++;
	}

	return 0;
}
