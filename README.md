## Project #2: My Thread Scheduler

### *** Due on 11:59pm, May 25 (Wednesday)***

### Goal
This programming assignment is to (1) implement your own user-level thread library and then (2) make schedulers (e.g., FIFO, RR, and SJF) for the threads. The goal of this project is to help you understand the role of a CPU scheduler in operating systems and the principle of how multiple threads are executed concurrently. 

### Background
- A thread is the smallest execution unit that can be managed by a scheduler. One process can create multiple threads that share code, data, and heap space in the same address space. In Linux, the Pthread library provides either a user-level or kernel-level thread.

- The user-level threads are implemented by users and the kernel is not aware of these threads. A kernel just knows them as single-threaded process. The user-level threads can create more easier and faster than kernel-level threads because the kernel does not interveve. However, the user-level threads are considered as single threaded process so that they can not fully utilize resource of multiprocessors or multicores.

- A CPU Scheduling is a policy deciding which process to run next, given a set of runnable processes. The scheduling policy has two types thar are non-preemptive scheduling and preemptive scheduiling.

### Problem Specification
- Make your own user level thread library and scheduler in `uthread.c`. The functions that will be implemented by you have `TODO` annotation.

#### User-level thread library and Scheduling
- Your first task is to implement your own user-level thread library with `ucontext.h`. This library provides some functions and structure that allow user-level context switching.

- With the user-level threads spawned from the library, your second job is to make your own scheduler to schedule them. Use `signal.h` to implement the scheduler.

- You are required to implement four scheduling policy (First-In-First-Out, Rount-Robin, Priority, and Shortest-Job-First). Please check the lecture notes to remind how they work.

- When each thread is terminated, they must inform their state to the main thread. So, you have to create the context that will be resumed after the terminated thread.

#### Testcases and Output
```
FIFO
CREATE 0 4 0
CREATE 1 3 0
JOIN 0
JOIN 1 
```
- In the `testcases` directory, you can see several `.txt` file. They are the input of `pa2.c`. An example of input is above.
- The first line of input file indicates the scheduling policy used in program. 
- The first column, **CREATE**, makes a function call to `uthread_create`. Each of the following columns means *tid*, *lifetime*, and *priority*, respectively. The **JOIN** command invokes the `uthread_join` function. 0 means the target thread’s tid is 0.
- When you implement `uthread_create`, use this information.

```
SWAP -1 -> 0
SWAP 0 -> 1
SWAP 1 -> -1
JOIN 0
JOIN 1
```
- In the `outputs` directory, you can see several `.txt` file. They are the output of `pa2.c`. An exmaple of output file is above.
- The **SWAP** keyword represents that the scheduler does context switch. For instance, the first line presents the scheduler switches -1 (tid) to 0 (tid). **JOIN** 0 means that the main thread joins successfully thread 0.

### Restriction and hints
- For your coding practice, the compiler is set to halt on some (important) warnings. Write your code to fully comply the C99 standard.

- Please check carefully the Linux Manual Page. There is a lot of information for the project.

- You have to print your output below format. Please do not print in situation that SWAP main -> main. 
```
fprintf(stderr, "SWAP %d -> %d\n", prev->tid, next->tid);
fprintf(stedrr, "JOIN %d\n", current->tid);
```

### Submission / Grading
- Source: ***uthread.c***

- Free to make a question through AjouBb. However, **YOU MIGHT NOT GET AN ANSWER IF THE ISSUE/TOPIC IS ALREADY DISCUSSED ON THIS HANDOUT**.

- **QUESTIONS OVER EMAIL WILL BE IGNORED unless it concerns your privacy**.
