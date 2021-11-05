package JogoDaVidaJavaThreads;

import JogoDaVidaJavaThreads.Grid;

public class newGeneration implements Runnable {

	int i, j, neighbors, size_t1, size_t2;
	Grid gridA, gridB;
	
	public newGeneration(int a, int b, Grid x, Grid y)
	{
		this.size_t1 = a;
		this.size_t2 = b;
		this.gridA = x;
		this.gridB = y;
	}
	
	public void run()
	{
		 for(i = size_t1; i < size_t2; i++){
	            for(j = 0; j < gridA.size; j++){
	                neighbors = gridA.getNeighbors(i, j);

	                //Qualquer celula viva com 2 (dois) ou 3 (tres) vizinhos deve sobreviver;
	                if(gridA.grid[i][j] == 1 && (neighbors == 2 || neighbors == 3))
	                    gridB.grid[i][j] = 1;
	                //Qualquer celula morta com 3 (tres) vizinhos torna-se viva;
	                else if(gridA.grid[i][j] == 0 && neighbors == 3)
	                    gridB.grid[i][j] = 1;
	                //Qualquer outro caso, celulas vivas devem morrer e celulas ja mortas devem continuar mortas.
	                else
	                    gridB.grid[i][j] = 0;
	            }
	        }
	}
}
