// === arvoreb.h ===
#ifndef ARVOREB_H
#define ARVOREB_H

typedef struct {
    int codigo;
    long pos;
} RegistroIndice;

extern RegistroIndice indices[];
extern int totalIndices;

void inicializarArvoreB();
void inserirNaArvoreB(RegistroIndice reg);
long buscarNaArvoreB(int codigo);
void removerDaArvoreB(int codigo);
void salvarIndice();

#endif
