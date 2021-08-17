#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4 // 4 mensagens no programa

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t bem_vindo, volte_sempre; // Temos duas condições de espera

/*
* A variável de impressão é necessária para duas coisas:
* a condição de corrida, caso ela esteja intercalada com as duas do meio, e para a naturalidade de aguardar ambas.
*
* A variável de bem_vindo_imprimiu é uma booleana necessária para o caso de corrida.
* Supondo que bem vindo esteja entre as duas do meio ou começado antes delas. Elas irão aguardar bem vindo ser impresso.
* Bem vindo irá sinalizar e liberar qualquer uma que esteja esperando.
* Mas, se não houvesse a variável booleana, iria ocorrer espera incondicionalmente.
* Porém, bem vindo já teria encerrado, e elas iriam aguardar e não receberiam sinalização da liberação.
* Logo, o caso que bem vindo libera o sinal antes das duas começarem ou está intercalado entra elas irá causar um deadlock.
* Por isso, ambas só devem esperar enquanto bem vindo não for impresso.
*/

int impressoes = 0, bem_vindo_imprimiu = 0;

void *imprime_fique_a_vontade(void *args)
{
	pthread_mutex_lock(&mutex);

	if (bem_vindo_imprimiu == 0) // Só irá aguardar se bem vindo não imprimiu
		pthread_cond_wait(&bem_vindo, &mutex);

	pthread_mutex_unlock(&mutex);

	puts("Fique à vontade.");	

	pthread_mutex_lock(&mutex);

	impressoes++;

	if (impressoes == 2) // Só libera para a impressão de volte sempre se fique à vontade e sente-se por favor foram impressos.
		pthread_cond_signal(&volte_sempre);

	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

void *imprime_seja_bem_vindo(void *args)
{
	pthread_mutex_lock(&mutex);

	puts("Seja bem-vindo!");

	bem_vindo_imprimiu++;

	pthread_cond_broadcast(&bem_vindo);

	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

void *imprime_volte_sempre(void *args)
{
	pthread_mutex_lock(&mutex);

	if (impressoes < 2) // Podemos usar if porque o sinal só será liberado quando sente-se por favor e fique à vontade tiverem sido impressos.
		pthread_cond_wait(&volte_sempre, &mutex);

	puts("Volte sempre!");	

	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

void *imprime_sente_se_por_favor(void *args)
{
	pthread_mutex_lock(&mutex);

	if (bem_vindo_imprimiu == 0)
		pthread_cond_wait(&bem_vindo, &mutex);

	pthread_mutex_unlock(&mutex);

	puts("Sente-se por favor.");

	pthread_mutex_lock(&mutex);

	impressoes++;

	if (impressoes == 2)
		pthread_cond_signal(&volte_sempre);

	pthread_mutex_unlock(&mutex);


	pthread_exit(NULL);
}

int main()
{
	int t1_analise, t2_analise, t3_analise, t4_analise;

	pthread_t tids[NUM_THREADS];

	pthread_cond_init(&bem_vindo, NULL);
	pthread_cond_init(&volte_sempre, NULL);

	t1_analise = pthread_create(&tids[0], NULL, imprime_fique_a_vontade, NULL);
	t2_analise = pthread_create(&tids[1], NULL, imprime_seja_bem_vindo, NULL);
	t3_analise = pthread_create(&tids[2], NULL, imprime_volte_sempre, NULL);
	t4_analise = pthread_create(&tids[3], NULL, imprime_sente_se_por_favor, NULL);

	if (t1_analise || t2_analise || t3_analise || t4_analise)
	{
		puts("Erro ao criar uma thread.");
		return 1;
	}

	for (int i = 0 ; i < NUM_THREADS ; i++)
	{
		if (pthread_join(tids[i], NULL))
		{
			puts("Erro ao aguardar uma thread.");
			return 2;
		}
	}

	pthread_cond_destroy(&bem_vindo);

	pthread_cond_destroy(&volte_sempre);

	pthread_mutex_destroy(&mutex);

	return 0;
}