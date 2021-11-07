
public class Grid
{
	public static final int SIZE = 2048;
	public static final int PRINT_SIZE = 50;
	public static final int GENERATIONS = 2000;
	public static final int GENERATIONS_DEMONSTRATION = 5;
	public static final boolean IS_DEMONSTRATION = false;
	
	//atributos
	int size;
	int[][] grid;
	
	//metodos
	public void setSize(int t)
	{
		this.size = t;
	}
	
	public Grid(int s)
	{
		int [][] g = new int[s][s];
		this.size = s;
		this.grid = g;
	}
	
	public void printGrid()
	{
		int i,j;
		for ( i=0; i<this.size; i++ )
		{
		    for ( j=0; j<this.size; j++ )
		        System.out.println( this.grid[i][j] );
		}
	}
	
	public void setFirstGeneration()
	{
		//Seta os valores iniciais na matriz grid. Forma um desenho de glider e de r-pentomino.

	    //GLIDER
	    int lin = 1, col = 1;
	    this.grid[lin  ][col+1] = 1;
	    this.grid[lin+1][col+2] = 1;
	    this.grid[lin+2][col  ] = 1;
	    this.grid[lin+2][col+1] = 1;
	    this.grid[lin+2][col+2] = 1;
	    
	    //R-pentomino
	    lin =10; col = 30;
	    this.grid[lin  ][col+1] = 1;
	    this.grid[lin  ][col+2] = 1;
	    this.grid[lin+1][col  ] = 1;
	    this.grid[lin+1][col+1] = 1;
	    this.grid[lin+2][col+1] = 1;
	}
	
	public int getNeighbors(int i, int j)
	{
		//Retorna o numero de vizinhos da celula da matriz grid que esta na posicao (i,j)

	    int neighborsCount, firstLin, firstCol, lastLin, lastCol, x, y;
	    int lin[] = new int[3];
	    int col[] = new int[3];
	    neighborsCount = 0;

	    //Calcula as linhas e colunas que serao verificadas nos arredores de (i,j)
	    if((i - 1) % this.size == -1)
	        firstLin = this.size-1;
	    else
	        firstLin = (i - 1) % this.size;
	    if((j - 1) % this.size == -1)
	        firstCol = this.size-1;
	    else
	        firstCol = (j - 1) % this.size;
	    lastLin = (i + 1) % this.size;
	    lastCol = (j + 1) % this.size;

	    lin[0] = firstLin;
	    lin[1] = i;
	    lin[2] = lastLin;
	    col[0] = firstCol;
	    col[1] = j;
	    col[2] = lastCol;

	    //Verifica quais celulas estao vivas nos arredores da posicao (i,j)
	    for(x = 0; x < 3; x++){
	        for(y = 0; y < 3; y++){
	            if(lin[x] < this.size && col[y] < this.size){
	                if(this.grid[lin[x]][col[y]] == 1){
	                    neighborsCount++;
	                }
	            }
	        }
	    }

	    //Remove a propria celula na posicao (i,j) da contagem
	    if(this.grid[i][j] == 1)
	        neighborsCount--;

	    return neighborsCount;
	}
	
	public int countCells()
	{
	    //Retorna numero de celulas vivas na matriz grid

	    int i, j;
	    int cells = 0;

	    for(i = 0; i < this.size; i++)
	    {
	        for(j = 0; j < this.size; j++)
	        {
	            if(this.grid[i][j] == 1)
	                cells++;
	        }
	    }
	    
	    return cells;
	}
	
	public void makeNewGeneration()
	{
		//Imprime a quantidade de celulas vivas para cada nova geracao considerando as regras do jogo 

	    int cells, gen, num_gen, i, j, neighbors;
	    int[][] aux;
	    Grid newGrid = new Grid(this.size);

	    if(IS_DEMONSTRATION)
	        num_gen = GENERATIONS_DEMONSTRATION;
	    else
	        num_gen = GENERATIONS;

	    //Atualiza o tabuleiro para cada geracao de acordo com as regras do jogo
	    for(gen = 1; gen <= num_gen; gen++){
	        for(i = 0; i < this.size; i++){
	            for(j = 0; j < this.size; j++){
	                neighbors = this.getNeighbors(i, j);

	                //Qualquer celula viva com 2 (dois) ou 3 (tres) vizinhos deve sobreviver;
	                if(this.grid[i][j] == 1 && (neighbors == 2 || neighbors == 3))
	                    newGrid.grid[i][j] = 1;
	                //Qualquer celula morta com 3 (tres) ou 6 (seis) vizinhos torna-se viva;
	                else if(this.grid[i][j] == 0 && (neighbors == 3 || neighbors == 6))
	                    newGrid.grid[i][j] = 1;
	                //Qualquer outro caso, celulas vivas devem morrer e celulas ja mortas devem continuar mortas.
	                else
	                    newGrid.grid[i][j] = 0;
	            }
	        }
	        
	        aux = this.grid;
	        this.grid = newGrid.grid;
	        newGrid.grid = aux;

	        cells = this.countCells();
	        System.out.println("Geracao " + gen +": " +  cells);
	        if(IS_DEMONSTRATION)
	            this.printGrid();
	    }
	}
	
	public static void main(final String[] args)
	{
		long beginTotal, endTotal, beginGen, endGen;
		//Inicia contagem do tempo de execucao
		beginTotal = System.currentTimeMillis();

		System.out.println("HIGHLIFE: VERSAO SERIAL");

	    Grid grid = new Grid(SIZE);
	    grid.setFirstGeneration();

	    //Conta celulas vivas do tabuleiro na condicao inicial
	    int cells;
	    cells = grid.countCells();
	    System.out.println("Condicao inicial: " + cells);
	    if(IS_DEMONSTRATION)
	        grid.printGrid();

	    //Aplica novas geracoes e exibe contagem de celulas vivas para geracao
	    beginGen = System.currentTimeMillis();
	    grid.makeNewGeneration();

	    //Encerra contagem de tempo e exibe tempo de execucao na criacao de novas geracoes
	    endGen = System.currentTimeMillis();
	    System.out.println("TEMPO DECORRIDO PARA ATUALIZAR TABULEIRO PARA TODAS AS GERACOES: " + (endGen - beginGen));

	    //Encerra contagem de tempo e exibe tempo total de execucao
	    endTotal = System.currentTimeMillis();
	    System.out.println("TEMPO TOTAL DE EXECUCAO: " + (endTotal - beginTotal));
		
	}

}
