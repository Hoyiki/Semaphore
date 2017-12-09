//
//  main.cpp
//  write
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
    
    if (lb<0 or ub>26 or lb>ub){
        cerr << "Invalid argument" << endl;
        lb = 0;
        ub = 26;
    }
    
    if (time < 0){
        cerr << "Invalid argument" << endl;
        time = 0;
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
    
    sem_t *sem_wrt = sem_open(WRT, 0);
    sem_t *sem_mutex = sem_open(MUTEX, 0);
    int ticket;
    
    sem_wait(sem_mutex);
    info[1] ++;
    ticket = info[1];
    if (ticket==1 and info[2]==0){ //might be the case that writer comes first, need to manually raise the current ticket
        info[2]++;
    }
    if (info[3]==0){
        info[3] = 1;
    }
    sem_post(sem_mutex);
    
    while (ticket != info[2]){
//        cout <<"ticket"<< ticket << "info2" << info[2] << endl;
        sleep(1);
        cout << "waiting to write..." << endl;
    }
    
    sem_wait(sem_wrt);
    
    char letters[] = "abcdefghijklmnopqrstuvwxyz";
    for (int i = lb; i <= ub; i++){
        record[i] = letters[ticket % 26];
    }
    
    cout << "sleeping for " << time << " seconds" << endl;
    sleep(time);
    
//    record[0] = 'b';
    
    sem_post(sem_wrt);
    
    sem_wait(sem_mutex);
    info[2] ++;
    sem_post(sem_mutex);
    
    
//    cout << "current ticket" << info[2] << endl;
    
}
