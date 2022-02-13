#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

#include "helpers.h"

#define THREADS 4

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

int main(int argc, char *argv[])
{   
    struct timeval beginTotal, endTotal, beginFilter, endFilter;

    //Inicia contagem do tempo de execucao
    gettimeofday(&beginTotal, NULL);

    // help option
    char *help = argv[1];
    if (argc == 2 && ((strcmp(help,"-h") == 0) || (strcmp(help,"--help") == 0)))
    {
        fprintf(stderr,"Options available\n");
        fprintf(stderr,"-b     blur filter\n");
        fprintf(stderr,"-e     edges filter\n");
        fprintf(stderr,"-s     sepia filter\n");
        fprintf(stderr,"-g     greyscale filter\n");
        fprintf(stderr,"-r     reflected filter\n");
        return 1;
    }

    // Define allowable filters
    char *filters = "begrs";

    // Get filter flag and check validity
    char filter = getopt(argc, argv, filters);
    if (filter == '?')
    {
        fprintf(stderr, "Invalid filter.\n");
        return 1;
    }
    // Ensure only one filter
    if (getopt(argc, argv, filters) != -1)
    {
        fprintf(stderr, "Only one filter allowed.\n");
        return 2;
    }

    // Ensure proper usage
    if (argc != optind + 2)
    {
        fprintf(stderr, "Usage: filter [flag] infile outfile\n");
        return 3;
    }



    // Remember filenames
    char *infile = argv[optind];
    char *outfile = argv[optind + 1];

    // Open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 4;
    }

    // Open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 5;
    }

    // Read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // Read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // Ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 6;
    }

    int height = abs(bi.biHeight);
    int width = bi.biWidth;

    // Allocate memory for image
    RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        fprintf(stderr, "Not enough memory to store image.\n");
        fclose(outptr);
        fclose(inptr);
        return 7;
    }

    // Determine padding for scanlines
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    // Iterate over infile's scanlines
    for (int i = 0; i < height; i++)
    {
        // Read row into pixel array
        fread(image[i], sizeof(RGBTRIPLE), width, inptr);

        // Skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }

    // Defining number of threads
    omp_set_num_threads(THREADS);

    // Filter image
    switch (filter)
    {
        // Blur
        case 'b':
            printf("Filtro blur com %d threads\n", THREADS);
            gettimeofday(&beginFilter, NULL);
            blur(height, width, image);
            gettimeofday(&endFilter, NULL);
            break;

        // Edges
        case 'e':
            printf("Filtro de bordas com %d threads\n", THREADS);
            gettimeofday(&beginFilter, NULL);
            edges(height, width, image);
            gettimeofday(&endFilter, NULL);
            break;

        // sepia
        case 's':
            printf("Filtro sepia com %d threads\n", THREADS);
            gettimeofday(&beginFilter, NULL);
            sepia(height, width, image);
            gettimeofday(&endFilter, NULL);
            break;

        // Grayscale
        case 'g':
            printf("Filtro escala de cinza com %d threads\n", THREADS);
            gettimeofday(&beginFilter, NULL);
            grayscale(height, width, image);
            gettimeofday(&endFilter, NULL);
            break;

        // Reflect
        case 'r':
            printf("Filtro para refletir imagem com %d threads\n", THREADS);
            gettimeofday(&beginFilter, NULL);
            reflect(height, width, image);
            gettimeofday(&endFilter, NULL);
            break;
    }

    // Write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Write new pixels to outfile
    for (int i = 0; i < height; i++)
    {
        // Write row to outfile
        fwrite(image[i], sizeof(RGBTRIPLE), width, outptr);

        // Write padding at end of row
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // Free memory for image
    free(image);

    // Close infile
    fclose(inptr);

    // Close outfile
    fclose(outptr);

    //Encerra contagem de tempo e exibe tempo total de execucao
    gettimeofday(&endTotal, NULL);
    printTimeElapsed(beginFilter, endFilter, "\nTEMPO DECORRIDO PARA APLICAR FILTRO:\n");
    printTimeElapsed(beginTotal, endTotal, "\nTEMPO TOTAL DE EXECUCAO:\n");

    return 0;
}
