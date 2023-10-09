#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


sem_t mesaEntrada, escribir, entrar;

sem_t colaClienteComun, entrarClienteComun, hayClienteEsperando, empleado3_libre, empleado3_atiende; //Asociado a Cliente comun.

sem_t colaClienteEmpresa, hayEmpresarioEsperando, hayEmpleadoLibre, empleado1_libre, terminaEmpleado1, empleado1_atiende, serAtendido, empleado2_libre, terminaEmpleado2, empleado2_atiende;

sem_t colaClientePolitico;


void* metodoEmpleado3(){
    while(1){
        sem_wait(&hayClienteEsperando);//Empleado3 espera a que tenga Usuarios comunes para atender.
        sem_wait(&empleado3_libre);// Este tiene que estar libre para poder atender.
        sem_post(&empleado3_atiende);// Si hay clientes comunes y esta libre, este puede atender a uno.
        sleep(0.05);//Esta atiendiendo --> Hay veces que con esta sin comentar se traba.
    }

}


void *clienteComun(){
    sem_wait(&entrar);
    if (sem_trywait(&mesaEntrada) == 0) {//Cliente intenta entrar al Banco
        sem_wait(&escribir);
        printf("Cliente se unió a la colaMesaEntrada, aún hay espacios en la cola\n");
        sem_post(&escribir);
        sem_post(&entrar);

        //El cliente entro al banco

        sem_wait(&colaClienteComun);
        sem_wait(&escribir);
        printf("cliente comun ingreso a cola de clientes comunes\n");
        sem_post(&escribir);

        sem_post(&mesaEntrada);//libera cola para mesa de entrada cuando ingresa a colaClienteComun


        //Espera a ser atendido por el empleado3:

        sem_post(&hayClienteEsperando);// Le avisa al empleado3 que hay almenos un cliente esperando a ser atendido.
        sem_wait(&empleado3_atiende);// Espera a ser atendido por el empleado3.


        //Una vez es atendido, libera la colaCliente comun :
        sem_post(&colaClienteComun);

        //probablemente se necesite un semaforo para recibir el aviso de que ya fue atendido.
        //una vez recibido el aviso:
        sleep(0.05);
        sem_post(&empleado3_libre);



        sem_wait(&escribir);
        printf("Cliente comun es atendido por el empleado y libero la colaClienteComun 3\n");
        sem_post(&escribir);
        //sleep(0.05);

        sem_post(&empleado3_libre);//Termina de ser atendido, entonces libera al empleado3.
    }else{
        printf("La cola está llena, el cliente se va del banco\n");
        sem_post(&entrar);
    }

}


//void *clientePolitico(){

//}

void* metodoEmpleado1(){
    while(1){
        sem_post(&hayEmpleadoLibre);//Al inicial su trabajo o cada vez que finaliza indica que hay empleado libre.
        sem_wait(&hayEmpresarioEsperando);// Espera hasta que haya algun empresario para atender.
        sem_wait(&empleado1_libre);// Hasta que no esta libre no atiende.
        sem_post(&empleado1_atiende);// Si esta libre, atiende a algun empresario.
        sleep(0.05);// Esta atendiendo.
        sem_wait(&terminaEmpleado1);// Espera hata que el empresario decida terminar su consulta con el empleado.
    }
}
void* metodoEmpleado2(){
    while(1){
        sem_post(&hayEmpleadoLibre);//Al inicial su trabajo o cada vez que finaliza indica que hay empleado libre.
        sem_wait(&hayEmpresarioEsperando);// Espera hasta que haya algun empresario para atender.
        sem_wait(&empleado2_libre);// Hasta que no esta libre no atiende.
        sem_post(&empleado2_atiende);// Si esta libre, atiende a algun empresario.
        sleep(0.05);// Esta atendiendo.
        sem_wait(&terminaEmpleado2);// Espera hata que el empresario decida terminar su consulta con el empleado.
    }
}


