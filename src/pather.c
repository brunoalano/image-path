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
  /* Cria a imagem filtrada */
  Imagem1C *filtrada = (Imagem1C *)malloc(sizeof(Imagem1C));
  filtrada->largura = img->largura;
  filtrada->altura = img->altura;
  filtrada->dados = (unsigned char**) malloc (sizeof (unsigned char*) * filtrada->altura);
  for (int i = 0; i < filtrada->altura; i++)
    filtrada->dados[i] = (unsigned char*)malloc(sizeof (unsigned char) * filtrada->largura);

  for (int y = 0; y < img->altura; y++)
    for (int x = 0; x < img->largura; x++)
      filtrada->dados[y][x] = img->dados[y][x];

  binariza_tadashi(img, filtrada);


  for (int y = 0; y < filtrada->altura; y++)
    for (int x = 0; x < filtrada->largura; x++)
      img->dados[y][x] = filtrada->dados[y][x];

  
  for (int y = 1; y < filtrada->altura - 1; y++)
  {
    for (int x = 1; x < filtrada->largura - 1; x++)
    {
      if ( filtrada->dados[y][x] == 0 ) break;
      printf("%s\n", "Printando neighbors manual");
      for (int i = -1; i < 2; i++)
      {
        for (int j = -1; j < 2; j++)
        {
          printf("%d ", filtrada->dados[y + i][x + j]);
        }
        printf("\n");
      }

      printf("%s\n", "Printando negibors funcao");
      unsigned char ** neighbors = get_neighbors(filtrada->dados, y, x);
      print_matrix(neighbors, 3, 3);
    }
  }

  



  /* Fitramos a Imagem */
  // filter(img, filtrada);

  // /* Calcula o histograma da imagem usando o algorithmo de Otsu */
  // uint8_t *histograma = (uint8_t *)malloc(256 * sizeof(uint8_t));
  // generate_histogram(filtrada, histograma);

  // /* Calcula a probabilidade de dado nível de cinza */
  // uint8_t valor_maximo_escala_cinza = histograma[0];
  // uint8_t valor_minimo_escala_cinza = histograma[0];
  // for (int i = 1; i < 256; i++)
  //   if ( histograma[i] > valor_maximo_escala_cinza )
  //     valor_maximo_escala_cinza = histograma[i];
  //   else if ( histograma[i] < valor_minimo_escala_cinza )
  //     valor_minimo_escala_cinza = histograma[i];

  // //for (int y = 0; y < img->altura; y++)
  // //  for (int x = 0; x < img->largura; x++)
  // //    filtrada->dados[y][x] = (unsigned char)normalize((float)filtrada->dados[y][x], 0.0, 255.0, (float)valor_minimo_escala_cinza, (float)valor_maximo_escala_cinza);
  
  // /* Calcula o threshold */
  // uint8_t threshold = otsu_threshold(filtrada, histograma);

  // /* Filtro da Média baseando-se no Threshold */
  // for (int y = 1; y < filtrada->altura - 1; y++)
  //   for (int x = 1; x < filtrada->largura - 1; x++)
  //     binarization(filtrada->dados, y, x, threshold - 39);

  //binariza_tadashi(filtrada);


  /* Save the image for debug purposes */
  salvaImagem1C(filtrada, "teste.bmp");

  /* Return the number of steps */
  return 0;
}

void binariza_tadashi(Imagem1C* img, Imagem1C* dest){
  int i, j, limiar=0;

  for (i = 0; i < img->altura; i++){
    for (j = 0; j < img->largura; j++){
      limiar+=img->dados[i][j];
    }
  }

  limiar=(limiar/(img->altura*img->largura));
  printf("%d\n",limiar);
  if (limiar>=130)
  limiar=limiar-47;
  else
  limiar=limiar-35;
  printf("%d\n",limiar);
  for (i = 0; i < img->altura; i++){
  for (j = 0; j < img->largura; j++){
  if (limiar<img->dados[i][j])
  dest->dados[i][j]=0;
  else
  dest->dados[i][j]=255;
  }
  }
} 

