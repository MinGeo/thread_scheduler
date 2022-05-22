#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <ucontext.h>
#include "uthread.h"
#include "list_head.h"
#include "types.h"

// 참고해야할 사이트 주소 -------------------------------
// coroutine 설명
// https://wooooooak.github.io/kotlin/2019/08/25/%EC%BD%94%ED%8B%80%EB%A6%B0-%EC%BD%94%EB%A3%A8%ED%8B%B4-%EA%B0%9C%EB%85%90-%EC%9D%B5%ED%9E%88%EA%B8%B0/

// signal.h 설명
// https://12bme.tistory.com/224

/* You can include other header file, if you want. */
// 스케줄 관리 방식을 uthread_init 함수에서만 전달 받음으로 전역 변수로 가지고 있어야 함수
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

ucontext_t exitContext;
int current_tid;

LIST_HEAD(tcbs);
int n_tcbs = 0;
struct ucontext_t *t_context;

/***************************************************************************************
 * next_tcb()
 *
 * DESCRIPTION
 *
 *    Select a tcb with current scheduling policy]'
 *    현재 일정 정책이 있는 tcb를 선택하세요.
 *
 **************************************************************************************/

void next_tcb() {
    /* TODO: You have to implement this function. */
    struct tcb *p_tcb;
    struct tcb *n_tcb;
    switch (sched_policy)
    {
        case FIFO:
            list_for_each_entry(n_tcb, &tcbs, list) {
                if (n_tcb != NULL && current_tid == n_tcb->tid) {
                    // fprintf(stderr, "LOOP : CURRENT %d TCDID %d P %d N %d\n", current_tid, n_tcb->tid, ((struct tcb *)n_tcb->list.prev)->tid, ((struct tcb *)n_tcb->list.next)->tid);
                    p_tcb = n_tcb;
                    while (true) {
                        if (list_is_last(&n_tcb->list, &tcbs) == 1) {
                        //    printf("LAST : list_first_entry\n");
                            n_tcb = list_first_entry(&tcbs, struct tcb, list);
                            break;
                        }
                        else
                        {
                        //    printf("NEXT : n_tcb->list.next\n");
                            n_tcb = ((struct tcb *)n_tcb->list.next);
                            if (n_tcb->lifetime > 0) break;
                        }
                    }
                    current_tid = n_tcb->tid;
                    if (n_tcb->lifetime > 0 && n_tcb->state != TERMINATED) {
                        n_tcb->state = RUNNING;
                        if (p_tcb->tid != n_tcb->tid) {
                            fprintf(stderr, "SWAP %d -> %d\n", p_tcb->tid, n_tcb->tid);
                            swapcontext(p_tcb->context, n_tcb->context);
                        }
                        n_tcb->lifetime = 0;
                    }
                }
            }
            break;
        case RR:
            list_for_each_entry(n_tcb, &tcbs, list) {
                // fprintf(stderr, "LOOP : CURRENT %d TCDID %d P %d N %d\n", current_tid, n_tcb->tid, ((struct tcb *)n_tcb->list.prev)->tid, ((struct tcb *)n_tcb->list.next)->tid);
                if (n_tcb != NULL && current_tid == n_tcb->tid) {
                    p_tcb = n_tcb;

                    if (list_is_last(&n_tcb->list, &tcbs) == 1) {
                    //    printf("LAST : list_first_entry\n");
                        n_tcb = list_first_entry(&tcbs, struct tcb, list);
                    }
                    else
                    {
                    //    printf("NEXT : n_tcb->list.next\n");
                        n_tcb = ((struct tcb *)n_tcb->list.next);
                    }
                    current_tid = n_tcb->tid;
                    p_tcb->state = READY;
                    n_tcb->state = RUNNING;
                    n_tcb->lifetime--;
                    fprintf(stderr, "SWAP %d -> %d\n", p_tcb->tid, n_tcb->tid);
                    swapcontext(p_tcb->context, n_tcb->context);
                }
            }
            break;
        case PRIO:
            break;
        case SJF:
            break;
        default:
            // error
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
    struct tcb *thread;
    thread = malloc(sizeof(struct tcb));
    thread->tid = MAIN_THREAD_TID;
    thread->lifetime = MAIN_THREAD_LIFETIME;
    thread->priority = MAIN_THREAD_PRIORITY;
    thread->state = RUNNING;
    thread->context = context;
    INIT_LIST_HEAD(&thread->list);
    list_add_tail(&thread->list, &tcbs);
    n_tcbs++;
    current_tid = MAIN_THREAD_TID;
    if (getcontext(context)) {
        printf("CHK : main getcontext error\n");
        return;
    }
    t_context = context;

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
 * stub : 전달 받지만 함수 내에서 사용하지 않음
 * pa2.c소스에서 (void *)__non_preemptive_worker, (void *)__preemptive_worker 두가지가 옴
 * 전달받은 값을 tcb 구조에 넣기만 하고 실제 스레드 실행을 안함??
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
    temp->context->uc_link = &exitContext;   
    temp->context->uc_stack.ss_sp = malloc(MAX_STACK_SIZE);
    temp->context->uc_stack.ss_size = MAX_STACK_SIZE;
    temp->context->uc_stack.ss_flags = 0;
    makecontext(temp->context, (void *)stub, 0);

    return temp->tid;
}

/***************************************************************************************
 * uthread_join(int tid)
 *
 * DESCRIPTION
 *
 *    Wait until thread context block is terminated.
 *    스레드 컨텍스트 블록이 종료될 때까지 기다립니다.
 *
 * tid : 조인할 스레드
 * pa2.c에서 사용자가 JOIN요구시 호출됨
 *
 **************************************************************************************/

void uthread_join(int tid) {
    /* TODO: You have to implement this function. */
    for (int i = 0; i < 10000000; i++);
    fprintf(stderr, "JOIN %d\n", tid);
    struct tcb *temp;
    list_for_each_entry(temp, &tcbs, list) {
        if (temp->tid == tid) {
        //    fprintf(stderr, "JOIN %d\n", tid);
            setcontext(temp->context);
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
 *    스레드가 종료되면 스레드는 tcb 블록에서 상태를 수정해야 합니다.
 *    스레드 종료될때 exit함수를 통해서 종료 상태 갱신해야 하는 듯
 *    README.md 참고
 *    When each thread is terminated, they must inform their state to the main thread. So, you have to create the context that will be resumed after the terminated thread.
 *
 **************************************************************************************/

void __exit() {
    /* TODO: You have to implement this function. */
    // printf("__exit\n");
    struct tcb *temp;
    list_for_each_entry(temp, &tcbs, list) {
        if (temp->lifetime <= 0) {
            temp->state = TERMINATED;
            // fprintf(stderr, "TERMINATED : %d\n", temp->tid);
        }
    }
}
 
/***************************************************************************************
 * __initialize_exit_context()
 *
 * DESCRIPTION
 *
 *    This function initializes exit context that your thread context will be linked.
 *    이 함수는 스레드 컨텍스트가 연결될 종료 컨텍스트를 초기화합니다.
 *
 **************************************************************************************/

void __initialize_exit_context() {
    /* TODO: You have to implement this function. */
    getcontext(&exitContext);
    exitContext.uc_link = 0;   
    exitContext.uc_stack.ss_sp = malloc(MAX_STACK_SIZE);
    exitContext.uc_stack.ss_size = MAX_STACK_SIZE;
    exitContext.uc_stack.ss_flags = 0;
    makecontext(&exitContext, (void *)__exit, 0);
}
 
/***************************************************************************************
 *
 * DO NOT MODIFY UNDER THIS LINE!!!
 *
 **************************************************************************************/

static struct itimerval time_quantum;
static struct sigaction ticker;

void __scheduler() {
   // printf("CHK : __scheduler\n");
   if (n_tcbs > 1)
       next_tcb();
}

void __create_run_timer() {
    // 타이머 설정값
    time_quantum.it_interval.tv_sec = 0;
    time_quantum.it_interval.tv_usec = SCHEDULER_FREQ;  // 1초 
    time_quantum.it_value = time_quantum.it_interval;

    ticker.sa_handler = __scheduler;                    // 핸들러 설정
    sigemptyset(&ticker.sa_mask);                       // 빈시그널
    sigaction(SIGALRM, &ticker, NULL);                  // SIGALRM 시그널 실행
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
