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
int encontraCaminho (Imagem1C* img, Coordenada** caminho);
unsigned char ** get_neighbors(unsigned char **dados, uint32_t y, uint32_t x);
void binarization(Imagem1C *origin, Imagem1C *output, uint32_t coordinate_y, uint32_t coordinate_x);
void generate_histogram(Imagem1C *img, uint8_t *histogram);
void image_equalization(Imagem1C *img, uint8_t *hist);
void depth_first_search(Imagem1C *img, int8_t **steps, uint32_t x, uint32_t y, int label);
/*============================================================================*/

#endif
