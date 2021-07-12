/* Disciplina: Computacao Concorrente */
/* Aluno.: Thiago Castro */
/* Módulo 1 - Laboratório: 1 */
/* Código: Elevar ao quadrado todos os 10000 elementosde um vetor */

#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <pthread.h>

/**
 * A ideia desse programa é utilizar o fato do número de elementos ser fixo.
 * Como são duas threads, uma thread percorre a primeira metade do vetor, de 0 a 4999, e a outra a segunda metade, de 5000 a 10000.
 * Utilizando apenas uma função, precisamos saber o início e o fim do vetor.
 * Assim, criamos uma estrutura para guardar o piso e o teto das duas metades. Ela será usada pela função que calcula o quadrado.
 * As duas threads executam paralelamente e calculam muito mais rápido que um programa sequencial.
*/

#define NUM_ELEMENTOS 10000

int vetor[NUM_ELEMENTOS];

typedef struct
{
    int piso;
    int teto;
}args_thread_t;


void *eleva_ao_quadrado (void *arg) 
{
    args_thread_t *args = (args_thread_t *) arg;
        
    for (int i = args->piso ; i < args->teto ; i++)
        vetor[i] = pow(vetor[i], 2);

    pthread_exit(NULL);
}


int main() 
{
    pthread_t tid_sistema[2]; //identificadores das threads no sistema

    args_thread_t primeira_metade, segunda_metade;

    primeira_metade.piso = 0;
    primeira_metade.teto = NUM_ELEMENTOS/2;
    segunda_metade.piso = NUM_ELEMENTOS/2;
    segunda_metade.teto = NUM_ELEMENTOS;

    // Para alterar o valor base da função rand.
    srand(time(NULL));

    puts("----------Início do vetor base---------");
    for (int i = 0 ; i < NUM_ELEMENTOS ; i++)
    {
        vetor[i] = rand() % 50;
        printf("%dº elemento: %d\n", i+1, vetor[i]);
    }

    puts("----------Fim do vetor base---------");

    if (pthread_create(&tid_sistema[0], NULL, eleva_ao_quadrado, (void *) &primeira_metade))
    {
        printf("Erro de criação da thread 1.\n");
        exit(1);
    }

    if (pthread_create(&tid_sistema[1], NULL, eleva_ao_quadrado, (void *) &segunda_metade))
    {
        printf("Erro de criação da thread 1.\n");
        exit(1);
    }

    if (pthread_join(tid_sistema[0], NULL))
    {
        printf("Erro ao aguardar thread 1.\n");
        exit(1);
    }

    if (pthread_join(tid_sistema[1], NULL))
    {
        printf("Erro ao aguardar thread 2.\n");
        exit(1);
    }

    for (int i = 0 ; i < NUM_ELEMENTOS ; i++)
        printf("%dº elemento: %d\n", i+1, vetor[i]);
  
    return 0;
}