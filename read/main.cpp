//
//  main.cpp
//  read
//
//  Created by 黄梓原 on 02/12/2017.
//  Copyright © 2017 MotaWolley. All rights reserved.
//

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include  <sys/types.h>
#include  <stdio.h>
#include <unistd.h>

#define MUTEX "/mutexsem"
#define WRT "/wrtsem"

#define SHMSZ 27

using namespace std;

int main(int argc, const char * argv[]) {
    
    int lb = 0;
    int ub = 26;
    int time = 0;
    
    for (int i=0; i< argc; i++){
        if (string(argv[i]) == "-r"){
            try {
                stoi(argv[i+1]);
                stoi(argv[i+2]);
            } catch (const std::invalid_argument& ia) {
                cerr << "Invalid argument: " << ia.what() << '\n';
            }
            lb = stoi(argv[i+1]);
            ub = stoi(argv[i+2]);
        }
        if (string(argv[i]) == "-d"){
            try {
                stoi(argv[i+1]);
            } catch (const std::invalid_argument& ia) {
                cerr << "Invalid argument: " << ia.what() << '\n';
            }
            time = stoi(argv[i+1]);
        }
    }
    
    if (time < 0){
        cerr << "Invalid argument" << endl;
        time = 0;
    }
    
    if (lb<0 or ub>26 or lb>ub){
        cerr << "Invalid argument" << endl;
        lb = 0;
        ub = 26;
    }
    
    
    int shmid, shmid_rc;
    key_t key, key_rc;
    char *record;
    int *info;
    /*
     * We need to get the segment named
     * "5678", created by the server.
     */
    key = 5678;
    key_rc = 5600;

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((record = (char *) shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    
    if ((shmid_rc = shmget(key_rc, 4, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    
    if ((info = (int *) shmat(shmid_rc, NULL, 0)) == (int *) -1) {
        perror("shmat");
        exit(1);
    }
    
    sem_t *sem_mutex = sem_open(MUTEX, 0); /* Open a preexisting semaphore in other processes. */
    sem_t *sem_wrt = sem_open(WRT, 0);
    
    int ticket;
    
    //giving ticket to a new reader
    sem_wait(sem_mutex);
    if (info[3]==0){ //last ticket given to reader
        ticket = info[1];
    }
    else{ //last ticket given to writer
        info[1]++;
        ticket = info[1];
        info[3] = 0;
    }
    sem_post(sem_mutex);
    
    while (ticket > info[2]){
        sleep(1);
        cout << "waiting to read..." << endl;
    }
    
    sem_wait(sem_mutex);
    info[0] ++;
    if (info[0]==1){
        sem_wait(sem_wrt);
    }
    sem_post(sem_mutex);
    
    for (int i=lb; i<=ub; i++){
        cout << record[i];
    }
    cout << endl;
    cout << "sleeping for " << time << " seconds" << endl;
    sleep(time);
    
    sem_wait(sem_mutex);
    info[0] --;
    if (info[0]==0){
        sem_post(sem_wrt);
        if (ticket == info[2]){
            info[2] ++; // current ticket increases
        }
        
    }
    sem_post(sem_mutex);
//    cout << "ticket" << ticket << endl;
//    cout << "current ticket" << info[2] << endl;
    
}

