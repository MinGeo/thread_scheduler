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

LIST_HEAD(tcbs);
int n_tcbs = 0;
ucontext_t *t_context;
sigset_t mask;
struct tcb *fifo_scheduling(struct tcb *next);
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
    printf("nexttcb");
    /* TODO: You have to implement this function. */

    // 스케쥴 관리 방법에 따라 다음 TCB 액션 있는듯, 함수 리턴이 없음
    // sched_policy 지정된 방법에 따라 스케쥴 순서에 맞는 tcb 값 확인하여
    // swapcontext로 실행을 넘겨야 할 것으로 보임
    // SWAP 할때 출력로그 남겨야 함
    // SWAP -1 -> 0
    // SWAP 0 -> 1

    switch (sched_policy)
    {
        case FIFO:
            //fprintf(stderr, "SWAP %d -> %d\n", next->tid, temp->tid);
            //setcontext(next->context);
            
            // README.md파일에 보면 아래와 같이 출력 로그 남기라고 되어 있음
            // FIFO등의 스케쥴 관리 방법에 따른 관리가 가능하도록 구조체 변경할 필요가 있어 보임
            break;
        case RR:
            //a = rr_scheduling(b);
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
    printf("This is fifo");
    /* TODO: You have to implement this function. */
    list_for_each_entry(next, &tcbs, list)
    {
        if (next != NULL && next->tid != -1) {
            if(next->state == READY)
            {
                next->state = RUNNING;
                return next;
            }
        
        }
    }
    // 스케쥴링 : FIFO 방식용

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

 

    // 스케쥴링 : PRIO 방식용

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

 

    // 스케쥴링 : SJF 방식용

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
    // policy : 자료 처리 방법을 전달 받음
    // policy 전달 받는 곳이 이곳 뿐임으로 전역변수 사용해서 보관
    sched_policy = policy;
    // ?
    //getcontext(); // t_contextdp 메인스레드 넣는 것인지 확인 필요함
    // ?
    // uthread.h 파일을 보면
    // #define MAIN_THREAD_TID -1
    // #define MAIN_THREAD_LIFETIME 1000
    // #define MAIN_THREAD_PRIORITY -1
    // LIST_HEAD(tcbs) tcbs에 메인스레드를 하나 추가해야 할 것으로 보임
    // 물론 메인스레드를 리스트에 넣지 않을수도 있음??? 좀더 분석해야 함
/*
    struct tcb *thread;
    thread = malloc(sizeof(struct tcb));
    thread->context = malloc(20000);
    //getcontext(thread->context);
    //getcontext(t_context);
    getcontext(thread->context);
    thread->tid = MAIN_THREAD_TID;
    thread->lifetime = MAIN_THREAD_LIFETIME;
    thread->priority = MAIN_THREAD_PRIORITY;
    thread->state = RUNNING;
    thread->context->uc_link = 0;   
    thread->context->uc_stack.ss_sp = malloc(MAX_STACK_SIZE);
    thread->context->uc_stack.ss_size = MAX_STACK_SIZE;
    thread->context->uc_stack.ss_flags = 0;
    swapcontext(thread->context, t_context);
*/


    /* DO NOT MODIFY THESE TWO LINES */
    __create_run_timer();
//    __initialize_exit_context();
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
    printf("CHK : uthread_create\n");
    struct tcb *thread;
    thread = malloc(sizeof(struct tcb));
    thread->state = READY;
    thread->context = malloc(2000);
    thread->tid = ((int *)args)[0];
    thread->lifetime = ((int *)args)[1];
    thread->priority = ((int *)args)[2];
    list_add_tail(&thread->list, &tcbs);
    n_tcbs++;
    makecontext(thread->context, (void *)stub, 0);

    printf("CHK : makecontext\n");
    setcontext(thread->context);
    printf("CHK : setcontext\n");



    // bgman -----------------------------
    //getcontext(thread->context);
    //thread->state = READY;
    //thread->list = list;
    //getcontext(thread->context);
    // thread->context.uc_link 스레드가 종료되면 돌아오는 스레드를 지정하는듯 함
    // __initialize_exit_context에서 생성한 스레드를 넣으면 될 것 같은데
    // __initialize_exit_context에서 어떻게 이 스레드가 종료되어서 넘어온건지 알 수 있는 방법 필요함
    // 전역변수에 있는 t_context가  __initialize_exit_context에서 생성한 종료일수도 있음
    // 스레드 실행하고 바로 SWAP 해야 하는지 확인 필요함
    // bgman -----------------------------
    return thread->tid;
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
    printf("This is uthread_join");
    /* TODO: You have to implement this function. */
    // pa2.c unblock 한 것이 바로 적용된다고 할 수 없음으로 블럭이 종료된 것을 확인후
    // 조인 체크 하라는 뜻으로도 해석됨
    struct tcb *temp;
    list_for_each_entry(temp, &tcbs, list) {
        if (temp != NULL && temp->tid != -1) {
            // ss_flags
            if (temp->tid == tid) {
                // if 해당 스레드가 실행인지 확인
                // fprintf
                if(temp->state == RUNNING)
                {
                    fprintf(stderr, "JOIN %d\n", temp->tid);
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
 *    스레드가 종료되면 스레드는 tcb 블록에서 상태를 수정해야 합니다.
 *    스레드 종료될때 exit함수를 통해서 종료 상태 갱신해야 하는 듯
 *    README.md 참고
 *    When each thread is terminated, they must inform their state to the main thread. So, you have to create the context that will be resumed after the terminated thread.
 *
 **************************************************************************************/

void __exit() {
    /* TODO: You have to implement this function. */
    printf("This is exit");
    // 스레드가 종료되었을때 실행되는 스레드
    struct tcb *temp;
    list_for_each_entry(temp, &tcbs, list) {
        if (temp != NULL && temp->tid != -1) {
            // ss_flags
            // 
            if (temp->lifetime == 0) {
                temp->state = TERMINATED;
            }
            
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
    printf("This is initialize exit context");
    struct tcb *thread;
    thread = malloc(sizeof(struct tcb));
    thread->context = malloc(20000);
    setcontext(thread->context);
    /*
    thread->tid = MAIN_THREAD_TID;
    thread->lifetime = MAIN_THREAD_LIFETIME;
    thread->priority = MAIN_THREAD_PRIORITY;
    */
    // 리스트에 추가하는 방법 맞는지 모름, 확인 필요함
    // 스레드 갯수 카운트 추가
    // ???????? 아닌것 같음
    thread->context->uc_link = 0;   
    thread->context->uc_stack.ss_sp = malloc(MAX_STACK_SIZE);
    thread->context->uc_stack.ss_size = MAX_STACK_SIZE;
    thread->context->uc_stack.ss_flags = 0;
    
    makecontext(thread->context, (void*)&exit, 0);
    



    
}
 
/***************************************************************************************
 *
 * DO NOT MODIFY UNDER THIS LINE!!!
 *
 **************************************************************************************/

static struct itimerval time_quantum;
static struct sigaction ticker;

// __create_run_timer에 의해 지정된 시간마다 타이머 실행
// 현재 실행된 스레드 갯수가 1이상이면 다음 스레드로 실행을 순차적으로 돌리는 듯 함수
// 동시성 스레드 실행 coroutine으로 보임

void __scheduler() {
    printf("CHK : __scheduler\n");
//    if (n_tcbs > 1)
//        next_tcb();
}

 
// 타이머 실행
// https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=skssim&logNo=121292037
// scheduler 함수를 지정된 설정에 의해서 실행시키는 것 같음 ???
// 

void __create_run_timer() {
    printf("CHK : __create_run_timer\n");

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
    // 자료 구조에 담겨 있는 자료 순환하면서 자료를 리스트에서 삭제하고
    // 스레드 메모리 반환
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
