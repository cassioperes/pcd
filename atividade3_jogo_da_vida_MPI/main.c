/*
UNIFESP - UNIVERSIDADE FEDERAL DE SAO PAULO
PROGRAMACAO CONCORRENTE E DISTRIBUIDA
PROJETO: 
Jogo da vida - versao MPI

INTEGRANTES:
Cassio Peres Vicente, 69421
Isabella de Souza Navarro, 86828
*/

#include "mpi.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<stdbool.h>

#define SIZE 2048
#define PRINT_SIZE 50
#define GENERATIONS 2000
#define GENERATIONS_DEMONSTRATION 5
#define IS_DEMONSTRATION false
#define ROOT_PROCESS_ID 0

void printTimeElapsed(struct timeval begin, struct timeval end, char *msg){
    //Imprime o tempo decorrido entre begin e end em minutos:segundos:milisegundos 
    //e o total de milisegundos

    int minutes, seconds, processId;
    long long miliseconds, milisecondsTotal;

    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if(processId == ROOT_PROCESS_ID){
        milisecondsTotal = (int) ((1000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec) / 1000));
        minutes = (int) (milisecondsTotal / 60000);
        seconds = (int) ((milisecondsTotal / 1000) % 60);
        miliseconds = milisecondsTotal % 1000;

        printf("%s", msg);
        printf("Tempo total em milisegundos: %lld\n", milisecondsTotal);
        printf("Tempo total (minutos:segundos:milisegundos): %d:%d:%lld\n", minutes, seconds, miliseconds);
    }
}

int **createGrid(){
    //Aloca e retorna uma matriz de tamanho SIZE X SIZE

    int i;
    int **grid = (int**) malloc(SIZE * sizeof(int*));
    
    for(i = 0; i < SIZE; i++){
        grid[i] = (int*) calloc(SIZE, sizeof(int));
    }

    return grid;
}

void freeGrid(int **grid){
    //Libera a memória ocupada pela matriz grid

    int i;
    
    for(i = 0; i < SIZE; i++){
        free(grid[i]);
    }
    free(grid);
}

void setFirstGeneration(int **grid){
    //Seta os valores iniciais na matriz grid. Forma um desenho de glider e de r-pentomino.
    
    int processId;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if(processId == ROOT_PROCESS_ID){
        //GLIDER
        int lin = 1, col = 1;
        grid[lin  ][col+1] = 1;
        grid[lin+1][col+2] = 1;
        grid[lin+2][col  ] = 1;
        grid[lin+2][col+1] = 1;
        grid[lin+2][col+2] = 1;
        
        //R-pentomino
        lin =10; col = 30;
        grid[lin  ][col+1] = 1;
        grid[lin  ][col+2] = 1;
        grid[lin+1][col  ] = 1;
        grid[lin+1][col+1] = 1;
        grid[lin+2][col+1] = 1;
    }
}

void printGrid(int **grid){
    //Imprime a matriz grid

    int i, j, processId;

    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if(processId == ROOT_PROCESS_ID){
        for(i = 0; i < PRINT_SIZE; i++){
            for(j = 0; j < PRINT_SIZE; j++){
                printf("%d ", grid[i][j]);
            }
            printf("\n");
        }
    }
}

int getNeighbors(int **grid, int i, int j){
    //Retorna o numero de vizinhos da celula da matriz grid que esta na posicao (i,j)

    int neighborsCount, firstLin, firstCol, lastLin, lastCol, x, y;
    int lin[3], col[3];
    neighborsCount = 0;

    //Calcula as linhas e colunas que serao verificadas nos arredores de (i,j)
    if((i - 1) % SIZE == -1)
        firstLin = SIZE-1;
    else
        firstLin = (i - 1) % SIZE;
    if((j - 1) % SIZE == -1)
        firstCol = SIZE-1;
    else
        firstCol = (j - 1) % SIZE;
    lastLin = (i + 1) % SIZE;
    lastCol = (j + 1) % SIZE;

    lin[0] = firstLin;
    lin[1] = i;
    lin[2] = lastLin;
    col[0] = firstCol;
    col[1] = j;
    col[2] = lastCol;

    //Verifica quais celulas estao vivas nos arredores da posicao (i,j)
    for(x = 0; x < 3; x++){
        for(y = 0; y < 3; y++){
            if(lin[x] < SIZE && col[y] < SIZE){
                if(grid[lin[x]][col[y]] == 1){
                    neighborsCount++;
                }
            }
        }
    }

    //Remove a propria celula na posicao (i,j) da contagem
    if(grid[i][j] == 1)
        neighborsCount--;

    return neighborsCount;
}

int countCells(int **grid){
    //Retorna numero de celulas vivas na matriz grid

    int i, j, cells, numProcesses, processId, problemSize, begin, end, totalCells;

    cells = 0;
    totalCells = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    problemSize = SIZE / numProcesses;
    begin = processId * problemSize;
    end = (processId * problemSize) + problemSize;

    for(i = begin; i < end; i++){
        for(j = 0; j < SIZE; j++){
            if(grid[i][j])
                cells++;
        }
    }

    //Operacao de reducao para soma das celulas
    MPI_Reduce(&cells, &totalCells, 1, MPI_INT, MPI_SUM, ROOT_PROCESS_ID, MPI_COMM_WORLD);

    return totalCells;
}

