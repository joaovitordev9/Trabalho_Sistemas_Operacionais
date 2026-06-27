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
        pessoas[item].Atendido = 1;
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

void Pessoa_init(Pessoa pessoas[]){
   
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

int preferencia(int p1, int p2) {
    if (p1 == p2) return 0;

    if (p1 == 0) return 0;
    if (p2 == 0) return 1;

    return (p1 == 3 && p2 == 2) ||
           (p1 == 2 && p2 == 1) ||
           (p1 == 1 && p2 == 3);
}

int Tem_deadlock(Monitor *mb, Pessoa pessoas[]) {
    int gravida = 0;
    int idoso = 0;
    int deficiente = 0;

    for (int i = 0; i < PESSOAS; i++) {
        if (mb->Fila[i] == -1) {
            continue;
        }

        int id = mb->Fila[i];

        if (pessoas[id].Prioridade == 3) {
            gravida = 1;
        } else if (pessoas[id].Prioridade == 2) {
            idoso = 1;
        } else if (pessoas[id].Prioridade == 1) {
            deficiente = 1;
        }
    }

    return gravida && idoso && deficiente;
}

void* Funcao_threads(void *Arg)
{
    ThreadArgs *t = (ThreadArgs *)Arg;

    for (int i = 0; i < t->vezes; i++)
    {
        Monitor_insert(t->mb, t->pessoas, t->id);

        pthread_mutex_lock(&t->mb->mutex);
        
        while (!t->pessoas[t->id].Atendido)
        {
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

int Proximo_na_fila(Monitor *mb, Pessoa pessoas[])
{
    int escolhido = -1;
    int pos_escolhido = -1;

    if (Tem_deadlock(mb, pessoas)) {
        return -1;
    }

    for (int i = 0; i < PESSOAS; i++) {
        if (mb->Fila[i] == -1){
            continue;
        }

        int id = mb->Fila[i];

        if (escolhido == -1 || preferencia(pessoas[id].Prioridade, pessoas[escolhido].Prioridade)) {
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

            if (preferencia(pessoas[escolhido].Prioridade, pessoas[id].Prioridade)) {
                pessoas[id].inanicao++;

                if (pessoas[id].inanicao >= 2) {
                    if (pessoas[id].Prioridade < 3) {
                        pessoas[id].Prioridade++;
                    }
                    pessoas[id].inanicao = 0;

                    printf("Gerente detectou inanição, aumentando prioridade de %s\n",pessoas[id].Nome);
                }
            }
        }
    }
    if (escolhido != -1) {

        pessoas[escolhido].Atendido = 1;
        pessoas[escolhido].inanicao = 0;
        pthread_cond_broadcast(&mb->chamado);
        return escolhido;
    }

    return -1;
}

void* Funcao_thread_gerente(void *Arg){
    ThreadArgs *t = (ThreadArgs *)Arg;
    int gravida ,idoso ,deficiente;
    int idc_gravida, idc_idoso, idc_deficiente;
    while (1) {
        sleep(5);
        gravida = 0;
        idoso = 0;
        deficiente = 0;
        pthread_mutex_lock(&t->mb->mutex);
        
        for (int i = 0; i < THREADS; i++)
        {
            if (t->mb->Fila[i] == -1)
            { 
                continue;
            }
            int id = t->mb->Fila[i];
            if (t->pessoas[id].Prioridade == 3)
            {
                gravida = 1;
                idc_gravida = id;
                
            }else if (t->pessoas[id].Prioridade == 2)
            {
                idoso = 1;
                idc_idoso = id;
            }else if (t->pessoas[id].Prioridade == 1)
            {
                deficiente = 1;
                idc_deficiente = id;
            }
        }
        int ocupado = 0;

        for (int i = 0; i < PESSOAS; i++) {
            if (t->pessoas[i].Atendido) {
                ocupado = 1;
                break;
            }
        }
        if (gravida && idoso && deficiente)
        {
            int vez = rand() % 3;
            
            switch (vez)
            {
            case 0:
                if (!ocupado) {
                    t->pessoas[idc_gravida].Atendido = 1;
                    printf("Gerente detectou deadlock, liberando %s para atendimento\n",t->pessoas[idc_gravida].Nome);
                }
                break;
            case 1:
                if (!ocupado) {
                    t->pessoas[idc_idoso].Atendido = 1;
                    printf("Gerente detectou deadlock, liberando %s para atendimento\n",t->pessoas[idc_idoso].Nome);
                }
                break;
            case 2:
                if (!ocupado) {
                    t->pessoas[idc_deficiente].Atendido = 1;
                    printf("Gerente detectou deadlock, liberando %s para atendimento\n",t->pessoas[idc_deficiente].Nome);
                }
                break;
            default:
                break;
            }
        }
        
        pthread_cond_broadcast(&t->mb->chamado);
        pthread_mutex_unlock(&t->mb->mutex);
    }

    return NULL;
}

void Criar_thread_Gerente(Monitor *mb, Pessoa pessoas[]){
    pthread_t gerente;
    ThreadArgs *gerenteArgs = malloc(sizeof(ThreadArgs));

    gerenteArgs->mb = mb;
    gerenteArgs->pessoas = pessoas;
    
    pthread_create(&gerente, NULL,Funcao_thread_gerente,gerenteArgs);
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
    srand(time(NULL));
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
    Criar_thread_Gerente(&monitor, pessoas);
    
    
    for (int i = 0; i < THREADS; i++) {
        pthread_join(monitor.threads[i], NULL);
    }
    
    return 0;
}