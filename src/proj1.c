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
#define MAX_EVENTOS_DECORRER 10000


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

void alteraDuracao(Evento eventos[], int indexEvento, int numeroEventos, int duracao);

int removeEvento(Evento eventos[], int numeroEventos, int indexEvento);

void listaEventos(Evento eventos[], int numeroEventos);

int getEventosDaSala(Evento eventos[], int numeroEventos, Evento eventosSala[MAX_EVENTOS_SALA], int sala);

void ordenaEventos(Evento eventos[], int minimo, int numeroEventos);

long getTimestamp(Evento evento);

void parteString(char componentes[][MAX_NAMES_LENGTH], char params[]);

void mudaSala(Evento eventos[], int indexEvento, int numeroEventos, int novaSala);

void adicionaParticipante(Evento eventos[], int indexEvento,int numeroEventos, char participante[MAX_NAMES_LENGTH]);

void removeParticipante(Evento eventos[], int indexEvento, char participante[MAX_NAMES_LENGTH]);

int verificaSala(Evento eventos[], int numeroEventos,int indexEvento, long timestamp, int duracao, int novaSala);

void alteraInicio(Evento eventos[], int indexEvento, int numeroEventos, char hora[]);

int getEventosDecorrer(Evento eventos[],int numeroEventos, int indexEvento,Evento eventosDecorrer[], long timestamp, int duracao);

int verificaPessoa(Evento eventosDecorrer[], int idxEventos, char pessoa[]);

int comparaTimestamps(long timestampEvent1, int duracao1, long timestampEvent2, int duracao2);

/* TODO: remove this */
void printTimestamp(time_t epoch);

int main() {
	/* TODO: adicionar comentarios a explicar o que isto e */
    Evento eventos[MAX_SALAS * MAX_EVENTOS_SALA];
    int numeroEventos = 0;
	char params [342]; /* 63 + 8 + 4 + 4 + 2 + 63 * 4 + 9 (':') */
	/* Ex: a Reuniao dos Ze:29032019:1000:60:1:Ze:Ze Carlos:Ze Pedro:Ze Joao */

	char componentes[MAX_COMPONENTES][MAX_NAMES_LENGTH];
	char cmd;

	while (1) {
	    int numParam, indexEvento = -1;

		/* Ler comando */
		scanf("%c", &cmd);
		if (cmd == 'x') {
			break;
		}

		/* Ler restantes parametros, caso existam, e adiciona-los a um array de componentes */
		if (cmd == 'a' || cmd == 'r' || cmd == 't' || cmd == 's' || cmd == 'm' || cmd == 'A' || cmd == 'R' || cmd == 'i') {
			char c;
			int idx = 0;
			while((c = getchar()) != '\n') {
				if (idx == 0 && c == ' ') {
					continue;
				}
				params[idx++] = c;
			}
			params[idx] = '\0';

			/* Partir a string, caso exista, nos diversos componentes */
			parteString(componentes, params);
			numParam = atoi(componentes[0]);
		}

		if (cmd == 'r' || cmd == 't' || cmd == 'm' || cmd == 'A' || cmd == 'R' || cmd == 'i') {
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
				if (numParam >= 6) {
					numeroEventos = criaEvento(eventos, numeroEventos, componentes);
				}
				break;
			case 'r':
				if (numParam == 1) {
					numeroEventos = removeEvento(eventos, numeroEventos, indexEvento);
				}
				break;
			case 'l' :
				/* indices de 0 ate numero de eventos -1 */
				ordenaEventos(eventos, 0, numeroEventos - 1);
				listaEventos(eventos, numeroEventos);
				break;
			case 't' :
				/* componentes[2] => nova duracao */
				alteraDuracao(eventos, indexEvento, numeroEventos, atoi(componentes[2]));
				break;
			/* Adicionar verificacao*/
			case 'm' :{
				mudaSala(eventos, indexEvento, numeroEventos, atoi(componentes[2]));
				break;
			}
			/* Adicionar verificacao*/
			case 'A' :{
				adicionaParticipante(eventos, indexEvento, numeroEventos, componentes[2]);
				break;
			}
			/* Adicionar verificacao*/
			case 'R' :{
				removeParticipante(eventos, indexEvento, componentes[2]);
				break;
			}	
			case 's':
				{
					Evento eventosSala[MAX_EVENTOS_SALA];
					int numeroEventosSala = getEventosDaSala(eventos, numeroEventos, eventosSala, atoi(componentes[1]));
					/* indices de 0 ate numero de eventos da sala -1 */
					ordenaEventos(eventosSala, 0, numeroEventosSala - 1);
					listaEventos(eventosSala, numeroEventosSala);
				}
				break;
			case 'i' :{
				alteraInicio(eventos, indexEvento, numeroEventos, componentes[2]);
			} 
				break;
		}

		/* limpa comando anterior */
		cmd = '\0';
	}

    return 0;
}

