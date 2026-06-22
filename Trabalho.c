#include "Trabalho.h"

void Reorganiza_fila(Monitor *mb) {
    int fila_temporaria[PESSOAS];
    int indice_temp = 0;

    // 1. Enche a fila temporária toda com -1
    for (int i = 0; i < PESSOAS; i++) {
        fila_temporaria[i] = -1;
    }

    // 2. Copia apenas os IDs válidos (!= -1) para o começo da fila temporária
    for (int i = 0; i < PESSOAS; i++) {
        if (mb->Fila[i] != -1) {
            fila_temporaria[indice_temp] = mb->Fila[i];
            indice_temp++;
        }
    }

    // 3. Devolve os valores compactados para a fila original do Monitor
    for (int i = 0; i < PESSOAS; i++) {
        mb->Fila[i] = fila_temporaria[i];
    }

    // 4. Como todo mundo foi empurrado para o começo, a próxima posição 
    // de inserção ('in') será exatamente igual à quantidade de pessoas na fila
    mb->in = mb->count; 
}


void Monitor_init(Monitor *mb) {
    mb->count = mb->in = mb->out = 0;
    pthread_mutex_init(&mb->mutex, NULL);
    pthread_cond_init(&mb->not_empty, NULL);
    pthread_cond_init(&mb->not_full, NULL);
    for (int i = 0; i < PESSOAS; i++)
    {
        mb->Fila[i] = -1;
    }
    
}

void Monitor_insert(Monitor *mb,Pessoa *pessoas,int item) {
    pthread_mutex_lock(&mb->mutex);

    while (mb->count == PESSOAS)
        pthread_cond_wait(&mb->not_full, &mb->mutex);

    mb->Fila[mb->in] = item;
    mb->in = (mb->in + 1) % PESSOAS;
    mb->count++;

    printf("%s está na fila do caixa\n",pessoas[item].Nome);

    if (mb->count == 1) {
        Proximo_na_fila(mb, pessoas);
    }

    pthread_cond_signal(&mb->not_empty);
    pthread_mutex_unlock(&mb->mutex);
}

void Monitor_remove(Monitor *mb,Pessoa *pessoas,int item) {
    pthread_mutex_lock(&mb->mutex);

    while (mb->count == 0)
        pthread_cond_wait(&mb->not_empty, &mb->mutex);
    for (int i = 0; i < PESSOAS; i++)
    {
        if (mb->Fila[i] == item)
        {
            mb->Fila[i] = -1;
            break;
        }
    }
    
    
    pessoas[item].Atendido = 0;
    mb->count--;
    Reorganiza_fila(mb);
    
    printf("%s está sendo atendido(a)\n",pessoas[item].Nome);

    pthread_cond_signal(&mb->not_full);
    pthread_mutex_unlock(&mb->mutex);

}

void Pessoa_init(Pessoa pessoas[])
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
    for (int i = 0; i < THREADS ;i++)
    {
        printf("%d ",mb->Fila[i]);
    }
    printf("\n");
}

void* Funcao_threads(void *Arg) {
    ThreadArgs *t = (ThreadArgs *)Arg;
    
    for (int i = 0; i < t->vezes; i++) {

        Monitor_insert(t->mb, t->pessoas, t->id);
        pthread_mutex_lock(&t->mb->mutex);

        while (!t->pessoas[t->id].Atendido) {
            pthread_cond_wait(&t->mb->not_full, &t->mb->mutex);
        }
        pthread_mutex_unlock(&t->mb->mutex);

        sleep(1);

        Monitor_remove(t->mb, t->pessoas, t->id);
        printf("%s vai para casa\n", t->pessoas[t->id].Nome);


        pthread_mutex_lock(&t->mb->mutex);
        Proximo_na_fila(t->mb, t->pessoas);
        pthread_mutex_unlock(&t->mb->mutex);

        int tempo_fora = (rand() % 3) + 3; // Gera 3, 4 ou 5
        sleep(tempo_fora);
    }
    
    return NULL;
}

void Proximo_na_fila(Monitor *mb,Pessoa pessoas[]){
    int prox = -1;
    int indice = -1;
    for (int i = 0; i < PESSOAS; i++)
    {
        if (mb->Fila[i] != -1)
        {
            
            if (pessoas[i].Prioridade > prox)
            {
                prox = pessoas[prox].Prioridade;
                indice = mb->Fila[i];
            }
        }
    }
    if (indice != -1) {
        pessoas[indice].Atendido = 1;
        pthread_cond_broadcast(&mb->not_full);
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
    
    // Espera todas as threads terminarem antes de fechar o programa
    for (int i = 0; i < THREADS; i++) {
        pthread_join(monitor.threads[i], NULL);
    }
    
    return 0;
}