#ifndef TRABALHO.H
#define TRABALHO.H

//#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREADS 9
#define PESSOAS 8

typedef struct {
    int Fila[THREADS];
    int count, in, out;
    pthread_t threads[THREADS];
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
}Monitor;

typedef struct 
{
    int Atendido;
    char Nome[17];
    int Prioridade;
}Pessoa;

typedef struct {
    Monitor *mb;
    Pessoa *pessoas;
    int id;
    int vezes;
} ThreadArgs;


void Monitor_init(Monitor *mb);
void Monitor_insert(Monitor *mb, int item);
int Monitor_remove(Monitor *mb);
void Mostra_fila(Monitor *Infos);
void Pessoa_init(Pessoa *pessoas);

#endif