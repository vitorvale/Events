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


int criaEvento(Evento eventos[], int numeroEventos, char componentes[MAX_COMPONENTES][MAX_NAMES_LENGTH]);

int procuraEvento(Evento eventos[], int numeroEventos, char descricaoEvento[]);

Evento alteraDuracao(Evento evento, char strDuracao[MAX_NAMES_LENGTH]);

int removeEvento(Evento eventos[], int numeroEventos, int indexEvento);

void listaEventos(Evento eventos[], int numeroEventos);


void parteString(char componentes[][MAX_NAMES_LENGTH], char params[]);

int main() {
	/* TODO: adicionar comentarios a explicar o que isto e */
    Evento eventos[MAX_SALAS * MAX_EVENTOS_SALA];
    int numeroEventos = 0;
	char params [342]; /* 63 + 8 + 4 + 4 + 2 + 63 * 4 + 9 (':') */
	/* Ex: a Reuniao dos Ze:29032019:1000:60:1:Ze:Ze Carlos:Ze Pedro:Ze Joao */

	char componentes[MAX_COMPONENTES][MAX_NAMES_LENGTH];
	char cmd;

	while (1){
		/* evento temporario manipulado pelos comandos do utilizador */
		Evento eventoTmp;
	    int numParam, indexEvento = -1;

		/* Ler comando */
		scanf("%c", &cmd);
		if (cmd == 'x') {
			break;
		}

		/* Ler restantes parametros, caso existam, e adiciona-los a um array de componentes */
		if (cmd == 'a' || cmd == 'r' || cmd == 't'){
			char c;
			int idx = 0;
			while((c = getchar()) != '\n') {
				if (idx == 0 && c == ' ') {
					continue;
				}
				params[idx++] = c;
			}
			params[idx] = '\0';

			/*
			printf("Command: [%c]\n", cmd);
			printf("Params: [%s]\n", params);
			*/

			/* Partir a string, caso exista, nos diversos componentes */
			parteString(componentes, params);
			numParam = atoi(componentes[0]);
		}

		if (cmd == 'r' || cmd == 't') {
			char descricaoEvento[MAX_NAMES_LENGTH];
			strcpy(descricaoEvento, componentes[1]);
			if(numParam <= 0) {
				/* TODO: adicionar a mensagem de erro correcta para este caso */
				printf("Parametros invalidos.\n");
                continue;
			}

			indexEvento = procuraEvento(eventos, numeroEventos, descricaoEvento);
			if(indexEvento < 0) {
				continue;
			}
		}

		switch(cmd) {
			case 'a' :
				if (numParam >= 6){
					numeroEventos = criaEvento(eventos, numeroEventos, componentes);
				}
				break;
			case 'r':
				if (numParam == 1){
					numeroEventos = removeEvento(eventos, numeroEventos, indexEvento);
				}
				break;
			case 'l' :
				listaEventos(eventos, numeroEventos);
				break;
			case 't' :
				eventoTmp = eventos[indexEvento];
				/* componentes[2] => nova duracao */
				eventos[indexEvento] = alteraDuracao(eventoTmp, componentes[2]);
				break;
		}

		/* limpa comando anterior */
		cmd = '\0';
	}

    return 0;
}

/* fazer printEvento(Evento evento)) no formato pedido */

void printEvento(Evento evento){
	int i;
	printf("%s %s %s %d Sala%d %s\n", evento.descricao, evento.data, evento.hora, evento.duracao, evento.sala, evento.responsavel);
	for (i = 0; i < 3; i++){
		/* Ignora participantes nao existentes */
		if (evento.participantes[i][0] == '\0') {
			break;
		}

		printf("[%s] ", evento.participantes[i]);
	}
	printf("\n");

}

void listaEventos(Evento eventos[], int numeroEventos) {
    int i;
    for(i = 0; i < numeroEventos; i++) {
		printEvento(eventos[i]);
    }
}

int procuraEvento(Evento eventos[], int numeroEventos,char descricaoEvento[MAX_NAMES_LENGTH]) {
	int i;

	/* TODO: remover esta linha de debug */
	printf("Descricao evento: [%s]\n", descricaoEvento);

    for(i = 0; i < numeroEventos; i++) {
		Evento evento = eventos[i];
        if(strcmp(descricaoEvento, evento.descricao) == 0) {
			return i;
		}
    }

	printf("Evento %s inexistente.\n", descricaoEvento);
	return -1;
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
    	memset(evento.participantes[i], '\0', sizeof(evento.participantes[i]));
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

	/*
	for ( i = 0; i < compIdx; i++) {
		printf("Componente [%d]: [%s]\n", i, componentes[i]);
	}
	*/
}


int removeEvento(Evento eventos[], int numeroEventos, int indexEvento){
	int i;

	for (i = 0; i < numeroEventos - 1; i++){
		if (i >= indexEvento) {
			/* a partir do indice que queremos remover, copia o proximo indice para o indice actual */
			eventos[i] = eventos[i + 1];
		}
	}
	eventos[numeroEventos--].descricao[0] = '\0';

	return numeroEventos;
}

Evento alteraDuracao(Evento evento, char strDuracao[MAX_NAMES_LENGTH]){
	evento.duracao = atoi(strDuracao);
	/* TODO: remover esta linha de debug */
	printf("nova duracao : %d\n", evento.duracao);

	return evento;
}







