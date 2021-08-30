/* Disciplina: Computação Concorrente */
/* Aluno: Thiago Castro */
/* Módulo 2 - Laboratório: 6 */
/* Código: Contar a quantidade de números pares em um vetor utilizando threads em Java. */

import java.util.Random;

public class Atividade
{
	private int[] vetor;
	private int numThreads;
	private int tamVetor;

	/*
	* Classe que contém o recurso dividido pelas threads.
	*/

	private class Contadora
	{
		private long contadoraPares = 0;

		public synchronized void contaPares(int elementoInicial)
		{
			for (int i = elementoInicial ; i < vetor.length ; i += numThreads)
			{
				if (vetor[i] % 2 == 0)
					contadoraPares++;
			}
		}

		public long getContagemPares()
		{
			return contadoraPares;
		}
	}

	/*
	* Classe que inicia cada thread.
	*/

	private class Lancadora extends Thread
	{
		private final int elementoInicial;
		private Contadora contadora;

		protected Lancadora(int idThread, Contadora contadora)
		{
			elementoInicial = idThread;
			this.contadora = contadora;
		}

		public void run()
		{
			contadora.contaPares(elementoInicial);
		}

	}

	private void inicializaThreads()
	{
		Thread threads[] = new Thread[numThreads];
		Contadora contadora = new Contadora();

		for (int i = 0 ; i < numThreads ; i++)
			threads[i] = new Lancadora(i, contadora);

		for (int i = 0 ; i < numThreads ; i++)
			threads[i].start();

		for (int i = 0 ; i < numThreads ; i++)
		{
			try
			{
				threads[i].join();
			}
			catch (InterruptedException e)
			{
				System.out.println("Erro ao aguardar uma thread.");
				return;
			}
		}

		System.out.println("Concorrente: " + contadora.getContagemPares() + " ocorrências.");
	}

	public boolean contaParesSequencial(String[] valores)
	{
		try
		{
			numThreads = Integer.parseInt(valores[0]);
			tamVetor = Integer.parseInt(valores[1]);
		}
		catch (NumberFormatException e)
		{
			System.out.println("Digite <nome do programa> <número de threads> <tamanho do vetor>.");
			return false;
		}
		catch (IndexOutOfBoundsException e)
		{
			System.out.println("Digite <nome do programa> <número de threads> <tamanho do vetor>.");
			return false;
		}

		vetor = new int[tamVetor];
		Random r = new Random();
		int contadoraParesSeq = 0;

		for (int i = 0 ; i < vetor.length ; i++)
		{
			vetor[i] = r.nextInt(100);

			if (vetor[i] % 2 == 0)
				contadoraParesSeq++;
		}

		System.out.println("Sequencial: " +  contadoraParesSeq + " ocorrências.");

		return true;

	}
	
	public static void main(String[] args)
	{
		Atividade inicio = new Atividade();

		if (inicio.contaParesSequencial(args))
			inicio.inicializaThreads();
	}
}