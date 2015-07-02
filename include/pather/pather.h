/**
 * Shortest Path in Image
 *
 * This algorithm tries to find the optima path between
 * 2 pointers that are connected with a line.
 *
 * This image contains a Bitmap, and we need to binarize it
 * for better results, and run some filters.
 */

/* Project Headers */
#include <pather/imagem.h>
#include <stdint.h>

/* Guards */
#ifndef _PATHER_PATHER_H
#define _PATHER_PATHER_H

/**
 * Point in Matrix
 *
 * We use this data type to represent a point in our
 * Euclidean Space Matrix.
 */
typedef struct
{
    int x;
    int y;
} Coordenada;

/*============================================================================*/
/* Função central do trabalho. */
void print_matrix(uint8_t **m, uint32_t y, uint32_t x);
int encontraCaminho (Imagem1C* img, Coordenada** caminho);
void filter(Imagem1C *img, Imagem1C *dest);
unsigned char ** get_neighbors(unsigned char **dados, uint32_t y, uint32_t x);
float convulution(unsigned char **base, int mask[3][3], int degree);
float normalize(float value, float base_min, float base_max, float destination_min, float destination_max);
void binarization(unsigned char **dados, uint32_t coordinate_y, uint32_t coordinate_x, uint8_t threshold);
void generate_histogram(Imagem1C *img, uint8_t *histogram);
uint8_t otsu_threshold(Imagem1C *img, uint8_t *histogram);
void depth_first_search(Imagem1C *img, int8_t steps[img->altura][img->largura], int32_t x, int32_t y, int label);
void binariza_tadashi(Imagem1C* img, Imagem1C* dest);

/*============================================================================*/

#endif
