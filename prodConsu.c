/*
 * Adapted from the ostep book
 * http://pages.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#define MAX 3

int buffer[MAX];
int posicaoInsercao = 0;
int posicaoLeitura = 0;
int contador = 0;

int produtosTotais = 100;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t livre;
sem_t preenchido;

void inserirBuffer(int valor) {
  buffer[posicaoInsercao] = valor;
  posicaoInsercao++;
  if (posicaoInsercao >= MAX)
    posicaoInsercao = 0;
  contador++;
}

int lerBuffer() {
  int tmp = buffer[posicaoLeitura];
  posicaoLeitura++;
  if (posicaoLeitura >= MAX)
    posicaoLeitura = 0;
  contador--;
  return tmp;
}

void *Elfos(void *arg) {
  int i;
  for (i = 0; i < produtosTotais; i++) {
    printf("\nElfo numero %d quer resolver um problema \n", i);
    printf("Verifica se pode entrar na fila \n", i);
    sem_wait(&livre);
    printf("Elfo entrou na posição %d \n", i);
    pthread_mutex_lock(&mutex);
    inserirBuffer(i);
    pthread_mutex_unlock(&mutex);
    sem_post(&preenchido);
  }
  return NULL;
}

void *PapaiNoel(void *arg) {
  int i;
  for (i = 0; i < produtosTotais; i++) {
    printf("\nPapai Noel acorda para ajudar o Elfo %d\n", i);
    sem_wait(&preenchido);
    printf("Resolvendo o problema do Elfo %d\n", i);
    pthread_mutex_lock(&mutex);
    int tmp = lerBuffer();
    pthread_mutex_unlock(&mutex);
    sem_post(&livre);
    printf("Problema  %d foi resolvido \n", tmp);
  }
  return NULL;
}

int main(void) {
  sem_init(&preenchido, 0, 0);
  sem_init(&livre, 0, MAX);
  pthread_t prod, consu;
  pthread_create(&prod, NULL, Elfos, NULL);
  pthread_create(&consu, NULL, PapaiNoel, NULL);
  pthread_join(prod, NULL);
  pthread_join(consu, NULL);
  return 0;
}
