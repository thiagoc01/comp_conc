/* Disciplina: Computação Concorrente */
/* Aluno: Thiago Castro */
/* Módulo 3 - Laboratório: 9 */
/* Código: Implementar produtores/consumidores utilizando semáforos em C, de tal forma que os consumidores aguardem os produtores encherem o buffer. */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TAM_BUFFER 10

sem_t slot_vazio, produtores, buffer_cheio;

int num_consumidores, num_produtores, buffer[TAM_BUFFER];

void *produz(void *args)
{
	static int in = 0;

	int id = *(int *) args;

	while (1)
	{
		sem_wait(&slot_vazio); // Aguarda um slot vazio. Os produtores não têm restrição, seguem o padrão.

		sem_wait(&produtores);

		buffer[in] = rand() % 10;

		printf("log.produz(%d, %d, %d)\n", id, buffer[in], in);

		in = (in + 1) % TAM_BUFFER;

		if (in == 0) // Quando o indicador de posição voltar a 0, o buffer ficou cheio. Irá sinalizar o semáforo de verificação de buffer dos consumidores.
		{
			printf("log.sinalizaBufferCheio(%d)\n", id);
			sleep(1);
			sem_post(&buffer_cheio);
		}

		sem_post(&produtores);
	}

}

void *consome(void *args)
{
	int id = *(int *) args;

	while (1)
	{
		sem_wait(&buffer_cheio); // Irá aguardar o buffer ficar completamente cheio. Como só há uma sinalização, apenas um consumidor irá prosseguir.

		for (int i = 0 ; i < TAM_BUFFER ; i++)
		{
			printf("log.consome(%d, %d, %d)\n", id, buffer[i], i);
			sem_post(&slot_vazio); // Libera slot normalmente
		}

		printf("log.sinalizaBufferVazio(%d)\n", id); // Sinaliza que leu tudo

		sleep(1);

	}
}

void inicializa_semaforos()
{
	sem_init(&slot_vazio, 0, TAM_BUFFER);
	sem_init(&produtores, 0, 1);
	sem_init(&buffer_cheio, 0, 0);
}

void libera_semaforos()
{
	sem_destroy(&slot_vazio);
	sem_destroy(&buffer_cheio);
	sem_destroy(&produtores);
}

void inicializa_threads()
{
	pthread_t threads[num_produtores + num_consumidores];
	int ids[num_produtores + num_consumidores];

	printf("import verificador\nlog = verificador.Log()\n");

	for (int i = 0 ; i < num_produtores ; i++)
	{
		ids[i] = i;
		if (pthread_create(&threads[i], NULL, produz, (void *) &ids[i]))
		{
			printf("Erro ao criar uma thread produtora. ID %d.\n", i);
			libera_semaforos();
			exit(255);
		}
	}

	for (int i = num_produtores ; i < num_consumidores + num_produtores ; i++)
	{
		ids[i] = i;
		if (pthread_create(&threads[i], NULL, consome, (void *) &ids[i]))
		{
			printf("Erro ao criar uma thread consumidora. ID %d.\n", i);
			libera_semaforos();
			exit(255);
		}
	}	

	for (int i = 0 ; i < num_produtores + num_consumidores ; i++)
	{
		if (pthread_join(threads[i], NULL))
		{
			printf("Erro ao aguardar uma thread. ID %d.\n", i);
			exit(255);
		}
	}

}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("Digite %s <número de consumidores> <número de produtores>.\n", argv[0]);
		return 1;
	}

	num_consumidores = atoi(argv[1]);
	num_produtores = atoi(argv[2]);

	if (num_produtores < 1 || num_consumidores < 1)
	{
		puts("Forneça um número de consumidores e produtores maior que 0.");
		return 2;
	}

	srand(time(NULL));

	inicializa_semaforos();

	inicializa_threads();

	libera_semaforos();

	return 0;

}