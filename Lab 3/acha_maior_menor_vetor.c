/* Disciplina: Computação Concorrente */
/* Aluno: Thiago Castro */
/* Módulo 1 - Laboratório: 3 */
/* Código: Encontrar o maior e o menor elemento de um vetor utilizando threads */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../Lab 2/timer.h"


/**
 * O procedimento para o cálculo da soma na função concorrente é igual a multiplicação de matrizes.
 * Fornecemos um elemento inicial correspondente ao número da ordem de criação de uma thread e,
 * a cada iteração, incrementa-se o iterador com o número de threads solicitada.
 */

long long int tam_vetor;
long long int num_threads;
float *vetor;

typedef struct
{
    float maior;
    float menor;
} retorno_t;

typedef struct
{
    retorno_t *ret;
    long long int elem_inicial;
} args_t;

void *retorna_maior_menor_concorrente(void *args)
{
    args_t *arg = (args_t *) args;

    long long int elem_inicial = arg->elem_inicial;

    retorno_t *ret = arg->ret;

    ret->maior = vetor[elem_inicial];
    ret->menor = vetor[elem_inicial];

    for (long long int i = elem_inicial ; i < tam_vetor ; i += num_threads)
    {
        if (ret->maior < vetor[i])
            ret->maior = vetor[i];

        if (ret->menor > vetor[i])
            ret->menor = vetor[i];
    }

    pthread_exit((void *) ret);
}

int exibe_resultados(pthread_t *ids_threads)
{
    retorno_t resultado, *retorno;

    retorno = (retorno_t *) malloc(sizeof(retorno_t));

    if (!retorno)
    {
        fputs("Erro de alocação de memória para a variável de retorno das threads.", stderr);
        free(ids_threads);
        free(vetor);
        return 1;
    }

    for (long long int i = 0 ; i < num_threads ; i++)
    {
        if (pthread_join(ids_threads[i], (void **) &retorno))
        {
            fprintf(stderr, "Erro de espera da thread %lld.\n", i);
            free(ids_threads);
            free(retorno);
            free(vetor);
            return 1;
        }

        if (i == 0)
        {
            resultado.maior = retorno->maior;
            resultado.menor = retorno->menor;
            free(retorno);
            continue;
        }

        if (retorno->maior > resultado.maior)
            resultado.maior = retorno->maior;

        if (retorno->menor < resultado.menor)
            resultado.menor = retorno->menor;

        free(retorno);
        
    }

    printf("O maior do vetor é %.5f e o menor é %.5f.\n", resultado.maior, resultado.menor);

    free(ids_threads);

    return 0;

}

void inicializa_vetor()
{
    srand(time(NULL));

    vetor = (float *) malloc(sizeof(float) * tam_vetor);

    if (!vetor)
    {
        fputs("Erro de alocação de memória para o vetor.", stderr);
        exit(255);
    }

    for (int i = 0 ; i < tam_vetor ; i++)
        vetor[i] = ((float)rand()/(float)(RAND_MAX)) * 1000000.0;
}

void inicializa_threads()
{
    pthread_t *ids_threads = (pthread_t *) malloc(sizeof(pthread_t) * num_threads);
    args_t *args = (args_t *) malloc(sizeof(args_t) * num_threads);

    if (!ids_threads)
    {
        fputs("Erro de alocação de memória para o vetor de threads.", stderr);
        free(vetor);
        exit(255);
    }

    if (!args)
    {
        fputs("Erro de alocação de memória para o vetor de argumentos.", stderr);
        free(vetor);
        free(ids_threads);
        exit(255);
    }

    for (long long int i = 0 ; i < num_threads ; i++)
    {
        args[i].ret = (retorno_t *) malloc(sizeof(retorno_t));
        args[i].elem_inicial = i;

        if (!args[i].ret)
        {
            fputs("Erro de alocação de memória para o vetor de argumentos.", stderr);
            free(vetor);
            free(ids_threads);
            free(args);
            exit(255);
        }

        if (pthread_create(ids_threads + i, NULL, retorna_maior_menor_concorrente, (void *) &args[i]))
        {
            fprintf(stderr, "Erro de criação da thread %lld.\n", i);
            free(ids_threads);
            free(vetor);
            exit(255);
        }
    }

    if (exibe_resultados(ids_threads) == 1)
    {
        free(args);
        exit(255);
    }

    free(args);

}
retorno_t retorna_maior_menor_sequencial()
{
    retorno_t ret;

    ret.maior = vetor[0];
    ret.menor = vetor[0];

    for (int i = 1 ; i < tam_vetor ; i++)
    {
        if (ret.maior < vetor[i])
            ret.maior = vetor[i];

        if (ret.menor > vetor[i])
            ret.menor = vetor[i];
    }

    return ret;
}

int main(int argc, char **argv)
{
    double inicio, fim;

    if (argc < 3)
    {
        fprintf(stderr, "Digite: %s <dimensão do vetor> <número de threads>.\n", argv[0]);
        return 1;
    }

    tam_vetor = atoll(argv[1]);
    num_threads = atoll(argv[2]);

    if (!tam_vetor || !num_threads)
    {
        fputs("Um dos parâmetros não está correto.", stderr);
        return 2;
    }

    if (tam_vetor < num_threads)
        num_threads = tam_vetor;

    inicializa_vetor();

    GET_TIME(inicio);

    retorno_t sequencial = retorna_maior_menor_sequencial();

    GET_TIME(fim);

    printf("O maior do vetor é %.5f e o menor é %.5f.\n", sequencial.maior, sequencial.menor);

    printf("Tempo de execução sequencial: %lf\n", fim-inicio);

    GET_TIME(inicio);

    inicializa_threads();

    GET_TIME(fim);

    printf("Tempo de execução concorrente: %lf\n", fim-inicio);

    free(vetor);

    return 0;
}
