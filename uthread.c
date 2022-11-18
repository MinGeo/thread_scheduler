#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <ucontext.h>
#include "uthread.h"
#include "list_head.h"
#include "types.h"

#include <sys/wait.h>

/* You can include other header file, if you want. */
enum uthread_sched_policy sched_policy;
 
/*******************************************************************
* struct tcb
 *
 * DESCRIPTION
 *    tcb is a thread control block.
 *    This structure contains some information to maintain thread.
 *
 ******************************************************************/

struct tcb {
    struct list_head list;
    ucontext_t *context;
    enum uthread_state state;
    int tid;
    int lifetime; // This for preemptive scheduling
    int priority; // This for priority scheduling
};

/***************************************************************************************
 * LIST_HEAD(tcbs);
 *
 * DESCRIPTION
 *    Initialize list head of thread control block.
 *
 **************************************************************************************/

int current_tid;
struct tcb *current_tcb;
int shortcheck = 10;
int priocheck = -1;
LIST_HEAD(tcbs);
int n_tcbs = 0;
struct ucontext_t *t_context;

/***************************************************************************************
 * next_tcb()
 *
 * DESCRIPTION
 *
 *    Select a tcb with current scheduling policy]'
 *
 **************************************************************************************/

void next_tcb() {
    /* TODO: You have to implement this function. */
    int test = 0;
    struct tcb *temp;
    struct tcb *p_tcb;
    struct tcb *n_tcb;
    switch (sched_policy)
    {
        case FIFO:
            if (current_tcb->tid != MAIN_THREAD_TID) {
                current_tcb->lifetime = 0;
                current_tcb->state = TERMINATED;
            }
            p_tcb = current_tcb;    
            while (true) {
                if (list_is_last(&p_tcb->list, &tcbs) == 1) { 
                    if (p_tcb->tid == MAIN_THREAD_TID)
                        return;     
                    else
                        n_tcb = list_first_entry(&tcbs, struct tcb, list);  
                } else {
                    n_tcb = ((struct tcb *)p_tcb->list.next);   
                }
                if (n_tcb->lifetime > 0)    
                    break;
                else
                    p_tcb = n_tcb;     
            }
            p_tcb = current_tcb;    
            current_tcb = n_tcb;    
            n_tcb->state = RUNNING; 
            fprintf(stderr, "SWAP %d -> %d\n", p_tcb->tid, n_tcb->tid);
            swapcontext(p_tcb->context, n_tcb->context);    
            break;
        case RR:
            p_tcb = current_tcb;    
            while (true) {
                if (list_is_last(&p_tcb->list, &tcbs) == 1) {   
                    if (current_tcb->tid == MAIN_THREAD_TID)
                    {
                        return;     
                    }
                    else
                        n_tcb = list_first_entry(&tcbs, struct tcb, list);  
                } else {
                    n_tcb = ((struct tcb *)p_tcb->list.next);   
                }
                if (n_tcb->lifetime > 0)     
                    break;
                else
                    p_tcb = n_tcb;     
            }
            p_tcb = current_tcb;    
            p_tcb->lifetime--;
            if (current_tcb->tid != MAIN_THREAD_TID) {
                if(current_tcb->lifetime == 0)
                {
                    current_tcb->state = TERMINATED;
                }
            }
            current_tcb = n_tcb;    
            n_tcb->state = RUNNING; 
            fprintf(stderr, "SWAP %d -> %d\n", p_tcb->tid, n_tcb->tid);
            swapcontext(p_tcb->context, n_tcb->context); 
            break;
        case PRIO:
            priocheck = -1;
            current_tcb->lifetime--;
            if (current_tcb->tid != MAIN_THREAD_TID) {
                if(current_tcb->lifetime == 0)
                {
                    current_tcb->state = TERMINATED;
                }
            }
            list_for_each_entry(temp, &tcbs, list)
            {
                if(temp->priority > priocheck && temp->state != TERMINATED)
                {
                    priocheck = temp->priority;
                }
            }
            p_tcb = current_tcb;   
            list_for_each_entry(temp, &tcbs, list)
            {
                if(temp->state != TERMINATED)
                {
                    test++;
                }
            }
            if (test == 1) {   
                if (p_tcb->tid == MAIN_THREAD_TID)
                {
                    return;    
                }
            }     
            list_for_each_entry(n_tcb, &tcbs, list)
            {
                if(n_tcb->priority == priocheck && n_tcb->state != TERMINATED)
                {
                    break;
                }
            }
            p_tcb = current_tcb;    
            current_tcb = n_tcb;
            n_tcb->state = RUNNING;
            fprintf(stderr, "SWAP %d -> %d\n", p_tcb->tid, n_tcb->tid);
            swapcontext(p_tcb->context, n_tcb->context);
            break;
        case SJF:
            if (current_tcb->tid != MAIN_THREAD_TID) {
                current_tcb->lifetime = 0;
                current_tcb->state = TERMINATED;
            }
            shortcheck = 100;
            list_for_each_entry(temp, &tcbs, list)
            {
                if(temp->lifetime < shortcheck && temp->state != TERMINATED)
                {
                    shortcheck = temp->lifetime;
                }
            }
            p_tcb = current_tcb;
            list_for_each_entry(temp, &tcbs, list)
            {
                if(temp->state != TERMINATED)
                {
                    test++;
                }
            }
            if (test == 1) {
                if (p_tcb->tid == MAIN_THREAD_TID)
                {
                    return;
                }
            }
            if(shortcheck == 100)
            {
                n_tcb = list_first_entry(&tcbs, struct tcb, list);
            }
            else{
                list_for_each_entry(n_tcb, &tcbs, list)
                {
                    if(n_tcb->lifetime == shortcheck && n_tcb->state != TERMINATED)
                    {
                        break;
                    }
                }
            }
            p_tcb = current_tcb;    
            current_tcb = n_tcb;    
            n_tcb->state = RUNNING; 
            fprintf(stderr, "SWAP %d -> %d\n", p_tcb->tid, n_tcb->tid);
            swapcontext(p_tcb->context, n_tcb->context); 
            break;
        default:
            break;
    }
}

