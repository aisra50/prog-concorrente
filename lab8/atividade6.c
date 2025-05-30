#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

int* buffer;
int itens_consumidos = 0;
int producao_finalizada = 0;  // Flag para evitar q rode infinitamente
int total_primos = 0;       

pthread_mutex_t mutex_contador;
pthread_mutex_t mutex_primos;  

typedef struct {
    int thread_id;
    int primos_encontrados;
} resultado_thread;

resultado_thread* resultados;  // Array para guardar resultados por thread

sem_t slotVazio;    // Conta slots vazios no buffer
sem_t slotCheio;    // Conta slots ocupados no buffer
sem_t mutexProd;    // Mutex para produtores
sem_t mutexCons;    // Mutex para consumidores

int ehPrimo(long long int n) {
    int i;
    if (n<=1) return 0;
    if (n==2) return 1;
    if (n%2==0) return 0;
    for (i=3; i<sqrt(n)+1; i+=2)
        if(n%i==0) return 0;
    return 1;
}

void printBuffer(int buffer[], int tam) {
    for(int i=0;i<tam;i++) 
       printf("%d ", buffer[i]); 
    puts("");
}

// Função das threads consumidoras
int retira_avalia(int M) {
    sem_wait(&slotCheio);   // Aguarda slot cheio
    sem_wait(&mutexCons);

    static int out=0;
    int int_retirado = buffer[out];
    buffer[out] = 0;
    out = (out + 1) % M;

    sem_post(&mutexCons);   // libera mutex
    sem_post(&slotVazio); 

    // incremento da contagem após retirada de item
    pthread_mutex_lock(&mutex_contador);
    itens_consumidos++;
    pthread_mutex_unlock(&mutex_contador);

    if (int_retirado == 0) {
        printf("\nretirou zero");
    }

    int eh_primo = ehPrimo(int_retirado);
    printf("%d - %d\n", int_retirado, eh_primo);
    return eh_primo;
}   

// consumidora
void *consumidor(void *arg) {
    int *params = (int*)arg;
    int N = params[0];
    int M = params[1];
    int id = params[2];

    int count_primos = 0;
    int continuar = 1;

    while (continuar) {
        // checa se os n ints ja foram consumidos
        pthread_mutex_lock(&mutex_contador);
        if (itens_consumidos >= N) {
            continuar = 0;
            pthread_mutex_unlock(&mutex_contador);
            break;
        }
        pthread_mutex_unlock(&mutex_contador);
        
        
        int valor;
        sem_getvalue(&slotCheio, &valor); // verifica se há itens disponíveis para consumo no slot pelo semaforo
        
        if (valor > 0) {
            // Há itens para consumir
            int resultado = retira_avalia(M);
            if (resultado) {
                count_primos++;
            }
            printf("pthread %d leu\n", id);
        } 
        else if (producao_finalizada) {
            // Se não há itens E produtor terminou, sai do loop
            continuar = 0;
        }
        else {
            // Espera um pouco para tentar novamente
            usleep(1000);  // 1 ms
        }
    }
    
    // Armazena o resultado desta thread
    resultados[id-1].thread_id = id;
    resultados[id-1].primos_encontrados = count_primos;
    
    // Atualiza o total global de primos
    pthread_mutex_lock(&mutex_primos);
    total_primos += count_primos;
    pthread_mutex_unlock(&mutex_primos);
    
    printf("primos da thread %d: %d\n", id, count_primos);
    pthread_exit(NULL);
}

// produtora
void *produtor(void *arg) {
    int *params = (int*)arg;
    int N = params[0];
    int M = params[1];
    int id = params[2];

    int int_gerado;
    int in = 0;

    for(int i = 0; i < N; i++) {
        int_gerado = rand() % 100;

        sem_wait(&slotVazio);
        sem_wait(&mutexProd); 
        
        buffer[in] = int_gerado;
        in = (in + 1) % M;
        
        sem_post(&mutexProd);
        sem_post(&slotCheio); 
    }
    
    // sinaliza que a produção acabou
    producao_finalizada = 1;
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t *tid; //identificadores das threads no sistema
    int N, M, C;

    srand(time(NULL));
 
    // parametros de entrada
    if(argc < 4) {
        printf("Digite: %s <N: Tamanho da sequência> <M: Tamanho do vetor> <C: N de threads consumidoras>\n", argv[0]);
        return 1;
    }
    N = atoi(argv[1]);
    M = atoi(argv[2]);
    C = atoi(argv[3]);
    printf("N:%d M:%d C:%d \n \n", N, M, C);

    // inicializa o array de resultados
    resultados = malloc(C * sizeof(resultado_thread));
    for (int i = 0; i < C; i++) {
        resultados[i].thread_id = i+1;
        resultados[i].primos_encontrados = 0;
    }

    buffer = (int*)malloc(M * sizeof(int));
    for (int i = 0; i < M; i++) buffer[i] = 0;  

    pthread_mutex_init(&mutex_contador, NULL);
    pthread_mutex_init(&mutex_primos, NULL);
    
    // semáforos
    sem_init(&slotVazio, 0, M);     // M slots vazios inicialmente
    sem_init(&slotCheio, 0, 0);     // 0 slots cheios inicialmente
    sem_init(&mutexProd, 0, 1);     // Mutex para produtores
    sem_init(&mutexCons, 0, 1);     // Mutex para consumidores
    
    //threads
    tid = (pthread_t*)malloc((1 + C) * sizeof(pthread_t));
    
    //cria produtora
    int *params_prod = malloc(3 * sizeof(int));
    params_prod[0] = N;  
    params_prod[1] = M;
    params_prod[2] = 0;  
    pthread_create(&tid[0], NULL, produtor, params_prod);
    
    // Cria threads consumidoras
    int **params_array = malloc(C * sizeof(int*));
    for (int i = 0; i < C; i++) {
        params_array[i] = malloc(3 * sizeof(int));
        params_array[i][0] = N;  
        params_array[i][1] = M;
        params_array[i][2] = i + 1;  // ID do consumidor
        pthread_create(&tid[1 + i], NULL, consumidor, params_array[i]);
    }
    
    // Aguarda todas as threads
    for (int i = 0; i < 1 + C; i++) {
        pthread_join(tid[i], NULL);
    }
    
    // Encontra a thread que achou mais primos
    int max_primos = -1;
    int thread_campeao = -1;
    
    for (int i = 0; i < C; i++) {
        if (resultados[i].primos_encontrados > max_primos) {
            max_primos = resultados[i].primos_encontrados;
            thread_campeao = resultados[i].thread_id;
        }
    }
    
    printf("\n=== RESUMO ===\n");
    printf("Total de números primos encontrados: %d\n", total_primos);
    printf("Thread campeã: %d com %d primos encontrados\n", thread_campeao, max_primos);
    
    printf("\nBuffer final:\n");
    printBuffer(buffer, M);
    
//////////////////////////
    free(buffer);
    free(tid);
    free(params_prod);
    
    for (int i = 0; i < C; i++) {
        free(params_array[i]);
    }
    free(params_array);
    free(resultados);
    
    pthread_mutex_destroy(&mutex_contador);
    pthread_mutex_destroy(&mutex_primos);
    sem_destroy(&slotVazio);
    sem_destroy(&slotCheio);
    sem_destroy(&mutexProd);
    sem_destroy(&mutexCons);
    
    return 0;
}