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
int encontraCaminho (Imagem1C* img, Coordenada** caminho, int i)
{
  char nome_saida [25]; /* String usada para salvar as saídas. */

  /* Create the filtered image */
  Imagem1C *filtrada = (Imagem1C *)malloc(sizeof(Imagem1C));
  filtrada->largura = img->largura;
  filtrada->altura = img->altura;
  filtrada->dados = (unsigned char**) malloc (sizeof (unsigned char*) * filtrada->altura);
  for (int i = 0; i < filtrada->altura; i++)
  {
    filtrada->dados[i] = (unsigned char*)malloc(sizeof (unsigned char) * filtrada->largura);
    for (int x = 0; x < img->largura; x++)
      filtrada->dados[i][x] = img->dados[i][x];
  }

  /* Binarize the image */
  for (uint32_t i = 1; i < img->altura - 1; i++)
    for (uint32_t j = 1; j < img->largura - 1; j++)
      binarization(img, filtrada, i, j);

  /* Binarize the Borders */
  for (int i = 0; i < img->largura; i++)
  {
    filtrada->dados[0][i] = 0;
    filtrada->dados[img->altura - 1][i] = 0;
  }
  for (int i = 0; i < img->altura; i++)
  {
    filtrada->dados[i][0] = 0;
    filtrada->dados[i][img->largura - 1] = 0;
  }

  /* Discover the Start Point */
  uint32_t start_x = 0, start_y = 0; /* Y = line; X = column; */
  discover_start_point( filtrada, &start_y, &start_x );

  /* Find the path */
  //dijkstra( filtrada->dados, filtrada->altura, filtrada->largura );

  /* Save */
  sprintf (nome_saida, "out%d.bmp", i);
  salvaImagem1C(filtrada, nome_saida);

  /* Clean the process */
  destroiImagem1C(filtrada);

  /* Return the number of steps */
  return 0;
}

/**
 * Discover the Start Point on Image
 */
void discover_start_point(Imagem1C *binary_image, uint32_t *y, uint32_t *x)
{
  for ( int i = 0; i < binary_image->altura; i++ )
  {
    if ( binary_image->dados[i][1] == 255 )
    {
      *y = i;
      *x = 0;
      break;
    }
  }
}

/**
 * Dijsktra Algorithm
 *
 * Find the shortest path from one side to other
 */
// void dijkstra( unsigned char ** grid, unsigned long height, unsigned long width )
// {
//   /* Create a matrix with the map */
//   uint8_t **map = (uint8_t **)malloc( height * sizeof( *uint8_t) );
//   for (int i = 0; i < height; i++)
//     map[i] = (uint8_t *)malloc( width * sizeof(uint8_t) );

//   /* Traverse based on the first point */
//   traverse( grid, map, 0, 0 );
// }

// void traverse( unsigned char ** grid, uint8_t ** map, uint32_t i, uint32_t j )
// {

// }

/**
 * Histogram Equalization
 *
 * Este processo irá pegar o nosso histograma anterior
 * e então equalizar para criar uma imagem com maiores
 * contrates.
 */
void image_equalization(Imagem1C *img, uint8_t *hist)
{
  /* Quantidade de Pixels na Imagem */
  uint32_t pixels = img->altura * img->largura;

  /* Calcula o valor mínimo e máximo do Histograma */
  int hsum=0;
  float avg;
  int hmax=-1,hmin=257;
  for(int i=0; i < 256; i++)
  {
    /* Max */
    if (hist[i]>hmax)
      hmax=hist[i];

    /* Minimum */
    if (hist[i]<hmin)
      hmin=hist[i];
  }

  /* Average */
  for(int i = 0; i < 256; i++)
    hsum += hist[i] * i;
  avg=(float)hsum/pixels;

  /********************************************************
   * Dispersão dos Dados
   *******************************************************/
  float cSum = 0;
  float cTable[256];
  float avgDev = 0;
  for(int i = 0; i < 256; i++)
  {
    cTable[i]=(abs((i-(int)avg)))*(abs((i-(int)avg)));
    cTable[i]=hist[i]*cTable[i];
    cSum+=cTable[i];
  }        
  cSum=cSum/pixels;

  /* Desvio Padrão */
  avgDev = sqrt(cSum);

  /********************************************************
   * Árvore de Probabilidade
   *******************************************************/
  float prt[256];
  for(int i=0;i<256;i++)
    prt[i]=(float) hist[i] / pixels;

  /********************************************************
   * Distribuição Probabilística
   *******************************************************/
  float cdfmax=-1,cdfmin=257;
  float cdf[256];
  cdf[0]=prt[0];
  for(int i=1;i<256;i++)
  {
    cdf[i] = prt[i] + cdf[ i - 1];
    if(cdf[i] > cdfmax)
      cdfmax=cdf[i];
    if(cdf[i] < cdfmin)
      cdfmin=cdf[i];
  }

  /********************************************************
   * Salva Imagem Equalizada
   *******************************************************/
  for (int i = 0; i < img->altura; i++)
  {
    for (int j = 0; j < img->largura; j++)
    {
      img->dados[i][j] = cdf[img->dados[i][j]]*255;
    }
  }
}

/**
 * Binazarization based on Neighbors Average
 *
 * O processo de binarização é aplicado depois da utilização
 * da remoção de ruídos pelo filtros de Sobel.
 */
void binarization(Imagem1C *origin, Imagem1C *output, uint32_t coordinate_y, uint32_t coordinate_x)
{
  /* Store average */
  float average = 0.0f;

  /* Get Neighbors */
  unsigned char ** neighbors;
  neighbors = get_neighbors(origin->dados, coordinate_y, coordinate_x);

  /* Average */
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      average += neighbors[i][j] / 9;

  if ( output->dados[coordinate_y][coordinate_x] > 99 )
    output->dados[coordinate_y][coordinate_x] = 0;
  else
    output->dados[coordinate_y][coordinate_x] = 255;
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