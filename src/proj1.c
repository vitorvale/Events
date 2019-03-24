#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_SALAS 10
#define MAX_EVENTOS_SALA 100
#define MAX_PARTICIPANTES 3
#define MAX_NAMES_LENGTH 64
#define MAX_COMPONENTES 10

/*    char data[9]; */
/*    char hora[5]; */

typedef struct {
    char descricao[MAX_NAMES_LENGTH];
    struct tm data;
    int duracao;
    int sala;
    char responsavel[MAX_NAMES_LENGTH];
    char participantes[(MAX_NAMES_LENGTH * 3) + 3];
} Evento;

/* TODO: ver se isto faz sentido */
typedef struct {
    Evento eventos[100];
} Sala;

void listaEventos(Evento eventos[], int numeroEventos);
int criaEvento(Evento eventos[], int numeroEventos, char camposEvento[]);
void parteString(char componentes[][MAX_NAMES_LENGTH], char params[]);
char lerComando(char params[]);

int main() {
    char c;
	int idx;

	/* TODO: adicionar comentarios a explicar o que isto e */
    Evento eventos[MAX_SALAS * MAX_EVENTOS_SALA];
    int numeroEventos = 0;
	char params [342]; /* 63 + 8 + 4 + 4 + 2 + 63 * 4 + 9 (':') */
	/* Ex: a Reuniao dos Ze:29032019:1000:60:1:Ze:Ze Carlos:Ze Pedro:Ze Joao */

	char componentes[MAX_COMPONENTES][MAX_NAMES_LENGTH];
	char cmd;

	/* Ler comando */
	scanf("%c", &cmd);

	/* Ler restantes parametros, caso existam, para uma unica string */
	idx = 0;
	while((c = getchar()) != '\n') {
		if (idx == 0 && c == ' ') {
			continue;
		}
		params[idx++] = c;
	}
	params[idx] = '\0';

	printf("Command: [%c]\n", cmd);
	printf("Params: [%s]\n", params);

	/* Partir a string, caso exista, nos diversos componentes */
	parteString(componentes, params);

/*
	switch(cmd) {
		case 'a': numeroEventos = criaEvento(eventos, numeroEventos, argv[][]);
	}
*/

    numeroEventos = criaEvento(eventos, numeroEventos, "");
    numeroEventos = criaEvento(eventos, numeroEventos, "");
    numeroEventos = criaEvento(eventos, numeroEventos, "");

    listaEventos(eventos, numeroEventos);

    return 0;
}

void listaEventos(Evento eventos[], int numeroEventos) {
    int i;
    for(i = 0; i < numeroEventos; i++) {
        Evento evento = eventos[i];
        printf("evento[%d]: %s\n", i, evento.descricao);
    }
}

int criaEvento(Evento eventos[], int numeroEventos, char camposEvento[]) {
    Evento evento;
   
    memset(evento.descricao, '\0', sizeof(evento.descricao));
    sprintf(evento.descricao, "Evento numero %d", numeroEventos);

    evento.data = *localtime(&(time_t){time(NULL)});
    evento.duracao = 1;
    evento.sala = 1;

    memset(evento.responsavel, '\0', sizeof(evento.responsavel));
    strcpy(evento.responsavel, "Vitor Vale");

    memset(evento.participantes, '\0', sizeof(evento.participantes));
    strcpy(evento.participantes, "Participante 1:Participante 2");

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
		printf("Componente [%d]: [%s]\n", i, componentes[i]);
	}
}

