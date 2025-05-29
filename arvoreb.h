#ifndef ARVOREB_H
#define ARVOREB_H

#include <stdbool.h>
#include <stdio.h>


#define ORDEM 4

typedef struct {
    int codigo;
    long pos;
} RegistroIndice;

typedef struct NoB {
    int num_chaves;
    RegistroIndice chaves[ORDEM-1];
    struct NoB *filhos[ORDEM];
    bool eh_folha;
} NoB;

extern NoB *raiz;

// Interface da Árvore B
void inicializarArvoreB();
bool inserirNaArvoreB(RegistroIndice reg);
long buscarNaArvoreB(int codigo);
bool removerDaArvoreB(int codigo);
void salvarIndice();
void percorrerEmOrdem(NoB *no, FILE *f);
void liberarArvore(NoB *no);
#endif