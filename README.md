# PCD

Projetos de Programação Concorrente e Distribuída (2021)

## Entregas

- **Entrega 1:** Jogo da vida e Highlife
- **Entrega 2:** Mecanismos de controle da seção crítica (Jogo da vida, Manna-Pnueli, Simulação de seção crítica em Java)
- **Entrega 3:** Jogo da vida em MPI
- **Entrega 4:** Aplicação dos conceitos de Programação Concorrente e Distribuída em projeto sequencial já existente.
  
## Detalhes da entrega 4:
Paralelização de filtros para processamento de imagens.  

**Repositório original:** https://github.com/onyx-storm/Filter-Program  

**Como compilar:**
```bash
gcc -o main filter.c helpers.c -lm -fopenmp
```

**Como executar:**
```bash
./main -b imagem_de_entrada.bmp imagem_de_saida.bmp
```

**Opções para selecionar filtros:**
- -b: filtro blur
- -e: filtro de bordas
- -s: filtro sépia
- -g: filtro escala de cinza
- -r: filtro para refletir imagem

## Integrantes:
Cássio Peres Vicente, 69421  
Isabella de Souza Navarro, 86828  

## Professores:
Denise Stringhini  
Álvaro Luiz Fazenda  

------------
ICT - Instituto de Ciência e Tecnologia  
Universidade Federal de São Paulo