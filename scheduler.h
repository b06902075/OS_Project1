#ifndef _SCHEDULING_H_
#define _SCHEDULING_H_

#include "process.h"

#define FIFO	1
#define RR	2
#define SJF	3
#define PSJF	4

int scheduling(struct process *Ps, int N, int policy);

#endif