void *clienteEmpresa(){
    sem_wait(&entrar);
    if (sem_trywait(&mesaEntrada) == 0) {//Cliente intenta entrar al Banco
        sem_wait(&escribir);
        printf("Cliente se unió a la colaMesaEntrada, aún hay espacios en la cola\n");
        sem_post(&escribir);
        sem_post(&entrar);

        //El empresario entro al banco

        sem_wait(&colaClienteEmpresa);// Espera en cola mesa entrada hasta entrar en cola propia de empresarios.
        sem_wait(&escribir);
        printf("Empresario ingreso a cola de Empresarios\n");
        sem_post(&escribir);
        sem_post(&mesaEntrada);//libera cola para mesa de entrada cuando ingresa a colaClienteComun

        //Espera a ser atendido por algun empleado:
        sem_post(&hayEmpresarioEsperando);// Le avisa los empleados 1 y 2 que hay almenos un empresario esperando a ser atendido.
        sem_wait(&hayEmpleadoLibre);//Empresario espera hasta que algun empleado este libre.


        sem_wait(&serAtendido); //Funciona de la misma manera que &entrar, para que ejecute bien el trywait de empleado 1 atiende.

        if(sem_trywait(&empleado1_atiende)==0){
            sem_post(&serAtendido);
            sem_post(&colaClienteEmpresa);//Una vez es atendido libera la cola de Empresarios.

            sem_wait(&escribir);
            printf("Cliente empresa es atendido por el empleado1 y libero la colaClienteEmpresa -->1\n");
            sem_post(&escribir);

            sleep(0.05);//Esta siendo atendido.
            sem_post(&terminaEmpleado1);//Termina de ser atendido.

            sem_post(&empleado1_libre);//libera a el empleado1.

        }else{
            sem_wait(&empleado2_atiende);// Si no es atendido por empleado1 si o si es atendido por el empleado 2 ya que entro porq le avisaron que habia empleado libre.
            sem_post(&serAtendido);
            sem_post(&colaClienteEmpresa);//Una vez es atendido libera la cola de Empresarios.
            sem_wait(&escribir);
            printf("Cliente empresa es atendido por el empleado2 y libero la colaClienteEmpresa -->2\n");
            sem_post(&escribir);
            sleep(0.05);//Este siendo atendido.
            sem_post(&terminaEmpleado2);//Termina de ser atendido.

            sem_post(&empleado2_libre);//libera a el empleado1.
        }
    }else{
        printf("La cola está llena, el cliente se va del banco\n");
        sem_post(&entrar);
    }
    return NULL;
}

//_-----------------

int main() {
    sem_init(&mesaEntrada, 0, 30);
    sem_init(&escribir, 0, 1);
    sem_init(&entrar, 0, 1);
    sem_init(&colaClientePolitico, 0, 15);

    //Semaforos para Cliente comun:
    sem_init(&colaClienteComun, 0, 15);

    //Semaforos para Empleado 3:
    sem_init(&empleado3_libre, 0, 1);
    sem_init(&hayClienteEsperando,0,0);
    sem_init(&empleado3_atiende, 0, 0);

    //Semaforos para Empresario:
    sem_init(&colaClienteEmpresa, 0, 15);
    sem_init(&hayEmpresarioEsperando, 0, 0);
    sem_init(&serAtendido, 0, 1);

    //Semaforo empleado 1 y 2:
    sem_init(&hayEmpleadoLibre, 0, 0);//Lo comparten entre los dos.

    sem_init(&empleado1_libre, 0, 1);
    sem_init(&terminaEmpleado1, 0, 0);
    sem_init(&empleado1_atiende, 0, 0);

    sem_init(&empleado2_libre, 0, 1);
    sem_init(&terminaEmpleado2, 0, 0);
    sem_init(&empleado2_atiende, 0, 0);

    //Creacion de hilos para empleados, politicos, empresarios y clientes comunes:
    pthread_t hiloEmpleado3;
    pthread_create(&hiloEmpleado3, NULL, metodoEmpleado3, NULL);

    pthread_t hiloEmpleado1;
    pthread_create(&hiloEmpleado1, NULL, metodoEmpleado1, NULL);

    pthread_t hiloEmpleado2;
    pthread_create(&hiloEmpleado2, NULL, metodoEmpleado2, NULL);

    pthread_t threadsClientesComunes[47];
    for(int i = 0; i < 47; i++) {
        pthread_create(&threadsClientesComunes[i], NULL, clienteComun, NULL);
    }

    pthread_t threadsClientesEmpresa[47];
    for(int i = 0; i < 47; i++) {
        pthread_create(&threadsClientesEmpresa[i], NULL, clienteEmpresa, NULL);
    }

    for(int i = 0; i < 47; i++) {
        pthread_join(threadsClientesComunes[i], NULL);
    }

    for(int i = 0; i < 47; i++) {
        pthread_join(threadsClientesEmpresa[i], NULL);
    }

    sem_destroy(&mesaEntrada);
    sem_destroy(&escribir);
    sem_destroy(&entrar);
    sem_destroy(&colaClienteComun);
    sem_destroy(&colaClientePolitico);
    sem_destroy(&empleado3_libre);
    sem_destroy(&empleado3_atiende);
    sem_destroy(&hayClienteEsperando);
    sem_destroy(&colaClienteEmpresa);
    sem_destroy(&hayEmpresarioEsperando);
    sem_destroy(&hayEmpleadoLibre);
    sem_destroy(&empleado1_libre);
    sem_destroy(&terminaEmpleado1);
    sem_destroy(&empleado1_atiende);
    sem_destroy(&serAtendido);
    sem_destroy(&empleado2_libre);
    sem_destroy(&terminaEmpleado2);
    sem_destroy(&empleado2_atiende);


    return 0;
}