#include "Trabalho.h"

void Monitor_init(Monitor *mb) {
    mb->count = mb->in = mb->out = 0;
    pthread_mutex_init(&mb->mutex, NULL);
    pthread_cond_init(&mb->not_empty, NULL);
    pthread_cond_init(&mb->not_full, NULL);
}

void Monitor_insert(Monitor *mb,Pessoa *pessoas,int item) {
    pthread_mutex_lock(&mb->mutex);

    while (mb->count == PESSOAS)
        pthread_cond_wait(&mb->not_full, &mb->mutex);

    mb->Fila[mb->in] = item;
    mb->in = (mb->in + 1) % PESSOAS;
    mb->count++;

    printf("%s está na fila do caixa\n",pessoas[item].Nome);

    pthread_cond_signal(&mb->not_empty);
    pthread_mutex_unlock(&mb->mutex);
}

int Monitor_remove(Monitor *mb,Pessoa *pessoas) {
    pthread_mutex_lock(&mb->mutex);

    while (mb->count == 0)
        pthread_cond_wait(&mb->not_empty, &mb->mutex);

    int item = mb->Fila[mb->out];
    mb->out = (mb->out + 1) % PESSOAS;
    mb->count--;
    printf("%s vai para casa\n",pessoas[item].Nome);

    pthread_cond_signal(&mb->not_full);
    pthread_mutex_unlock(&mb->mutex);

    return item;
}

void Pessoa_init(Pessoa *pessoas)
{
    for (int i = 0; i < PESSOAS; i++)
    {
        pessoas[i].Atendido = 0;
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

void Mostra_fila(Monitor *mb){
    for (int i = 0; i < THREADS i++)
    {
        printf("%d ",mb->Fila[i]);
    }
    printf("\n");
}

void Funcao_threads(Monitor *mb,Pessoa *pessoas){
    
}

void Criar_threads(Monitor *mb){
    for (int i = 0; i < THREADS; i++)
    {
        thread_data[i].id = i; 
        if (pthread_create(&mb->threads[i], NULL,Funcao_threads, &thread_data[i]) != 0) {
            perror("Erro ao criar a thread");
            return 1;
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
    Pessoa_init(&pessoas);
 
    
    
    return 0;
}