/**
 * Shortest Path in Image
 *
 * This algorithm tries to find the optima path between
 * 2 pointers that are connected with a line.
 *
 * This image contains a Bitmap, and we need to binarize it
 * for better results, and run some filters.
 */

/* Standard Libraries */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* File Header */
#include <pather/pather.h>

/* Define the Threshold */
#define THRESHOLD 127
#define THRESHOLD_AMPLITUDE 10

/* Macro for evaluate true if bigger than zero */
#define BIGGER_THEN_ZERO(X) ( (X) > (0) ? (1) : (0) )

void depth_first_search(Imagem1C *img, int8_t steps[img->altura][img->largura], int32_t x, int32_t y, int label)
{
	/* Base */
	if ( x < 0 || x == img->altura ) return;
	if ( y < 0 || y == img->largura ) return;
	if ( steps[x][y] != 0 || img->dados[x][y] != 0 ) return;

	printf("X = %d\tY = %d\n", x, y);
	
	/* Definimos a posição atual */
	steps[x][y] = label;
	
	/* Atuamos nos vizinhos */
	depth_first_search(img, steps, x - 1, y - 1, label);
	depth_first_search(img, steps, x - 1, y, label);
	depth_first_search(img, steps, x - 1, y + 1, label);
	depth_first_search(img, steps, x, y - 1, label);
	depth_first_search(img, steps, x, y + 1, label);
	depth_first_search(img, steps, x + 1, y - 1, label);
	depth_first_search(img, steps, x + 1, y, label);
	depth_first_search(img, steps, x + 1, y + 1, label);
}

/**
 * Menor Caminho na Imagem
 *
 * Recebemos como parâmetro `img` que contém uma matriz
 * de caracteres em tons de cinza (1 canal). Após recebê-los
 * devemos remover os ruídos, e completar as falhas existentes
 * nas linhas da matriz.
 * 
 * @param  img     pointer to structure
 * @param  caminho pointer to matrix
 * 
 * @return         number of steps
 */
int encontraCaminho (Imagem1C* img, Coordenada** caminho)
{
	/* Transformamos em binária (preto/branco) */
	probabilistic_binarization(img);

	/* Criamos uma matriz auxiliar contendo os possíveis caminhos */
	int8_t steps[img->altura][img->largura];
	for (int i = 0; i < img->altura; ++i) 
    	for (int j = 0; j < img->largura; ++j) 
    		steps[i][j] = 0;

	int label_value = 1;
  	for (int i = 0; i < img->altura; ++i) 
    	for (int j = 0; j < img->largura; ++j) 
    		if (steps[i][j] != -1 && img->dados[i][j] == 0)
    			depth_first_search(img, steps, i, j, label_value++);

    for (int i = 0; i < img->altura; ++i) 
    {
    	for (int j = 0; j < img->largura; ++j) 
    		printf("%3d ", steps[i][j]);
    	printf("\n");
    }

	/* Return the number of steps */
	return 10;
}

/**
 * Binarização baseada em Probabilidade
 *
 * Checamos os 8 cantos referenets à posição do pixel para
 * classificarmos se devemos classificar como preto ou
 * branco caso esteja próximo do limiar.
 * 
 * @param img pointer to struct
 */
void probabilistic_binarization(Imagem1C* img) {
	/* Store the current color */
	uint8_t color;
	uint32_t sum;

	/* Iterate over height */
	for (int h = 0; h < img->altura; h++)
	{
		/* Iterate over width */
		for (int w = 0; w < img->largura; w++)
		{
			/* Current color in a 8-bit format */
			color = img->dados[h][w];

			/* Find the current color */
			if ( color + THRESHOLD_AMPLITUDE < THRESHOLD && color - THRESHOLD_AMPLITUDE < THRESHOLD )
				img->dados[h][w] = 0;
			else if ( color + THRESHOLD_AMPLITUDE > THRESHOLD && color - THRESHOLD_AMPLITUDE > THRESHOLD )
				img->dados[h][w] = 255;
			else {
				/*
				 * Check the neighboors looking the average
				 * of the area of bit.
				 */
				sum = 0;
				sum += img->dados[h-1][w-1];
				sum += img->dados[h-1][w];
				sum += img->dados[h-1][w+1];
				sum += img->dados[h][w-1];
				sum += img->dados[h][w+1];
				sum += img->dados[h+1][w-1];
				sum += img->dados[h+1][w];
				sum += img->dados[h+1][w+1];
				sum /= 8;
				if ( sum + THRESHOLD_AMPLITUDE > THRESHOLD && sum - THRESHOLD_AMPLITUDE > THRESHOLD )
					img->dados[h][w] = 255;
				else
					img->dados[h][w] = 0;
			}
		}
	}
}