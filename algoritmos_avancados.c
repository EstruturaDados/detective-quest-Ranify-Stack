#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//
// -----------------------------------------------
//                ESTRUTURAS
// -----------------------------------------------
//

// --------- ÁRVORE DE SALAS ---------
typedef struct Sala {
    char nome[40];
    struct Sala *esq;
    struct Sala *dir;
} Sala;

// --------- ÁRVORE DE PISTAS (BST) ---------
typedef struct Pista {
    char texto[80];
    struct Pista *esq;
    struct Pista *dir;
} Pista;

// --------- HASH DE SUSPEITOS ---------
typedef struct NoSus {
    char nome[40];
    char pistas[10][80];
    int qtdPistas;
    int contador;       
    struct NoSus *prox;
} NoSus;

#define TAM_HASH 26
NoSus *tabela[TAM_HASH];

//
// -----------------------------------------------
//        FUNÇÕES: SALAS (Árvore Binária)
// -----------------------------------------------
//

Sala* criarSala(char *nome) {
    Sala *s = malloc(sizeof(Sala));
    strcpy(s->nome, nome);
    s->esq = s->dir = NULL;
    return s;
}

void explorarSalas(Sala *atual) {
    char op;

    while (1) {
        printf("\nVocê está em: %s\n", atual->nome);
        printf("[e] esquerda, [d] direita, [s] sair exploração: ");
        scanf(" %c", &op);

        if (op == 's')
            break;

        if (op == 'e') {
            if (atual->esq)
                atual = atual->esq;
            else
                printf("Não há sala à esquerda!\n");
        } else if (op == 'd') {
            if (atual->dir)
                atual = atual->dir;
            else
                printf("Não há sala à direita!\n");
        } else {
            printf("Opção inválida!\n");
        }
    }
}

//
// -----------------------------------------------
//     FUNÇÕES: PISTAS (Árvore de Busca BST)
// -----------------------------------------------
//

Pista* inserirBST(Pista *raiz, char *texto) {
    if (!raiz) {
        Pista *p = malloc(sizeof(Pista));
        strcpy(p->texto, texto);
        p->esq = p->dir = NULL;
        return p;
    }

    if (strcmp(texto, raiz->texto) < 0)
        raiz->esq = inserirBST(raiz->esq, texto);
    else
        raiz->dir = inserirBST(raiz->dir, texto);

    return raiz;
}

void emOrdem(Pista *raiz) {
    if (!raiz) return;
    emOrdem(raiz->esq);
    printf("- %s\n", raiz->texto);
    emOrdem(raiz->dir);
}

//
// -----------------------------------------------
//     FUNÇÕES: TABELA HASH DE SUSPEITOS
// -----------------------------------------------
//

int hash(char *nome) {
    return toupper(nome[0]) - 'A';
}

void inicializarHash() {
    for (int i = 0; i < TAM_HASH; i++)
        tabela[i] = NULL;
}

NoSus* buscarSuspeito(char *nome) {
    int h = hash(nome);
    NoSus *aux = tabela[h];

    while (aux) {
        if (strcmp(aux->nome, nome) == 0)
            return aux;
        aux = aux->prox;
    }
    return NULL;
}

void inserirHash(char *suspeito, char *pista) {
    int h = hash(suspeito);
    NoSus *no = buscarSuspeito(suspeito);

    if (!no) {
        no = malloc(sizeof(NoSus));
        strcpy(no->nome, suspeito);
        no->qtdPistas = 0;
        no->contador = 0;
        no->prox = tabela[h];
        tabela[h] = no;
    }

    strcpy(no->pistas[no->qtdPistas++], pista);
    no->contador++;   // conta frequência
}

void listarAssociacoes() {
    printf("\n=== Suspeitos e Pistas ===\n");

    for (int i = 0; i < TAM_HASH; i++) {
        NoSus *aux = tabela[i];
        while (aux) {
            printf("\nSuspeito: %s\n", aux->nome);
            for (int j = 0; j < aux->qtdPistas; j++)
                printf("  - %s\n", aux->pistas[j]);
            aux = aux->prox;
        }
    }
}

void suspeitoMaisProvavel() {
    NoSus *melhor = NULL;

    for (int i = 0; i < TAM_HASH; i++) {
        NoSus *aux = tabela[i];
        while (aux) {
            if (!melhor || aux->contador > melhor->contador)
                melhor = aux;
            aux = aux->prox;
        }
    }

    if (melhor)
        printf("\n🎯 Suspeito mais provável: %s (%d pistas)\n", melhor->nome, melhor->contador);
    else
        printf("\nNenhum suspeito registrado.\n");
}

//
// -----------------------------------------------
//                     MAIN
// -----------------------------------------------
//

int main() {
    // ---------------- Árvore de salas ----------------
    Sala *hall = criarSala("Hall de Entrada");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *cozinha = criarSala("Cozinha");
    Sala *sotao = criarSala("Sótão");
    Sala *porao = criarSala("Porão");

    // Conexões fixas
    hall->esq = biblioteca;
    hall->dir = cozinha;

    biblioteca->esq = sotao;
    biblioteca->dir = porao;

    // ---------------- BST de pistas ----------------
    Pista *arvorePistas = NULL;

    // ---------------- Hash ----------------
    inicializarHash();

    int op;

    do {
        printf("\n\n=== DETECTIVE QUEST ===\n");
        printf("1 - Explorar salas\n");
        printf("2 - Adicionar pista manualmente\n");
        printf("3 - Listar pistas (ordem alfabética)\n");
        printf("4 - Relacionar pista com suspeito\n");
        printf("5 - Mostrar suspeitos e pistas\n");
        printf("6 - Suspeito mais provável\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &op);

        if (op == 1) {
            explorarSalas(hall);
        }
        else if (op == 2) {
            char texto[80];
            printf("Digite a pista: ");
            getchar();  // limpar buffer
            fgets(texto, 80, stdin);
            texto[strcspn(texto, "\n")] = 0;

            arvorePistas = inserirBST(arvorePistas, texto);
            printf("Pista adicionada!\n");
        }
        else if (op == 3) {
            printf("\n=== Pistas Coletadas ===\n");
            if (!arvorePistas) printf("Nenhuma pista.\n");
            emOrdem(arvorePistas);
        }
        else if (op == 4) {
            char pista[80], suspeito[40];

            getchar();
            printf("Pista: ");
            fgets(pista, 80, stdin);
            pista[strcspn(pista, "\n")] = '\0';

            printf("Suspeito: ");
            fgets(suspeito, 40, stdin);
            suspeito[strcspn(suspeito, "\n")] = '\0';

            inserirHash(suspeito, pista);
            printf("Relação registrada!\n");
        }
        else if (op == 5) {
            listarAssociacoes();
        }
        else if (op == 6) {
            suspeitoMaisProvavel();
        }

    } while (op != 0);

    return 0;
}
