/* Disciplina: Computacao Concorrente */
/* Aluno.: Thiago Castro */
/* Módulo 1 - Laboratório: 2 */
/* Código: Multiplicar matrizes quadradas utilizando threads para melhora de desempenho */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

/**
 * Abordagem utilizada: Threads percorrem a linha de forma intercalada. Ou seja, múltiplos de ordem de execução.
 * A matriz 1 está carregada com 2 e a matriz 2 está carregada com 3.
 */

int dimensao_matriz;
int numero_threads;
float *matriz1_entrada;
float *matriz2_entrada;
float *matriz_saida;

void libera_memoria_matrizes()
{
    free(matriz1_entrada);
    free(matriz2_entrada);
    free(matriz_saida);
}

/**
 * Como uma matriz está carregada com 2 e outra com 3, cada célula terá valor 2 * 3 * n, onde n é a dimensão fornecida.
 * Se tudo estiver correto, o programa irá imprimir a última mensagem.
 * Caso contrário, ele encerra ao encontrar um valor incorreto e uma mensagem é impressa informando qual célula está incorreta.
*/

void testa_resultado()
{
    float referencia = 2 * 3 * dimensao_matriz;

    for (int i = 0 ; i < dimensao_matriz ; i++)
    {
        for (int j = 0 ; j < dimensao_matriz ; j++)
        {
            if (matriz_saida[i * dimensao_matriz + j] != referencia)
            {
                printf("A célula %d apresenta o valor %f. Esperado: %f\n", i * dimensao_matriz + j, matriz_saida[i * dimensao_matriz + j], referencia);
                libera_memoria_matrizes();
                exit(255);
            }
        }
    }

    puts("A integridade da matriz resultado está preservada.\n");

    libera_memoria_matrizes();
}

void inicializa_matrizes()
{
    matriz1_entrada = (float *) malloc (sizeof(float) * dimensao_matriz * dimensao_matriz);
    matriz2_entrada = (float *) malloc (sizeof(float) * dimensao_matriz * dimensao_matriz);
    matriz_saida = (float *) malloc (sizeof(float) * dimensao_matriz * dimensao_matriz);

    if (!matriz1_entrada || !matriz2_entrada || !matriz_saida)
    {
        printf("Erro de alocação de memória.\n");
        free(matriz1_entrada);
        free(matriz2_entrada);
        free(matriz_saida);
        exit(255);
    }

    for (int i = 0 ; i < dimensao_matriz ; i++)
    {
        for (int j = 0 ; j < dimensao_matriz ; j++)
        {
            matriz1_entrada[i * dimensao_matriz + j] = 2;
            matriz2_entrada[i * dimensao_matriz + j] = 3;
            matriz_saida[i * dimensao_matriz + j] = 0;
        }
    }
}

/**
 * Abordagem similar aos vídeos dessa semana. Porém, agora precisamos de um loop para as colunas e outro para as linhas da coluna dentro desse loop.
 */

void *multiplica_matrizes(void *arg)
{
    int linha_inicial = *(int *) arg;

    for (int i = linha_inicial; i < dimensao_matriz ; i += numero_threads)
    {
        for (int j = 0 ; j < dimensao_matriz ; j++)
        {
            for (int k = 0 ; k < dimensao_matriz ; k++)
                matriz_saida[i * dimensao_matriz + j] += matriz1_entrada[i * dimensao_matriz + k] * matriz2_entrada[k * dimensao_matriz + j];
                
        }
    }
    pthread_exit(NULL);
}

/**
 * Inicialização típica de threads. Não é necessário utilizar uma estrutura de dados, pois iremos passar apenas a primeira linha para a thread.
 */

void inicializa_threads()
{
    pthread_t *ids_threads = (pthread_t *) malloc(sizeof(pthread_t) * numero_threads);

    int *linhas_iniciais = (int *) malloc(sizeof(int) * numero_threads); // Guarda o ponto de partida para cada thread

    if (!ids_threads)
    {
        printf("Erro de alocação de memória para vetor de threads.\n");
        libera_memoria_matrizes();
        free(ids_threads);
        free(linhas_iniciais);
        exit(255);
    }

    if (!linhas_iniciais)
    {
        printf("Erro de alocação de memória para vetor de argumentos.\n");
        libera_memoria_matrizes();
        free(ids_threads);
        free(linhas_iniciais);
        exit(255);
    }
 
    for (int i = 0 ; i < numero_threads ; i++)
    {
        linhas_iniciais[i] = i;
        int teste = pthread_create(&ids_threads[i], NULL, multiplica_matrizes, (void *) &linhas_iniciais[i]);

        if (teste)
        {
            printf("Erro de criação da thread %d.\n", i+1);
            libera_memoria_matrizes();
            free(ids_threads);
            free(linhas_iniciais);
            exit(255);
        }
    }

    for (int i = 0 ; i < numero_threads ; i++)
    {
        int teste = pthread_join(ids_threads[i], NULL);

        if (teste)
        {
            printf("Erro de espera da thread %d.\n", i+1);
            libera_memoria_matrizes();
            free(ids_threads);
            free(linhas_iniciais);
            exit(255);
        }
    }

    free(ids_threads);
    free(linhas_iniciais);


}


int main(int argc, char **argv)
{
    double inicio, fim;    

    if (argc < 3)
    {
        printf("A entrada pela linha de comando deve ser: <nome do programa> <dimensão da matriz> <número de threads>.\n");
        return 1;
    }

    dimensao_matriz = atoi(argv[1]);
    numero_threads = atoi(argv[2]);

    if (numero_threads == 0)
    {
        printf("O número de threads deve ser maior que 1.\n");
        return 2;
    }

    if (dimensao_matriz <= 0)
    {
        printf("Digite valores acima de 0 para a dimensão.\n");
        return 3;
    }

    if (dimensao_matriz < numero_threads)
        numero_threads = dimensao_matriz;

    GET_TIME(inicio);

    inicializa_matrizes();

    GET_TIME(fim);

    printf("Tempo de início do programa: %lf\n\n", fim-inicio);

    GET_TIME(inicio);

    inicializa_threads();

    GET_TIME(fim);

    printf("Tempo de execução de threads e cálculo total: %lf\n\n", fim-inicio);

    GET_TIME(inicio);

    testa_resultado();

    GET_TIME(fim);

    printf("Tempo de testes e fim do programa: %lf\n", fim-inicio);

    return 0;
}
