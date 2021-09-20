#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 4 // 4 mensagens


/* O primeiro semáforo limita a impressão das duas threads intermediárias. O segundo limita a impressão de volte sempre depois das três. */

sem_t controle_bem_vindo, controle_volte_sempre;


void * imprime_bem_vindo(void * arg)
{
	puts("Seja bem-vindo!");

	/* Libera para as duas threads intermediárias */

	sem_post(&controle_bem_vindo);
	sem_post(&controle_bem_vindo);


	pthread_exit(NULL);
}

void * imprime_fique_a_vontade(void * arg)
{
	sem_wait(&controle_bem_vindo);

	puts("Fique à vontade.");

	sem_post(&controle_volte_sempre);

	pthread_exit(NULL);
}

void * imprime_sente_se_por_favor(void * arg)
{
	sem_wait(&controle_bem_vindo);

	puts("Sente-se por favor.");

	sem_post(&controle_volte_sempre);

	pthread_exit(NULL);
}

void * imprime_volte_sempre(void * arg)
{
	/* Como devemos esperar duas mensagens, basta aguardamos dois sinais */

	sem_wait(&controle_volte_sempre);
	sem_wait(&controle_volte_sempre);

	puts("Volte sempre!");

	pthread_exit(NULL);
}


void inicializa_threads()
{
	pthread_t threads[NUM_THREADS];
	int analise1, analise2, analise3, analise4;

	sem_init(&controle_bem_vindo, 0, 0);
	sem_init(&controle_volte_sempre, 0, 0);

	analise1 = pthread_create(&threads[0], NULL, imprime_bem_vindo, NULL);
	analise2 = pthread_create(&threads[1], NULL, imprime_fique_a_vontade, NULL);
	analise3 = pthread_create(&threads[2], NULL, imprime_sente_se_por_favor, NULL);
	analise4 = pthread_create(&threads[3], NULL, imprime_volte_sempre, NULL);

	
	if (analise1 || analise2 || analise3 || analise4)
	{
		puts("Erro ao criar uma thread.");
		exit(1);
	}

	for (int i = 0 ; i < NUM_THREADS; i++)
	{
		if (pthread_join(threads[i], NULL))
		{
			printf("Erro ao aguardar a thread %d.\n", i);
			exit(2);
		}
	}

	sem_destroy(&controle_bem_vindo);
	sem_destroy(&controle_volte_sempre);
}

int main()
{
	inicializa_threads();

	return 0;
} 