void exchangeData(int **grid, int processId){
    //Troca as linhas das bordas superior e inferior do pedaco do tabuleiro atribuido
    //a cada processo com os processos vizinhos

    int problemSize, numProcesses, prevProcess, proxProcess, superiorBorderLine, inferiorBorderLine, i;
    int superiorBorderSend[SIZE], inferiorBorderSend[SIZE], superiorBorderRecv[SIZE], inferiorBorderRecv[SIZE];

    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    proxProcess = (processId + 1) % numProcesses;
    prevProcess = (processId + numProcesses - 1) % numProcesses;
    problemSize = SIZE / numProcesses;

    //Calcula posicao da linha da borda superior e inferior
    superiorBorderLine = processId * problemSize;
    inferiorBorderLine = ((processId * problemSize) + problemSize) - 1;

    //Copia as bordas superior e inferior para os buffers de envio
    for(i = 0; i < SIZE; i++){
        superiorBorderSend[i] = grid[superiorBorderLine][i];
        inferiorBorderSend[i] = grid[inferiorBorderLine][i];
    }

    //Envia a borda inferior para o proximo vizinho e recebe a borda superior
    //do vizinho anterior
    MPI_Sendrecv(
        &inferiorBorderSend, SIZE, MPI_INT, proxProcess, 10,
        &superiorBorderRecv, SIZE, MPI_INT, prevProcess, 10,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE
    );

    //Envia a borda superior para o vizinho anterior e recebe a borda inferior
    //do proximo vizinho
    MPI_Sendrecv(
        &superiorBorderSend, SIZE, MPI_INT, prevProcess, 20,
        &inferiorBorderRecv, SIZE, MPI_INT, proxProcess, 20,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE
    );

    //Calcula a posicao das linhas recebidas para serem copiadas para o tabuleiro
    superiorBorderLine = (superiorBorderLine + SIZE - 1) % SIZE;
    inferiorBorderLine = (inferiorBorderLine + 1) % SIZE;

    //Copia as bordas superior e inferior dos buffers de recebimento para o tabuleiro
    for(i = 0; i < SIZE; i++){
        grid[superiorBorderLine][i] = superiorBorderRecv[i];
        grid[inferiorBorderLine][i] = inferiorBorderRecv[i];
    }
}

void makeNewGeneration(int **grid){
    //Imprime a quantidade de celulas vivas para cada nova geracao considerando as regras do jogo 

    int cells, g, num_gen, i, j, neighbors, numProcesses, processId, problemSize, begin, end;
    int **aux, **newGrid;

    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    problemSize = SIZE / numProcesses;
    begin = processId * problemSize;
    end = (processId * problemSize) + problemSize;

    newGrid = createGrid();

    if(IS_DEMONSTRATION)
        num_gen = GENERATIONS_DEMONSTRATION;
    else
        num_gen = GENERATIONS;

    //Atualiza o tabuleiro para cada geracao de acordo com as regras do jogo
    for(g = 1; g <= num_gen; g++){
        exchangeData(grid, processId);
        MPI_Barrier(MPI_COMM_WORLD);
        for(i = begin; i < end; i++){
            for(j = 0; j < SIZE; j++){
                neighbors = getNeighbors(grid, i, j);

                //Qualquer célula viva com 2 (dois) ou 3 (três) vizinhos deve sobreviver;
                if(grid[i][j] == 1 && (neighbors == 2 || neighbors == 3))
                    newGrid[i][j] = 1;
                //Qualquer célula morta com 3 (três) vizinhos torna-se viva;
                else if(grid[i][j] == 0 && neighbors == 3)
                    newGrid[i][j] = 1;
                //Qualquer outro caso, células vivas devem morrer e células já mortas devem continuar mortas.
                else
                    newGrid[i][j] = 0;
            }
        }

        aux = grid;
        grid = newGrid;
        newGrid = aux;

        cells = countCells(grid);
        if(processId == ROOT_PROCESS_ID)
            printf("Geracao %d: %d\n", g, cells);
        if(IS_DEMONSTRATION)
            printGrid(grid);
    }

    freeGrid(grid);
    freeGrid(newGrid);
}

int main(int argc, char * argv[]){
    int g, cells, numProcesses, processId;
    int **grid;
    struct timeval beginTotal, endTotal, beginGen, endGen;

    //Inicia contagem do tempo de execucao
    gettimeofday(&beginTotal, NULL);

    //Inicia MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    if(processId == ROOT_PROCESS_ID)
        printf("JOGO DA VIDA: VERSAO MPI - %d THREADS EM EXECUCAO\n", numProcesses);

    grid = createGrid();
    setFirstGeneration(grid);

    //Conta celulas vivas do tabuleiro na condicao inicial
    cells = countCells(grid);
    if(processId == ROOT_PROCESS_ID)
        printf("Condicao inicial: %d\n", cells);
    if(IS_DEMONSTRATION)
        printGrid(grid);

    //Aplica novas geracoes e exibe contagem de celulas vivas para geracao
    gettimeofday(&beginGen, NULL);
    makeNewGeneration(grid);

    //Encerra contagem de tempo e exibe tempo de execucao na criacao de novas geracoes
    gettimeofday(&endGen, NULL);
    printTimeElapsed(beginGen, endGen, "\nTEMPO DECORRIDO PARA ATUALIZAR TABULEIRO PARA TODAS AS GERACOES:\n");

    //Encerra contagem de tempo e exibe tempo total de execucao
    gettimeofday(&endTotal, NULL);
    printTimeElapsed(beginTotal, endTotal, "\nTEMPO TOTAL DE EXECUCAO:\n");

    //Encerra MPI
    MPI_Finalize();

    return 0;
}