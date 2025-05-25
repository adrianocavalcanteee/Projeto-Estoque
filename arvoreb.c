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
    // Verifica duplicatas primeiro
    for (int i = 0; i < totalIndices; i++) {
        if (indices[i].codigo == reg.codigo) {
            printf("Aviso: Codigo %d ja existe no indice (pos: %ld)\n", reg.codigo, indices[i].pos);
            return;
        }
    }
    
    // Verifica capacidade
    if (totalIndices >= MAX_INDICES) {
        printf("Erro: Limite maximo de indices atingido!\n");
        return;
    }
    
    // Insere ordenado
    int i = totalIndices - 1;
    while (i >= 0 && indices[i].codigo > reg.codigo) {
        indices[i + 1] = indices[i];
        i--;
    }
    indices[i + 1] = reg;
    totalIndices++;
    
    salvarIndice();
}

long buscarNaArvoreB(int codigo) {
    // Busca binária para melhor performance
    int inicio = 0, fim = totalIndices - 1;
    
    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        
        if (indices[meio].codigo == codigo)
            return indices[meio].pos;
            
        if (indices[meio].codigo < codigo)
            inicio = meio + 1;
        else
            fim = meio - 1;
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
