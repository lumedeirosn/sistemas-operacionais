#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX 1000000000
pthread_mutex_t the_mutex;
pthread_cond_t condc, condp;

int buffer[10];
int pos = 0;

int qtd_itens_buffer(){
	return pos;
}

void inserir_item(int item){
	if(qtd_itens_buffer() < 10){
		buffer[pos++] = item;
		return;	
	}
	printf("buffer cheio, erro ao inserir\n");
	exit(1);
}

int remover_item(){
	if(qtd_itens_buffer() > 0) return buffer[(pos--)-1];
	printf("buffer vazio, erro ao remover\n");
	exit(1);
}


const clock_t MAXDELAY = 2000000;

void delay(clock_t ticks) { /* a "busy" delay */
  clock_t start = clock();
  do
    ; while (clock() < start + ticks);
}

void *producer(void *ptr) {
    int cont = 1; /* variavel que contem o item a ser inserido no buffer */
    for (int i = 1; i <= MAX; i++) {
        pthread_mutex_lock(&the_mutex); /* obtem acesso exclusivo ao buffer */
		while (qtd_itens_buffer() == 10) /* enquanto buffer cheio */
            	pthread_cond_wait(&condp, &the_mutex); /* produtor dorme */

		int num = rand() % 10; /* itens a serem inseridos */
		while(num-- && qtd_itens_buffer() < 10){
			printf("|COLOCANDO| item %d no buffer\n", cont);
			inserir_item(cont++); /*coloca item no buffer */
        	delay(MAXDELAY/2);
		}

        pthread_cond_signal(&condc); /* acorda consumidor */
        pthread_mutex_unlock(&the_mutex);/* libera acesso ao buffer */
    }

    pthread_exit(0);
}

void *consumer(void *ptr) {
    for (int i = 1; i <= MAX; i++) {
        pthread_mutex_lock(&the_mutex); /* obtem acesso exclusivo ao buffer */
		while (qtd_itens_buffer() == 0) /* enquanto buffer vazio */
				pthread_cond_wait(&condc, &the_mutex); /* consumidor dorme */
        
		int num = rand() % 10; /* itens a serem consumidos */
		while(num-- && qtd_itens_buffer()){
			printf("|RETIRANDO| item %d no buffer\n", remover_item()); /* retira o item do buffer */
        	delay(MAXDELAY/2);
		}
		
		/* se restarem itens no buffer ao final do código, os itens sao consumidos no while abaixo */
		while(i == MAX && qtd_itens_buffer()){
			printf("|RETIRANDO| item %d no buffer\n", remover_item()); /* retira o item do buffer */
        	delay(MAXDELAY/2);			
		}		
			
        pthread_cond_signal(&condp); /* acorda o produtor */
        pthread_mutex_unlock(&the_mutex);/* libera acesso ao buffer */
	}

    pthread_exit(0);
}

int main(int argc, char **argv)
{	
    pthread_t pro, con;
    pthread_mutex_init(&the_mutex, 0);
    pthread_cond_init(&condc, 0);
    pthread_cond_init(&condp, 0);
    pthread_create(&con, 0, consumer, 0);
    pthread_create(&pro, 0, producer, 0);
    pthread_join(pro, 0);
    pthread_join(con, 0);
    pthread_cond_destroy(&condc);
    pthread_cond_destroy(&condp);
    pthread_mutex_destroy(&the_mutex);
}
