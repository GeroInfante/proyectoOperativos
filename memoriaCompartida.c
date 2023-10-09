#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/shm.h>

#define KEY ((key_t) 1234)
#define SEGSIZE sizeof(struct semaforos)


struct semaforos{
    sem_t sem_A, sem_B, sem_C, sem_D, sem_E;
};

int main() {    

    int id;
    pid_t pid;
    struct semaforos * sem;

    //Creo el segmento de memoria compartida
    id = shmget (KEY, SEGSIZE, IPC_CREAT | 0666);
    sem = (struct semaforos *) shmat(id, 0, 0);

    if(sem<= (struct semaforos *)(0)){
        printf("Error en el shmat\n");
        exit(0);
    }

    //inicializo los semaforos
    sem_init(&sem->sem_A , 1 , 1);
    sem_init(&sem->sem_B , 1 , 0);
    sem_init(&sem->sem_C , 1 , 0);
    sem_init(&sem->sem_D , 1 , 0);
    sem_init(&sem->sem_E , 1 , 0);

    pid = fork();

    if(pid == 0){
        //proceso A
            while(1){
                sem_wait(&sem->sem_A);
                printf("A\n");
                sem_post(&sem->sem_B);
            }
        exit(0);
    }

    pid = fork();

    if(pid == 0){
        //proceso A
            while(1){
                sem_wait(&sem->sem_B);
                printf("B\n");
                sem_post(&sem->sem_C);
            }
        exit(0);
    }

    pid = fork();

    if(pid == 0){
        //proceso A
            while(1){
                sem_wait(&sem->sem_C);
                printf("C\n");
                sem_post(&sem->sem_D);
            }
        exit(0);
    }

    pid = fork();

    if(pid == 0){
        //proceso A
            while(1){
                sem_wait(&sem->sem_D);
                printf("D\n");
                sem_post(&sem->sem_E);
            }
        exit(0);
    }

    pid = fork();

    if(pid == 0){
        //proceso A
            int seguir = 1;
            while(seguir){
                sem_wait(&sem->sem_E);
                printf("E\n");
                printf("Desea imprimir otra secuencia? Y/n");
                scanf("%i", &seguir);
                sem_post(&sem->sem_A);
            }
        exit(0);
    }

    for(int i=0 ; i<5 ; i++)
            wait(NULL);
    exit(0);
}
