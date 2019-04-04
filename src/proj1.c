/* Projeto de IAED - Programa que agenda eventos em salas - Aluno : Vitor Vale nº 92570 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define __USE_XOPEN

#include <time.h>

#define MAX_SALAS 10 /* numero maximo de salas */
#define MAX_EVENTOS_SALA 100 /* numero maximo de eventos por sala */
#define MAX_PARTICIPANTES 3 /* numero maximo de participantes por evento */
#define MAX_NAMES_LENGTH 64 /* numero de maximo de carateres para nomes */
#define MAX_COMPONENTES 10  /* numero maximo de componentes no vetor que guarda as componentes do input */

#define MAX_SIZE_PARAMS 350 /* numero maximo de caracteres em cada linha de comando */

#define MAX_EVENTOS (MAX_SALAS * MAX_EVENTOS_SALA) /* numero maximo de eventos */

/* Estrutura que define um evento*/
typedef struct evento {
    char descricao[MAX_NAMES_LENGTH];
    struct tm dataHora; /* tipo de estrutura que guarda o tempo */
    long timestamp; /* tempo em segundos desde uma determinada data anterior a' minima estabelecida */
    int duracao; 
    int sala;
    char responsavel[MAX_NAMES_LENGTH];
    char participantes[MAX_PARTICIPANTES][MAX_NAMES_LENGTH];
} Evento;


int criaEvento(Evento eventos[], int numeroEventos, char componentes[MAX_COMPONENTES][MAX_NAMES_LENGTH]);

int procuraEvento(Evento eventos[], int numeroEventos, char descricaoEvento[]);

void alteraDuracao(Evento eventos[], int indexEvento, int numeroEventos, int duracao);

int removeEvento(Evento eventos[], int numeroEventos, int indexEvento);

void listaEventos(Evento eventos[], int numeroEventos);

int getEventosDaSala(Evento eventos[], int numeroEventos, Evento eventosSala[MAX_EVENTOS_SALA], int sala);

void ordenaEventos(Evento eventos[], int minimo, int numeroEventos);

long getTimestamp(struct tm tmDataHora);

void parteString(char componentes[MAX_COMPONENTES][MAX_NAMES_LENGTH], char params[]);

void mudaSala(Evento eventos[], int indexEvento, int numeroEventos, int novaSala);

void adicionaParticipante(Evento eventos[], int indexEvento,int numeroEventos, char participante[MAX_NAMES_LENGTH]);

void removeParticipante(Evento eventos[], int indexEvento, char participante[MAX_NAMES_LENGTH]);

int salaOcupada(const Evento eventos[], int numeroEventos,int indexEvento, long timestamp, int duracao, int novaSala);

void alteraInicio(Evento eventos[], int indexEvento, int numeroEventos, char hora[]);

int getEventosDecorrer(const Evento eventos[], int numeroEventos, int indexEvento, Evento eventosDecorrer[], long timestamp, int duracao);

int pessoaIndisponivel(const Evento eventosDecorrer[], int idxEventos, char pessoa[]);

int comparaTimestamps(long inicioEvento1, int duracao1, long inicioEvento2, int duracao2);

