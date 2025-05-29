#include "arvoreb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"


NoB *raiz = NULL;

static NoB *criarNo() {
    NoB *novo = malloc(sizeof(NoB));
    novo->eh_folha = true;
    novo->num_chaves = 0;
    for (int i = 0; i < ORDEM; i++) {
        novo->filhos[i] = NULL;
    }
    return novo;
}

// --- SERIALIZAÇÃO ---

static void salvarNo(FILE *f, NoB *no) {
    if (!no) {
        bool marcador = false;
        fwrite(&marcador, sizeof(bool), 1, f);
        return;
    }

    bool marcador = true;
    fwrite(&marcador, sizeof(bool), 1, f);

    fwrite(&no->eh_folha, sizeof(bool), 1, f);
    fwrite(&no->num_chaves, sizeof(int), 1, f);
    fwrite(no->chaves, sizeof(RegistroIndice), ORDEM - 1, f);

    for (int i = 0; i < ORDEM; i++) {
        salvarNo(f, no->filhos[i]);
    }
}

void salvarIndice() {
    FILE *f = fopen("dados/indice.idx", "wb");
    if (!f) return;
    salvarNo(f, raiz);
    fclose(f);
}

// --- DESSERIALIZAÇÃO ---

static NoB *carregarNo(FILE *f) {
    bool marcador;
    if (fread(&marcador, sizeof(bool), 1, f) != 1 || !marcador)
        return NULL;

    NoB *no = criarNo();

    fread(&no->eh_folha, sizeof(bool), 1, f);
    fread(&no->num_chaves, sizeof(int), 1, f);
    fread(no->chaves, sizeof(RegistroIndice), ORDEM - 1, f);

    for (int i = 0; i < ORDEM; i++) {
        no->filhos[i] = carregarNo(f);
    }

    return no;
}

void inicializarArvoreB() {
    FILE *f = fopen("dados/indice.idx", "rb");
    if (!f) return;
    raiz = carregarNo(f);
    fclose(f);
}

// --- DEMAIS FUNÇÕES (inserção mínima para funcionar) ---

static void dividirNo(NoB *pai, int index);

static void inserirNaArvoreBRec(NoB *no, RegistroIndice reg) {
    int i = no->num_chaves - 1;

    if (no->eh_folha) {
        while (i >= 0 && reg.codigo < no->chaves[i].codigo) {
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }
        no->chaves[i + 1] = reg;
        no->num_chaves++;
    } else {
        while (i >= 0 && reg.codigo < no->chaves[i].codigo)
            i--;

        i++;
        if (no->filhos[i]->num_chaves == ORDEM - 1) {
            dividirNo(no, i);
            if (reg.codigo > no->chaves[i].codigo)
                i++;
        }
        inserirNaArvoreBRec(no->filhos[i], reg);
    }
}

static void dividirNo(NoB *pai, int index) {
    NoB *filho = pai->filhos[index];
    NoB *novo = criarNo();
    novo->eh_folha = filho->eh_folha;
    novo->num_chaves = ORDEM / 2 - 1;

    for (int i = 0; i < ORDEM / 2 - 1; i++)
        novo->chaves[i] = filho->chaves[i + ORDEM / 2];

    if (!filho->eh_folha) {
        for (int i = 0; i < ORDEM / 2; i++)
            novo->filhos[i] = filho->filhos[i + ORDEM / 2];
    }

    filho->num_chaves = ORDEM / 2 - 1;

    for (int i = pai->num_chaves; i >= index + 1; i--)
        pai->filhos[i + 1] = pai->filhos[i];

    pai->filhos[index + 1] = novo;

    for (int i = pai->num_chaves - 1; i >= index; i--)
        pai->chaves[i + 1] = pai->chaves[i];

    pai->chaves[index] = filho->chaves[ORDEM / 2 - 1];
    pai->num_chaves++;
}

bool inserirNaArvoreB(RegistroIndice reg) {
    if (buscarNaArvoreB(reg.codigo) != -1)
        return false;

    if (!raiz) {
        raiz = criarNo();
        raiz->chaves[0] = reg;
        raiz->num_chaves = 1;
        return true;
    }

    if (raiz->num_chaves == ORDEM - 1) {
        NoB *novo = criarNo();
        novo->eh_folha = false;
        novo->filhos[0] = raiz;
        dividirNo(novo, 0);

        int i = 0;
        if (novo->chaves[0].codigo < reg.codigo)
            i++;
        inserirNaArvoreBRec(novo->filhos[i], reg);
        raiz = novo;
    } else {
        inserirNaArvoreBRec(raiz, reg);
    }

    return true;
}

long buscarNaArvoreB(int codigo) {
    NoB *atual = raiz;
    while (atual) {
        int i = 0;
        while (i < atual->num_chaves && codigo > atual->chaves[i].codigo)
            i++;

        if (i < atual->num_chaves && codigo == atual->chaves[i].codigo)
            return atual->chaves[i].pos;

        if (atual->eh_folha)
            return -1;
        atual = atual->filhos[i];
    }
    return -1;
}

void percorrerEmOrdem(NoB *no, FILE *f) {
    if (!no) return;
    for (int i = 0; i < no->num_chaves; i++) {
        percorrerEmOrdem(no->filhos[i], f);
        long pos = no->chaves[i].pos;
        fseek(f, pos, SEEK_SET);
        char linha[REGISTRO_TAM + 2];
        if (fgets(linha, sizeof(linha), f))
            printf("%s", linha);
    }
    percorrerEmOrdem(no->filhos[no->num_chaves], f);
}

void liberarArvore(NoB *no) {
    if (!no) return;
    for (int i = 0; i < ORDEM; i++) {
        liberarArvore(no->filhos[i]);
    }
    free(no);
}

bool removerDaArvoreB(int codigo) {
    // Opcional – ainda não implementado
    return false;
}
