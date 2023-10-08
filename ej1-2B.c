#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int semA[2], semB[2], semC[2], semD[2];
int varSincronizacion;


void* printA(){
    while(1){
        read(semA[0], &varSincronizacion, sizeof(varSincronizacion));
        printf("A\n");
        write(semB[1], &varSincronizacion, sizeof(varSincronizacion));
    }
}
void* printB(){
    while(1){
        read(semB[0], &varSincronizacion, sizeof(varSincronizacion));
        printf("B\n");
        write(semC[1], &varSincronizacion, sizeof(varSincronizacion));
        read(semB[0], &varSincronizacion, sizeof(varSincronizacion));
        write(semA[1], &varSincronizacion, sizeof(varSincronizacion));
    }
}

void* printC(){
    while(1){
        read(semC[0], &varSincronizacion, sizeof(varSincronizacion));
        write(semB[1], &varSincronizacion, sizeof(varSincronizacion));
        read(semC[0], &varSincronizacion, sizeof(varSincronizacion));
        printf("C\n");
        write(semB[1], &varSincronizacion, sizeof(varSincronizacion));
        read(semC[0], &varSincronizacion, sizeof(varSincronizacion));
        printf("C\n");
        write(semD[1], &varSincronizacion, sizeof(varSincronizacion));
        
    }
}

void* printD(){
    while(1){
        read(semD[0], &varSincronizacion, sizeof(varSincronizacion));
        printf("D\n");
        write(semB[1], &varSincronizacion, sizeof(varSincronizacion));
        
    }
}


int main() {
	pipe(semA);
	pipe(semB);
	pipe(semC);
	pipe(semD);
	
	pid_t pid;

	pid=fork();
	if(pid==0){// Proceso para printear A.
		close(semA[1]);
		close(semB[0]);
		close(semC[0]);
		close(semC[1]);
		close(semD[0]);
		close(semD[1]);
		printA();
		exit(0);
	}
	
	pid=fork();
	if(pid==0){//Proceso para printear B
		close(semA[0]);
		close(semB[1]);
		close(semC[0]);
		close(semD[0]);
		close(semD[1]);
		printB();
		exit(0);
	}

	pid=fork();
	if(pid==0){//Proceso para printear C
		close(semA[0]);
		close(semA[1]);
		close(semB[0]);
		close(semC[1]);
		close(semD[0]);
		printC();
		exit(0);
	}

	pid=fork();
	if(pid==0){//Proceso para printear D
		close(semA[0]);
		close(semA[1]);
		close(semB[0]);
		close(semC[0]);
		close(semC[1]);
		close(semD[1]);
		printD();
		exit(0);
	}
	
	//Inicializamos los semaforos implementados con pipes.
	write(semA[1], &varSincronizacion, sizeof(varSincronizacion));
	close(semA[1]);
	
	close(semA[0]);
	close(semB[0]);
	close(semB[1]);
	close(semC[0]);
	close(semC[1]);
	close(semD[0]);
	close(semD[1]);

    // Esperar a que los hilos terminen (esto normalmente no se hace en un programa de este tipo)
    
    for(int i=0; i<4; i++){
		wait(NULL);
	}

    return 0;
}
