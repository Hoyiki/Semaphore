//
//  main.cpp
//  Semaphore
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

#define MUTEX "/mutexsem"
#define WRT "/wrtsem"

#define SHMSZ 27

using namespace std;

int main(int argc, const char * argv[]) {
    
    sem_t *sem_mutex = sem_open(MUTEX, O_CREAT, 0644, 1);
    
    sem_t *sem_wrt = sem_open(WRT, O_CREAT, 0644, 1);

    
    int shmid;
    key_t key;
    char *record, *s;
    
    key = 5678;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    
    /*
     * Now we attach the segment to our data space.
     */
    shmat(shmid, NULL, 0);
    if ((record = (char *) shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    
    /*
     * Now put some things into the memory for the
     * other process to read.
     */
    s = record;
    for (int i=0; i<=26; i++){
        s[i]='a';
    }
    
    
    int shmid_rc;
    int *info;
    key_t key_rc;
    
    key_rc = 5600;
    if ((shmid_rc = shmget(key_rc, 4, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    
    /*
     * Now we attach the segment to our data space.
     */
    shmat(shmid_rc, NULL, 0);
    if ((info = (int *) shmat(shmid_rc, NULL, 0)) == (int *) -1) {
        perror("shmat");
        exit(1);
    }
    //0-readCount, 1-lastTicket: last one given to the new comers, 2-currentTicket, 3-lastType:R-0/W-1
    info[0] = 0;
    info[1] = 0;
    info[2] = 0;
    info[3] = 0;
    
    while(true){
        ;
    }
    

}
