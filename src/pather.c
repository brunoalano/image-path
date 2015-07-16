/**
 * Shortest Path in Image
 *
 * This algorithm tries to find the optima path between
 * 2 pointers that are connected with a line.
 *
 * This image contains a Bitmap, and we need to binarize it
 * for better results, and run some filters.
 *
 * @author Bruno Alano Medina <bruno [at] appzlab [dot] com>
 * @author Paulo Tadashi
 * @license LGPL
 */

/* Standard Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/* File Header */
#include <pather/pather.h>
#include <pather/imagem.h>

/* Definitions */
#define AVAILABLE 0 /* Está disponível */
#define PROCESSED 9 /* Já andamos aqui */
#define BLOCKED -1 /* Não andar por aqui */

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
      binarization(img, filtrada, i, j, 99);

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

  /* Cria um vetor dinâmico para armazenar as posições do melhor caminho */
  Vector vector;
  vector_init(&vector);

  /* Armazena o caminho retornado pela função */
  *caminho = captura_caminho(filtrada, &vector, 1);
  
  /* Clean the process */
  destroiImagem1C(filtrada);

  /* Return the number of steps */
  return vector.size - 1;
}

Coordenada *captura_caminho(Imagem1C *filtrada, Vector *vector, int iteration)
{
  /* Maximum of iterations */
  if ( iteration > 2 ) return NULL;

  /**
   * Queue para Breadth First Search
   *
   * Irá armazenar os pontos à serem processados.
   */
  Queue queue = createQueue();

  /**
   * Procura o Ponto Inicial
   *
   * Devemos saber de qual ponto devemos partir à nossa
   * busca.
   */
  int32_t start_x = 0, start_y = 0; /* Y = line; X = column; */
  discover_start_point( filtrada, &start_y, &start_x );
  Coordenada start;
  start.x = start_x;
  start.y = start_y;
  start.parent = NULL;
  Coordenada *startPtr = (Coordenada *)malloc(sizeof(Coordenada));
  startPtr->x = start_x - 1;
  startPtr->y = start.y;
  Coordenada *sstartPtr = (Coordenada *)malloc(sizeof(Coordenada));
  sstartPtr->x = start_x;
  sstartPtr->y = start.y;

  /**
   * Cria um Mapa
   *
   * Este mapa estará definido como 0 para todas as poissições
   * onde pode andar, e -1 para posições bloqueadas.
   */
  int8_t **map = (int8_t **)malloc( filtrada->altura * sizeof( int8_t * ) );
  for (int i = 0; i < filtrada->altura; i++)
    map[i] = (int8_t *)malloc( filtrada->largura * sizeof(int8_t) );
  for (int i = 0; i < filtrada->altura; i++)
    for (int j = 0; j < filtrada->largura; j++)
      if ( filtrada->dados[i][j] == 255)
        map[i][j] = AVAILABLE;
      else
        map[i][j] = BLOCKED;

  /**
   * Algoritmo de Busca
   * ~~~~~~~~~~~~~~~~~~~
   */
  
  /* Armazena o Ponto Final da Busca (possívelmente NULL) */
  Coordenada *dest = (Coordenada *)malloc(sizeof(Coordenada));

  /* Breadth First Search */
  dest = bfs( filtrada->dados, start, queue, map, filtrada->altura, filtrada->largura);

  /* Verifica se encontrou caminho */
  if ( dest == NULL )
  {
    /* Error, pay more attetion */
    printf("Não foi possível encontrar o caminho\n");
    
    /* Dilatamos duas vezes a imagem */
    dilate(filtrada);
    dilate(filtrada);

    /* Tenta novamente (última vez) */
    Coordenada *caminho;
    caminho = captura_caminho(filtrada, vector, iteration + 1);

    /* Check if null */
    if ( caminho == NULL )
    {
      printf("Limite de iterações execedidas.\n");
    } else {
      printf("Encontrou caminho pela %d iteração!\n", iteration + 1);
    }
  } else {
    /* Got the path */
    printf("Encontrou caminho sem dilatação\n");

    /* Add the borders */
    Coordenada *border_1 = (Coordenada *)malloc(sizeof(Coordenada));
    border_1->x = dest->x + 2;
    border_1->y = dest->y;
    vector_append(vector, border_1);
    Coordenada *border_3 = (Coordenada *)malloc(sizeof(Coordenada));
    border_3->x = dest->x + 1;
    border_3->y = dest->y;
    vector_append(vector, border_3);
    
    /* Append coordinates */
    while ( dest->parent != NULL )
    {
      /* Add to vector */
      vector_append(vector, dest);

      /* Go to next parent */
      dest = dest->parent;
    }

    /* Add the first point */
    vector_append(vector, sstartPtr);
    vector_append(vector, startPtr);
  }

  /* Malloc the Caminho */
  Coordenada *caminho = (Coordenada *)malloc((vector->size + 3) * sizeof(Coordenada));
  
  /* Reordenada da esquerda para direita */
  for ( int i = 0; i < vector->size; i++ )
    caminho[i] = *vector_get(vector, vector->size - i - 1);

  /* Retorna o caminho */
  return caminho;
}

