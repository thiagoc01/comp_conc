/* Disciplina: Computação Concorrente */
/* Aluno: Thiago Castro */
/* Módulo 2 - Laboratório: 7 */
/* Código: Utilizar o padrão leitores/escritores em Java para acesso de uma variável compartilhada. */

import java.util.Random;

public class Atividade
{
	private int numeroCentral = 0;
	private static int numIteracoes;

	private void colocaParaDormir() // Função para aumentar disputa entre as threads
	{
		try
		{
			Thread.sleep(500);
		}
		catch (InterruptedException e)
		{
			System.out.println("log.printErro(\"Erro ao colocar para dormir.\")");
			System.exit(255);
		}
	}
	
	//Implementação clássica de monitores

	private class Monitora
	{
		private int lendo = 0;
		private int escrevendo = 0;

		public synchronized void tentaEscrever(int id)
		{
			try
			{
				while (lendo > 0 || this.escrevendo > 0)
				{
					System.out.println("monitor.escritorBloqueado(" + id + ")");
					wait();
				}

				escrevendo++;
				System.out.println("monitor.escritorEscrevendo(" + id + ")");

			}

			catch (InterruptedException e)
			{
				System.out.println("log.printErro(\"Erro ao fazer uma thread aguardar.\")");
				System.exit(255);
			}

		}

		public synchronized void tentaLer(int id)
		{
			try
			{
				while (escrevendo > 0)
				{
					System.out.println("monitor.leitorBloqueado(" + id + ")");
					wait();
				}

				lendo++;

				System.out.println("monitor.leitorLendo(" + id + ")");

			}
			catch (InterruptedException e)
			{
				System.out.println("log.printErro(\"Erro ao fazer uma thread aguardar.\")");
				System.exit(255);
			}

		}

		public synchronized void liberaEscritor(int id)
		{
			escrevendo--;
		
			notifyAll();

		    System.out.println("monitor.liberaEscritor(" + id +")");
		}

		public synchronized void liberaLeitor(int id)
		{
			lendo--;

		    if (lendo == 0) 
		         this.notify();

		    System.out.println("monitor.liberaLeitor(" + id +")");
		}
	}

	// Classe para verificar se o número é primo. Preferível estar separada para termos um objeto para cada thread.

	private class VerificadoraPrimalidade
	{
		public void eNumeroPrimo(int n)
		{
			if (n == 0 || n == 1)
			{
				System.out.println("log.printLog(\"" + n + " não é primo.\")");
				return;
			}

			for (int d = 2 ; d < n ; d++)
			{
				if (n % d == 0)
				{
					System.out.println("log.printLog(\"" + n + " não é primo.\")");
					return;
				}
			}

			System.out.println("log.printLog(\"" + n + " é primo.\")");
		}
	}

	private class Leitora extends Thread
	{
		private int id;
		private Monitora monitora;

		Leitora(int id, Monitora monitora)
		{
			System.out.println("log.printLog(\"Criada classe Leitora. ID: " + id + "\")");
			this.id = id;
			this.monitora = monitora;
		}		

		public void run()
		{
			VerificadoraPrimalidade verificadoraPrimalidade = new VerificadoraPrimalidade();

			for (int i = 0 ; i < numIteracoes ; i++)
			{
				monitora.tentaLer(id);

				int numero = numeroCentral;

				System.out.println("log.printLog(\"Leitora " + id + " leu " + numero + ".\")");

				verificadoraPrimalidade.eNumeroPrimo(numero);

				monitora.liberaLeitor(id);

				colocaParaDormir();
			}
		}

	}

	private class Escritora extends Thread
	{
		private int id;
		private Monitora monitora;

		Escritora(int id, Monitora monitora)
		{
			System.out.println("log.printLog(\"Criada classe Escritora. ID: " + id + "\")");
			this.id = id;
			this.monitora = monitora;
		}

		public void run()
		{
			for (int i = 0 ; i < numIteracoes ; i++)
			{
				monitora.tentaEscrever(id);

				numeroCentral = id;

				System.out.println("log.printLog(\"Escritora " + id + " escreveu.\")");

				monitora.liberaEscritor(id);

				colocaParaDormir();
			}
		}
	}

	private class LeitoraEscritora extends Thread
	{
		private Monitora monitora;

		private int id;

		LeitoraEscritora(int id, Monitora monitora)
		{
			System.out.println("log.printLog(\"Criada classe LeitoraEscritora. ID: " + id + "\")");
			this.id = id;
			this.monitora = monitora;
		}

