#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include "process.h"
#include "scheduler.h"

int main(int argc, char* argv[])
{

	char S[32];
	int policy;
	int N;
	struct process *Ps;

	scanf("%s", S);
	scanf("%d", &N);
	
	Ps = (struct process *)malloc(N * sizeof(struct process));

	for (int i = 0; i < N; i++) {
		scanf("%s%d%d", Ps[i].name,
			&Ps[i].t_ready, &Ps[i].t_exec);
	}

	if (strcmp(S, "FIFO") == 0) {
		policy = FIFO;
	}
	else if (strcmp(S, "RR") == 0) {
		policy = RR;
	}
	else if (strcmp(S, "SJF") == 0) {
		policy = SJF;
	}
	else if (strcmp(S, "PSJF") == 0) {
		policy = PSJF;
	}
	else {
		fprintf(stderr, "Invalid policy: %s", S);
		exit(1);
	}

	scheduling(Ps, N, policy);

	exit(0);
}