/**
 * Print a Matrix
 *
 * Função básica para imprimir matrizes y * x na
 * saída de console.
 */
void print_matrix(uint8_t **m, uint32_t y, uint32_t x)
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
      pixel_neighbors = get_neighbors(dest->dados, y, x);

      /* Apply the masks */
      float result_mask_y = convulution(pixel_neighbors, mask_y, 3);
      float result_mask_x = convulution(pixel_neighbors, mask_x, 3);

      /* Values in range [0 .. 255] */
      float normalized_y = normalize(result_mask_y, -1020.0f, 1020.0f, 0, 255);
      float normalized_x = normalize(result_mask_x, -1020.0f, 1020.0f, 0, 255);
      double value = sqrt((normalized_y * normalized_y) + (normalized_x * normalized_x));

      /* Apply the value into our matrix */
      dest->dados[y][x] = ceil(value);

      /* Free the memory block */
      free(pixel_neighbors);
    }
  }
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

/**
 * Binazarization based on Neighbors Average
 *
 * O processo de binarização é aplicado depois da utilização
 * da remoção de ruídos pelo filtros de Sobel.
 */
void binarization(unsigned char **dados, uint32_t coordinate_y, uint32_t coordinate_x, uint8_t threshold)
{
  /* Get Neighbors */
  unsigned char ** neighbors;
  neighbors = get_neighbors(dados, coordinate_y, coordinate_x);

  /* Store average */
  float average = 0.0f;

  /* Average */
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      average += neighbors[i][j] / 9;
  // printf("dado: %d\n", dados[coordinate_y][coordinate_x]);
  if ( dados[coordinate_y][coordinate_x] > threshold )
    dados[coordinate_y][coordinate_x] = 0;
  else
    dados[coordinate_y][coordinate_x] = 255;
}

/**
 * Graylevel Histogram Generation
 *
 * Criamos um histograma contendo os níveis em escala cinza
 * na imagem para sabermos uma distribuição de probabilidade
 * do valor de `threshold` ideal para a imagem
 */
void generate_histogram(Imagem1C *img, uint8_t *histogram)
{
  /* Fill with zeros */
  for (int i = 0; i < 256; i++) histogram[i] = 0;
  for (int y = 0; y < img->altura; y++)
    for (int x = 0; x < img->largura; x++)
      histogram[ img->dados[y][x] ]++;
}

/**
 * Calculates the Density Probability with Otsu Algorithm
 *
 * Esta função irá retornar o valor do threshold que deve ser
 * aplicado sob a imagem atráves da densidade da distribuição
 * de níveis de cinza na imagem.
 */
uint8_t otsu_threshold(Imagem1C *img, uint8_t *histogram)
{
  /* Probability of each density */
  double probability[256], omega[256];

  /* Value for separation */
  double myu[256];

  /* Inter-class variance */
  double max_sigma, sigma[256];

  /* Store the predict of threshold */
  uint8_t threshold;

  /* Distribution */
  for (int i = 0; i < 256; i++)
    probability[i] = (double)histogram[i] / (img->altura * img->largura);

  /* Omega & MYU */
  omega[0] = probability[0];
  myu[0] = 0.0;
  for (int i = 1; i < 256; i++)
  {
    omega[i] = omega[i - 1] + probability[i];
    myu[i] = myu[i - 1] + (i * probability[i]);
  }

  /* Maximization of Sigma Value */
  threshold = 0;
  max_sigma = 0.0;
  for ( int i = 0; i < 256; i++ )
  {
    if ( omega[i] != 0.0 && omega[i] != 1.0 )
      sigma[i] = pow( myu[255] * omega[i] - myu[i], 2) / ( omega[i] * (1.0 - omega[i]));
    else
      sigma[i] = 0.0;

    /* Check if its the optima of Sigma */
    if (sigma[i] > max_sigma)
    {
      max_sigma = sigma[i];
      threshold = i;
    }
  }

  /* Return the prediction */
  return threshold;
}