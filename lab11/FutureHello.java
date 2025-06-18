/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de futures */
/* -------------------------------------------------------------------*/

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import java.util.ArrayList;
import java.util.List;


//classe runnable original
class MyCallable implements Callable<Long> {
  //construtor
  MyCallable() {
  }
 
  //método para execução
  public Long call() throws Exception {
    long s = 0;
    for (long i=1; i<=100; i++) {
      s++;
    }
    return s;
  }
}

//classe callable para verificar se um número é primo
class PrimoCallable implements Callable<Boolean> {
  private long numero;
  
  //construtor
  PrimoCallable(long n) {
    this.numero = n;
  }
 
  //método para execução - verifica se o número é primo
  public Boolean call() throws Exception {
    return ehPrimo(numero);
  }
  
  //funcao para determinar se um numero é primo
  private boolean ehPrimo(long n) {
    if(n <= 1) return false;
    if(n == 2) return true;
    if(n % 2 == 0) return false;
    
    for(long i = 3; i <= Math.sqrt(n); i += 2) {
      if(n % i == 0) return false;
    }
    return true;
  }
  
  //getter para o número (útil para debug)
  public long getNumero() {
    return numero;
  }
}

//classe do método main
public class FutureHello  {
  private static final int N = 3;
  private static final int NTHREADS = 10;

  public static void main(String[] args) {
    //cria um pool de threads (NTHREADS)
    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);
    
    //cria uma lista para armazenar referencias de chamadas assincronas originais
    List<Future<Long>> list = new ArrayList<Future<Long>>();
    //cria uma lista para armazenar referencias das verificações de primos
    List<Future<Boolean>> primoList = new ArrayList<Future<Boolean>>();
    List<Long> numerosTestados = new ArrayList<Long>(); // para rastrear quais números foram testados

    // Executa as tarefas originais
    for (int i = 0; i < N; i++) {
      Callable<Long> worker = new MyCallable();
      Future<Long> submit = executor.submit(worker);
      list.add(submit);
    }

    // Executa verificações de números primos
    long[] numerosParaTestar = {2, 3, 17, 25, 29, 97, 100, 101, 997, 1000};
    for (long numero : numerosParaTestar) {
      Callable<Boolean> primoWorker = new PrimoCallable(numero);
      Future<Boolean> primoSubmit = executor.submit(primoWorker);
      primoList.add(primoSubmit);
      numerosTestados.add(numero);
    }

    System.out.println("Tarefas originais: " + list.size());
    System.out.println("Verificações de primo: " + primoList.size());
    
    //pode fazer outras tarefas...

    //recupera os resultados das tarefas originais e faz o somatório final
    long sum = 0;
    for (Future<Long> future : list) {
      try {
        sum += future.get(); //bloqueia se a computação nao tiver terminado
      } catch (InterruptedException e) {
        e.printStackTrace();
      } catch (ExecutionException e) {
        e.printStackTrace();
      }
    }
    System.out.println("Soma das tarefas originais: " + sum);
    
    //recupera os resultados das verificações de primos
    System.out.println("\nResultados das verificações de números primos:");
    int contadorPrimos = 0;
    for (int i = 0; i < primoList.size(); i++) {
      try {
        Future<Boolean> future = primoList.get(i);
        Long numeroTestado = numerosTestados.get(i);
        Boolean ehPrimo = future.get(); //bloqueia se a computação nao tiver terminado
        
        System.out.println(numeroTestado + (ehPrimo ? " é primo" : " não é primo"));
        if (ehPrimo) contadorPrimos++;
        
      } catch (InterruptedException e) {
        e.printStackTrace();
      } catch (ExecutionException e) {
        e.printStackTrace();
      }
    }
    
    System.out.println("\nTotal de números primos encontrados: " + contadorPrimos + " de " + numerosTestados.size() + " números testados");
    
    executor.shutdown();
  }
}