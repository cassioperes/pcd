/*
UNIFESP - UNIVERSIDADE FEDERAL DE SAO PAULO
PROGRAMACAO CONCORRENTE E DISTRIBUIDA
PROJETO: 
Jogo da vida - versao OpenMP

INTEGRANTES:
Cassio Peres Vicente, 69421
Isabella de Souza Navarro, 86828
*/

#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<stdbool.h>
#include<omp.h>

#define SIZE 2048
#define PRINT_SIZE 50
#define GENERATIONS 2000
#define GENERATIONS_DEMONSTRATION 5
#define IS_DEMONSTRATION false
#define THREADS 1

void printTimeElapsed(struct timeval begin, struct timeval end, char *msg){
    //Imprime o tempo decorrido entre begin e end em minutos:segundos:milisegundos 
    //e o total de milisegundos

    int minutes, seconds;
    long long miliseconds, milisecondsTotal;

    milisecondsTotal = (int) ((1000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec) / 1000));
    minutes = (int) (milisecondsTotal / 60000);
    seconds = (int) ((milisecondsTotal / 1000) % 60);
    miliseconds = milisecondsTotal % 1000;

    printf("%s", msg);
    printf("Tempo total em milisegundos: %lld\n", milisecondsTotal);
    printf("Tempo total (minutos:segundos:milisegundos): %d:%d:%lld\n", minutes, seconds, miliseconds);
}

int **createGrid(){
    //Aloca e retorna uma matriz de tamanho SIZE X SIZE

    int i;
    int **grid = (int**) malloc(SIZE * sizeof(int*));
    
    #pragma omp parallel for \
    private(i) \
    shared(grid)
    for(i = 0; i < SIZE; i++){
        grid[i] = (int*) calloc(SIZE, sizeof(int));
    }

    return grid;
}

void freeGrid(int **grid){
    //Libera a memória ocupada pela matriz grid

    int i;
    
    #pragma omp parallel for \
    private(i) \
    shared(grid)
    for(i = 0; i < SIZE; i++){
        free(grid[i]);
    }
    free(grid);
}

void setFirstGeneration(int **grid){
    //Seta os valores iniciais na matriz grid. Forma um desenho de glider e de r-pentomino.

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

void printGrid(int **grid){
    //Imprime a matriz grid

    int i, j;
    for(i = 0; i < PRINT_SIZE; i++){
        for(j = 0; j < PRINT_SIZE; j++){
            printf("%d ", grid[i][j]);
        }
        printf("\n");
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

    int i, j, cells;
    cells = 0;

    #pragma omp parallel for \
     private(i, j) \
     shared(grid) \
     reduction(+:cells)
    for(i = 0; i < SIZE; i++){
        for(j = 0; j < SIZE; j++){
            if(grid[i][j])
                cells++;
        }
    }

    return cells;
}

void makeNewGeneration(int **grid){
    //Imprime a quantidade de celulas vivas para cada nova geracao considerando as regras do jogo 

    int cells, g, num_gen, i, j, neighbors;
    int **aux, **newGrid;

    newGrid = createGrid();

    if(IS_DEMONSTRATION)
        num_gen = GENERATIONS_DEMONSTRATION;
    else
        num_gen = GENERATIONS;

    //Atualiza o tabuleiro para cada geracao de acordo com as regras do jogo
    for(g = 1; g <= num_gen; g++){
        #pragma omp parallel for \
        private(i, j, neighbors) \
        shared(grid, newGrid)
        for(i = 0; i < SIZE; i++){
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
        printf("Geracao %d: %d\n", g, cells);
        if(IS_DEMONSTRATION)
            printGrid(grid);
    }

    freeGrid(grid);
    freeGrid(newGrid);
}

int main(){
    int g, cells;
    int **grid;
    struct timeval beginTotal, endTotal, beginGen, endGen;

    //Inicia contagem do tempo de execucao
    gettimeofday(&beginTotal, NULL);

    printf("JOGO DA VIDA: VERSAO OPENMP\n");

    omp_set_num_threads(THREADS);

    grid = createGrid();
    setFirstGeneration(grid);

    //Conta celulas vivas do tabuleiro na condicao inicial
    cells = countCells(grid);
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

    return 0;
}