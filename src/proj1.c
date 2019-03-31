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

time_t getTimestamp(Evento evento);

void parteString(char componentes[][MAX_NAMES_LENGTH], char params[]);

void mudaSala(Evento eventos[], int indexEvento, int numeroEventos, int novaSala);

void adicionaParticipante(Evento eventos[], int indexEvento,int numeroEventos, char participante[MAX_NAMES_LENGTH]);

void removeParticipante(Evento eventos[], int indexEvento,int numeroEventos, char participante[MAX_NAMES_LENGTH]);

int verificaSala(Evento eventos[], int numeroEventos,int indexEvento,time_t Timestamp, int duracao, int novaSala);

void alteraInicio(Evento eventos[], int indexEvento, int numeroEventos, char hora[]);

int buscaEventosAdecorrer(Evento eventos[],int numeroEventos, int indexEvento,Evento eventosDecorrer[], time_t Timestamp, int duracao);

int verificaPessoa(Evento eventosAdecorrer[], int idxEventos, char pessoa[]);

int comparaTimestamps(time_t TimestampEvent1, int duracao1, time_t TimestampEvent2, int duracao2);

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
		if (cmd == 'a' || cmd == 'r' || cmd == 't' || cmd == 's' || cmd == 'm' || cmd == 'A' || cmd == 'R' || cmd == 'i'){
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
				removeParticipante(eventos, indexEvento, numeroEventos, componentes[2]);
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

void printEvento(Evento evento){
	int i, numParticipantes = 0;
	char data[9];
	char hora[5];

	strftime(data, sizeof(data), "%d%m%Y", &evento.dataHora);
	strftime(hora, sizeof(hora), "%H%M", &evento.dataHora);

	printf("%s %s %s %d Sala%d %s\n", evento.descricao, data, hora, evento.duracao, evento.sala, evento.responsavel);
	printf("* ");
	for (i = 0; i < 3; i++){
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
	int i, numComp = atoi(componentes[0]), contSala, indexFalso = -5, contDisp, contResp, numEventosDecorrer, idx;
	char dataHora[8 + 4 + 1], listaIndisp[4][MAX_NAMES_LENGTH], stringBarraZero[1];
	Evento eventosAdecorrer[MAX_EVENTOS_DECORRER];
	memset(stringBarraZero, '\0', sizeof(stringBarraZero));;
   
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

	numEventosDecorrer = buscaEventosAdecorrer(eventos, numeroEventos, indexFalso, eventosAdecorrer, getTimestamp(evento), evento.duracao);

	contResp = verificaPessoa(eventosAdecorrer, numEventosDecorrer, evento.responsavel);
	if (contResp != 0){
		strcpy(listaIndisp[idx], evento.responsavel);
		idx++;
	}

	for (i = 0; i < 3; i++){
		if (strcmp(evento.participantes[i],stringBarraZero) != 0)
			contDisp += verificaPessoa(eventosAdecorrer, numEventosDecorrer, evento.participantes[i]);
		if (contDisp != 0){
			strcpy(listaIndisp[idx], evento.participantes[i]);
			idx++;
		}
		contDisp = 0;	
	}

	contSala = verificaSala(eventos, numeroEventos, indexFalso,getTimestamp(evento), evento.duracao, evento.sala);
	if (contSala != 0){
		printf("Impossivel agendar evento %s. Sala%d ocupada.\n", evento.descricao, evento.sala);
	}
	else if ((contSala == 0) && (idx != 0)){
		printf("Impossivel agendar evento %s. Participante %s %s %s %s tem um evento sobreposto.\n", evento.descricao, listaIndisp[0], listaIndisp[1], listaIndisp[2], listaIndisp[3]);
	}
	else if ((contSala == 0) && (idx == 0)){
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


void alteraDuracao(Evento eventos[], int indexEvento, int numeroEventos, int duracao){
	int contSala, numEventosDecorrer, idx, i, contDisp, contResp;
	Evento eventosAdecorrer[MAX_EVENTOS_DECORRER];
	char listaIndisp[4][MAX_NAMES_LENGTH], stringBarraZero[1];
	memset(stringBarraZero, '\0', sizeof(stringBarraZero));

	numEventosDecorrer = buscaEventosAdecorrer(eventos, numeroEventos, indexEvento, eventosAdecorrer, getTimestamp(eventos[indexEvento]), duracao);

	contResp = verificaPessoa(eventosAdecorrer, numEventosDecorrer, eventos[indexEvento].responsavel);
	if (contResp != 0){
		strcpy(listaIndisp[idx], eventos[indexEvento].responsavel);
		idx++;
	}

	for (i = 0; i < 3; i++){
		if (strcmp(eventos[indexEvento].participantes[i],stringBarraZero) != 0)
			contDisp += verificaPessoa(eventosAdecorrer, numEventosDecorrer, eventos[indexEvento].participantes[i]);
		if (contDisp != 0){
			strcpy(listaIndisp[idx], eventos[indexEvento].participantes[i]);
			idx++;
		}
		contDisp = 0;	
	}

	contSala = verificaSala(eventos, numeroEventos, indexEvento,getTimestamp(eventos[indexEvento]), duracao, eventos[indexEvento].sala);

	/* TODO: remover esta linha de debug */
	if (contSala != 0){
		printf("Impossivel agendar evento %s. Sala%d ocupada.\n", eventos[indexEvento].descricao, eventos[indexEvento].sala);
	}
	else if ((contSala == 0) && (idx != 0)){
		printf("Impossivel agendar evento %s. Participante %s %s %s %s tem um evento sobreposto.\n", eventos[indexEvento].descricao, listaIndisp[0], listaIndisp[1], listaIndisp[2], listaIndisp[3]);
	}
	/*Muda o evento de sala*/
	else if ((contSala == 0) && (idx == 0)){
		printf("nova duracao : %d\n", duracao);
		eventos[indexEvento].duracao = duracao;
	}

}


void mudaSala(Evento eventos[], int indexEvento, int numeroEventos, int novaSala){
	int contSala = 0;

	contSala = verificaSala(eventos, numeroEventos, indexEvento,getTimestamp(eventos[indexEvento]), eventos[indexEvento].duracao, novaSala);

	if (contSala != 0){
		printf("Impossivel agendar evento %s. Sala%d ocupada.\n", eventos[indexEvento].descricao, novaSala);
	}
	/*Muda o evento de sala*/
	else if (contSala == 0){
		/* Eliminar comentario*/
		printf("nova sala : [%d]\n", novaSala);
		eventos[indexEvento].sala = novaSala;
	}
	
}

int verificaSala(Evento eventos[], int numeroEventos,int indexEvento,time_t timestampIniSala1, int duracao, int Sala){
	int i, cont = 0;
	time_t timestampFinSala1 = (timestampIniSala1 + (duracao * 60)), timestampIniSala2, timestampFinSala2;
	for (i = 0; i < numeroEventos; i++){
		if ((eventos[i].sala == Sala) && (i != indexEvento)){
			timestampIniSala2 = getTimestamp(eventos[i]);
			timestampFinSala2 = timestampIniSala2 + (eventos[i].duracao * 60);
			/* Eliminar comentario*/
			printf("Timestamp incial sala2 = %lu e TimestampFinal sala2 = %lu e Timestamp inicial sala 1 = %lu e Timestamp final sala 1 = %lu .\n", timestampIniSala2, timestampFinSala2, timestampIniSala1, timestampFinSala1);
			if (((timestampIniSala2 <= timestampIniSala1) && (timestampIniSala1 <= timestampFinSala2)) || ((timestampIniSala2 <= timestampFinSala1) && (timestampFinSala1 <= timestampFinSala2))){
				/*Eliminar comentario*/
				printf("Deu hit no intervalo .\n");
				cont++;
			}
		}
	}
	return cont;
}

void removeParticipante(Evento eventos[], int indexEvento,int numeroEventos, char participante[MAX_NAMES_LENGTH]){
	int i, contPresenca = 0, contPar = 0, indexParticipante = 0;
	char stringBarraZero[1];
	memset(stringBarraZero, '\0', sizeof(stringBarraZero));

	for (i = 0; i < 3; i++){
		if (strcmp(eventos[indexEvento].participantes[i], participante) == 0){
			indexParticipante = i;
			contPresenca++;
			contPar++;
		}
		else if (strcmp(eventos[indexEvento].participantes[i], stringBarraZero) != 0){
			contPar++;
		}
	}
	printf("Presenca : %d , numero de participantes : %d\n", contPresenca, contPar);
	if ((contPresenca != 0) && (contPar == 1)){
		printf("Impossivel remover participante. Participante %s e o unico participante no evento %s.\n", participante, eventos[indexEvento].descricao);
	}
	else if ((contPresenca != 0) && (contPar > 1)){
		for (i = indexParticipante; i < 2 ; i++){
			if (strcmp(eventos[indexEvento].participantes[i], stringBarraZero) != 0){
				strcpy(eventos[indexEvento].participantes[i], eventos[indexEvento].participantes[i + 1]);
			}
		}
		if (contPar == 2){
			strcpy(eventos[indexEvento].participantes[1], stringBarraZero);
		}
		else if (contPar == 3){
			strcpy(eventos[indexEvento].participantes[2], stringBarraZero);
		}
	}
}

void alteraInicio(Evento eventos[], int indexEvento, int numeroEventos, char hora[]){
	int contSala, contDisp, i, contResp = 0, idx = 0, numEventosDecorrer;
	Evento eventoAux = eventos[indexEvento];
	Evento	eventosAdecorrer[MAX_EVENTOS_DECORRER];
	char listaIndisp[4][MAX_NAMES_LENGTH], stringBarraZero[1];
	memset(stringBarraZero, '\0', sizeof(stringBarraZero));

	strptime(hora,"%H%M", &eventoAux.dataHora);

	contSala = verificaSala(eventos, numeroEventos, indexEvento, getTimestamp(eventoAux), eventos[indexEvento].duracao, eventos[indexEvento].sala);

	numEventosDecorrer = buscaEventosAdecorrer(eventos, numeroEventos, indexEvento, eventosAdecorrer, getTimestamp(eventoAux), eventos[indexEvento].duracao);
	
	contResp = verificaPessoa(eventosAdecorrer, numEventosDecorrer, eventos[indexEvento].responsavel);
	if (contResp != 0){
		strcpy(listaIndisp[idx], eventos[indexEvento].responsavel);
		idx++;
	}

	for (i = 0; i < 3; i++){
		if (strcmp(eventos[indexEvento].participantes[i],stringBarraZero) != 0)
			contDisp += verificaPessoa(eventosAdecorrer, numEventosDecorrer, eventos[indexEvento].participantes[i]);
		if (contDisp != 0){
			strcpy(listaIndisp[idx], eventos[indexEvento].participantes[i]);
			idx++;
		}
		contDisp = 0;	
	}
	/*Eliminar comentario*/
	printf("idx = %d e contResp = %d\n", idx, contResp);

	if (contSala != 0){
		printf("Impossivel agendar evento %s. Sala%d ocupada.\n", eventos[indexEvento].descricao, eventos[indexEvento].sala);
	}
	else if ((contSala == 0) && (idx != 0)){
		printf("Impossivel agendar evento %s. Participante %s %s %s %s tem um evento sobreposto.\n", eventos[indexEvento].descricao, listaIndisp[0], listaIndisp[1], listaIndisp[2], listaIndisp[3]);
	}
	else if ((contSala == 0) && (idx == 0)){
		eventos[indexEvento] = eventoAux;
	}
}


void adicionaParticipante(Evento eventos[], int indexEvento,int numeroEventos, char participante[MAX_NAMES_LENGTH]){
	int i, contPar = 0, contPresenca = 0, contDisp = 0, numEventosDecorrer;
	char stringBarraZero[1];
	Evento eventosAdecorrer[MAX_EVENTOS_DECORRER];
	memset(stringBarraZero, '\0', sizeof(stringBarraZero));
	
	numEventosDecorrer = buscaEventosAdecorrer(eventos, numeroEventos, indexEvento, eventosAdecorrer, getTimestamp(eventos[indexEvento]), eventos[indexEvento].duracao);

	for (i = 0; i < 3 ; i++){
		if (strcmp(eventos[indexEvento].participantes[i], participante) == 0){
			contPresenca++;
			contPar++;
		}
		else if (strcmp(eventos[indexEvento].participantes[i], stringBarraZero) != 0){
			contPar++;
		}
	}

	contDisp = verificaPessoa(eventosAdecorrer, numEventosDecorrer, participante);
	
	if ((contPar == 3) && (contPresenca == 0)){
		printf("Impossivel adicionar participante. Evento %s ja tem 3 participantes.\n", eventos[indexEvento].descricao);
	}
	/*Falta ver se o participante já está noutro evento a essa hora*/
	else if ((contPresenca == 0) && (contDisp != 0)) {
		printf("Impossivel adicionar participante. Participante %s tem um evento sobreposto.\n", participante);
	}
	else if ((contPresenca == 0) && (contDisp == 0)){
		strcpy(eventos[indexEvento].participantes[contPar], participante);
	}
}
/******************************************************** ABAIXO ESTAO AS FUNCOES AINDA NAO TESTADAS COM VALORES **************************************************************************************************/

int verificaPessoa(Evento eventosAdecorrer[], int idxEventos, char pessoa[]){
	int i, e;
	for (i = 0; i < idxEventos; i++){
		if (strcmp(pessoa, eventosAdecorrer[i].responsavel) == 0){
			return 1;
		}
		for (e = 0; e < 3; e++){
			if (strcmp(pessoa, eventosAdecorrer[i].participantes[e]) == 0){
				return 1;
			}
		}
	}
	return 0;
}
	

int buscaEventosAdecorrer(Evento eventos[],int numeroEventos, int indexEvento, Evento eventosDecorrer[], time_t Timestamp, int duracao){
	int i, contInterset = 0, idx = 0;
	for (i = 0; i < numeroEventos; i++){
		if (i != indexEvento){
			contInterset = comparaTimestamps(Timestamp, duracao, getTimestamp(eventos[i]), eventos[i].duracao);
		}
		if (contInterset != 0){
			eventosDecorrer[idx++] = eventos[i];
		}
		contInterset = 0;
	}
	return idx;
}

int comparaTimestamps(time_t TimestampEvent1, int duracao1, time_t TimestampEvent2, int duracao2){
	int contInterset = 0;
	time_t TimestampFin1 = TimestampEvent1 + (duracao1 * 60), TimestampFin2 = TimestampEvent2 + (duracao2 * 60);
	if (((TimestampEvent2 <= TimestampEvent1) && (TimestampEvent1 <= TimestampFin2)) || ((TimestampEvent2 <= TimestampFin1) && (TimestampFin1 <= TimestampFin2))){
				/*Eliminar comentario*/
				printf("Deu hit no intervalo .\n");
				contInterset++;
		} 
		return contInterset;
}






