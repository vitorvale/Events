#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define __USE_XOPEN

#include <time.h>

#define MAX_SALAS 10
#define MAX_EVENTOS_SALA 100
#define MAX_PARTICIPANTES 3
#define MAX_NAMES_LENGTH 64
#define MAX_COMPONENTES 10


typedef struct evento {
    char descricao[MAX_NAMES_LENGTH];
    struct tm dataHora;
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

int getEventosDaSala(Evento eventos[], int numeroEventos, Evento eventosSala[MAX_EVENTOS_SALA], int sala);

void ordenaEventos(Evento eventos[], int minimo, int numeroEventos);

time_t getTimestamp(Evento evento);

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
		if (cmd == 'a' || cmd == 'r' || cmd == 't' || cmd == 's'){
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
				/* indices de 0 ate numero de eventos -1 */
				ordenaEventos(eventos, 0, numeroEventos - 1);
				listaEventos(eventos, numeroEventos);
				break;
			case 't' :
				eventoTmp = eventos[indexEvento];
				/* componentes[2] => nova duracao */
				eventos[indexEvento] = alteraDuracao(eventoTmp, componentes[2]);
				break;
			case 's':
				{
					Evento eventosSala[MAX_EVENTOS_SALA];
					int numeroEventosSala = getEventosDaSala(eventos, numeroEventos, eventosSala, atoi(componentes[1]));
					/* indices de 0 ate numero de eventos da sala -1 */
					ordenaEventos(eventosSala, 0, numeroEventosSala - 1);
					listaEventos(eventosSala, numeroEventosSala);
				}
				break;
		}

		/* limpa comando anterior */
		cmd = '\0';
	}

    return 0;
}

/* fazer printEvento(Evento evento)) no formato pedido */

void printEvento(Evento evento){
	int i, numParticipantes = 0;
	char data[9];
	char hora[5];

	strftime(data, sizeof(data), "%d%m%Y", &evento.dataHora);
	strftime(hora, sizeof(hora), "%H%M", &evento.dataHora);

	printf("%s %s %s %d Sala%d %s\n", evento.descricao, data, hora, evento.duracao, evento.sala, evento.responsavel);
	for (i = 0; i < 3; i++){
		/* Ignora participantes nao existentes */
		if (evento.participantes[i][0] == '\0') {
			break;
		}
		numParticipantes++;
		printf("[%s] ", evento.participantes[i]);
	}
	if(numParticipantes > 0) {
		printf("\n");
	}

}

void listaEventos(Evento eventos[], int numeroEventos) {
    int i;
    for(i = 0; i < numeroEventos; i++) {
		printEvento(eventos[i]);
    }
}

int getEventosDaSala(Evento eventos[], int numeroEventos, Evento eventosSala[MAX_EVENTOS_SALA], int sala) {
	int i, numeroEventosSala = 0;

    for(i = 0; i < numeroEventos; i++) {
		if(eventos[i].sala == sala) {
			eventosSala[numeroEventosSala++] = eventos[i];
		}
    }

	return numeroEventosSala;
}

time_t getTimestamp(Evento evento) {
	time_t epoch = mktime(&evento.dataHora);
	return epoch;
}

void ordenaEventos(Evento eventos[], int min, int numeroEventos) {
	int i = min, j = numeroEventos;
	Evento eventoTmp = eventos[(min + numeroEventos) / 2];
     
    while(i <= j) {
        while(getTimestamp(eventos[i]) < getTimestamp(eventoTmp) && i < numeroEventos) {
            i++;
        }
        while(getTimestamp(eventos[j]) > getTimestamp(eventoTmp) && j > min) {
            j--;
        }
        if(i <= j) {
			/* No caso de existirem vários eventos que se iniciem ao mesmo tempo, deverão ser listados pela ordem crescente do número da sala */
			if(getTimestamp(eventos[i]) != getTimestamp(eventos[j]) ||
					(getTimestamp(eventos[i]) == getTimestamp(eventos[j]) && eventos[i].sala > eventos[j].sala)) {
            	Evento eventoTmp2 = eventos[i];
	            eventos[i] = eventos[j];
	            eventos[j] = eventoTmp2;
			}

            i++;
            j--;
        }
    }
     
    if(j > min) {
        ordenaEventos(eventos, min, j);
    }
    if(i < numeroEventos) {
        ordenaEventos(eventos, i, numeroEventos);
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
	char dataHora[8 + 4 + 1];
   
    memset(evento.descricao, '\0', sizeof(evento.descricao));
    strcpy(evento.descricao, componentes[1]);

	sprintf(dataHora, "%s %s", componentes[2], componentes[3]);
	strptime(dataHora, "%d%m%Y %H%M", &evento.dataHora);

    evento.duracao = atoi(componentes[4]);
    evento.sala = atoi(componentes[5]);
	

    memset(evento.responsavel, '\0', sizeof(evento.responsavel));
    strcpy(evento.responsavel, componentes[6]);

   	memset(evento.participantes, '\0', sizeof(evento.participantes));
	for(i = 0; i < (numComp - 6); i++){
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







