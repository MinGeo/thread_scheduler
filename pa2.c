#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "uthread.h"

#define PARSE_SCHED(buf, policy) if (strcmp(buf, "FIFO") == 0) policy = FIFO; \
                                 else if (strcmp(buf, "RR") == 0) policy = RR; \
                                 else if (strcmp(buf, "PRIO") == 0) policy = PRIO; \
                                 else if (strcmp(buf, "SJF") == 0) policy = SJF; \
                                 else policy = -1;\

#define PARSE_FN(buf, res) if (strcmp(buf, "CREATE") == 0) res = 0; \
                           else if (strcmp(buf, "JOIN") == 0) res = 1; \
                           else res = -1;\

/*******************************************************************
 * __preemptive_worker(void* args)
 *
 * DESCRIPTION
 *    This function is job for preemptive scheduling thread.
 *    If you want to check implementation of user_level_thread, 
 *    add fprintf statement.
 *
 ******************************************************************/
void *__preemptive_worker(void* args) {
    while(1);
}

/*******************************************************************
 * __non_preemptive_worker(void* args)
 *
 * DESCRIPTION
 *    This function is job for non_preemptive scheduling thread.
 *    If you want to check implementation of user_level_thread,
 *    add fprintf statement.
 *
 ******************************************************************/
void *__non_preemptive_worker(void* args) {
    for (int i = 0; i < 100000; i++);
}

/*******************************************************************
 *
 * DO NOT MODIFY UNDER THIS LINE!!!
 *
 ******************************************************************/ 

int main(int argc, char* argv[]) {
    
    int params[3];
    char buf[15], *ptr = NULL;
    int target, res = -2;
    enum uthread_sched_policy policy;
    sigset_t mask;
   
    /* Set scheduling policy. */ 
    fgets(buf, sizeof(buf), stdin);
    ptr = strtok(buf, "\n");
    PARSE_SCHED(buf, policy);
    sigaddset(&mask, SIGALRM);

    /* Init user level thread library. */
    uthread_init(policy);

    /* Read input file and run the correct function. */
    while (fgets(buf, sizeof(buf), stdin)) {
        ptr = strtok(buf, " ");
        PARSE_FN(ptr, res);

        switch(res) {
            case 0: // CREATE
                sigprocmask(SIG_BLOCK, &mask, NULL);
                for (int i = 0; i < 3; i++) 
                    params[i] = atoi(strtok(NULL, " "));

                if (policy == FIFO) 
                    uthread_create((void *)__non_preemptive_worker, (void *)params);
                else 
                    uthread_create((void *)__preemptive_worker, (void *)params);
                break;
            case 1: // JOIN
                sigprocmask(SIG_UNBLOCK, &mask, NULL);
                target = atoi(strtok(NULL, " "));
                uthread_join(target);
                break;
        }
    }

    __free_all_tcbs();
    
    return EXIT_SUCCESS;
}
