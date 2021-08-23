/* Disciplina: Computação Concorrente */
/* Aluno: Thiago Castro */
/* Módulo 2 - Laboratório: 5 */
/* Código: Somar todos valores de um vetor utlizando threads e gerar novos valores para o vetor, com sincronização entre o fim de cada tarefa. */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t cond_espera;

long int num_threads;
long int trabalhando; // Número de threads trabalhando
int *vetor;

/*
* Função que controla a barreira.
* Podemos usá-la para as duas sincronizações, de soma e escrita.
* Isso pode ser feito devido à própria definição de barreira.
* Não há como as verificações de escrita cruzarem a de soma e vice-versa.
* Logo, as mesmas variáveis podem ser usadas.
*/

void opera_barreira()
{
    pthread_mutex_lock(&mutex);

    if (trabalhando == 1) // Última thread encerrou parte do seu trabalho. 
    {
        pthread_cond_broadcast(&cond_espera);
        printf("---Liberando threads---\n");
        trabalhando = num_threads; // Reinicia a contagem.
    }

    else
    {
        trabalhando--;
        printf("Threads trabalhando: %ld\n", trabalhando);
        pthread_cond_wait(&cond_espera, &mutex); // Vai aguardar até todas as threads realizarem suas somas/escritas.
    }

    pthread_mutex_unlock(&mutex);
}


void *soma_valores_vetor(void *arg)
{
    long int id_thread = *(long int *) arg;
    long int *soma_local = (long int *) malloc(sizeof(long int));
    long int num_iteracoes = 0;

    if (!soma_local)
    {
        printf("Erro ao alocar memória para a variável de soma local da thread %ld.\n", id_thread);
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond_espera);
        free(vetor);
        exit(255);
    }

    *soma_local = 0;

    do
    {
        for (long int i = 0 ; i < num_threads ; i++) // Loop de soma.
            *soma_local += vetor[i];

        printf("Thread %ld encerrou seu somatório. Soma atual: %ld \n", id_thread, *soma_local);

        opera_barreira(); // Primeira barreira. Sincroniza todas as somas antes de trocar os valores do vetor.

        printf("Thread %ld liberada.\n", id_thread);

        num_iteracoes++;

        if (num_iteracoes == num_threads) // Apenas para evitar N escritas desnecessárias, já que essa seria a última iteração.
            break;

        vetor[id_thread] = rand() % 10; // Novo valor para próxima iteração.

        printf("Thread %ld encerrou sua escrita. Escreveu %d \n", id_thread, vetor[id_thread]);

        opera_barreira(); // Aguarda todas receberem novos valores para próxima iteração.

    } while (num_iteracoes < num_threads);



    pthread_exit((void *) soma_local);
}

void imprime_resultado_verifica_corretude(long int retornos[], long int resultado_referencia)
{
    puts("-------------------VERIFICAÇÃO-------------------");

    for (long int i = 0 ; i < num_threads ; i++)
    {
        printf("Resultado da thread %ld:    %ld\n", i, retornos[i]);

        if (retornos[i] != resultado_referencia)
        {
            printf("Resultado incorreto. Esperado: %ld.\n", resultado_referencia);
            return;
        }
    }

    puts("Todos os valores são iguais.");
}

void inicializa_vetor()
{
    srand(time(NULL));

    for (long int i = 0 ; i < num_threads ; i++)
        vetor[i] = rand() % 10;
}

int inicializa_threads()
{
    pthread_t tids[num_threads];
    long int args[num_threads];
    long int retornos[num_threads];
    long int *ponte_retorno; // Armazenerá o retorno temporariamente antes do salvamento no vetor de retornos.

    for (long int i = 0 ; i < num_threads ; i++)
    {
        args[i] = i; // ID da thread

        if (pthread_create(&tids[i], NULL, soma_valores_vetor, (void *) &args[i]))
        {
            printf("Erro ao criar a thread %ld.\n", i);
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&cond_espera);
            return 1;
        }
    }

    for (long int i = 0 ; i < num_threads ; i++)
    {
        if (pthread_join(tids[i], (void **) &ponte_retorno))
        {
            printf("Erro ao aguardar a thread %ld.\n", i);
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&cond_espera);
            return 2;            
        }

        retornos[i] = *ponte_retorno;

        free(ponte_retorno);
    }
    
    /* Primeira posição do vetor como referência, mas poderia ser qualquer uma, pois todas devem ser iguais. */

    imprime_resultado_verifica_corretude(retornos, retornos[0]);

    return 0;

}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Digite %s <número de threads>\n", argv[0]);
        return 1;
    }

    num_threads = atol(argv[1]);

    if (num_threads <= 0)
    {
        puts("Digite um número de threads maior que 0.");
        return 2;
    }

    vetor = (int *) malloc(sizeof(int) * num_threads);

    if (!vetor)
    {
        puts("Erro de alocação de memória para o vetor.");
        return 3;
    }

    inicializa_vetor();

    trabalhando = num_threads; // Inicialmente, todas as threads trabalharão.

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_espera, NULL);

    if (inicializa_threads()) // Caso ocorra algum erro no controle de criação e junção das threads.
    {
        free(vetor);
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond_espera);

        return 4;
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_espera);

    free(vetor);

    return 0;

    


}