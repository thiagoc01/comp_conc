Ambiente 1
=========================

* Sistema Ubuntu 21.04 64 bits
* 4 GB de RAM
* Processador AMD FX 6300 4.02 GHz - Virtualizando apenas 2 threads

<br>
<br>

***Devido ao fato de haver 4 GB de RAM e o programa com dimensão 1 bilhão solicitar em torno de 3,8 GB de RAM, o sistema não pode manter o processo aberto.***


**Sequencial**
------------------
<br>

Dimensão - 10<sup>5</sup><br>
----------------
**0.000294**
<br>

Dimensão - 10<sup>7</sup><br>
----------------
**0.033083**
<br>

Dimensão - 10<sup>9</sup>
---------------
<br>

### **Em todos os casos, o sistema sempre matava o processo. Não foi possível calcular um resultado.**
<br>
<br>

**Threads**
------------------
<br>

Dimensão - 10<sup>5</sup>
----------------
<br>

Nº de threads | Tempo
--------------|-------
2 Threads | 0.000551
4 Threads | 0.000625

<br>

Dimensão - 10<sup>7</sup>
----------------
<br>

Nº de threads | Tempo
--------------|-------
2 Threads | 0.030991
4 Threads | 0.028100

<br>

Dimensão - 10<sup>9</sup>
---------------
<br>

### **Em todos os casos, o sistema sempre matava o processo. Não foi possível calcular um resultado.**
<br>
<br>

**Ganhos**
------------------
<br>

Dimensão - 10<sup>5</sup>
----------------
<br>

Nº de threads | Ganho
--------------|-------
2 Threads | 0,533575
4 Threads | 0,470400

<br>

Dimensão - 10<sup>7</sup>
----------------
<br>

Nº de threads | Tempo
--------------|-------
2 Threads | 1,067503
4 Threads | 1,177330

<br>


<br>
<br>

----------------------

Ambiente 2
=========================

* Sistema Windows 10 Pro x64
* 8 GB de RAM
* Processador AMD FX 6300 4.02 GHz - 3 núcleos / 6 threads

<br>

**Sequencial**
------------------
<br>

Dimensão - 10<sup>5</sup><br>
----------------
**0.000319**
<br>

Dimensão - 10<sup>7</sup><br>
----------------
**0.033294**
<br>

Dimensão - 10<sup>9</sup>
---------------
**3.302056**
<br>

**Threads**
------------------
<br>

Dimensão - 10<sup>5</sup>
----------------
<br>

Nº de threads | Tempo
--------------|-------
2 Threads | 0.001406
4 Threads | 0.002116

<br>

Dimensão - 10<sup>7</sup>
----------------
<br>

Nº de threads | Tempo
--------------|-------
2 Threads | 0.019574
4 Threads | 0.015879

<br>

Dimensão - 10<sup>9</sup>
---------------
<br>

Nº de threads | Tempo
--------------|-------
2 Threads | 1.910828
4 Threads | 1.554101

<br>

**Ganhos**
------------------
<br>

Dimensão - 10<sup>5</sup>
----------------
<br>

Nº de threads | Ganho
--------------|-------
2 Threads | 0,226884
4 Threads | 0,150756

<br>

Dimensão - 10<sup>7</sup>
----------------
<br>

Nº de threads | Tempo
--------------|-------
2 Threads | 1,700929
4 Threads | 2,096731

<br>

Dimensão - 10<sup>9</sup>
---------------
<br>

Nº de threads | Tempo
--------------|-------
2 Threads | 1,728075
4 Threads | 2,124737