/* fazer printEvento(Evento evento)) no formato pedido */

void printEvento(Evento evento) {
	int i, numParticipantes = 0;
	char data[9];
	char hora[5];

	strftime(data, sizeof(data), "%d%m%Y", &evento.dataHora);
	strftime(hora, sizeof(hora), "%H%M", &evento.dataHora);

	printf("%s %s %s %d Sala%d %s\n", evento.descricao, data, hora, evento.duracao, evento.sala, evento.responsavel);
	printf("* ");
	for (i = 0; i < 3; i++) {
		/* Ignora participantes nao existentes */
		if (evento.participantes[i][0] == '\0') {
			break;
		}
		numParticipantes++;
		printf("%s ", evento.participantes[i]);
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

long getTimestamp(Evento evento) {
	time_t epoch = mktime(&evento.dataHora);
	return (long)epoch;
}

/* TODO: remove this */
void printTimestamp(time_t epoch) {
	printf("%.f\n", difftime(epoch, (time_t) 0));
}

void ordenaEventos(Evento eventos[], int min, int numeroEventos) {
	int i = min, j = numeroEventos;
	Evento eventoTmp = eventos[(min + numeroEventos) / 2];

    while(i <= j) {
		long timestamp = getTimestamp(eventoTmp);
		
        while((getTimestamp(eventos[i]) < timestamp || 
				(getTimestamp(eventos[i]) == timestamp && eventos[i].sala < eventoTmp.sala)) && i < numeroEventos) {
            i++;
        }
        while((getTimestamp(eventos[j]) > timestamp || 
				(getTimestamp(eventos[j]) == timestamp && eventos[j].sala > eventoTmp.sala)) && j > min) {
            j--;
        }
        if(i <= j) {
			/* No caso de existirem vários eventos que se iniciem ao mesmo tempo,
			   deverão ser listados pela ordem crescente do número da sala */
			if(getTimestamp(eventos[i]) != getTimestamp(eventos[j]) 
					|| (getTimestamp(eventos[i]) == getTimestamp(eventos[j]) && eventos[i].sala > eventos[j].sala)) {
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
	int i, numComp = atoi(componentes[0]), contSala = 0;
	int indexFalso = -5, contDisp = 0, contResp = 0, numEventosDecorrer = 0, idx = 0;
	char dataHora[8 + 4 + 1], listaIndisp[4][MAX_NAMES_LENGTH];
	Evento evento, eventosDecorrer[MAX_EVENTOS_DECORRER];

	memset(listaIndisp, '\0', sizeof(listaIndisp));
   
    memset(evento.descricao, '\0', sizeof(evento.descricao));
    strcpy(evento.descricao, componentes[1]);

	sprintf(dataHora, "%s %s", componentes[2], componentes[3]);
	strptime(dataHora, "%d%m%Y %H%M", &evento.dataHora);

    evento.duracao = atoi(componentes[4]);

	evento.sala = atoi(componentes[5]);
	
    memset(evento.responsavel, '\0', sizeof(evento.responsavel));
    strcpy(evento.responsavel, componentes[6]);

   	memset(evento.participantes, '\0', sizeof(evento.participantes));
	for(i = 0; i < (numComp - 6); i++) {
    	strcpy(evento.participantes[i], componentes[7 + i]);
    }

	numEventosDecorrer = getEventosDecorrer(eventos, numeroEventos, indexFalso, eventosDecorrer, getTimestamp(evento), evento.duracao);

	contResp = verificaPessoa(eventosDecorrer, numEventosDecorrer, evento.responsavel);
	if (contResp != 0) {
		strcpy(listaIndisp[idx], evento.responsavel);
		idx++;
	}

	for (i = 0; i < 3; i++) {
		if (evento.participantes[i][0] != '\0') {
			contDisp += verificaPessoa(eventosDecorrer, numEventosDecorrer, evento.participantes[i]);
		}
		if (contDisp != 0) {
			strcpy(listaIndisp[idx], evento.participantes[i]);
			idx++;
		}
		contDisp = 0;	
	}

	contSala = verificaSala(eventos, numeroEventos, indexFalso, getTimestamp(evento), evento.duracao, evento.sala);
	if (contSala != 0) {
		printf("Impossivel agendar evento %s. Sala%d ocupada.\n", evento.descricao, evento.sala);
	}
	else if ((contSala == 0) && (idx != 0)) {
		for (i = 0; i < idx; i++) {
			printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", evento.descricao, listaIndisp[i]);
		}
	}
	else if ((contSala == 0) && (idx == 0)) {
		/*printf("Criou evento : [%s]\n", evento.descricao);*/
    	eventos[numeroEventos++] = evento;
	}

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


int removeEvento(Evento eventos[], int numeroEventos, int indexEvento) {
	int i;

	for (i = 0; i < numeroEventos - 1; i++) {
		if (i >= indexEvento) {
			/* a partir do indice que queremos remover, copia o proximo indice para o indice actual */
			eventos[i] = eventos[i + 1];
		}
	}
	eventos[numeroEventos--].descricao[0] = '\0';

	return numeroEventos;
}


void alteraDuracao(Evento eventos[], int indexEvento, int numeroEventos, int duracao) {
	int contSala = 0, numEventosDecorrer = 0, idx = 0, i, contDisp = 0, contResp = 0;
	Evento eventosDecorrer[MAX_EVENTOS_DECORRER];
	char listaIndisp[4][MAX_NAMES_LENGTH];

	memset(listaIndisp, '\0',sizeof(listaIndisp));

	numEventosDecorrer = getEventosDecorrer(eventos, numeroEventos, indexEvento, eventosDecorrer, getTimestamp(eventos[indexEvento]), duracao);

	contResp = verificaPessoa(eventosDecorrer, numEventosDecorrer, eventos[indexEvento].responsavel);
	if (contResp != 0) {
		strcpy(listaIndisp[idx], eventos[indexEvento].responsavel);
		idx++;
	}

	for (i = 0; i < 3; i++) {
		if (eventos[indexEvento].participantes[i][0] != '\0')
			contDisp += verificaPessoa(eventosDecorrer, numEventosDecorrer, eventos[indexEvento].participantes[i]);
		if (contDisp != 0) {
			strcpy(listaIndisp[idx], eventos[indexEvento].participantes[i]);
			idx++;
		}
		contDisp = 0;	
	}

	contSala = verificaSala(eventos, numeroEventos, indexEvento, getTimestamp(eventos[indexEvento]), duracao, eventos[indexEvento].sala);

	/* TODO: remover esta linha de debug */
	if (contSala != 0) {
		printf("Impossivel agendar evento %s. Sala%d ocupada.\n", eventos[indexEvento].descricao, eventos[indexEvento].sala);
	}
	else if ((contSala == 0) && (idx != 0)) {
		for (i = 0; i < idx; i++) {
			printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", eventos[indexEvento].descricao, listaIndisp[i]);
		}
	}
	/*Muda o evento de sala*/
	else if ((contSala == 0) && (idx == 0)) {
		eventos[indexEvento].duracao = duracao;
	}

}


void mudaSala(Evento eventos[], int indexEvento, int numeroEventos, int novaSala) {
	int contSala = 0;

	contSala = verificaSala(eventos, numeroEventos, indexEvento, getTimestamp(eventos[indexEvento]), eventos[indexEvento].duracao, novaSala);

	if (contSala != 0) {
		printf("Impossivel agendar evento %s. Sala%d ocupada.\n", eventos[indexEvento].descricao, novaSala);
	}
	/*Muda o evento de sala*/
	else if (contSala == 0) {
		eventos[indexEvento].sala = novaSala;
	}
	
}

int verificaSala(Evento eventos[], int numeroEventos,int indexEvento, long timestampInicio, int duracao, int Sala) {
	int i, cont = 0;
	long timestampFimSala1 = (timestampInicio + (duracao * 60)), timestampIniSala2, timestampFimSala2;

	for (i = 0; i < numeroEventos; i++) {
		if ((eventos[i].sala == Sala) && (i != indexEvento)) {
			timestampIniSala2 = getTimestamp(eventos[i]);
			timestampFimSala2 = timestampIniSala2 + (eventos[i].duracao * 60);

			if (((timestampIniSala2 < timestampInicio) && (timestampInicio < timestampFimSala2)) ||
				 ((timestampIniSala2 < timestampFimSala1) && (timestampFimSala1 < timestampFimSala2)) ||
					((timestampInicio < timestampIniSala2) && (timestampIniSala2 < timestampFimSala1)) ||
						((timestampInicio < timestampFimSala2) && (timestampFimSala2 < timestampFimSala1)) ||
							((timestampInicio < timestampIniSala2) && (timestampFimSala2 < timestampFimSala1)) ||
								((timestampIniSala2 < timestampInicio) && (timestampFimSala1 < timestampFimSala2)) ||
									((timestampInicio == timestampIniSala2) && (timestampFimSala1 == timestampFimSala2))) {
				cont++;
			}
		}
	}

	return cont;
}

void removeParticipante(Evento eventos[], int indexEvento, char participante[MAX_NAMES_LENGTH]) {
	int i, contPresenca = 0, contPar = 0, indexParticipante = 0;

	for (i = 0; i < 3; i++) {
		if (strcmp(eventos[indexEvento].participantes[i], participante) == 0) {
			indexParticipante = i;
			contPresenca++;
			contPar++;
		}
		else if (eventos[indexEvento].participantes[i][0] != '\0') {
			contPar++;
		}
	}
	if ((contPresenca != 0) && (contPar == 1)) {
		printf("Impossivel remover participante. Participante %s e o unico participante no evento %s.\n", participante, eventos[indexEvento].descricao);
	}
	else if ((contPresenca != 0) && (contPar > 1)) {
		for (i = indexParticipante; i < 2 ; i++) {
			if (eventos[indexEvento].participantes[i][0] != '\0') {
				strcpy(eventos[indexEvento].participantes[i], eventos[indexEvento].participantes[i + 1]);
			}
		}
		if (contPar == 2) {
			memset(eventos[indexEvento].participantes[1], '\0', sizeof(eventos[indexEvento].participantes[1]));
		}
		else if (contPar == 3) {
			memset(eventos[indexEvento].participantes[2], '\0', sizeof(eventos[indexEvento].participantes[2]));
		}
	}
}

void alteraInicio(Evento eventos[], int indexEvento, int numeroEventos, char hora[]) {
	int salaOcupada = 0, contDisp = 0, i, contResp = 0, partIndisp = 0, numEventosDecorrer = 0;
	Evento eventoAux = eventos[indexEvento];
	Evento	eventosDecorrer[MAX_EVENTOS_DECORRER];
	char listaIndisp[4][MAX_NAMES_LENGTH];

	memset(listaIndisp, '\0',sizeof(listaIndisp));

	strptime(hora,"%H%M", &eventoAux.dataHora);

	salaOcupada = verificaSala(eventos, numeroEventos, indexEvento, getTimestamp(eventoAux), eventos[indexEvento].duracao, eventos[indexEvento].sala);
	if (salaOcupada != 0) {
		printf("Impossivel agendar evento %s. Sala%d ocupada.\n", eventos[indexEvento].descricao, eventos[indexEvento].sala);
		return;
	}

	numEventosDecorrer = getEventosDecorrer(eventos, numeroEventos, indexEvento, eventosDecorrer, getTimestamp(eventoAux), eventos[indexEvento].duracao);
	
	contResp = verificaPessoa(eventosDecorrer, numEventosDecorrer, eventos[indexEvento].responsavel);
	if (contResp != 0) {
		strcpy(listaIndisp[partIndisp++], eventos[indexEvento].responsavel);
	}

	for (i = 0; i < 3; i++) {
		if (eventos[indexEvento].participantes[i][0] != '\0')
			contDisp += verificaPessoa(eventosDecorrer, numEventosDecorrer, eventos[indexEvento].participantes[i]);
		if (contDisp != 0) {
			strcpy(listaIndisp[partIndisp++], eventos[indexEvento].participantes[i]);
		}
		contDisp = 0;	
	}

	if (partIndisp > 0) {
		for (i = 0; i < partIndisp; i++) {
			printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", eventos[indexEvento].descricao, listaIndisp[i]);
		}
	}
	else {
		eventos[indexEvento] = eventoAux;
	}
}


void adicionaParticipante(Evento eventos[], int indexEvento,int numeroEventos, char participante[MAX_NAMES_LENGTH]) {
	int i, contPar = 0, contPresenca = 0, contDisp = 0, numEventosDecorrer = 0;
	Evento eventosDecorrer[MAX_EVENTOS_DECORRER];

	numEventosDecorrer = getEventosDecorrer(eventos, numeroEventos, indexEvento, eventosDecorrer, getTimestamp(eventos[indexEvento]), eventos[indexEvento].duracao);

	for (i = 0; i < 3 ; i++) {
		if (strcmp(eventos[indexEvento].participantes[i], participante) == 0) {
			contPresenca++;
			contPar++;
		}
		else if (eventos[indexEvento].participantes[i][0] != '\0') {
			contPar++;
		}
	}

	contDisp = verificaPessoa(eventosDecorrer, numEventosDecorrer, participante);
	
	if ((contPar == 3) && (contPresenca == 0)) {
		printf("Impossivel adicionar participante. Evento %s ja tem 3 participantes.\n", eventos[indexEvento].descricao);
	}
	/*Falta ver se o participante já está noutro evento a essa hora*/
	else if ((contPresenca == 0) && (contDisp != 0)) {
		printf("Impossivel adicionar participante. Participante %s tem um evento sobreposto.\n", participante);
	}
	else if ((contPresenca == 0) && (contDisp == 0)) {
		strcpy(eventos[indexEvento].participantes[contPar], participante);
	}
}

int verificaPessoa(Evento eventosDecorrer[], int idxEventos, char pessoa[]) {
	int i, e;

	for (i = 0; i < idxEventos; i++) {
		if (strcmp(pessoa, eventosDecorrer[i].responsavel) == 0) {
			return 1;
		}
		for (e = 0; e < 3; e++) {
			if (strcmp(pessoa, eventosDecorrer[i].participantes[e]) == 0) {
				return 1;
			}
		}
	}

	return 0;
}
	

int getEventosDecorrer(Evento eventos[],int numeroEventos, int indexEvento, Evento eventosDecorrer[], long timestamp, int duracao) {
	int i, contInterset = 0, numeroEventosDecorrer = 0;

	for (i = 0; i < numeroEventos; i++) {
		if (i != indexEvento) {
			contInterset = comparaTimestamps(timestamp, duracao, getTimestamp(eventos[i]), eventos[i].duracao);
		}
		if (contInterset != 0) {
			eventosDecorrer[numeroEventosDecorrer++] = eventos[i];
		}
		contInterset = 0;
	}

	return numeroEventosDecorrer;
}

int comparaTimestamps(long timestampEvent1, int duracao1, long timestampEvent2, int duracao2) {
	int contInterset = 0;
	long timestampFim1 = timestampEvent1 + (duracao1 * 60), timestampFim2 = timestampEvent2 + (duracao2 * 60);

	/* FIZ ALTERACAO DO INTERVALO PARA NAO DAR FLAG SE O FIM OU INICIO COINCIDIREM APENAS*/
	if (((timestampEvent2 < timestampEvent1) && (timestampEvent1 < timestampFim2)) ||
		((timestampEvent2 < timestampFim1) && (timestampFim1 < timestampFim2)) ||
			((timestampEvent1 < timestampEvent2) && (timestampEvent2 < timestampFim1)) ||
				((timestampEvent1 < timestampFim2) && (timestampFim2 < timestampFim1)) ||
					((timestampEvent1 < timestampEvent2) && (timestampFim2 < timestampFim1)) ||
						((timestampEvent2 < timestampEvent1) && (timestampFim1 < timestampFim2)) ||
							((timestampEvent1 == timestampEvent2) && (timestampFim1 == timestampFim2))) {
		contInterset++;
	} 

	return contInterset;
}






