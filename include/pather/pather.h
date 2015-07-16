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
#include <stdbool.h>

/* Constants */
#define VECTOR_INITIAL_CAPACITY 100

/* Guards */
#ifndef _PATHER_PATHER_H
#define _PATHER_PATHER_H

/**
 * Point in Matrix
 *
 * We use this data type to represent a point in our
 * Euclidean Space Matrix.
 */
typedef struct Coordenada
{
  /**
   * The x coordinate of the node on the grid.
   * @type number
   */
  int x;

  /**
   * The y coordinate of the node on the grid.
   * @type number
   */
  int y;

  /**
   * Coordinate Parent
   */
  struct Coordenada *parent;
} Coordenada;

/**
 * The Node struct,
 * contains item and the pointer that point to next node.
 *
 * Ref: http://ben-bai.blogspot.com.br/2012/04/simple-queue-data-structure-in-ansi-c.html
 */
typedef struct Node {
  /* Coordenada do Nó */
  Coordenada item;

  /* Próximo item na Queue */
  struct Node* next;
} Node;

/**
 * The Queue struct, contains the pointers that
 * point to first node and last node, the size of the Queue,
 * and the function pointers.
 */
typedef struct Queue {
  Node* head;
  Node* tail;

  void (*push) (struct Queue*, Coordenada); // add item to tail
  // get item from head and remove it from queue
  Coordenada (*pop) (struct Queue*);
  // get item from head but keep it in queue
  Coordenada (*peek) (struct Queue*);
  // display all element in queue
  void (*display) (struct Queue*);
  // size of this queue
  int size;
} Queue;

/**
 * Push an item into queue, if this is the first item,
 * both queue->head and queue->tail will point to it,
 * otherwise the oldtail->next and tail will point to it.
 */
void push (Queue* queue, Coordenada item);
/**
 * Return and remove the first item.
 */
Coordenada pop (Queue* queue);
/**
 * Return but not remove the first item.
 */
Coordenada peek (Queue* queue);
/**
 * Show all items in queue.
 */
void display (Queue* queue);
Queue createQueue();

/**
 * Dynamic Vector
 *
 * This vector will increses of size when adds element
 * to it.
 */
typedef struct {
  int size;      // slots used so far
  int capacity;  // total available slots
  Coordenada **data;     // array of integers we're storing
} Vector;

void vector_init(Vector *vector);
void vector_append(Vector *vector, Coordenada *value);
Coordenada* vector_get(Vector *vector, int index);
void vector_set(Vector *vector, int index, Coordenada *value);
void vector_double_capacity_if_full(Vector *vector);
void vector_free(Vector *vector);


/*============================================================================*/

/* Image Related */
int encontraCaminho (Imagem1C* img, Coordenada** caminho);
unsigned char ** get_neighbors(unsigned char **dados, uint32_t y, uint32_t x);
void binarization(Imagem1C *origin, Imagem1C *output, uint32_t coordinate_y, uint32_t coordinate_x, int threshold);
void generate_histogram(Imagem1C *img, uint8_t *histogram);
void image_equalization(Imagem1C *img, uint8_t *hist);

/* Path Related */
void discover_start_point(Imagem1C *binary_image, int32_t *y, int32_t *x);
Coordenada *bfs( unsigned char ** grid, Coordenada local, Queue queue, int8_t **map, unsigned long height, unsigned long width);

void dilate(Imagem1C *img);

Coordenada *captura_caminho(Imagem1C *filtrada, Vector *vector, int iteration);
/*============================================================================*/



#endif
