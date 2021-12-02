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
#define ITERACOES 1000000000

int request = 0;
int respond = 0;
int SOMA = 0;
char *resetColor = "\033[0m";

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
        return "\033[0;45m"; //rosa
    }
    else{
        return resetColor; //retorna a cor original
    }
}

void client(int i){
    int count = 0;
    char *msg = "%s Cliente %d %s incrementou soma na secao critica. Soma = %d \n";

    while(count < ITERATIONS){
        while(respond != i){
            request = i;
        }

        //SECAO CRITICA
        int local = SOMA;
        sleep(rand()%2);
        SOMA = local + 1;
        printf(msg, getTextColorCode(i), i, getTextColorCode(0), SOMA);

        respond = 0;
        count++;
    }
    
}

void server(){
    char *msg = "%s Servidor %s%s cliente %d%s entrou na secao critica ";

    while(true){
        while(request == 0){
            //nao faz nada
        }
        respond = request;
        printf(msg, getTextColorCode(5), getTextColorCode(0), getTextColorCode(respond), respond, getTextColorCode(0));
        
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