/***************************************************************************************
 * struct tcb *fifo_scheduling(struct tcb *next)
 *
 * DESCRIPTION
 *
 *    This function returns a tcb pointer using First-In-First-Out policy
 *
 **************************************************************************************/

struct tcb *fifo_scheduling(struct tcb *next) {
    /* TODO: You have to implement this function. */
}

/***************************************************************************************
 * struct tcb *rr_scheduling(struct tcb *next)
 *
 * DESCRIPTION
 *
 *    This function returns a tcb pointer using round robin policy
 *
 **************************************************************************************/

struct tcb *rr_scheduling(struct tcb *next) {
    /* TODO: You have to implement this function. */
    // 스케쥴링 : RR 방식용

}

/***************************************************************************************
 * struct tcb *prio_scheduling(struct tcb *next)
 *
 * DESCRIPTION
 *
 *    This function returns a tcb pointer using priority policy
 *
 **************************************************************************************/

struct tcb *prio_scheduling(struct tcb *next) {
    /* TODO: You have to implement this function. */
}

/***************************************************************************************
 * struct tcb *sjf_scheduling(struct tcb *next)
 *
 * DESCRIPTION
 *
 *    This function returns a tcb pointer using shortest job first policy
 *
 **************************************************************************************/

struct tcb *sjf_scheduling(struct tcb *next) {
    /* TODO: You have to implement this function. */
}
 
/***************************************************************************************
 * uthread_init(enum uthread_sched_policy policy)
 *
 * DESCRIPTION
 *    Initialize main tread control block, and do something other to schedule tcbs
 *
 **************************************************************************************/

void uthread_init(enum uthread_sched_policy policy) {
    /* TODO: You have to implement this function. */
    sched_policy = policy;
    ucontext_t *context;
    context = malloc(sizeof(ucontext_t));
    struct tcb *temp;
    temp = malloc(sizeof(struct tcb));
    temp->tid = MAIN_THREAD_TID;
    temp->lifetime = MAIN_THREAD_LIFETIME;
    temp->priority = MAIN_THREAD_PRIORITY;
    temp->state = RUNNING;
    temp->context = context;
    INIT_LIST_HEAD(&temp->list);
    list_add_tail(&temp->list, &tcbs);
    n_tcbs++;

    current_tid = MAIN_THREAD_TID;
    current_tcb = temp;
    getcontext(context);

    /* DO NOT MODIFY THESE TWO LINES */
    __create_run_timer();
    __initialize_exit_context();
}