/**
 * Dilate the Image
 *
 * Esta operação irá realizar a dilatação da imagem
 * baseando-se nas suas valorações.
 *
 * Este algoritmo é lento, e pode ser otimizado. Seu tempo
 * assintótico atual é O(n^2).
 */
void dilate(Imagem1C *img)
{
  for ( int i = 0; i < img->altura; i++ )
  {
    for ( int j = 0; j < img->largura; j++ )
    {
      if ( img->dados[i][j] == 255 )
      {
        if (i>0 && img->dados[i-1][j]==0) img->dados[i-1][j] = 2;
        if (j>0 && img->dados[i][j-1]==0) img->dados[i][j-1] = 2;
        if (i+1<img->altura && img->dados[i+1][j]==0) img->dados[i+1][j] = 2;
        if (j+1<img->largura && img->dados[i][j+1]==0) img->dados[i][j+1] = 2;
      }
    }
  }

  for ( int i = 0; i < img->altura; i++ )
    for ( int j = 0; j < img->largura; j++ )
      if ( img->dados[i][j] == 2 )
        img->dados[i][j] = 255;
}

/**
 * Discover the Start Point on Image
 *
 * Procura o ponto na primeira coluna da matriz onde
 * seja branco, e o define como ponto inicial de nossa
 * operação de busca.
 */
void discover_start_point(Imagem1C *binary_image, int32_t *y, int32_t *x)
{
  for ( int i = 0; i < binary_image->altura; i++ )
  {
    if ( binary_image->dados[i][1] == 255 )
    {
      *y = i;
      *x = 1;
      break;
    }
  }
}

/**
 * Breadth-first search
 *
 * The beauty of this algorithms, that is it always
 * find the shortest path.
 *
 * @param grid = matriz contendo imagem binarizada
 * @param local = ponto atual
 * @param height = altura do grid
 * @param width = largura do grid
 */

/* Andar pelas 8 direções */
int dx[] = {-1,1,0,0,-1,1,-1,1};
int dy[] = {0,0,1,-1,-1,-1,1,1};

path* head;
path* cur;

Coordenada *bfs( unsigned char ** grid, Coordenada startNode, Queue queue, int8_t **map, unsigned long height, unsigned long width)
{
  /* Insere a coordenada atual na queue */
  queue.push(&queue, startNode);
  head = NULL;
  cur = NULL;

  /* While queue are not empty */
  while ( queue.size != 0 )
  {
    /* Retrieve some point */
    Coordenada p = queue.pop(&queue);

    /* Check if its the target */
    if(p.x == width - 2)
    {
      Coordenada *obj = (Coordenada *)malloc(sizeof(Coordenada));
      obj->y = p.y;
      obj->x = p.x;
      obj->parent = p.parent;

      Coordenada *temp = (Coordenada *)malloc(sizeof(Coordenada));
      temp = obj->parent;
      while(temp!=NULL)
      {
          head = (path *)malloc(sizeof(path));
          head->x = temp->x;
          head->y = temp->y;
          head->next = cur;
          cur = head;

          temp = temp->parent;
      }

      return obj;
    }

    /* Percorre vizinhos */
    for(int i=0;i<8;i++)
    {
      int new_x = p.x + dx[i];
      int new_y = p.y + dy[i];

      if(new_x >= 0 && new_x<width && new_y >= 0 && new_y<height)
      {
        if(map[new_y][new_x] == AVAILABLE)
        {
          map[new_y][new_x] = PROCESSED;

          Coordenada *parent = (Coordenada *)malloc(sizeof(Coordenada));
          parent->y = p.y;
          parent->x = p.x;
          parent->parent = p.parent;

          /* Create the next point based on this as parent */
          Coordenada next;
          next.y = new_y;
          next.x = new_x;
          next.parent = parent;

          queue.push(&queue, next);
        }
      }
    }
  }

  /* Retorna vazio se não encontrar nada */
  return NULL;
}

