/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <math.h>
#include <time.h>

pthread_mutex_t mutex;
pthread_mutex_t mutex0;
long long int n_primos;
long long int n_ultimo;
int nthreads; //qtde de threads (passada linha de comando)
long long int n;

//função para saber quantos numeros ate n são primos 
int ehPrimo(long long int n) {
    int i;
    if (n<=1) return 0;
    if (n==2) return 1;
    if (n%2==0) return 0;
    for (i=3; i<sqrt(n)+1; i+=2)
        if(n%i==0) return 0;
    return 1;
}

//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
    long int id = (long int) arg;
    long long int n_ultimot;

    while(1) {
        pthread_mutex_lock(&mutex);
        if (n_ultimo >= n) {// Se o n atual é o valor de n escolhido, a thread encerra
            pthread_mutex_unlock(&mutex);
            break;
        }
        n_ultimot = n_ultimo;     // Se não, copiamos o valor com o qual a thread atual ira trabalhar
        n_ultimo++; // incrementamos o valor original para a proxima thread
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex0);
        n_primos += ehPrimo(n_ultimot);
        pthread_mutex_unlock(&mutex0);
        
    }

    pthread_exit(NULL);
  }

int main(int argc, char *argv[]) {
    pthread_t *tid; //identificadores das threads no sistema
 
    //--le e avalia os parametros de entrada
    if(argc<3) {
        printf("Digite: %s <Valor de N> <Número de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[2]);
    n = atoi(argv[1]);
    printf("Qtd de threads: %d\n Valor de N: %lld \n",nthreads, n);

 
    //--aloca as estruturas
    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {puts("ERRO--malloc"); return 2;}
 
    //--inicilaiza o mutex (lock de exclusao mutua)
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex0, NULL);

    clock_t start = clock();
 
    //--cria as threads
    for(long int t=0; t<nthreads; t++) {
      if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
        printf("--ERRO: pthread_create()\n"); exit(-1);
      }
    }
 
    //--espera todas as threads terminarem
    for (int t=0; t<nthreads; t++) {
      if (pthread_join(tid[t], NULL)) {
          printf("--ERRO: pthread_join() \n"); exit(-1); 
      } 
    } 
 
    //--finaliza o mutex
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex0);
    clock_t end = clock();
    float time = (float) (end - start) / CLOCKS_PER_SEC;

    printf("Qtd de primos encontrada = %lld\n", n_primos);
    printf("Tempo: %f\n", time);

    return 0;
 }