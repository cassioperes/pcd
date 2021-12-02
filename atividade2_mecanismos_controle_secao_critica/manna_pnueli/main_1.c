/*
UNIFESP - UNIVERSIDADE FEDERAL DE SAO PAULO
PROGRAMACAO CONCORRENTE E DISTRIBUIDA
PROJETO: 
Algoritmo Cliente-Servidor de Manna Pnueli - versao OpenMP

INTEGRANTES:
Cassio Peres Vicente, 69421
Isabella de Souza Navarro, 86828
*/

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<omp.h>
#include<unistd.h>

#define NUM_THREADS 4
#define USE_PRE_PROTOCOL false

#define ITERATIONS 1000000000
#define RESET_COLOR 0
#define SERVER_COLOR 5

int request = 0;
int respond = 0;
int SOMA = 0;

char *getTextColorCode(int i){
    if(i == 1){
        return "\033[0;41m"; //vermelho
    }
    else if(i == 2){
        return "\033[0;42m"; //verde
    }
    else if(i == 3){
        return "\033[0;43m"; //amarelo
    }
    else if(i == 4){
        return "\033[0;44m"; //azul
    }
    else if(i == 5){
        return "\033[0;45m"; //rosa - cor do servidor
    }
    else{
        return "\033[0m"; //retorna a cor original
    }
}

void clientMsg(int c){
    char *msg = "%s Cliente %d %s incrementou soma na secao critica. Soma = %d \n";
    printf(msg, getTextColorCode(c), c, getTextColorCode(0), SOMA);
}

void serverMsg(int c){
    char *msg = "%s Servidor %s%s Cliente %d %s entrou na secao critica ";
    printf(msg, getTextColorCode(SERVER_COLOR), getTextColorCode(RESET_COLOR), 
        getTextColorCode(c), c, getTextColorCode(RESET_COLOR));
}

void client(int i){
    int count = 0;
    while(count < ITERATIONS){
        while(respond != i && USE_PRE_PROTOCOL){
            request = i;
        }

        //SECAO CRITICA
        int local = SOMA;
        sleep(rand()%2);
        SOMA = local + 1;
        clientMsg(i);
        
        respond = 0;
        count++;
    }
    
}

void server(){
    while(true){
        while(request == 0){
            //nao faz nada
        }

        respond = request;
        serverMsg(respond);

        while(respond != 0){
            //nao faz nada
        }
        
        request = 0;
    }
}

int main(){
    printf("Algoritmo Cliente-Servidor de Manna Pnueli\n");
    printf("Executando com %d clientes e %d threads \n", NUM_THREADS, NUM_THREADS + 1);
    printf("Uma thread para o servidor e %d threads para clientes \n\n", NUM_THREADS);

    omp_set_num_threads(NUM_THREADS + 1);

    #pragma omp parallel \
    shared(SOMA, request, respond)
    {
        #pragma omp sections nowait
        {
            #pragma omp section
            server();

            #pragma omp section
            client(1);

            #pragma omp section
            client(2);
            
            #pragma omp section
            client(3);

            #pragma omp section
            client(4);
        }
    }

    return 0;
}