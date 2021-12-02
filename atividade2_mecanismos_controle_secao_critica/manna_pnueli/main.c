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
#define USE_COLORS_ON_TERMINAL true //flag para decidir se deseja mostrar as cores no terminal

#define ITERATIONS 1000000000
#define RESET_COLOR 0
#define SERVER_COLOR 5

int request = 0;
int respond = 0;
int SOMA = 0;

char *getTextColorCode(int i){
    //Retorna uma string que representa um código que altera a cor do texto no terminal

    if(i == 0 && USE_COLORS_ON_TERMINAL){
        return "\033[0m"; //reseta para cor original
    }
    else if(i == 1 && USE_COLORS_ON_TERMINAL){
        return "\033[0;41m"; //vermelho
    }
    else if(i == 2 && USE_COLORS_ON_TERMINAL){
        return "\033[0;42m"; //verde
    }
    else if(i == 3 && USE_COLORS_ON_TERMINAL){
        return "\033[0;43m"; //amarelo
    }
    else if(i == 4 && USE_COLORS_ON_TERMINAL){
        return "\033[0;44m"; //azul
    }
    else if(i == 5 && USE_COLORS_ON_TERMINAL){
        return "\033[0;45m"; //rosa - cor do servidor
    }
    else{
        return ""; //string vazia, sem codigo para cor
    }
}

void clientMsg(int c){
    //Imprime uma mensagem com a identificacao do cliente colorido
    //e qual o valor da variavel SOMA apos o incremento

    char *msg = "%s Cliente %d %s incrementou soma na secao critica. Soma = %d. \n";
    printf(msg, getTextColorCode(c), c, getTextColorCode(0), SOMA);
}

void serverMsg(int c){
    //Imprime uma mensagem indicando pelo servidor 
    //que um cliente entrou na secao critica

    char *msg = "%s Servidor: %s%s Cliente %d %s entrou na secao critica. ";
    printf(msg, getTextColorCode(SERVER_COLOR), getTextColorCode(RESET_COLOR), 
        getTextColorCode(c), c, getTextColorCode(RESET_COLOR));
}

void client(int i){
    //Funcao cliente do algoritmo

    int count = 0;
    while(count < ITERATIONS){
        while(respond != i && USE_PRE_PROTOCOL){ //pre protocolo a ser desativado para comparacao
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
    //Funcao servidor do algoritmo

    while(true){
        while(request == 0){ //await request != 0
            //nao faz nada
        }
        
        respond = request;
        serverMsg(respond);

        while(respond != 0){ //await respond == 0
            //nao faz nada
        }
        
        request = 0;
    }
}

void run2Clients(){
    //Executa algoritmo com 2 clientes e 1 servidor utilizando sections

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
        }
    }
}

void run4Clients(){
    //Executa algoritmo com 4 clientes e 1 servidor utilizando sections

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
}

int main(){
    int i;

    printf("Algoritmo Cliente-Servidor de Manna Pnueli\n");
    printf("Executando com %d clientes e %d threads \n", NUM_THREADS, NUM_THREADS + 1);
    printf("1 thread para o servidor e %d threads para clientes \n\n", NUM_THREADS);

    omp_set_num_threads(NUM_THREADS + 1);

    if(NUM_THREADS == 2)
        run2Clients();
    else
        run4Clients();

    return 0;
}