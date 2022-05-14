#ifndef MYTHREAD_H_
#define MYTHREAD_H_

#include "types.h"

#define MAIN_THREAD_TID -1
#define MAIN_THREAD_LIFETIME 1000
#define MAIN_THREAD_PRIORITY -1
#define MAX_STACK_SIZE SIGSTKSZ
#define SCHEDULER_FREQ 1000


enum uthread_sched_policy {
    FIFO,
    RR,
    PRIO,
    SJF,
};

enum uthread_state {
    READY,
    RUNNING,
    TERMINATED,
};

void uthread_init(enum uthread_sched_policy policy);

int uthread_create(void* stub(void*), void* args);

void uthread_join(int tid);

struct tcb *fifo_scheduling(struct tcb *next);

struct tcb *rr_scheduling(struct tcb *next);

struct tcb *prio_scheduling(struct tcb *next);

struct tcb *sjf_scheduling(struct tcb *next);

void __create_run_timer();

void __initialize_exit_context();

void __exit();

void __free_all_tcbs();

#endif
