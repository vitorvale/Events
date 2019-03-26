#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SALAS 10
#define MAX_EVENTOS_SALA 100
#define MAX_PARTICIPANTES 3
#define MAX_NAMES_LENGTH 64
#define MAX_COMPONENTES 10


typedef struct {
    char descricao[MAX_NAMES_LENGTH];
    /*struct tm data;*/
	char data[9]; 
	char hora[5];
    int duracao;
    int sala;
    char responsavel[MAX_NAMES_LENGTH];
    char participantes[3][MAX_NAMES_LENGTH];
} Evento;

void alteraDuracao(Evento eventos[], int numeroEventos, char strDescricao[MAX_NAMES_LENGTH], char strDuracao[MAX_NAMES_LENGTH]);
void listaEventos(Evento eventos[], int numeroEventos);
int criaEvento(Evento eventos[], int numeroEventos, char componentes[MAX_COMPONENTES][MAX_NAMES_LENGTH]);
void parteString(char componentes[][MAX_NAMES_LENGTH], char params[]);
char lerComando(char params[]);
int removeEvento(Evento eventos[],int numeroEventos,char str_descricao[MAX_NAMES_LENGTH]);

int main() {
    char c;
	int idx, numParam;

	/* TODO: adicionar comentarios a explicar o que isto e */
    Evento eventos[MAX_SALAS * MAX_EVENTOS_SALA];
    int numeroEventos = 0;
	char params [342]; /* 63 + 8 + 4 + 4 + 2 + 63 * 4 + 9 (':') */
	/* Ex: a Reuniao dos Ze:29032019:1000:60:1:Ze:Ze Carlos:Ze Pedro:Ze Joao */

	char componentes[MAX_COMPONENTES][MAX_NAMES_LENGTH];
	char cmd;

	while (cmd != 'x'){
		/* Ler comando */
		scanf("%c", &cmd);

		/* Ler restantes parametros, caso existam, e adiciona-los a um array de componentes */
		if (cmd == 'a' || cmd == 'r' || cmd == 't'){
			idx = 0;
			while((c = getchar()) != '\n') {
				if (idx == 0 && c == ' ') {
					continue;
				}
				params[idx++] = c;
			}
			params[idx] = '\0';

			/*printf("Command: [%c]\n", cmd);*/
			/*printf("Params: [%s]\n", params);*/

			/* Partir a string, caso exista, nos diversos componentes */
			parteString(componentes, params);
			numParam = atoi(componentes[0]);
		}

		switch(cmd) {
			case 'a' : if (numParam >= 6){
				numeroEventos = criaEvento(eventos, numeroEventos, componentes);
			}
			break;
			case 'r': if (numParam == 1){
				numeroEventos = removeEvento(eventos, numeroEventos, componentes[1]);
			}
			break;
			case 'l' : listaEventos(eventos, numeroEventos);
			break;
			case 't' :{
				alteraDuracao(eventos, numeroEventos, componentes[1], componentes[2]);
			}
			break;
		}

	}

    return 0;
}

/* fazer printEvento(Evento evento)) no formato do stor*/

void printEvento(Evento evento){
	int i;
	printf("%s %s %s %d Sala%d %s\n", evento.descricao, evento.data, evento.hora, evento.duracao, evento.sala, evento.responsavel);
	for (i = 0; i < 3; i++){
		printf("%s ", evento.participantes[i]);
	}
	printf("\n");

}

void listaEventos(Evento eventos[], int numeroEventos) {
    int i;
    for(i = 0; i < numeroEventos; i++) {
		/*printEvento(eventos[i]);*/
		printEvento(eventos[i]);
        /*Evento evento = eventos[i];*/
        /*printf("evento[%d]: %s\n", i, evento.descricao);*/
    }
}

int criaEvento(Evento eventos[], int numeroEventos,char componentes[MAX_COMPONENTES][MAX_NAMES_LENGTH]) {
    Evento evento;
	int i, numComp = atoi(componentes[0]);
   
    memset(evento.descricao, '\0', sizeof(evento.descricao));
    strcpy(evento.descricao, componentes[1]);

    /*evento.data = *localtime(&(time_t){time(NULL)});*/
	memset(evento.data, '\0', sizeof(evento.data));
	strcpy(evento.data,componentes[2]);
	memset(evento.hora, '\0', sizeof(evento.hora));
	strcpy(evento.hora,componentes[3]);
    evento.duracao = atoi(componentes[4]);
    evento.sala = atoi(componentes[5]);
	

    memset(evento.responsavel, '\0', sizeof(evento.responsavel));
    strcpy(evento.responsavel, componentes[6]);

	for(i = 0; i < (numComp - 6); i++){
    	memset(evento.participantes[i], ' ', sizeof(evento.participantes[i]));
    	strcpy(evento.participantes[i], componentes[7 + i]);
    }
    eventos[numeroEventos++] = evento;

    return numeroEventos;
}

void parteString(char componentes[][MAX_NAMES_LENGTH], char params[]) {
    int i = 0, idx = 0, compIdx = 1;
	char c;
	char componente[MAX_NAMES_LENGTH];

	/* Reuniao dos Ze:29032019:1000:60:1:Ze:Ze Carlos:Ze Pedro:Ze Joao */
	while ((c = params[i++]) != '\0') {
		if(c == ':') {
			componente[idx] = '\0';
			strcpy(componentes[compIdx++], componente);
			memset(componente, '\0', sizeof(componente));
			idx = 0;
		} else {
			componente[idx++] = c;
		}
	}
	/* adicionar ultimo elemento */
	strcpy(componentes[compIdx++], componente);

	/* Adicionar o numero de componentes na primeira posicao */
	sprintf(componentes[0], "%d", (compIdx - 1));

	for ( i = 0; i < compIdx; i++) {
		/*printf("Componente [%d]: [%s]\n", i, componentes[i]);*/
	}
}


int removeEvento(Evento eventos[],int numeroEventos,char strDescricao[MAX_NAMES_LENGTH]){
	int i, e = 0, numEventosAux; 
	Evento eventos_aux[MAX_SALAS * MAX_EVENTOS_SALA];

	numEventosAux = numeroEventos; 
	for (i = 0; i < numeroEventos; i++){
		if (strcmp(eventos[i].descricao, strDescricao) == 0){
			numEventosAux--;
		}
		else{
			eventos_aux[e++] = eventos[i];
		}
	}

	/* Encontrou um evento com uma descricao igual */
	if (numeroEventos != numEventosAux){
		for (i = 0; i < numEventosAux; i++){
			eventos[i] = eventos_aux[i];
		}
		return numEventosAux;
	}	

	printf("Evento %s inexistente.\n", strDescricao);
	return numeroEventos;
}

void alteraDuracao(Evento eventos[], int numeroEventos, char strDescricao[MAX_NAMES_LENGTH], char strDuracao[MAX_NAMES_LENGTH]){
	int i, cont = 0;
	printf("strDescricao = %s\n", strDescricao);
	for (i = 0; i < numeroEventos; i++){
		if (strcmp(strDescricao, eventos[i].descricao) == 0){
			eventos[i].duracao = atoi(strDuracao);
			printf("nova duracao : %d\n", eventos[i].duracao);
			/*Ativa a variavel de modo a que o erro nao ocorra*/
			cont++;
		}
	}
	if (cont == 0){
		printf("Evento %s inexistente.\n", strDescricao);
	}
}







