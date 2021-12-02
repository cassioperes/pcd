/*
UNIFESP - UNIVERSIDADE FEDERAL DE SAO PAULO
PROGRAMACAO CONCORRENTE E DISTRIBUIDA
PROJETO: 
Algoritmo de Manna Pnueli - versao OpenMP

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

#define ITERACOES 1000000000
#define RESET_COLOR 0
#define SERVER_COLOR 5

int request = 0;
int respond = 0;
int SOMA = 0;

void setTextColorOutput(int i){
    switch (i){
    case 1:
        printf("\033[0;41m"); //vermelho
        break;
    case 2:
        printf("\033[0;42m"); //verde
        break;
    case 3:
        printf("\033[0;43m"); //amarelo
        break;
    case 4:
        printf("\033[0;44m"); //azul
        break;
    case 5:
        printf("\033[0;45m"); //rosa
        break;
    
    default:
        printf("\033[0m"); //retorna a cor original
        break;
    }
}

void clientMsg(int c){
    setTextColorOutput(c);
    printf(" Cliente %d ", c);
    setTextColorOutput(RESET_COLOR);
    printf(" incrementou soma na secao critica. Soma = %d\n", SOMA);
}

void serverMsg(int c){
    setTextColorOutput(SERVER_COLOR);
    printf(" Servidor ");
    setTextColorOutput(RESET_COLOR);

    setTextColorOutput(c);
    printf(" cliente %d ", c);
    setTextColorOutput(RESET_COLOR);

    printf(" entrou na secao critica - ");
}

void client(int i){
    int count = 0;
    while(count < ITERATIONS){
        while(respond != i){
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

        while(respond != 0 && USE_PRE_PROTOCOL){
            //nao faz nada
        }
        
        request = 0;
    }
}

int main(){
    printf("Algoritmo Cliente-Servidor de Manna Pnueli\n");
    printf("Executando com %d clientes e %d threads \n", NUM_THREADS-1, NUM_THREADS);
    printf("Uma thread para o servidor e %d threads para clientes \n\n", NUM_THREADS-1);

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