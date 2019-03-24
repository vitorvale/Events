#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_SALAS 10
#define MAX_EVENTOS_SALA 100
#define MAX_PARTICIPANTES 3

/*    char data[9]; */
/*    char hora[5]; */

typedef struct {
    char descricao[64];
    struct tm data;
    int duracao;
    int sala;
    char responsavel[64];
    char participantes[(64 * 3) + 3];
} Evento;

typedef struct {
    Evento eventos[100];
} Sala;

void listaEventos(Evento eventos[], int numeroEventos);
int criaEvento(Evento eventos[], int numeroEventos, char camposEvento[]);

int main(int argc, char *argv[]) {
    int c;

    Evento eventos[MAX_SALAS * MAX_EVENTOS_SALA];
    int numeroEventos = 0;

    /*
    for (c = 0; c < argc; c++) {
        printf("%d argument is %s\n", c + 1, argv[c]);
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

