

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>

#ifdef linux
#include <semaphore.h>
#elif __APPLE__
#include "zemaphore.h"
#endif
using namespace std;

pthread_mutex_t lock; //lock to prevent race.

struct listNode{
    int id;
    int size;
    int index;

    listNode * next;
    listNode * previous;

    listNode():next(NULL), previous(NULL){}

    listNode(int _id, int _size, int _index)
        :id(_id), size(_size), index(_index), next(NULL), previous(NULL){}
};



class HeapManager{
    public:
        HeapManager():head(NULL){}

        int initHeap(int size){;
            head = new listNode(-1,size,0);
            print();
            return 1;
        }

        void print(){
            listNode * p = head;
            int count = 0;

            while(p){
                if(count > 0)
                    cout << "---";
                cout << "[" << p->id << "]" << "[" << p->size << "]" << "[" << p->index << "]";
                count++;
                p = p->next;
            }
            cout << endl;
        }


        int myMalloc(int id, int size){
            pthread_mutex_lock(&lock);
            listNode * p = head;
            bool found = false;
            while(p){
                if(p->id == -1 && p->size >= size){
                    int remaining = p->size - size;

                    
                    p->id = id;
                    p->size = size;
                    if(remaining > 0){
                        listNode * right = new listNode(-1,remaining, p->index + size);
                        right->next = p->next;
                        right->previous = p;
                        p->next = right;

                        if(right->next)
                            right->next->previous = right;
                    }
                    found = true;
                    break;
                }
                else{
                    p = p->next;
                }
            }

            if(found){
                cout << "Allocated for thread " << id << endl;
                print();
                pthread_mutex_unlock(&lock);
                return p->index;
            }

            cout << "Can not allocate, requested size "<< size << " for thread "<< id << " is bigger than remaining size " << endl;
            print();
            pthread_mutex_unlock(&lock);

            return -1;

        }


         int myFree(int id, int index){
            pthread_mutex_lock(&lock);
            listNode * p = head;
            bool found = false;
            while(p){
                if(p->id == id && p->index == index){
                    p->id = -1;

                    if(p->next && p->next->id == -1){
                        p->size = p->size + p->next->size;
                        p->next = p->next->next;
                        if(p->next)
                            p->next->previous = p;
                    }

                    if(p->previous && p->previous->id == -1){
                        p->previous->size = p->previous->size + p->size;
                        p->previous->next = p->next;
                        if(p->next)
                            p->next->previous = p->previous;

                    }

                    found = true;

                    break;
                }
                else{
                    p = p->next;
                }

            }

            if(found){
                cout << "Freed for thread " << id << endl;
                print();
                pthread_mutex_unlock(&lock);
                return 1;
            }

            cout << "No allocated memory found for thread "<< id << " at index "<< index  << endl;
            print();
            pthread_mutex_unlock(&lock);

            return -1;

        }

    private:
        listNode * head; // list with head node pointer
};
