#include "Trabalho.h"

void Reorganiza_fila(Monitor *mb) {
    int novaFila[PESSOAS];
    int j = 0;

    for (int i = 0; i < PESSOAS; i++) {
        if (mb->Fila[i] != -1) {
            novaFila[j] = mb->Fila[i];
            j++;
        }
    }

    for (int i = j; i < PESSOAS; i++) {
        novaFila[i] = -1;
    }

    for (int i = 0; i < PESSOAS; i++) {
        mb->Fila[i] = novaFila[i];
    }

    mb->in = j;
}

void Monitor_init(Monitor *mb) {
    mb->count = mb->in = 0;
    pthread_mutex_init(&mb->mutex, NULL);
    pthread_cond_init(&mb->not_empty, NULL);
    pthread_cond_init(&mb->not_full, NULL);
    pthread_cond_init(&mb->chamado, NULL);
    for (int i = 0; i < PESSOAS; i++)
    {
        mb->Fila[i] = -1;
    }
    
}

void Monitor_insert(Monitor *mb, Pessoa *pessoas, int item) {
    pthread_mutex_lock(&mb->mutex);

    printf("%s está na fila do caixa\n", pessoas[item].Nome);

    while (mb->count == PESSOAS)
        pthread_cond_wait(&mb->not_full, &mb->mutex);

    mb->Fila[mb->in] = item;
    mb->in = (mb->in + 1) % PESSOAS;
    mb->count++;



    if (mb->count == 1) {
        Proximo_na_fila(mb, pessoas);
    }

    pthread_cond_signal(&mb->not_empty);
    pthread_mutex_unlock(&mb->mutex);
}

void Monitor_remove(Monitor *mb, Pessoa *pessoas, int item) {
    pthread_mutex_lock(&mb->mutex);
    printf("%s vai para casa\n",pessoas[item].Nome);

    while (mb->count == 0){
        pthread_cond_wait(&mb->not_empty, &mb->mutex);
    }

    for (int i = 0; i < PESSOAS; i++) {
        if (mb->Fila[i] == item) {
            mb->Fila[i] = -1;
            break;
        }
    }

    pessoas[item].Atendido = 0;
    mb->count--;

    Reorganiza_fila(mb);

    Proximo_na_fila(mb, pessoas);

    pthread_cond_signal(&mb->not_full);
    pthread_mutex_unlock(&mb->mutex);
}

void Pessoa_init(Pessoa pessoas[])
{
   
    for (int i = 0; i < PESSOAS; i++)
    {
        pessoas[i].Atendido = 0;
        pessoas[i].inanicao = 0;
    }

    strcpy(pessoas[0].Nome, "Maria");
    strcpy(pessoas[1].Nome, "Marcos");
    pessoas[0].Prioridade = 3;
    pessoas[1].Prioridade = 3;

    strcpy(pessoas[2].Nome, "Vanda");
    strcpy(pessoas[3].Nome, "Valter");
    pessoas[2].Prioridade = 2;
    pessoas[3].Prioridade = 2;

    strcpy(pessoas[4].Nome, "Paula");
    strcpy(pessoas[5].Nome, "Pedro");
    pessoas[4].Prioridade = 1;
    pessoas[5].Prioridade = 1;

    strcpy(pessoas[6].Nome, "Sueli");
    strcpy(pessoas[7].Nome, "Silas");
    pessoas[6].Prioridade = 0;
    pessoas[7].Prioridade = 0;
}

void* Funcao_threads(void *Arg) {
    ThreadArgs *t = (ThreadArgs *)Arg;

    for (int i = 0; i < t->vezes; i++) {

        Monitor_insert(t->mb, t->pessoas, t->id);

        pthread_mutex_lock(&t->mb->mutex);

        while (!t->pessoas[t->id].Atendido) {
            pthread_cond_wait(&t->mb->chamado, &t->mb->mutex);
        }

        pthread_mutex_unlock(&t->mb->mutex);
        printf("%s está sendo atendido(a)\n", t->pessoas[t->id].Nome);
        sleep(1);

        Monitor_remove(t->mb, t->pessoas, t->id);

        int tempo_fora = (rand() % 3) + 3;
        sleep(tempo_fora);
    }

    return NULL;
}

void Proximo_na_fila(Monitor *mb, Pessoa pessoas[])
{
    int escolhido = -1;
    int pos_escolhido = -1;

    for (int i = 0; i < PESSOAS; i++) {
        if (mb->Fila[i] == -1){
            continue;
        }

        int id = mb->Fila[i];

        if (escolhido == -1 || pessoas[id].Prioridade > pessoas[escolhido].Prioridade) {
            escolhido = id;
        }
    }

    for (int i = 0; i < PESSOAS; i++) {
        if (mb->Fila[i] == escolhido) {
            pos_escolhido = i;
            break;
        }
    }

    
    for (int i = 0; i < pos_escolhido ; i++) {

        if (mb->Fila[i] != -1 && mb->Fila[i] != escolhido){
            
            int id = mb->Fila[i];

            pessoas[id].inanicao++;

            if (pessoas[id].inanicao >= 2) {
                pessoas[id].Prioridade++;
                pessoas[id].inanicao = 0;

                printf("Gerente detectou inanição, aumentando prioridade de %s\n",pessoas[id].Nome);
            }
        }
    }
    if (escolhido != -1) {
        pessoas[escolhido].Atendido = 1;
        pessoas[escolhido].inanicao = 0;
        pthread_cond_broadcast(&mb->chamado);
    }
}

void Criar_threads(Monitor *mb, Pessoa pessoas[], int vezes){
    
    for (int i = 0; i < THREADS; i++) {

        ThreadArgs *args = malloc(sizeof(ThreadArgs));

        args->mb = mb;
        args->pessoas = pessoas;
        args->id = i;
        args->vezes = vezes;

        if (pthread_create(&mb->threads[i],NULL,Funcao_threads,args) != 0) {
            printf("\nErro ao Criar a Thread!\n");
            exit(1);
        }
    }
}


int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("\nArgumentos Invalidos!\n");
        exit(1);
    }
    Monitor monitor;
    Pessoa pessoas[PESSOAS];
    int rodar = atoi(argv[1]);

    Monitor_init(&monitor);
    Pessoa_init(pessoas);
 
    Criar_threads(&monitor, pessoas, rodar);
    
    
    for (int i = 0; i < THREADS; i++) {
        pthread_join(monitor.threads[i], NULL);
    }
    
    return 0;
}