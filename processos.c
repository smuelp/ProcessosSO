/* Alunos: Samuel I. Pena e Luiz Phellipe Aguiar */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Processo {
    char nome[10];
    int tempoExecucao;
    struct Processo *prox;
};

struct Fila {
    struct Processo *frente;
    struct Processo *tras;
};

void enfileirar(struct Fila *fila, struct Processo *processo) {
    if (fila->tras == NULL) {
        fila->frente = fila->tras = processo;
    } else {
        fila->tras->prox = processo;
        fila->tras = processo;
    }
    processo->prox = NULL;
}

struct Processo *desenfileirar(struct Fila *fila) {
    if (fila->frente == NULL) {
        return NULL;
    }
    struct Processo *processo = fila->frente;
    fila->frente = fila->frente->prox;
    if (fila->frente == NULL) {
        fila->tras = NULL;
    }
    return processo;
}

void exibirFilas(struct Fila *pronto, struct Fila *espera, struct Fila *execucao, struct Fila *finalizado) {
    printf("Estado atual das filas de processos:\n");
    
    printf("Pronto: ");
    struct Processo *p = pronto->frente;
    if (p == NULL) {
        printf("vazia");
    }
    while (p != NULL) {
        printf("%s = %d", p->nome, p->tempoExecucao);
        p = p->prox;
        if (p != NULL) {
            printf(" <- ");
        }
    }

    printf("\nEspera: ");
    p = espera->frente;
    if (p == NULL) {
        printf("vazia");
    }
    while (p != NULL) {
        printf("%s = %d", p->nome, p->tempoExecucao);
        p = p->prox;
        if (p != NULL) {
            printf(" <- ");
        }
    }

    printf("\nExecucao: ");
    p = execucao->frente;
    if (p == NULL) {
        printf("vazia");
    }
    while (p != NULL) {
        printf("%s = %d", p->nome, p->tempoExecucao);
        p = p->prox;
        if (p != NULL) {
            printf(" <- ");
        }
    }

    printf("\nFinalizados: ");
    p = finalizado->frente;
    if (p == NULL) {
        printf("vazia");
    }
    while (p != NULL) {
        printf("%s = %d", p->nome, p->tempoExecucao);
        p = p->prox;
        if (p != NULL) {
            printf(" <- ");
        }
    }
    printf("\n");
}

void mudarFila(struct Fila *origem, struct Fila *destino) {
    for (int i = 0; origem->frente != NULL; i++) {
        struct Processo *processo = desenfileirar(origem);
        enfileirar(destino, processo);
    }
}

int main() {

    char fPath[100];

    scanf("%s", fPath);
    FILE* arquivo = fopen(fPath, "r");
    if (arquivo == NULL) {
        printf("Arquivo nao encontrado.");
        return 1;
    }

    int quantum;
    fscanf(arquivo, "%d", &quantum);

    struct Fila pronto = { NULL, NULL };
    struct Fila espera = { NULL, NULL };
    struct Fila execucao = { NULL, NULL };
    struct Fila finalizado = { NULL, NULL };

    while (!feof(arquivo)) {
        struct Processo *processo = (struct Processo*)malloc(sizeof(struct Processo));
        fscanf(arquivo, "%s %d", processo->nome, &processo->tempoExecucao);
        enfileirar(&pronto, processo);
    }

    fclose(arquivo);

    exibirFilas(&pronto, &espera, &execucao, &finalizado);

    printf("\nQuantum: %d", quantum);

    printf("\n\n----------> Iniciando execucao\n");

    while (pronto.frente != NULL || espera.frente != NULL || execucao.frente != NULL) {
        if (execucao.frente == NULL && espera.frente != NULL) {
            mudarFila(&espera, &execucao);
            printf("\n-------------------------------------\n");
            printf("A fila de execucao esta vazia, movendo processos para a fila de execucao\n");
            exibirFilas(&pronto, &espera, &execucao, &finalizado);
        }

        if (execucao.frente == NULL && pronto.frente != NULL) {
            mudarFila(&pronto, &execucao);
            //printf("\n-------------------------------------\n");
            printf("Processos foram adicionados a fila de execucao\n");
            exibirFilas(&pronto, &espera, &execucao, &finalizado);
        }

        if (execucao.frente != NULL) {
            struct Processo* processo = execucao.frente;
            printf("%s esta executando...\n", processo->nome);
            if (processo->tempoExecucao <= quantum) {
                printf("%s terminou a execucao, %s foi adicionado a fila de finalizados\n", processo->nome, processo->nome);
                processo->tempoExecucao = 0;
                enfileirar(&finalizado, desenfileirar(&execucao));
                if (execucao.frente == NULL && (pronto.frente != NULL || espera.frente != NULL)) {
                    mudarFila(&espera, &pronto);
                    printf("\n-------------------------------------\n");
                    printf("A fila de execucao esta vazia, movendo processos para a fila de prontos\n");
                    exibirFilas(&pronto, &espera, &execucao, &finalizado);
                    printf("\n-------------------------------------\n");
                } else{
                    printf("\n-------------------------------------\n");
                    exibirFilas(&pronto, &espera, &execucao, &finalizado);
                }
            } else {
                printf("Quantum expirou, %s sofreu preempcao\n", processo->nome);
                processo->tempoExecucao -= quantum;
                enfileirar(&espera, desenfileirar(&execucao));
                printf("\n%s foi adicionado a fila de espera\n", processo->nome);
                if (execucao.frente == NULL) {
                    mudarFila(&espera, &pronto);
                    printf("\n-------------------------------------\n");
                    printf("A fila de execucao esta vazia, movendo processos para a fila de prontos\n");
                    exibirFilas(&pronto, &espera, &execucao, &finalizado);
                    printf("\n-------------------------------------\n");
                } else {
                    printf("\n-------------------------------------\n");
                    exibirFilas(&pronto, &espera, &execucao, &finalizado);
                }
            }
        }
    }

    printf("Nao ha mais processos a serem executados\n----------> Simulacao finalizada\n");

    return 0;
}