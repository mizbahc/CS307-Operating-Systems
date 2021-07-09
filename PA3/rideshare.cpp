

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <pthread.h>

#ifdef linux
#include <semaphore.h>
#elif __APPLE__
#include "zemaphore.h"
#endif
using namespace std;

pthread_cond_t waitCar;
pthread_cond_t waitPrint;
pthread_mutex_t lock;


int readyA = 0;
int readyB = 0;

int goneA = 0;
int goneB = 0;


//Function declerations
void* goA(void* a);
void* goB(void* b);


void* goA(void* a) {
    pthread_mutex_lock(&lock);

    while ((readyA + readyB > 4)) {
        pthread_cond_wait(&waitPrint, &lock);

    }
    while ((readyA + readyB > 3) && (readyA == 0 || readyA == 2 || readyA > 3)) {
        pthread_cond_wait(&waitPrint, &lock);

    }
    if ((readyA == 1 && readyB >= 2) || readyA == 3) {
        cout << "Thread ID:" << pthread_self() << ", Team A, I am looking for a car" << endl;
        cout << "Thread ID:" << pthread_self() << ", Team A, I have found a spot in a car" << endl;
        readyA++;
        goneA++;
        pthread_cond_signal(&waitCar);
        pthread_mutex_unlock(&lock);
    }
    else {
        readyA++;
        cout << "Thread ID:" << pthread_self() << ", Team A, I am looking for a car" << endl;

        while ((goneA + goneB) < 1 ) {
            pthread_cond_wait(&waitCar, &lock);
        }

        while ((goneA + goneB) < 1  || readyA < 2 ) {
            pthread_cond_signal(&waitCar);
            pthread_cond_wait(&waitCar, &lock);
        }
        while (1) {
            if (readyA == 4) {
                cout << "Thread ID:" << pthread_self() << ", Team A, I have found a spot in a car" << endl;
                goneA++;
                if (goneA == 4) {
                    readyA = 0;
                    goneA = 0;
                    cout << "Thread ID:" << pthread_self() << ", Team A, I am the captain and driving the car" << endl;
                    pthread_cond_broadcast(&waitPrint);
                    pthread_mutex_unlock(&lock);

                }
                else
                {
                    pthread_cond_signal(&waitCar);
                    pthread_mutex_unlock(&lock);
                }

                break;

            }
            else if (goneA < 2)
            {
                cout << "Thread ID:" << pthread_self() << ", Team A, I have found a spot in a car" << endl;
                goneA++;
                if (goneA + goneB == 4) {
                    readyA -= 2;
                    readyB -= 2;
                    goneA = 0;
                    goneB = 0;

                    cout << "Thread ID:" << pthread_self() << ", Team A, I am the captain and driving the car" << endl;
                    pthread_cond_broadcast(&waitPrint);
                    pthread_mutex_unlock(&lock);

                }
                else
                {
                    pthread_cond_signal(&waitCar);
                    pthread_mutex_unlock(&lock);
                }
                break;

            }
            else
            {
                if (goneA + goneB == 4) {
                    readyA -= 2;
                    readyB -= 2;
                    goneA = 0;
                    goneB = 0;

                }
                pthread_cond_signal(&waitCar);
                pthread_cond_wait(&waitCar, &lock);

            }

        }


    }


}


void* goB(void* a) {
    pthread_mutex_lock(&lock);
    while ((readyA + readyB > 4)) {
        pthread_cond_wait(&waitPrint, &lock);

    }
    while ((readyA + readyB > 3) && (readyB == 0 || readyB == 2 || readyB > 3)) {
        pthread_cond_wait(&waitPrint, &lock);

    }
    if ((readyB == 1 && readyA >= 2) || readyB == 3) {
        cout << "Thread ID:" << pthread_self() << ", Team B, I am looking for a car" << endl;
        cout << "Thread ID:" << pthread_self() << ", Team B, I have found a spot in a car" << endl;
        readyB++;
        goneB++;
        pthread_cond_signal(&waitCar);
        pthread_mutex_unlock(&lock);
    }
    else {
        readyB++;
        cout << "Thread ID:" << pthread_self() << ", Team B, I am looking for a car" << endl;
        while ((goneA + goneB) < 1  ) {
            pthread_cond_wait(&waitCar, &lock);
        }
        while ( (goneA + goneB) < 1 || readyB < 2) {
            pthread_cond_signal(&waitCar);
            pthread_cond_wait(&waitCar, &lock);
        }

        while (1) {
            if (readyB == 4) {
                cout << "Thread ID:" << pthread_self() << ", Team B, I have found a spot in a car" << endl;
                goneB++;
                if (goneB == 4) {
                    readyB = 0;
                    goneB = 0;
                    cout << "Thread ID:" << pthread_self() << ", Team B, I am the captain and driving the car" << endl;
                    pthread_cond_broadcast(&waitPrint);
                    pthread_mutex_unlock(&lock);


                }
                else
                {
                    pthread_cond_signal(&waitCar);
                    pthread_mutex_unlock(&lock);

                }
                break;

            }
            else if (goneB < 2)
            {
                cout << "Thread ID:" << pthread_self() << ", Team B, I have found a spot in a car" << endl;
                goneB++;
                if (goneA + goneB == 4) {
                    readyA -= 2;
                    readyB -= 2;
                    goneA = 0;
                    goneB = 0;

                    cout << "Thread ID:" << pthread_self() << ", Team B, I am the captain and driving the car" << endl;
                    pthread_cond_broadcast(&waitPrint);
                    pthread_mutex_unlock(&lock);


                }
                else {
                    pthread_cond_signal(&waitCar);
                    pthread_mutex_unlock(&lock);
                }
                break;


            }
            else
            {
                if (goneA + goneB == 4) {
                    readyA -= 2;
                    readyB -= 2;
                    goneA = 0;
                    goneB = 0;

                }
                pthread_cond_signal(&waitCar);
                pthread_cond_wait(&waitCar, &lock);
            }
        }
    }
}


int main(int argc, char* argv[]) {

    int A = atoi(argv[1]); // number of Team A fans.
    int B = atoi(argv[2]); /// number of Team B fans.

    if ((A + B) % 4 == 0 && A % 2 == 0 && B % 2 == 0) {
        pthread_t a[A];
        pthread_t b[B];

        for (int i = 0; i < A; i++)
        {
            int rc1 = pthread_create(&a[i], NULL, goA, NULL);

        }
        for (int i = 0; i < B; i++)
        {
            int rc1 = pthread_create(&b[i], NULL, goB, NULL);

        }

        for (int i = 0; i < A; i++)
        {
            (void)pthread_join(a[i], NULL);
        }
        for (int i = 0; i < B; i++)
        {

            (void)pthread_join(b[i], NULL);

        }

    }
    cout << "The main terminates" << endl;


    return 0;
}
