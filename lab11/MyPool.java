/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Criando um pool de threads em Java */
import java.util.LinkedList;

//-------------------------------------------------------------------------------
/**
 * Classe que implementa um pool de threads para executar tarefas de forma concorrente.
 * 
 * Esta classe gerencia um conjunto fixo de threads trabalhadoras que executam tarefas
 * (objetos Runnable) de uma fila compartilhada. O padrão Thread Pool é útil para:
 * - Controlar o número de threads simultâneas
 * - Reutilizar threads para múltiplas tarefas
 * - Evitar overhead de criação/destruição de threads
 */

class FilaTarefas {
    /** Número fixo de threads no pool */
    private final int nThreads;
    
    /** Array das threads trabalhadoras */
    private final MyPoolThreads[] threads;
    
    /** Fila sincronizada de tarefas aguardando execução */
    private final LinkedList<Runnable> queue;
    
    /** Flag indicando se o pool foi encerrado */
    private boolean shutdown;

    /**
     * Construtor que inicializa o pool de threads.
     * 
     * @param nThreads número de threads a serem criadas no pool
     * @throws IllegalArgumentException se nThreads <= 0
     */
    public FilaTarefas(int nThreads) {
        if (nThreads <= 0) {
            throw new IllegalArgumentException("Número de threads deve ser positivo");
        }
        
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];
        
        // Cria e inicia todas as threads trabalhadoras
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    /**
     * Submete uma tarefa para execução no pool de threads.
     * A tarefa é adicionada à fila e será executada por uma thread disponível.
     * 
     * @param r tarefa (Runnable) a ser executada
     * @throws NullPointerException se r for null
     */
    public void execute(Runnable r) {
        if (r == null) {
            throw new NullPointerException("Tarefa não pode ser null");
        }
        
        synchronized(queue) {
            // Se o pool foi encerrado, não aceita novas tarefas
            if (this.shutdown) return;
            
            // Adiciona tarefa na fila e notifica threads esperando
            queue.addLast(r);
            queue.notify();
        }
    }
    
    /**
     * Encerra o pool de threads de forma ordenada.
     * 
     * Este método:
     * 1. Para de aceitar novas tarefas
     * 2. Permite que tarefas em execução terminem
     * 3. Notifica todas as threads para terminarem
     * 4. Aguarda todas as threads terminarem (join)
     * 
     * Após chamar shutdown(), o pool não pode ser reutilizado.
     */
    public void shutdown() {
        synchronized(queue) {
            this.shutdown = true;
            queue.notifyAll(); // Acorda todas as threads esperando
        }
        
        // Aguarda todas as threads terminarem
        for (int i=0; i<nThreads; i++) {
          try { 
              threads[i].join(); 
          } catch (InterruptedException e) { 
              // Interrupção durante shutdown - retorna imediatamente
              Thread.currentThread().interrupt();
              return; 
          }
        }
    }

    /**
     * Classe interna que representa uma thread trabalhadora do pool.
     * 
     * Cada thread executa um loop infinito:
     * 1. Aguarda por tarefas na fila
     * 2. Remove e executa a próxima tarefa
     * 3. Repete até receber sinal de shutdown
     */
    private class MyPoolThreads extends Thread {
       /**
        * Método principal da thread trabalhadora.
        * Processa tarefas da fila até receber sinal de encerramento.
        */
       public void run() {
         Runnable r;
         
         while (true) {
           synchronized(queue) {
             // Aguarda enquanto fila estiver vazia E não foi dado shutdown
             while (queue.isEmpty() && (!shutdown)) {
               try { 
                   queue.wait(); // Aguarda notificação de nova tarefa
               }
               catch (InterruptedException ignored) {
                   // Ignora interrupções durante wait
               }
             }
             
             // Se fila vazia após shutdown, encerra thread
             if (queue.isEmpty()) return;   
             
             // Remove próxima tarefa da fila
             r = (Runnable) queue.removeFirst();
           }
           
           // Executa a tarefa fora do bloco sincronizado
           try { 
               r.run(); 
           }
           catch (RuntimeException e) {
               // Captura exceções das tarefas para não derrubar a thread
               System.err.println("Erro na execução da tarefa: " + e.getMessage());
           }
         } 
       } 
    } 
}
//-------------------------------------------------------------------------------

//--PASSO 1: cria uma classe que implementa a interface Runnable 
class Hello implements Runnable {
   String msg;
   public Hello(String m) { msg = m; }

   //--metodo executado pela thread
   public void run() {
      System.out.println(msg);
   }
}

/**
 * Classe que implementa Runnable para verificar se um número é primo
 */
class Primo implements Runnable {
   private long numero;
   
   public Primo(long n) { 
       this.numero = n; 
   }

   //--metodo executado pela thread
   public void run() {
       boolean resultado = ehPrimo(numero);
       if (resultado) {
           System.out.println(numero + " é primo");
       } else {
           System.out.println(numero + " não é primo");
       }
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
}

//Classe da aplicação (método main)
class MyPool {
    private static final int NTHREADS = 10;

    public static void main (String[] args) {
      //--PASSO 2: cria o pool de threads
      FilaTarefas pool = new FilaTarefas(NTHREADS); 
      
      //--PASSO 3: dispara a execução dos objetos runnable usando o pool de threads
      for (int i = 0; i < 25; i++) {
        final String m = "Hello da tarefa " + i;
        Runnable hello = new Hello(m);
        pool.execute(hello);
        Runnable primo = new Primo(i);
        pool.execute(primo);
      }

      //--PASSO 4: esperar pelo termino das threads
      pool.shutdown();
      System.out.println("Terminou");
   }
}