/**
 * Push an item into queue, if this is the first item,
 * both queue->head and queue->tail will point to it,
 * otherwise the oldtail->next and tail will point to it.
 */
void push (Queue* queue, Coordenada item) {
    // Create a new node
    Node* n = (Node*) malloc (sizeof(Node));
    n->item = item;
    n->next = NULL;

    if (queue->head == NULL) { // no head
        queue->head = n;
    } else{
        queue->tail->next = n;
    }
    queue->tail = n;
    queue->size++;
}

/**
 * Return and remove the first item.
 */
Coordenada pop (Queue* queue) {
    // get the first item
    Node* head = queue->head;
    Coordenada item = head->item;
    // move head pointer to next node, decrease size
    queue->head = head->next;
    queue->size--;
    // free the memory of original head
    free(head);
    return item;
}

/**
 * Return but not remove the first item.
 */
Coordenada peek (Queue* queue) {
    Node* head = queue->head;
    return head->item;
}

/**
 * Create and initiate a Queue
 */
Queue createQueue () {
  Queue queue;
  queue.size = 0;
  queue.head = NULL;
  queue.tail = NULL;
  queue.push = &push;
  queue.pop = &pop;
  queue.peek = &peek;
  queue.display = &display;
  return queue;
}

/**
 * Show all items in queue.
 */
void display (Queue* queue) {
    printf("\nDisplay: ");
    // no item
    if (queue->size == 0)
        printf("No item in queue.\n");
    else { // has item(s)
        Node* head = queue->head;
        int i, size = queue->size;
        printf("%d item(s):\n", queue->size);
        for (i = 0; i < size; i++) {
            if (i > 0)
                printf(", ");
            printf("(%d, %d)", head->item.x, head->item.y);
            head = head->next;
        }
    }
    printf("\n\n");
}

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
void binarization(Imagem1C *origin, Imagem1C *output, uint32_t coordinate_y, uint32_t coordinate_x, int threshold)
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

  if ( output->dados[coordinate_y][coordinate_x] > threshold )
    output->dados[coordinate_y][coordinate_x] = 0;
  else
    output->dados[coordinate_y][coordinate_x] = 255;

  for (int i = 0; i < 3; i++)
    free(neighbors[i]);
  free(neighbors);
}

/**
 * Get Neightboors
 *
 * Retorna uma matriz de 3x3 contendo os vizinhos de uma
 * determinada coordenada de uma determinada matriz.
 *
 * Lembre-se: desalocar matriz posteriormente!
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

/**
 * Inicializa um Vetor Dinâmico
 *
 * Inicializamos um vetor dinâmico com uma capacidade
 * pré-definida.
 */
void vector_init(Vector *vector) {
  // initialize size and capacity
  vector->size = 0;
  vector->capacity = VECTOR_INITIAL_CAPACITY;

  // allocate memory for vector->data
  vector->data = malloc(sizeof(Coordenada *) * vector->capacity);
}

/**
 * Adiciona Elemento ao Vetor
 */
void vector_append(Vector *vector, Coordenada *value) {
  // make sure there's room to expand into
  vector_double_capacity_if_full(vector);

  // append the value and increment vector->size
  vector->data[vector->size++] = value;
}

/**
 * Captura Elemento do Vetor
 */
Coordenada* vector_get(Vector *vector, int index) {
  if (index >= vector->size || index < 0) {
    printf("Indice %d explode o tamanho %d do vetor\n", index, vector->size);
    exit(1);
  }
  return vector->data[index];
}

/**
 * Define Valor em Determinado Índice
 */
void vector_set(Vector *vector, int index, Coordenada *value) {
  // zero fill the vector up to the desired index
  while (index >= vector->size) {
    vector_append(vector, NULL);
  }

  // set the value at the desired index
  vector->data[index] = value;
}

/**
 * Aumenta o Tamanho do Vetor
 *
 * Utilizamos um aumento na base 2 devido ao `realloc`
 * ser um processo custo e um tanto lento, onde ele faz
 * o freeze do vetor inicial, e cria uma cópia.
 */
void vector_double_capacity_if_full(Vector *vector) {
  if (vector->size >= vector->capacity) {
    // double vector->capacity and resize the allocated memory accordingly
    vector->capacity *= 2;
    vector->data = realloc(vector->data, sizeof(Coordenada *) * vector->capacity);
  }
}

/**
 * Yay!
 */
void vector_free(Vector *vector) {
  free(vector->data);
}