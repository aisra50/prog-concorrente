/* Disciplina: Programação Concorrente */
/* Profa.: Silvana Rossetto */
/* Aluna.: Raisa Christina Nascimento Gonçalves */
/* Lab2:  */
/* Codigo: Calcula o produto interno de dois vetores*/

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h> 
#include <math.h>

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h> 
#include <time.h>

//variaveis globais
//tamanho do vetor
long int n;
//vetor de elementos
float *vet0;
float *vet1;
double prod_int;
double prod_int_resp;
//numero de threads
int nthreads;
size_t ret; //retorno da funcao de leitura no arquivo de saida

pthread_t *vthreads_id;  // vetor de identificadores de threads

void *calc_prod(void *nt){
    int start;
    int step;
    double prod_in_parc;
    double *ret;
    
    int q = n / nthreads;
    int r = n % nthreads;

    long int nthread = (long int) nt;

    if(nthread < r) {  // primeiros r threads carregam q+1 posições do vetor
        step = q + 1;
    }
    else {
        step = q;
    }

    if (nthread <= r){
        start = nthread*(q+1);
    }
    else {
        start = (q+1)*r + (nthread-r)*q;  // threads depois das r primeiras tem sua posição de inicio calculadas considerando as demais
    }
    
    for(int i = start; i< start+step; i++ ) {
        double prod = vet0[i]*vet1[i];
        prod_in_parc += prod;
    }

    ret = malloc(sizeof(double)); // alocar espaço para ponteiro para retorno
    *ret = prod_in_parc;

    pthread_exit( (void*) ret);
}



int main(int argc, char*argv[]) {

    FILE * descritorArquivo; //descritor do arquivo de entrada

    // lendo argumentos de entrada
    if(argc < 3) {
        fprintf(stderr, "Digite: %s <nthreads> <nome arquivo entrada>\n", argv[0]);
        return 1;
     }
    nthreads = atoi(argv[1]);
    descritorArquivo = fopen(argv[2], "rb");
    
    if(!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }
    
    //lendo arquivo de entrada 

    ret = fread(&n, sizeof(long int), 1, descritorArquivo);
    
    vet0 = malloc(sizeof(float) * n);
    ret = fread(vet0, sizeof(float), n, descritorArquivo);
    if(ret < n) {
       fprintf(stderr, "Erro de leitura no  arquivo\n");
       return 4;
    }

    if(nthreads>n) nthreads = n; // limita n de threads

    vet1 = malloc(sizeof(float) * n);
    ret = fread(vet1, sizeof(float), n, descritorArquivo);
    if(ret < n) {
       fprintf(stderr, "Erro de leitura no  arquivo\n");
       return 4;
    }

    ret = fread(&prod_int_resp, sizeof(long int), 1, descritorArquivo);

    vthreads_id = (pthread_t *) malloc(sizeof(pthread_t)* nthreads); 


    clock_t start = clock();

    //criando threads
    for(long int nt = 0; nt < nthreads; nt ++){
        if (pthread_create(&vthreads_id[nt], NULL, calc_prod, (void*) nt)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
         }
    }


    //unindo resultados 
    double *prod_thread;  

    for(long int nt = 0; nt < nthreads; nt ++){

        if (pthread_join(vthreads_id[nt], (void *) &prod_thread)) {
            printf("--ERRO: pthread_join()\n"); exit(-1);
         }

        prod_int += *prod_thread;
        free(prod_thread);
    }
    
    clock_t end = clock();
    float time = (float) (end - start) / CLOCKS_PER_SEC;

    printf("%d THREADS:\n", nthreads);
    printf("Valor calculado: %lf\n", prod_int);
    printf("Valor resposta: %lf\n", prod_int_resp);
    
    double var = fabs(prod_int - prod_int_resp)/prod_int_resp;
    printf("Variação relativa: %lf\n", var);
    
    printf("Tempo: %f\n", time);


    free(vet0);
    free(vet1);
    fclose(descritorArquivo);
}