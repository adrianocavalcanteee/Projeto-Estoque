// === arvoreb.c ===
#include "arvoreb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INDICES 1000
RegistroIndice indices[MAX_INDICES];
int totalIndices = 0;

void inicializarArvoreB() {
    FILE *f = fopen("dados/indice.idx", "rb");
    if (f) {
        fread(&totalIndices, sizeof(int), 1, f);
        fread(indices, sizeof(RegistroIndice), totalIndices, f);
        fclose(f);
    }
}

void salvarIndice() {
    FILE *f = fopen("dados/indice.idx", "wb");
    if (f) {
        fwrite(&totalIndices, sizeof(int), 1, f);
        fwrite(indices, sizeof(RegistroIndice), totalIndices, f);
        fclose(f);
    }
}

void inserirNaArvoreB(RegistroIndice reg) {
    for (int i = 0; i < totalIndices; i++) {
        if (indices[i].codigo == reg.codigo) return;
    }
    indices[totalIndices++] = reg;
    salvarIndice();
}

long buscarNaArvoreB(int codigo) {
    for (int i = 0; i < totalIndices; i++) {
        if (indices[i].codigo == codigo) return indices[i].pos;
    }
    return -1;
}

void removerDaArvoreB(int codigo) {
    for (int i = 0; i < totalIndices; i++) {
        if (indices[i].codigo == codigo) {
            for (int j = i; j < totalIndices - 1; j++) {
                indices[j] = indices[j + 1];
            }
            totalIndices--;
            salvarIndice();
            return;
        }
    }
}
