#ifndef TRABALHO_H
#define TRABALHO_H

//#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define THREADS 8
#define PESSOAS 8

typedef struct {
    int Fila[THREADS];
    int count, in;
    pthread_t threads[THREADS];
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    pthread_cond_t chamado;
}Monitor;

typedef struct 
{
    int Atendido;
    char Nome[17];
    int Prioridade;
    int inanicao;
}Pessoa;

typedef struct {
    Monitor *mb;
    Pessoa *pessoas;
    int id;
    int vezes;
} ThreadArgs;

void Reorganiza_fila(Monitor *mb);
void Monitor_init(Monitor *mb);
void Monitor_insert(Monitor *mb,Pessoa *pessoas,int item);
void Monitor_remove(Monitor *mb,Pessoa *pessoas,int item);
void Pessoa_init(Pessoa *pessoas);
void* Funcao_threads(void *Arg);
void Proximo_na_fila(Monitor *mb,Pessoa *pessoas);
void Criar_threads(Monitor *mb, Pessoa *pessoas, int vezes);
void* Funcao_thread_gerente(void *Arg);
void Criar_thread_Gerente(Monitor *mb, Pessoa pessoas[]);
int Passar_vez(Monitor *mb, Pessoa *pessoas,int escolhido);


#endif