/***************************************************************************************

 * uthread_create(void* stub(void *), void* args)
 *
 * DESCRIPTION
 *
 *    Create user level thread. This function returns a tid.
 *
 **************************************************************************************/

int uthread_create(void* stub(void *), void* args) {
    /* TODO: You have to implement this function. */
    struct tcb *temp;
    temp = malloc(sizeof(struct tcb));
    temp->tid = ((int *)args)[0];
    temp->lifetime = ((int *)args)[1];
    temp->priority = ((int *)args)[2];
    temp->state = READY;
    temp->context = malloc(sizeof(ucontext_t));
    INIT_LIST_HEAD(&temp->list);
    list_add_tail(&temp->list, &tcbs);
    n_tcbs++;

    getcontext(temp->context);
    temp->context->uc_link = t_context;   
    temp->context->uc_stack.ss_sp = malloc(MAX_STACK_SIZE);
    temp->context->uc_stack.ss_size = MAX_STACK_SIZE;
    temp->context->uc_stack.ss_flags = 0;
    makecontext(temp->context, (void *)stub, 0);
    sigemptyset(&temp->context->uc_sigmask);

    return temp->tid;
}

/***************************************************************************************
 * uthread_join(int tid)
 *
 * DESCRIPTION
 *
 *    Wait until thread context block is terminated.
 *
 **************************************************************************************/

void uthread_join(int tid) {
    /* TODO: You have to implement this function. */
    while (true) {
        struct tcb *temp;
        list_for_each_entry(temp, &tcbs, list) {
            if (temp->tid == tid) {
                if (temp->state == TERMINATED) {
                    list_del(&temp->list);
                    n_tcbs--;
                    fprintf(stderr, "JOIN %d\n", tid);
                    return;
                }
            }
        }
    }
}

/***************************************************************************************
 * __exit()
 *
 * DESCRIPTION
 *
 *    When your thread is terminated, the thread have to modify its state in tcb block.
 *
 **************************************************************************************/

void __exit() {
    /* TODO: You have to implement this function. */
}
 
/***************************************************************************************
 * __initialize_exit_context()
 *
 * DESCRIPTION
 *
 *    This function initializes exit context that your thread context will be linked.
 *
 **************************************************************************************/

void __initialize_exit_context() {
    /* TODO: You have to implement this function. */
    t_context = malloc(sizeof(ucontext_t));
    getcontext(t_context);
    t_context->uc_link = 0;   
    t_context->uc_stack.ss_sp = malloc(MAX_STACK_SIZE);
    t_context->uc_stack.ss_size = MAX_STACK_SIZE;
    t_context->uc_stack.ss_flags = 0;
    makecontext(t_context, (void *)__exit, 0);
}
 
/***************************************************************************************
 *
 * DO NOT MODIFY UNDER THIS LINE!!!
 *
 **************************************************************************************/

static struct itimerval time_quantum;
static struct sigaction ticker;

void __scheduler() {
   if (n_tcbs > 1)
       next_tcb();
}

void __create_run_timer() {
    time_quantum.it_interval.tv_sec = 0;
    time_quantum.it_interval.tv_usec = SCHEDULER_FREQ; 
    time_quantum.it_value = time_quantum.it_interval;

    ticker.sa_handler = __scheduler;                  
    sigemptyset(&ticker.sa_mask);                      
    sigaction(SIGALRM, &ticker, NULL);                  
    ticker.sa_flags = 0;

    setitimer(ITIMER_REAL, &time_quantum, (struct itimerval*) NULL);
}

void __free_all_tcbs() {
    struct tcb *temp;
    list_for_each_entry(temp, &tcbs, list) {
        if (temp != NULL && temp->tid != -1) {
            list_del(&temp->list);
            free(temp->context);
            free(temp);
            n_tcbs--;
            temp = list_first_entry(&tcbs, struct tcb, list);
        }
    }
    temp = NULL;
    free(t_context);
}