int main() {
    /* vetor que guarda todos os eventos agendados */
    Evento eventos[MAX_EVENTOS] = {{{0}, {0}, 0, 0, 0, {0}, {{0}}}};
    int numeroEventos = 0;
    /* tamanho maximo de cada linha de comandos */
    char params [MAX_SIZE_PARAMS];

    /* Crio um vetor de strings onde guardo as componentes do input */
    char componentes[MAX_COMPONENTES][MAX_NAMES_LENGTH];
    /* Guarda o comando dado no input */
    char cmd;

    while (1) {
        int numParam = 0, indexEvento = -1;

        memset(params, '\0', sizeof(char) * MAX_SIZE_PARAMS);
        memset(componentes, '\0', sizeof(char) * MAX_COMPONENTES * MAX_NAMES_LENGTH);

        /* Ler comando */
        scanf("%c", &cmd);
        if (cmd == 'x') {
            break;
        }

        /* Ler restantes parametros, caso existam, e adiciona-los a um array de componentes */
        /* Portanto sao os comandos que introduzem input necessario para a operacao
           e e' necessario extrair os componentes de cada comando */
        if (cmd == 'a' || cmd == 'r' || cmd == 't' || cmd == 's' || cmd == 'm' || cmd == 'A' || cmd == 'R' || cmd == 'i') {
            char c;
            int idx = 0;
            while((c = getchar()) != '\n' && (idx + 1) < MAX_SIZE_PARAMS) {
                /* skip do primeiro espaco */
                if (idx == 0 && c == ' ') {
                    continue;
                }
                params[idx++] = c;
            }
            params[idx] = '\0';

            /* Partir a string nos diversos componentes, que neste caso sao
            strings sendo posteriormente convertidos nos tipos respetivos */
            parteString(componentes, params);
            /* o numero de parametros esta guardado no primeiro indice do vetor */
            numParam = atoi(componentes[0]);
			if(numParam <= 0) {
				continue;
			}
        }

        /* Comandos que precisam de verificar se a descricao e' igual a' de algum evento existente */
        if (cmd == 'r' || cmd == 't' || cmd == 'm' || cmd == 'A' || cmd == 'R' || cmd == 'i') {
            char descricaoEvento[MAX_NAMES_LENGTH];

            memset(descricaoEvento, '\0', sizeof(char) * MAX_NAMES_LENGTH);
            if(strlen(componentes[1]) < sizeof(descricaoEvento)) {
                strcpy(descricaoEvento, componentes[1]);
            }
            if(numParam <= 0) {
                continue;
            }

            /* Procura um evento que tenha uma descricao igual a' fornecida e devolve o seu indice */
            indexEvento = procuraEvento(eventos, numeroEventos, descricaoEvento);
            if(indexEvento < 0) {
                continue;
            }
        }

        /* Uso o vetor de strings componentes[] para dar os parametros a's operacoes */
        /* Guardo sempre o numero de eventos para saber ate onde tenho de ir no vetor de eventos */
        switch(cmd) {
            case 'a' :
                {
                    int numeroEventosSala = 0;
                    /* vetor que guarda os eventos de uma determinada sala */
                    Evento eventosSala[MAX_EVENTOS_SALA] = {{{0}, {0}, 0, 0, 0, {0}, {{0}}}};

                    /* numero da sala estÃ¡ no componentes[5] */
                    numeroEventosSala = getEventosDaSala(eventos, numeroEventos, eventosSala, atoi(componentes[5]));
                    if (numeroEventos < MAX_EVENTOS && numeroEventosSala < MAX_EVENTOS_SALA && numParam >= 6) {
                        numeroEventos = criaEvento(eventos, numeroEventos, componentes);
                    }
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
            case 'm' :
                {
                    int numeroEventosSala = 0;
                    Evento eventosSala[MAX_EVENTOS_SALA] = {{{0}, {0}, 0, 0, 0, {0}, {{0}}}};

                    /* numero da sala esta no componentes[2] */
                    numeroEventosSala = getEventosDaSala(eventos, numeroEventos, eventosSala, atoi(componentes[2]));
                    if(numeroEventosSala < MAX_EVENTOS_SALA) {
                        mudaSala(eventos, indexEvento, numeroEventos, atoi(componentes[2]));
                    }
                }
                break;
            case 'A' :
                {
                    /* o participante esta no componente[2] */
                    adicionaParticipante(eventos, indexEvento, numeroEventos, componentes[2]);
                }
                break;
            case 'R' :{
                /* o participante esta no componente[2] */
                removeParticipante(eventos, indexEvento, componentes[2]);
                break;
            }    
            case 's':
                {
                    int numeroEventosSala = 0;
                    Evento eventosSala[MAX_EVENTOS_SALA] = {{{0}, {0}, 0, 0, 0, {0}, {{0}}}};

                    /* numero da sala esta no componentes[1] */
                    numeroEventosSala = getEventosDaSala(eventos, numeroEventos, eventosSala, atoi(componentes[1]));
                    /* indices de 0 ate numero de eventos -1 */
                    ordenaEventos(eventosSala, 0, numeroEventosSala - 1);
                    listaEventos(eventosSala, numeroEventosSala);
                }
                break;
            case 'i' :
                {
                    /* o novo inicio esta no componentes[2] */
                    alteraInicio(eventos, indexEvento, numeroEventos, componentes[2]);
                } 
                break;
        }

        /* limpa comando anterior */
        cmd = '\0';
    }

    return 0;
}

/* Funcao que recebe um evento e faz print dos seus diferentes parametros */
void printEvento(const Evento evento) {
    int i, numParticipantes = 0;
    char data[9];
    char hora[5];

    memset(data, '\0', sizeof(char) * 9);
    memset(hora, '\0', sizeof(char) * 5);

    /* Uma vez que e usada uma estrutura tm para guardar o tempo
      e necessario usar o strftime para colocar a data / hora
      em strings antes de fazer print de um evento */
    strftime(data, sizeof(data), "%d%m%Y", &evento.dataHora);
    strftime(hora, sizeof(hora), "%H%M", &evento.dataHora);

    printf("%s %s %s %d Sala%d %s\n", evento.descricao, data, hora, evento.duracao, evento.sala, evento.responsavel);
    for (i = 0; i < MAX_PARTICIPANTES; i++) {
        /* Ignora participantes nao existentes */
        if (strlen(evento.participantes[i]) == 0) {
            break;
        }
        numParticipantes++;
        if(numParticipantes == 1) {
            printf("* %s", evento.participantes[i]);
        } else {
            printf(" %s", evento.participantes[i]);
        }
    }
    if(numParticipantes > 0) {
        printf("\n");
    }

}

/* Funcao que recebe o vetor dos eventos e o numero de eventos e imprime todos os eventos */
void listaEventos(Evento eventos[], int numeroEventos) {
    int i;
    for(i = 0; i < numeroEventos; i++) {
        printEvento(eventos[i]);
    }
}

/* Funcao que recebe o vetor dos eventos, o numero de eventos, o vetor que vai guardar eventos de uma sala, 
o numero da sala e insere os eventos dessa sala no vetor eventosSala */
int getEventosDaSala(Evento eventos[], int numeroEventos, Evento eventosSala[MAX_EVENTOS_SALA], int sala) {
    int i, numeroEventosSala = 0;

    for(i = 0; i < numeroEventos; i++) {
        if(eventos[i].sala == sala) {
            eventosSala[numeroEventosSala++] = eventos[i];
        }
    }

    return numeroEventosSala;
}

/* Converte o valor da estrutura tm que guarda o tempo do evento,
  usando a funcao mktime para obter um timestamp correspondente
  (UNIX timestamp - numero de segundos desde 00H00 de 01/01/1970).
  Usando o timestamp, o calculo dos intervaços de tempo e mais simples */
long getTimestamp(struct tm tmDataHora) {
    time_t epoch = mktime(&tmDataHora);
    
    return (long) epoch;
}

/* Implementacao do algoritmo quick sort para ordenar os eventos,
  recebe o indice minimo e maximo, respetivamente, e tendo como elemento
  de comparacao os timestamps e o numero da sala ordena os eventos */
void ordenaEventos(Evento eventos[], int min, int numeroEventos) {
    int i = min, j = numeroEventos;
    Evento eventoTmp = eventos[(min + numeroEventos) / 2];

    while(i <= j) {
        long timestamp = eventoTmp.timestamp;
        
        while((eventos[i].timestamp < timestamp || 
                (eventos[i].timestamp == timestamp && eventos[i].sala < eventoTmp.sala)) && i < numeroEventos) {
            i++;
        }
        while((eventos[j].timestamp > timestamp || 
                (eventos[j].timestamp == timestamp && eventos[j].sala > eventoTmp.sala)) && j > min) {
            j--;
        }
        if(i <= j) {
            /* No caso de existirem varios eventos que se iniciem ao mesmo tempo,
              deverao ser listados pela ordem crescente do numero da sala */
            if(eventos[i].timestamp != eventos[j].timestamp 
                    || (eventos[i].timestamp == eventos[j].timestamp && eventos[i].sala > eventos[j].sala)) {
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

/* Funcao que recebe o vetor de eventos, o numero de eventos,
  uma descricao e que devolve o indice do evento com essa descricao */
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

/* Funcao que recebe um vetor que guarda os eventos, o numero de eventos
  nesse vetor, um vetor de componentes e que cria um evento colocando os
  componentes nos campos respetivos sendo que so guarda o evento no vetor
  se passar nas verificacoes da sala e dos participantes */
int criaEvento(Evento eventos[], int numeroEventos,char componentes[][MAX_NAMES_LENGTH]) {
    int i, numComp = atoi(componentes[0]), salaOcup = 0;
    int indexFalso = -1, participanteIndisponivel = 0;
    int responsavelIndisponivel = 0, numEventosDecorrer = 0, numeroParticipIndisp = 0;
    char dataHora[8 + 1 + 4 + 1], listaIndisp[(MAX_PARTICIPANTES + 1)][MAX_NAMES_LENGTH];
    Evento evento = {{0}, {0}, 0, 0, 0, {0}, {{0}}};
    Evento eventosDecorrer[MAX_EVENTOS] = {{{0}, {0}, 0, 0, 0, {0}, {{0}}}};;

	/* valida tamanho dos componentes - tem de ter no minimo 7 componentes
	   1-descricao, 2-data, 3-hora, 4-duracao, 5-sala, 6-responsavel, 7-particpante1 */
	if(strlen(componentes[1]) >= MAX_NAMES_LENGTH || strlen(componentes[2]) > 8 ||
		strlen(componentes[3]) > 4 || strlen(componentes[4]) > 4 ||
			strlen(componentes[5]) > 2 || strlen(componentes[6]) >= MAX_NAMES_LENGTH ||
				strlen(componentes[7]) >= MAX_NAMES_LENGTH || numComp < 7) {
		return numeroEventos;
	}
    memset(dataHora, '\0', sizeof(char) * 12);
    memset(listaIndisp, '\0', sizeof(char) * (MAX_PARTICIPANTES + 1) * MAX_NAMES_LENGTH);

    /* guarda as componentes nos campos respetivos */

    /* Coloca a data e a hora numa string com um formato de modo a usar
      a funcao strptime para guardar a data e hora na estrutura tm */
    sprintf(dataHora, "%s %s", componentes[2], componentes[3]);
    strptime(dataHora, "%d%m%Y %H%M", &evento.dataHora);

    /* Converte o valor da estrutura tm num (UNIX) timestamp,
      o qual e usado posteriormente nas diferentes funcoes de verificacao.
      Com o uso deste timestamp o processamento sera mais rapido
      porque nao e necessario estar sempre a chamar o mktime */
    evento.timestamp = getTimestamp(evento.dataHora);

    if(strlen(componentes[1]) < sizeof(evento.descricao)) {
        strcpy(evento.descricao, componentes[1]);
    }

    strcpy(evento.descricao, componentes[1]);
    evento.duracao = atoi(componentes[4]);
    evento.sala = atoi(componentes[5]);
    if(strlen(componentes[6]) < sizeof(evento.responsavel)) {
        strcpy(evento.responsavel, componentes[6]);
    }

    for(i = 0; i < (numComp - 6); i++) {
        if(strlen(componentes[7 + i]) >= sizeof(evento.participantes[i])) {
			return numeroEventos;
        }
        strcpy(evento.participantes[i], componentes[7 + i]);
    }

    /* como o evento ainda nao foi adicionado aos eventos, e passado um index falso */
    numEventosDecorrer = getEventosDecorrer(eventos, numeroEventos, indexFalso, eventosDecorrer, evento.timestamp, evento.duracao);

    /* verifico se o responsavel ja esta em algum evento
     que se sobreponha ao que estamos a criar */
    responsavelIndisponivel = pessoaIndisponivel(eventosDecorrer, numEventosDecorrer, evento.responsavel);
    if (responsavelIndisponivel != 0) {
        if(strlen(evento.responsavel) < sizeof(listaIndisp[numeroParticipIndisp])) {
            strcpy(listaIndisp[numeroParticipIndisp++], evento.responsavel);
        }
    }
    /* Fazemos as mesmas verificacoes para cada um dos participantes
      e pomos os participantes indisponiveis numa lista */
    for (i = 0; i < MAX_PARTICIPANTES; i++) {
        if (strlen(evento.participantes[i]) > 0) {
            participanteIndisponivel = pessoaIndisponivel(eventosDecorrer, numEventosDecorrer, evento.participantes[i]);
            if (participanteIndisponivel != 0) {
                if(strlen(evento.participantes[i]) < sizeof(listaIndisp[numeroParticipIndisp])) {
                    strcpy(listaIndisp[numeroParticipIndisp++], evento.participantes[i]);
                }
            }
        }
    }

    /* verificamos se existe algum evento que neste intervalo
     de tempo esteja a ocupar a sala que queremos agendar */
    salaOcup = salaOcupada(eventos, numeroEventos, indexFalso, evento.timestamp, evento.duracao, evento.sala);
    if (salaOcup != 0) {
        printf("Impossivel agendar evento %s. Sala%d ocupada.\n", evento.descricao, evento.sala);
    } else {
        if (numeroParticipIndisp != 0) {
            for (i = 0; i < numeroParticipIndisp; i++) {
                printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", evento.descricao, listaIndisp[i]);
            }
        } else {
            /* Incremento o numero de eventos sempre que crio um evento para saber quantos existem no vetor */
            eventos[numeroEventos++] = evento;
        }
    }

    return numeroEventos;
}

/* Funcao que recebe um vetor de componentes, uma string de parametros
 e que divide a string de input nos diferentes parametros e os insere
  no vetor componentes */
void parteString(char componentes[MAX_COMPONENTES][MAX_NAMES_LENGTH], char params[]) {
    int i = 0, idx = 0, compIdx = 1;
    char c;
    char componente[MAX_NAMES_LENGTH];

    memset(componente, '\0', sizeof(char) * MAX_NAMES_LENGTH);

    /* Enquanto a string nao chegar ao fim, vamos dividindo os
      parametros pelo separador ':' e inserimo-los no vetor componentes */
    while ((c = params[i++]) != '\0') {
        if(c == ':') {
            componente[idx] = '\0';
            if(strlen(componente) < sizeof(componentes[compIdx])) {
                strcpy(componentes[compIdx++], componente);
            }
            memset(componente, '\0', sizeof(char) * MAX_NAMES_LENGTH);
            idx = 0;
        } else {
            componente[idx++] = c;
        }
    }
    /* adicionar ultimo elemento */
    if(strlen(componente) < sizeof(componentes[compIdx])) {
        strcpy(componentes[compIdx++], componente);
    }

    /* Adicionar o numero de componentes na primeira posicao */
    sprintf(componentes[0], "%d", (compIdx - 1));
}


/* Funcao que recebe o vetor de eventos, o numero de eventos,
 o indice do evento que queremos eliminar e que
faz exatamente essa acao de remover o evento */ 
int removeEvento(Evento eventos[], int numeroEventos, int indexEvento) {
    int i;

    for (i = 0; i < numeroEventos - 1; i++) {
        if (i >= indexEvento) {
            /* a partir do indice que queremos remover,
            copia o proximo indice para o indice actual */
            eventos[i] = eventos[i + 1];
        }
    }

    /* "apaga" evento no array de eventos (coloca posicao do evento a 'zero') */
    memset(&eventos[numeroEventos--], 0, sizeof(Evento));

    return numeroEventos;
}

/* Funcao que recebe um vetor de eventos, o indice do evento
 que pretende alterar, o numero de eventos no vetor, e a nova
 duracao. Se o evento com a nova duracao passar nos testes de
  verificacao entao o evento fica com a nova duracao */
void alteraDuracao(Evento eventos[], int indexEvento, int numeroEventos, int duracao) {
    int salaOcup = 0, numEventosDecorrer = 0, idx = 0, i, participanteIndisponivel = 0, responsavelIndisponivel = 0;
    /* vetor de eventos que decorrem no intervalo de tempo
     que interseta com o do evento que queremos comparar */
    Evento eventosDecorrer[MAX_EVENTOS] = {{{0}, {0}, 0, 0, 0, {0}, {{0}}}};
    char listaIndisp[(MAX_PARTICIPANTES + 1)][MAX_NAMES_LENGTH];

    memset(listaIndisp, '\0', sizeof(char) * (MAX_PARTICIPANTES + 1) * MAX_NAMES_LENGTH);

    /* Envia a nova duracao para ver quais eventos ocorrem no mesmo intervalo de tempo */
    numEventosDecorrer = getEventosDecorrer(eventos, numeroEventos, indexEvento, eventosDecorrer, eventos[indexEvento].timestamp, duracao);

    /* verifica se o responsavel esta disponivel */
    responsavelIndisponivel = pessoaIndisponivel(eventosDecorrer, numEventosDecorrer, eventos[indexEvento].responsavel);
    if (responsavelIndisponivel != 0) {
        if(strlen(eventos[indexEvento].responsavel) < sizeof(listaIndisp[idx])) {
            strcpy(listaIndisp[idx++], eventos[indexEvento].responsavel);
        }
    }

    /* verifica se os participantes estao disponiveis e se nao
     estiverem insere-os numa lista de indisponibilidade */
    for (i = 0; i < MAX_PARTICIPANTES; i++) {
        if (strlen(eventos[indexEvento].participantes[i]) > 0 ) {
            participanteIndisponivel = pessoaIndisponivel(eventosDecorrer, numEventosDecorrer, eventos[indexEvento].participantes[i]);

            if (participanteIndisponivel != 0) {
                if(strlen(eventos[indexEvento].participantes[i]) < sizeof(listaIndisp[idx])) {
                    strcpy(listaIndisp[idx++], eventos[indexEvento].participantes[i]);
                }
            }
        }
    }

    /* verifica se a sala esta ocupada */
    salaOcup = salaOcupada(eventos, numeroEventos, indexEvento, eventos[indexEvento].timestamp, duracao, eventos[indexEvento].sala);

    if (salaOcup != 0) {
        printf("Impossivel agendar evento %s. Sala%d ocupada.\n", eventos[indexEvento].descricao, eventos[indexEvento].sala);
    }
    else {
        if (idx != 0) {
            for (i = 0; i < idx; i++) {
                printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", eventos[indexEvento].descricao, listaIndisp[i]);
            }
        } else {
            /* Altera a duracao do evento */
            eventos[indexEvento].duracao = duracao;
        }
    }
}

/* Funcao que recebe um vetor de eventos, o indice do evento que
 queremos mudar a sala, o numero da nova sala e que muda a sala
 do evento se o evento com essa nova sala passar nas verificacoes */
void mudaSala(Evento eventos[], int indexEvento, int numeroEventos, int novaSala) {
    int salaOcup = 0;

    /* verifica se a sala esta ocupada */
    salaOcup = salaOcupada(eventos, numeroEventos, indexEvento, eventos[indexEvento].timestamp, eventos[indexEvento].duracao, novaSala);

    if (salaOcup != 0) {
        printf("Impossivel agendar evento %s. Sala%d ocupada.\n", eventos[indexEvento].descricao, novaSala);
    } else {
        /*Muda o evento de sala*/
        eventos[indexEvento].sala = novaSala;
    }
    
}

/* Funcao que recebe um vetor de eventos, o numero de eventos no vetor,
 o indice do evento que pretende comparar, o timestamp inicial,a
 duracao, a sala que pretende verificar e que para as salas com o
 mesmo numero do argumento verifica se ha' uma intersecao nos
 intervalos de tempo em que ocorrem */
int salaOcupada(const Evento eventos[], int numeroEventos,int indexEvento, long timestampInicio, int duracao, int sala) {
    int i;
    long timestampFimSala1 = (timestampInicio + (duracao * 60)), timestampIniSala2, timestampFimSala2;

    for (i = 0; i < numeroEventos; i++) {
        const Evento evento = eventos[i];
        /* se encontrar um evento com o mesmo numero de sala que nao seja o proprio evento que se esta a comparar
        entao corre as verificacoes dos intervalos */
        if ((evento.sala == sala) && (i != indexEvento)) {
            timestampIniSala2 = evento.timestamp;
            timestampFimSala2 = timestampIniSala2 + (evento.duracao * 60);

            /* verifica se ha' intersecao nos intervalos de tempo em que os eventos ocorrem */
            if (((timestampIniSala2 < timestampInicio) && (timestampInicio < timestampFimSala2)) ||
                 ((timestampIniSala2 < timestampFimSala1) && (timestampFimSala1 < timestampFimSala2)) ||
                    ((timestampInicio < timestampIniSala2) && (timestampIniSala2 < timestampFimSala1)) ||
                        ((timestampInicio < timestampFimSala2) && (timestampFimSala2 < timestampFimSala1)) ||
                            ((timestampInicio < timestampIniSala2) && (timestampFimSala2 < timestampFimSala1)) ||
                                ((timestampIniSala2 < timestampInicio) && (timestampFimSala1 < timestampFimSala2)) ||
                                    ((timestampInicio == timestampIniSala2) && (timestampFimSala1 == timestampFimSala2))) {
                return 1;
            }
        }
    }

    return 0;
}

/* Funcao que recebe um vetor de eventos, o indice do evento que
 pretende remover, uma string com o nome do participante que se quer
 remover e que caso passe as verificacoes elimina o participante
 do evento com o indice fornecido */
void removeParticipante(Evento eventos[], int indexEvento, char participante[MAX_NAMES_LENGTH]) {
    int i, numParticipantes = 0, indexParticipante = -1;

    /* Se o participante estiver presente guarda o indice e aumenta o
     numero de participantes sempre que a string nao seja vazia */
    for (i = 0; i < MAX_PARTICIPANTES; i++) {
        if (strcmp(eventos[indexEvento].participantes[i], participante) == 0) {
            indexParticipante = i;
            numParticipantes++;
        }
        else if (strlen(eventos[indexEvento].participantes[i]) > 0) {
            numParticipantes++;
        }
    }

    /* Participante nao esta no evento */
    if (indexParticipante < 0) {
        return;
    }

    if(numParticipantes <= 1) {
        printf("Impossivel remover participante. Participante %s e o unico participante no evento %s.\n", participante, eventos[indexEvento].descricao);
    }    
    else if (indexParticipante >= 0 && numParticipantes > 1) {
        /* Copia os participantes para as posicoes anteriores disponiveis
          depois da remocao, se existirem, e apaga as posicoes a remover */
        for (i = indexParticipante; i < 2 ; i++) {
            if (strlen(eventos[indexEvento].participantes[i + 1]) > 0) {
                if(strlen(eventos[indexEvento].participantes[i + 1]) < sizeof(eventos[indexEvento].participantes[i])) {
                    strcpy(eventos[indexEvento].participantes[i], eventos[indexEvento].participantes[i + 1]);
                }
            }
        }
        for (i = numParticipantes - 1; i < MAX_PARTICIPANTES; i++) {
            memset(eventos[indexEvento].participantes[i], '\0', sizeof(char) * MAX_NAMES_LENGTH);
        }
    }
}

/* Funcao que recebe o vetor de eventos, o indice do evento que
 se pretende alterar, o numero de eventos no vetor, uma string
 com a nova hora e que caso o evento auxiliar, criado com a nova
 hora de inicio, passe nos testes de verificacao entao altera a hora do evento */
void alteraInicio(Evento eventos[], int indexEvento, int numeroEventos, char hora[]) {
    int salaOcup = 0, participanteIndisponivel = 0, i, responsavelIndisponivel = 0, numPartIndisponiveis = 0, numEventosDecorrer = 0;
    /* Crio um evento novo auxiliar com a nova hora de
     inicio e mando-o para dentro das verificacoes */
    Evento evento = eventos[indexEvento];
    Evento    eventosDecorrer[MAX_EVENTOS] = {{{0}, {0}, 0, 0, 0, {0}, {{0}}}};
    char listaIndisp[(MAX_PARTICIPANTES + 1)][MAX_NAMES_LENGTH];

    memset(listaIndisp, '\0', sizeof(char) * (MAX_PARTICIPANTES + 1) * MAX_NAMES_LENGTH);

    /* Ponho a nova hora na estrutura tm de tempo */
    strptime(hora,"%H%M", &evento.dataHora);
    /* Vou buscar o timestamp correspondente a essa nova hora */
    evento.timestamp = getTimestamp(evento.dataHora);

    /* verifico a sala*/
    salaOcup = salaOcupada(eventos, numeroEventos, indexEvento, evento.timestamp, eventos[indexEvento].duracao, eventos[indexEvento].sala);
    if (salaOcup != 0) {
        printf("Impossivel agendar evento %s. Sala%d ocupada.\n", eventos[indexEvento].descricao, eventos[indexEvento].sala);
        return;
    }

    /* vou buscar eventos que estao a decorrer no mesmo periodo
     de tempo que o evento que pretendo comparar */
    numEventosDecorrer = getEventosDecorrer(eventos, numeroEventos, indexEvento, eventosDecorrer, evento.timestamp, eventos[indexEvento].duracao);
    
    /* verifico o responsavel */
    responsavelIndisponivel = pessoaIndisponivel(eventosDecorrer, numEventosDecorrer, eventos[indexEvento].responsavel);
    if (responsavelIndisponivel != 0) {
        if(strlen(eventos[indexEvento].responsavel) < sizeof(listaIndisp[numPartIndisponiveis])) {
            strcpy(listaIndisp[numPartIndisponiveis++], eventos[indexEvento].responsavel);
        }
    }

    /* verifico os participantes */
    for (i = 0; i < MAX_PARTICIPANTES; i++) {
        if (strlen(eventos[indexEvento].participantes[i]) > 0) {
            participanteIndisponivel = pessoaIndisponivel(eventosDecorrer, numEventosDecorrer, eventos[indexEvento].participantes[i]);

            if (participanteIndisponivel != 0) {
                if(strlen(eventos[indexEvento].participantes[i]) < sizeof(listaIndisp[numPartIndisponiveis])) {
                    strcpy(listaIndisp[numPartIndisponiveis++], eventos[indexEvento].participantes[i]);
                }
            }
        }
    }

    if (numPartIndisponiveis > 0) {
        for (i = 0; i < numPartIndisponiveis; i++) {
            printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", eventos[indexEvento].descricao, listaIndisp[i]);
        }
    }
    else {
        /* copio o evento auxiliar para o evento que se pretende alterar */
        eventos[indexEvento] = evento;
    }
}

/* Funcao que recebe um vetor de eventos, o indice do evento ao qual
 se quer adicionar um participante, o numero de eventos do vetor,
 o participante e que, caso os testes de verificacao seja passados,
 adiciona o participante ao evento pretendido */
void adicionaParticipante(Evento eventos[], int indexEvento,int numeroEventos, char participante[MAX_NAMES_LENGTH]) {
    int i, numParticipantes = 0, participanteIndisponivel = 0, numEventosDecorrer = 0;
    /* vetor que tens os eventos cujo periodo de ocorrencia
     interseta o do evento que queremos alterar */
    Evento eventosDecorrer[MAX_EVENTOS] = {{{0}, {0}, 0, 0, 0, {0}, {{0}}}};

    /* vou buscar eventos que estao a decorrer no mesmo periodo
     de tempo que o evento que pretendo comparar */
    numEventosDecorrer = getEventosDecorrer(eventos, numeroEventos, indexEvento, eventosDecorrer, eventos[indexEvento].timestamp, eventos[indexEvento].duracao);

    for (i = 0; i < MAX_PARTICIPANTES ; i++) {
        if (strcmp(eventos[indexEvento].participantes[i], participante) == 0) {
            /* Participante ja esta no evento */
            return;
        }
        else if (strlen(eventos[indexEvento].participantes[i]) > 0) {
            /* conto o numero de participantes */
            numParticipantes++;
        }
    }
    /* verifico se o participante que pretendo adicionar esta disponivel */
    participanteIndisponivel = pessoaIndisponivel(eventosDecorrer, numEventosDecorrer, participante);
    
    if (numParticipantes >= MAX_PARTICIPANTES) {
        printf("Impossivel adicionar participante. Evento %s ja tem %d participantes.\n", eventos[indexEvento].descricao, MAX_PARTICIPANTES);
    }
    else if (participanteIndisponivel != 0) {
        printf("Impossivel adicionar participante. Participante %s tem um evento sobreposto.\n", participante);
    }
    else {
        /* adiciono o participante */
        if(strlen(participante) < sizeof(eventos[indexEvento].participantes[numParticipantes])) {
            strcpy(eventos[indexEvento].participantes[numParticipantes], participante);
        }
    }
}

/* Funcao que recebe os eventos a decorrer num determinado periodo
 de tempo, o numero de eventos nesse vetor, uma string com o nome da
 pessoa e que verifica se essa pessoa esta' nalgum desses eventos
 como responsavel ou como participante */
int pessoaIndisponivel(const Evento eventosDecorrer[], int numeroEventos, char pessoa[]) {
    int i, e;

    /* Ve nos eventos a decorrer no mesmo periodo de tempo que o evento
     que pretendemos comparar se a pessoa jÃ¡ se encontra presente, 
     estando portanto indisponivel*/
    for (i = 0; i < numeroEventos; i++) {
        if (strcmp(pessoa, eventosDecorrer[i].responsavel) == 0) {
            return 1;
        }
        for (e = 0; e < MAX_PARTICIPANTES; e++) {
            if (strcmp(pessoa, eventosDecorrer[i].participantes[e]) == 0) {
                return 1;
            }
        }
    }

    return 0;
}

/* Funcao que recebe o vetor dos eventos, o numeros de eventos nesse vetor,
 o indice do evento que queremos comparar, o vetor onde vamos adicionar os
 eventos que estao a decorrer num determinado periodo de tempo, um timestamp,
 uma duracao e que procura nos eventos agendados se ha' algum evento que interseta
 o intervalo de tempo calculado a partir do timestamp e a duracao de um evento
 sendo que depois insere esses eventos no vetor eventosDecorrer */    
int getEventosDecorrer(const Evento eventos[], int numeroEventos, int indexEvento, Evento eventosDecorrer[], long timestamp, int duracao) {
    int i, intersecaoIntervalos = 0, numeroEventosDecorrer = 0;

    /* Usa o comparaTimestamps para ver se ha intersecao entre
     os periodos de ocorrerncia dos eventos */
    for (i = 0; i < numeroEventos; i++) {
        if (i != indexEvento) {
            intersecaoIntervalos = comparaTimestamps(timestamp, duracao, eventos[i].timestamp, eventos[i].duracao);
            /* se houver intersecao mete-os no vetor correspondente*/
            if (intersecaoIntervalos != 0) {
                eventosDecorrer[numeroEventosDecorrer++] = eventos[i];
            }
        }
    }

    return numeroEventosDecorrer;
}

/* Funcao que recebe dois timestamps e duas duracoes correspondentes
 a eventos diferentes e verifica se existe ou nao intersecao */
int comparaTimestamps(long inicioEvento1, int duracao1, long inicioEvento2, int duracao2) {
    long fimEvento1 = inicioEvento1 + (duracao1 * 60);
    long fimEvento2 = inicioEvento2 + (duracao2 * 60);

    if(inicioEvento2 == fimEvento1 || inicioEvento1 == fimEvento2) {
        /* se os eventos coincidem entao houve intersecao */
        return 0;
    }

    if((inicioEvento2 >= inicioEvento1 && inicioEvento2 <= fimEvento1) ||
        (inicioEvento2 < inicioEvento1 && fimEvento2 >= inicioEvento1 && fimEvento2 <= fimEvento1) ||
            (inicioEvento2 >= inicioEvento1 && inicioEvento2 < fimEvento1 && fimEvento2 >= fimEvento1) ||
                (inicioEvento1 < inicioEvento2 && fimEvento1 > fimEvento2) || (inicioEvento2 < inicioEvento1 && fimEvento2 > fimEvento1)) {
        /* ha intersecao de eventos */
        return 1;
    }

    return 0;
}