		public void run()
		{
			VerificadoraPrimalidade verificadoraPrimalidade = new VerificadoraPrimalidade();

			for (int i = 0; i < numIteracoes ; i++)
			{
				monitora.tentaLer(id);

				int numero = numeroCentral;

				System.out.println("log.printLog(\"LeitoraEscritora " + id + " leu " + numero + ".\")");
				
				if (numeroCentral % 2 == 0)
					System.out.println("log.printLog(\"" + numero + " é par.\")");
				else
					System.out.println("log.printLog(\"" + numero + " é ímpar.\")");

				monitora.liberaLeitor(id);

				monitora.tentaEscrever(id);

				/*
				* As verificações a seguir protegem contra overflow de int
				* caso as threads LeitorasEscritoras sejam executadas repetidas vezes.
				*/

				if (numero * 2 < 0)
				{
					Random r = new Random();

					numeroCentral = r.nextInt(9) + 1;
				}
				else
					numeroCentral = numero * 2;

				System.out.println("log.printLog(\"LeitoraEscritora " + id + " escreveu " + numeroCentral + ".\")");

				monitora.liberaEscritor(id);

				colocaParaDormir();

				
			}
		}
	}

	private class Lancadora
	{
		private int numEscritores;
		private int numLeitores;
		private int numLeitoresEscritores;		

		Lancadora(int numLeitores, int numEscritores, int numLeitoresEscritores)
		{
			this.numEscritores = numEscritores;
			this.numLeitores = numLeitores;
			this.numLeitoresEscritores = numLeitoresEscritores;
		}

		public void inicializaThreads()
		{
			Monitora monitora = new Monitora();
			Leitora[] leitores = new Leitora[numLeitores];
			Escritora[] escritores = new Escritora[numEscritores];
			LeitoraEscritora[] leitoresEscritores = new LeitoraEscritora[numLeitoresEscritores];

			System.out.println("import verificador");
    		System.out.println("monitor = verificador.Monitor()");
    		System.out.println("log = verificador.Log()");

    		for (int i = 0 ; i < numLeitores ; i++)
    			leitores[i] = new Leitora(i + 1, monitora);  			

    		for (int i = 0 ; i < numEscritores ; i++)
    			escritores[i] = new Escritora(i + 1, monitora);

    		for (int i = 0 ; i < numLeitoresEscritores ; i++)
    			leitoresEscritores[i] = new LeitoraEscritora(i + 1, monitora);

    		for (int i = 0 ; i < numEscritores ; i++)
    			escritores[i].start();

    		for (int i = 0 ; i < numLeitores ; i++)
    			leitores[i].start();  			

    		for (int i = 0 ; i < numLeitoresEscritores ; i++)
    			leitoresEscritores[i].start();


		}
	}

	public void passaArgumentos(int numLeitores, int numEscritores, int numLeitoresEscritores)
	{
		Lancadora lancadora = new Lancadora(numLeitores, numEscritores, numLeitoresEscritores);
		lancadora.inicializaThreads();		
	}

	public static void main(String[] args)
	{
		Atividade atividade = new Atividade();

		try
		{
			int numLeitores = Integer.parseInt(args[0]);
			int numEscritores = Integer.parseInt(args[1]);
			int numLeitoresEscritores = Integer.parseInt(args[2]);
			numIteracoes = Integer.parseInt(args[3]);
			
			// Limita até 100 iterações, já que teremos (numLeitores + numEscritores + numLeitoresEscritores) * numIteracoes iteracoes

			if (numLeitores < 1 || numEscritores < 1 || numLeitoresEscritores < 1 || numIteracoes < 1 || numIteracoes > 100)
			{
				System.out.println("import verificador");
				System.out.println("log.printErro(\"Digite uma quantidade para leitores, escritores e leitores/escritores acima de 1. " +
					"Digite uma quantidade de iterações entre 1 e 100.\")");
				System.exit(255);
			}

			atividade.passaArgumentos(numLeitores, numEscritores, numLeitoresEscritores);			
		}

		catch (NumberFormatException e)
		{
			System.out.println("import verificador");
			System.out.println("log = verificador.Log()");
			System.out.println("log.printErro(\"Digite uma quantidade para leitores, escritores e leitores/escritores acima de 1. " +
				"Digite uma quantidade de iterações entre 1 e 100.\")");
			System.exit(255);
		}
		catch (IndexOutOfBoundsException e)
		{
			System.out.println("import verificador");
			System.out.println("log = verificador.Log()");
			System.out.println("log.printErro(\"Digite java Atividade <número de leitores> <número de escritores> <número de leitores/escritores> <número de iterações>.\")");
			System.exit(255);
		}


		
	}
}