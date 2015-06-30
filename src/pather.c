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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* File Header */
#include <pather/pather.h>
#include <pather/imagem.h>

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
	/* Store the destination image */
	Imagem1C *filtrada = (Imagem1C *)malloc(sizeof(Imagem1C *));

	/* Fitramos a Imagem */
	filter(img, filtrada);

	/* Return the number of steps */
	return 10;
}

/**
 * Print a Matrix
 *
 * Função básica para imprimir matrizes y * x na
 * saída de console.
 */
void print_matrix(unsigned char **m, uint32_t y, uint32_t x)
{
	for (int i = 0; i < y; i++)
	{
		for (int z = 0; z < x; z++)
			printf("%3d ", (int)m[i][z]);
		printf("\n");
	}
	printf("\n\n\n");
}

/**
 * Algoritmo de Convulação
 *
 * Esta seção realiza a convulução de uma dada matriz
 * através de superposição
 */
float convulution(unsigned char **base, int mask[3][3], int degree)
{
	float sum = 0;

	for (int y = 0; y < degree; y++)
		for (int x = 0; x < degree; x++)
			sum += base[y][x] * mask[y][x];
	return sum;
}

/**
 * Filtragem utilizando Operadores de Sobel
 *
 * Removemos ruídos da imagem utilizando a convulsão das
 * matrizes de sobel
 */
void filter(Imagem1C *img, Imagem1C *dest)
{
	/*
	 * Pior caso da máscara Y:
	   	0   0   0
	   	0   0   0
	 	  255 255 255
	 */
	/* Vertical Mask */
	int mask_y[3][3] = {
		{ -1, -2, -1 },
		{  0,  0,  0 },
		{  1,  2,  1 },
	};

	/*
	 * Pior caso da máscara X:
	   	0   0   255
	   	0   0   255
	 	  0   0   255
	 */
	/* Horizontal Mask */
	int mask_x[3][3] = {
		{ -1,  0,  1 },
		{ -2,  0,  2 },
		{ -1,  0,  1 }
	};

	/* Pixel Neighbors */
	unsigned char ** pixel_neighbors;

	/* Iterate over height + 1 to height - 1 */
	for (int y = 1; y < img->altura - 1; y++)
	{
		for (int x = 1; x < img->largura - 1; x++)
		{
			/* Matrix of 3x3 with neighbors */
			pixel_neighbors = get_neighbors(img->dados, y, x);

			/* Apply the masks */
			float result_mask_y = convulution(pixel_neighbors, mask_y, 3);
			float result_mask_x = convulution(pixel_neighbors, mask_x, 3);

			/* Values in range [0 .. 1] */
      float normalized_y = normalize(result_mask_y, -1020.0f, 1020.0f, 0, 255);
			float normalized_x = normalize(result_mask_x, -1020.0f, 1020.0f, 0, 255);
			double value = sqrt((normalized_y * normalized_y) + (normalized_x * normalized_x));

			/* Apply the value into our matrix */
      if ( value > 255.0 )
        img->dados[y][x] = 255;
      else if ( value < 0 )
        img->dados[y][x] = 0;
      else
        img->dados[y][x] = ceil(value);

			/* Free the memory block */
			free(pixel_neighbors);
		}
	}

	salvaImagem1C(img, "teste.bmp");
}


/**
 * Normalize a Value into a Range
 *
 * If you have a range in value [A, B] and want to scale
 * into a range [C, D].
 * 
 * @param  value           value to scale
 * @param  base_min        The minimum of our old range (A)
 * @param  base_max        The maximum of our old range (B)
 * @param  destination_min The minimum of our new range (C)
 * @param  destination_max The maximum of our new range (D)
 * @return                 Scaled value in range [C, D]
 */
float normalize(float value, float base_min, float base_max, float destination_min, float destination_max)
{
  return ( destination_min * ( 1 - ( (value - base_min) / (base_max - base_min) )) \
          + destination_max * ( (value - base_min) / (base_max - base_min) ) );
}

/**
 * Get Neightboors
 *
 * Retorna uma matriz de 3x3 contendo os vizinhos de uma
 * determinada coordenada de uma determinada matriz.
 */
unsigned char ** get_neighbors(unsigned char **dados, uint32_t coordinate_y, uint32_t coordinate_x)
{
	/* Create a matrix */
	unsigned char ** neighbors;

	/* Allocate memory */
	neighbors = (unsigned char **)malloc(3 * sizeof(unsigned char *));
	for (int i = 0; i < 3; i++)
		neighbors[i] = (unsigned char *)malloc(3 * sizeof(unsigned char));

	/* Parse the matrix */
	for (int y = -1; y < 2; y++)
		for (int x = -1; x < 2; x++)
			neighbors[y + 1][x + 1] = dados[coordinate_y + y][coordinate_x + x];

	/* Return the pointer to neighbors */
	return neighbors;
}
