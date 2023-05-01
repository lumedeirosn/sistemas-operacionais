#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct st_no{
	char id[5];
	int tempo;
	struct st_no *prox;
}No;

typedef struct st_fila{
	No *ini;
	No *fim;
}Fila;

typedef struct st_dados{
	char id[5];
	int tempo;
}Dados;

/* implementacao da estrutura de dados*/
Fila * criar_fila();
void enqueue(Fila* fila, char id[5], double tempo);
Dados dequeue(Fila *fila);
void imprimir(Fila *fila);
void apagar(Fila *fila);

/* funcoes para o escalonamento*/
void carregar_dados(Fila *fila); /* Carrega os dados do arquivo numa fila passada como parametro para a funcao */
void escalonar_CPU(Fila *fila);  /* escalona a CPU na logica do round robin com a ajuda das fuções auxiliares */
Fila *carregar_fila(Fila *fila); /* carrega uma fila de processos que devem ser executados */
int executar_processo(int quantum, No *processo); /* executa um processo e retorna o tempo que deve ser decrementado de sua demanda total para finalizacao */
void decrementar_tempo(Fila *fila, char id[5] , int num); /* decrementa do tempo de demanda total, o tempo que o processo passou em execucao conforme o quantum*/


int main(int argc ,char *args[]){
	system("clear");
	Fila *fila = criar_fila();
	carregar_dados(fila);
	escalonar_CPU(fila);
	apagar(fila); /* devolve a memoria alocada, uma vez que todos os processos de entrada foram escalonados e finalizados com sucesso */
	return 0;
}

Fila * criar_fila(){
	Fila *fila = (Fila*)malloc(sizeof(Fila));
	if(!fila) return NULL;
	fila->ini = NULL;
	fila->fim = NULL;
	return fila;
}

void enqueue(Fila* fila, char id[5], double tempo){
	No *novo = (No*)malloc(sizeof(No));
	if(!novo) return;
	strcpy(novo->id, id);
	novo->tempo = tempo;
	novo->prox = NULL;

	if(!fila->ini) fila->ini = fila->fim = novo;
	else{
		fila->fim->prox = novo;
		fila->fim = novo;
	}
}

Dados dequeue(Fila *fila){
	if(!fila->ini){
		printf("Fila vazia, impossivel fazer dequeue()...\n");
		exit(1);
	}

	Dados dados;
	strcpy(dados.id, fila->ini->id);
	dados.tempo = fila->ini->tempo;
	No *aux = fila->ini;
	fila->ini = aux->prox;
	free(aux);

	return dados;
}

void imprimir(Fila *fila){
	printf("Inicio-> [\n");
	No*aux = fila->ini;
	while(aux){
		printf("(ID-> %s, tempo-> %ds)\n", aux->id, aux->tempo);
		aux = aux->prox;
	}
	printf("]\n");
}

void apagar(Fila *fila){
	No* aux = fila->ini, *ant;
	while(aux){
		ant = aux;
		aux = aux->prox;
		free(ant);
	}
	fila->ini = fila->fim = NULL;
}

void carregar_dados(Fila *fila){
	FILE *arq = fopen("info_processos.txt", "rt");
	char linha[30], *continua, *id, *tempo;
	Dados info_processo;

	if(arq){
		while(!feof(arq)){
			continua = fgets(linha, 30, arq);
			if(continua){
				id = strtok(linha, ";");
				tempo = strtok(NULL, ";");
				enqueue(fila, id, atoi(tempo));
			}
		}
	}
	else{
		printf("Erro ao tentar abrir o arquivo...\n");
		exit(1);
	}
	
	fclose(arq);
}

void escalonar_CPU(Fila *fila){
	/* a função conclui quando a demanda de tempo atual de todos  os processos for 0, o que indica que todos os processos finalizaram */
	int quantum = 5;/* considere um Quantum de 5s*/
	int alternancia = quantum * 0.2; /* tempo de alternancia*/
	int decremento; /* tempo a ser decrementado de demanda total apos a execucao */
	char anterior[5]; /* processo executado anteriormente */
	strcpy(anterior, "X");
	
	Fila *fila_execucao = carregar_fila(fila);
	do{
		
		while(fila_execucao->ini){
			if(strcmp(anterior, fila_execucao->ini->id)){ /* so carrega registradores e memoria se ocorre a alternancia para um novo processo na cpu */
				printf("\nSelecionando novo processo...\nCarregando registradores....\nSalvando conteudo dos registradores....\nCarregando mapa de memoria....\nCarregando cache...\n");
				sleep(alternancia);
			}
			decremento = executar_processo(quantum, fila_execucao->ini);
			strcpy(anterior, fila_execucao->ini->id);
			decrementar_tempo(fila, fila_execucao->ini->id, decremento);
			dequeue(fila_execucao);
		}
		apagar(fila_execucao);
		fila_execucao = carregar_fila(fila);
	
	}while(fila_execucao->ini);

	printf("\nProcessos computados e finalizados com sucesso!\n");
}

Fila *carregar_fila(Fila *fila){
	/*	- com base na fila principal de processos presente no arquivo, carrega uma fila de execucao com base na demanda atual dos mesmos;
		- se o tempo de demanda atual do processo for 0(o processo finalizou), a função não aloca o processo na fila de execucao;
	*/
	Fila *retorno = criar_fila();
	No *aux = fila->ini;
	while(aux){
		if(aux->tempo) enqueue(retorno, aux->id, aux->tempo);
		aux = aux->prox;
	}
	return retorno;
}

int executar_processo(int quantum, No *processo){
	/* demanda total == tempo total que o processo leva para ser finalizado */
	/* demanda atual == tempo restante que o processo precisa para finalizar */
	int cont = 0; /* incrementa no maximo até 5 por conta do tempo limite; mas pode incrementar menos dependendo da demanda atual,ex: a demanda atual é 3, ent ele so vai incrementar ate 3 */
	
	printf("\nProcesso em execucao...\n");
	printf("---------------------\n");
	printf("ID->            %s\nDemanda atual-> %ds\n", processo->id, processo->tempo);
	printf("---------------------\n");

	while(processo->tempo > 0 && quantum != 0){
		processo->tempo--;
		quantum--;
		cont++;
	}
	sleep(cont);
	
	
	printf("\nTomei a CPU do processo-> %s\n", processo->id);
	sleep(2); /* necessario para visualizar melhor a sequencia de mensagens na saida*/
	
	if(processo->tempo == 0){
		printf("\nO processo %s finalizou...\n", processo->id);
			sleep(2); /* necessario para visualizar melhor as mensagens na saida*/
	}
	
	return cont;
}

void decrementar_tempo(Fila *fila, char id[5] , int num){
	No *aux = fila->ini;
	while(aux){
		if(!strcmp(aux->id, id)){
			aux->tempo -= num;
		}
		aux = aux->prox;
	}
}


