/*
Programa auxiliar para gerar um vetor de floats 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000 //valor maximo de um elemento do vetor
//descomentar o define abaixo caso deseje imprimir uma versao do vetor gerado no formato texto
//#define TEXTO 

int main(int argc, char*argv[]) {
   float *vetor0; //vetor que será gerada
   float *vetor1; //vetor que será gerada
   long int n; //dimensão dos vetores
   float elem; //valor gerado para incluir no vetor
   double prod_in = 0 ; //soma total dos elementos gerados
   int fator=1; //fator multiplicador para gerar números negativos
   FILE * descritorArquivo; //descritor do arquivo de saida
   size_t ret; //retorno da funcao de escrita no arquivo de saida

   //recebe os argumentos de entrada
   if(argc < 3) {
      fprintf(stderr, "Digite: %s <dimensao> <nome arquivo saida>\n", argv[0]);
      return 1;
   }
   n = atoi(argv[1]);

   //aloca memoria para os vetores
   vetor0 = (float*) malloc(sizeof(float) * n);
   if(!vetor0) {
      fprintf(stderr, "Erro de alocaçao da memoria do vetor\n");
      return 2;
   }

   vetor1 = (float*) malloc(sizeof(float) * n);
   if(!vetor1) {
      fprintf(stderr, "Erro de alocaçao da memoria do vetor\n");
      return 2;
   }

   //preenche os vetores com valores float aleatorios
   srand(time(NULL));

   for(long int i=0; i<n; i++) {
        elem = (rand() % MAX)/3.0 * fator;
        vetor0[i] = elem;
        fator*=-1;
   }

   for(long int i=0; i<n; i++) {
        elem = (rand() % MAX)/3.0 * fator;
        vetor1[i] = elem;
        fator*=-1;
   }
   
   for(long int i=0; i<n; i++) {
      double prod = vetor0[i]*vetor1[i];
      prod_in += prod;
 }


   //imprimir na saida padrao o vetor gerado
   #ifdef TEXTO
   fprintf(stdout, "%ld\n", n);
   for(long int i=0; i<n; i++) {
      fprintf(stdout, "%f ",vetor0[i]);
   }

   for(long int i=0; i<n; i++) {
      fprintf(stdout, "%f ",vetor1[i]);
   }

   fprintf(stdout, "\n");
   fprintf(stdout, "%lf\n",prod_in);
   #endif

   //escreve o vetor no arquivo
   //abre o arquivo para escrita binaria
   descritorArquivo = fopen(argv[2], "wb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }
   //escreve a dimensao
   ret = fwrite(&n, sizeof(long int), 1, descritorArquivo);
   //escreve os elementos do vetor
   ret = fwrite(vetor0, sizeof(float), n, descritorArquivo);
   if(ret < n) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
   }
   ret = fwrite(vetor1, sizeof(float), n, descritorArquivo);
   if(ret < n) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
   }
   //escreve o produto interno
   ret = fwrite(&prod_in, sizeof(double), 1, descritorArquivo);

   //finaliza o uso das variaveis
   fclose(descritorArquivo);
   free(vetor0);
   free(vetor1);
   return 0;
} 
