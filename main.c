#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define MAX_WAITING_STUDENTS 5
#define TOTAL_STUDENTS 10
#define TA_IDLE_TIMEOUT 3  // seconds to wait before terminating if no student arrives

pthread_mutex_t mutex;
pthread_cond_t ta_cond;
pthread_cond_t student_cond;

int waiting_students = 0;
int ta_sleeping = 1;
int students_helped = 0;
int ta_busy = 0;  // 0 means TA is free, 1 means busy helping a student

void* student(void* id) {
    int student_id = *(int*)id;
    free(id);

    // Simulate student arriving at random times
    sleep(rand() % 3);

    pthread_mutex_lock(&mutex);

    // Special case: if TA is sleeping and no waiting students, student goes directly to TA, no waiting count increment
    if (ta_sleeping && waiting_students == 0 && ta_busy == 0) {
        ta_busy = 1;  // TA is now busy helping this student
        printf("Student %d goes directly to the TA without waiting.\n", student_id);

        // Wake TA up
        ta_sleeping = 0;
        pthread_cond_signal(&ta_cond);

        // Wait for TA to finish helping
        pthread_cond_wait(&student_cond, &mutex);

        students_helped++;
        printf("Student %d is being helped by the TA.\n", student_id);
        printf("Student %d leaves the room.\n", student_id);

        ta_busy = 0;

    } else {
        if (waiting_students < MAX_WAITING_STUDENTS) {
            waiting_students++;
            printf("Student %d enters the room. Waiting students: %d\n", student_id, waiting_students);

            // Wake up TA in case sleeping
            if (ta_sleeping) {
                ta_sleeping = 0;
                pthread_cond_signal(&ta_cond);
            }

            // Wait until TA signals this student to be helped
            pthread_cond_wait(&student_cond, &mutex);
            students_helped++;
            waiting_students--;
            printf("Student %d is being helped by the TA.\n", student_id);
            printf("Student %d leaves the room. Waiting students: %d\n", student_id, waiting_students);

            // Mark TA as free after helping this student
            ta_busy = 0;

        } else {
            printf("Student %d finds no available chairs and leaves.\n", student_id);
        }
    }

    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* ta(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // TA sleeps if no students waiting and not busy
        while (waiting_students == 0 && ta_busy == 0) {
            if (students_helped >= TOTAL_STUDENTS) {
                pthread_mutex_unlock(&mutex);
                printf("TA is done helping all students and is exiting.\n");
                return NULL; // exit TA thread
            }
            ta_sleeping = 1;
            printf("TA is sleeping.\n");

            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += TA_IDLE_TIMEOUT;

            int wait_res = pthread_cond_timedwait(&ta_cond, &mutex, &ts);
            if (wait_res == ETIMEDOUT) {
                pthread_mutex_unlock(&mutex);
                printf("TA waited for %d seconds with no arriving students, terminating program.\n", TA_IDLE_TIMEOUT);
                return NULL; // exit TA thread due to timeout
            }
        }

        // Help next student if there are waiting students
        if (waiting_students > 0) {
            ta_busy = 1;
            // Signal one waiting student to be helped
            pthread_cond_signal(&student_cond);
            printf("TA is helping a student.\n");

        } else if (ta_busy) {
            // TA is helping the direct student
            printf("TA is helping a student.\n");
        }

        pthread_mutex_unlock(&mutex);

        // Simulate helping time
        sleep(2);
    }

    return NULL;
}

int main() {
    pthread_t ta_thread;
    pthread_t students[TOTAL_STUDENTS];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&ta_cond, NULL);
    pthread_cond_init(&student_cond, NULL);

    srand(time(NULL));

    // Create TA thread
    pthread_create(&ta_thread, NULL, ta, NULL);

    // Create student threads
    for (int i = 0; i < TOTAL_STUDENTS; i++) {
        int* student_id = malloc(sizeof(int));
        *student_id = i + 1;
        pthread_create(&students[i], NULL, student, student_id);
    }

    // Wait for all students to finish
    for (int i = 0; i < TOTAL_STUDENTS; i++) {
        pthread_join(students[i], NULL);
    }

    // Signal the TA in case should exit
    pthread_mutex_lock(&mutex);
    if (waiting_students == 0 && ta_busy == 0) {
        pthread_cond_signal(&ta_cond);
    }
    pthread_mutex_unlock(&mutex);

    // Wait for TA thread to finish
    pthread_join(ta_thread, NULL);

    printf("Program has terminated.\n");

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&ta_cond);
    pthread_cond_destroy(&student_cond);

    return 0;
}

