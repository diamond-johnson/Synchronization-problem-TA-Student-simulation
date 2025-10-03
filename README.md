# Synchronization Problem: TA-Student Simulation

## Overview

This project simulates the classic "Sleeping Barber" synchronization problem, adapted to a Teaching Assistant (TA) and students scenario, using multi-threading to demonstrate thread synchronization in a concurrent environment. Students arrive at random intervals and attempt to get help from the TA; if fewer than 5 students are waiting, they sit and wait; otherwise, they leave. The TA helps one student at a time, sleeps if no students are waiting (with a 3-second timeout before terminating), and uses mutexes and condition variables to coordinate access and signaling without race conditions.
 
The simulation runs with 10 students and tracks metrics like waiting students and total helped, ensuring proper synchronization to prevent issues like multiple students being helped simultaneously or the TA waking unnecessarily.

## Key Features

- **Student Behavior**: Students arrive randomly (0-2 seconds delay), check for available "chairs" (max 5 waiting), wait for TA signal, get helped, and leave.
- **TA Behavior**: TA sleeps when idle, wakes on student arrival, helps one student at a time (2-second simulation), and terminates after all students or on timeout.
- **Synchronization Primitives**: Uses `pthread_mutex_t` for mutual exclusion, `pthread_cond_t` for condition signaling (TA wakes students, students wake TA), and timed waits for idle TA termination.
- **Edge Cases**: Handles direct access if TA is sleeping and no queue, student rejection if full, and program termination after all students are helped.

## Prerequisites

- A Unix-like system (Linux, macOS) with GCC compiler.
- POSIX threads library (pthreads) support (standard on most Unix systems).

## How to Compile and Run

1. **Save the Code**: Place the provided C code in a file named `ta_student_simulation.c`.

2. **Compile**:
   ```
   gcc -o ta_student_simulation ta_student_simulation.c -lpthread
   ```
   The `-lpthread` flag links the POSIX threads library.

3. **Run**:
   ```
   ./ta_student_simulation
   ```
   The program will output real-time events like student arrivals, waiting counts, TA actions, and terminations. Each run may vary due to random sleep times.

4. **Expected Runtime**: Approximately 20-30 seconds, depending on random arrivals and helping times.

## Sample Output

```
Student 1 goes directly to the TA without waiting.
Student 1 is being helped by the TA.
Student 1 leaves the room.
TA is helping a student.
Student 2 enters the room. Waiting students: 1
TA is sleeping.
Student 3 enters the room. Waiting students: 2
TA is helping a student.
Student 2 is being helped by the TA.
Student 2 leaves the room. Waiting students: 1
... (continues until all students are processed)
TA is done helping all students and is exiting.
Program has terminated.
```

Note: Output varies per run due to randomness, but all students should be helped or rejected appropriately, with no synchronization errors.

## References

- Classic Problem: [Sleeping Barber Problem](https://en.wikipedia.org/wiki/Sleeping_barber_problem) (adapted here to TA-Student).
- POSIX Threads Documentation: [pthreads Man Pages](https://man7.org/linux/man-pages/man7/pthreads.7.html).
