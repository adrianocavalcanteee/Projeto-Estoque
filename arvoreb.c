#include "arvoreb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"


NoB *raiz = NULL;

// Função auxiliar para criar um novo nó
static NoB *criarNo() {
    NoB *novo = malloc(sizeof(NoB));
    if (!novo) return NULL;
    
    novo->eh_folha = true;
    novo->num_chaves = 0;
    memset(novo->chaves, 0, sizeof(RegistroIndice) * (ORDEM - 1));
    memset(novo->filhos, 0, sizeof(NoB *) * ORDEM);
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
    
    // Salvar apenas as chaves válidas
    fwrite(no->chaves, sizeof(RegistroIndice), no->num_chaves, f);
    
    // Salvar espaço vazio para manter o formato fixo
    if (no->num_chaves < ORDEM - 1) {
        RegistroIndice vazio = {0, -1};
        for (int i = no->num_chaves; i < ORDEM - 1; i++) {
            fwrite(&vazio, sizeof(RegistroIndice), 1, f);
        }
    }

    for (int i = 0; i <= no->num_chaves; i++) {
        salvarNo(f, no->filhos[i]);
    }
}

void salvarIndice() {
    FILE *f = fopen("dados/indice.idx", "wb");
    if (!f) {
        perror("Erro ao abrir arquivo de indice para escrita");
        return;
    }
    salvarNo(f, raiz);
    fclose(f);
}

// --- DESSERIALIZAÇÃO ---

static NoB *carregarNo(FILE *f) {
    bool marcador;
    if (fread(&marcador, sizeof(bool), 1, f) != 1 || !marcador)
        return NULL;

    NoB *no = criarNo();
    if (!no) return NULL;

    if (fread(&no->eh_folha, sizeof(bool), 1, f) != 1 ||
        fread(&no->num_chaves, sizeof(int), 1, f) != 1) {
        free(no);
        return NULL;
    }

    // Carregar todas as chaves (válidas e inválidas)
    if (fread(no->chaves, sizeof(RegistroIndice), ORDEM - 1, f) != ORDEM - 1) {
        free(no);
        return NULL;
    }

    for (int i = 0; i <= no->num_chaves; i++) {
        no->filhos[i] = carregarNo(f);
        if (i < no->num_chaves && !no->filhos[i] && !no->eh_folha) {
            // Erro na carga de um filho necessário
            for (int j = 0; j < i; j++) liberarArvore(no->filhos[j]);
            free(no);
            return NULL;
        }
    }

    return no;
}

void inicializarArvoreB() {
    liberarArvore(raiz);
    raiz = NULL;
    
    FILE *f = fopen("dados/indice.idx", "rb");
    if (!f) return;
    
    raiz = carregarNo(f);
    fclose(f);
    
    if (!raiz) {
        printf("Aviso: Nao foi possivel carregar o indice. Criando novo.\n");
    }
}

// --- OPERAÇÕES DA ÁRVORE B ---

static void dividirNo(NoB *pai, int index) {
    NoB *filho = pai->filhos[index];
    NoB *novo = criarNo();
    if (!novo) return;
    
    novo->eh_folha = filho->eh_folha;
    novo->num_chaves = (ORDEM / 2) - 1;

    // Copiar metade superior do filho para o novo nó
    for (int i = 0; i < novo->num_chaves; i++) {
        novo->chaves[i] = filho->chaves[i + (ORDEM / 2)];
    }

    if (!filho->eh_folha) {
        for (int i = 0; i <= novo->num_chaves; i++) {
            novo->filhos[i] = filho->filhos[i + (ORDEM / 2)];
            filho->filhos[i + (ORDEM / 2)] = NULL;
        }
    }

    filho->num_chaves = (ORDEM / 2) - 1;

    // Abrir espaço no pai para o novo filho
    for (int i = pai->num_chaves; i > index; i--) {
        pai->filhos[i + 1] = pai->filhos[i];
        pai->chaves[i] = pai->chaves[i - 1];
    }

    pai->filhos[index + 1] = novo;
    pai->chaves[index] = filho->chaves[(ORDEM / 2) - 1];
    pai->num_chaves++;
}

bool inserirNaArvoreB(RegistroIndice reg) {
    if (buscarNaArvoreB(reg.codigo) != -1) {
        return false; // Chave duplicada
    }

    if (!raiz) {
        raiz = criarNo();
        if (!raiz) return false;
        raiz->chaves[0] = reg;
        raiz->num_chaves = 1;
        return true;
    }

    // Caso a raiz esteja cheia
    if (raiz->num_chaves == ORDEM - 1) {
        NoB *novaRaiz = criarNo();
        if (!novaRaiz) return false;
        
        novaRaiz->eh_folha = false;
        novaRaiz->filhos[0] = raiz;
        raiz = novaRaiz;
        
        dividirNo(raiz, 0);
    }

    // Inserção recursiva
    NoB *atual = raiz;
    while (!atual->eh_folha) {
        int i = 0;
        while (i < atual->num_chaves && reg.codigo > atual->chaves[i].codigo) {
            i++;
        }

        // Verificar se o filho está cheio
        if (atual->filhos[i]->num_chaves == ORDEM - 1) {
            dividirNo(atual, i);
            // Decidir qual filho seguir após a divisão
            if (reg.codigo > atual->chaves[i].codigo) {
                i++;
            }
        }
        atual = atual->filhos[i];
    }

    // Inserir na folha
    int i = atual->num_chaves - 1;
    while (i >= 0 && reg.codigo < atual->chaves[i].codigo) {
        atual->chaves[i + 1] = atual->chaves[i];
        i--;
    }
    atual->chaves[i + 1] = reg;
    atual->num_chaves++;
    return true;
}

long buscarNaArvoreB(int codigo) {
    NoB *atual = raiz;
    while (atual) {
        int i = 0;
        while (i < atual->num_chaves && codigo > atual->chaves[i].codigo) {
            i++;
        }

        if (i < atual->num_chaves && codigo == atual->chaves[i].codigo) {
            return atual->chaves[i].pos;
        }

        if (atual->eh_folha) {
            return -1;
        }
        atual = atual->filhos[i];
    }
    return -1;
}

void percorrerEmOrdem(NoB *no, FILE *f) {
    if (!no) return;
    
    for (int i = 0; i < no->num_chaves; i++) {
        percorrerEmOrdem(no->filhos[i], f);
        
        long pos = no->chaves[i].pos;
        if (pos >= 0) {
            fseek(f, pos, SEEK_SET);
            char linha[REGISTRO_TAM + 2];
            if (fgets(linha, sizeof(linha), f)) {
                printf("%s", linha);
            }
        }
    }
    percorrerEmOrdem(no->filhos[no->num_chaves], f);
}

void liberarArvore(NoB *no) {
    if (!no) return;
    
    for (int i = 0; i <= no->num_chaves; i++) {
        liberarArvore(no->filhos[i]);
    }
    free(no);
}
// --- FUNÇÕES DE BALANCEAMENTO ---

// Função auxiliar para pegar o predecessor (maior chave na subárvore esquerda)
static RegistroIndice getPredecessor(NoB *no, int idx) {
    NoB *atual = no->filhos[idx];
    while (!atual->eh_folha) {
        atual = atual->filhos[atual->num_chaves];
    }
    return atual->chaves[atual->num_chaves - 1];
}

// Função auxiliar para pegar o sucessor (menor chave na subárvore direita)
static RegistroIndice getSuccessor(NoB *no, int idx) {
    NoB *atual = no->filhos[idx + 1];
    while (!atual->eh_folha) {
        atual = atual->filhos[0];
    }
    return atual->chaves[0];
}

// Redistribui chaves entre nós irmãos
static void redistribuir(NoB *pai, int idx, NoB *filho, NoB *irmao, bool isIrmaoEsquerdo) {
    if (isIrmaoEsquerdo) {
        // Movimentação para a direita (irmão à esquerda)
        filho->num_chaves++;

        // Abrir espaço no filho
        for (int i = filho->num_chaves - 1; i > 0; i--) {
            filho->chaves[i] = filho->chaves[i - 1];
        }
        for (int i = filho->num_chaves; i > 0; i--) {
            filho->filhos[i] = filho->filhos[i - 1];
        }

        // Mover chave do pai para o filho
        filho->chaves[0] = pai->chaves[idx - 1];

        // Mover último filho do irmão para o filho
        if (!irmao->eh_folha) {
            filho->filhos[0] = irmao->filhos[irmao->num_chaves];
        }

        // Mover última chave do irmão para o pai
        pai->chaves[idx - 1] = irmao->chaves[irmao->num_chaves - 1];

        irmao->num_chaves--;
    } else {
        // Movimentação para a esquerda (irmão à direita)
        filho->chaves[filho->num_chaves] = pai->chaves[idx];

        if (!irmao->eh_folha) {
            filho->filhos[filho->num_chaves + 1] = irmao->filhos[0];
        }

        pai->chaves[idx] = irmao->chaves[0];

        // Deslocar chaves e filhos do irmão para a esquerda
        for (int i = 1; i < irmao->num_chaves; i++) {
            irmao->chaves[i - 1] = irmao->chaves[i];
        }

        if (!irmao->eh_folha) {
            for (int i = 1; i <= irmao->num_chaves; i++) {
                irmao->filhos[i - 1] = irmao->filhos[i];
            }
        }

        filho->num_chaves++;
        irmao->num_chaves--;
    }
}


// Concatena dois nós filhos e a chave do pai entre eles
static void concatenar(NoB *pai, int idx, NoB *filho, NoB *irmao) {
    // Mover chave do pai para o filho
    filho->chaves[ORDEM/2 - 1] = pai->chaves[idx];
    
    // Copiar chaves e filhos do irmão para o filho
    for (int i = 0; i < irmao->num_chaves; i++) {
        filho->chaves[i + ORDEM/2] = irmao->chaves[i];
    }
    if (!irmao->eh_folha) {
        for (int i = 0; i <= irmao->num_chaves; i++) {
            filho->filhos[i + ORDEM/2] = irmao->filhos[i];
        }
    }
    
    // Ajustar contagem de chaves
    filho->num_chaves += irmao->num_chaves + 1;
    
    // Remover chave do pai e ajustar filhos
    for (int i = idx; i < pai->num_chaves - 1; i++) {
        pai->chaves[i] = pai->chaves[i + 1];
        pai->filhos[i + 1] = pai->filhos[i + 2];
    }
    pai->num_chaves--;
    
    free(irmao);
}

// Função principal de remoção com balanceamento
static bool removerRec(NoB *no, int codigo) {
    int idx = 0;
    while (idx < no->num_chaves && codigo > no->chaves[idx].codigo) {
        idx++;
    }
    
    // Caso 1: A chave está neste nó
    if (idx < no->num_chaves && codigo == no->chaves[idx].codigo) {
        if (no->eh_folha) {
            // Caso 1a: Remoção em folha
            for (int i = idx; i < no->num_chaves - 1; i++) {
                no->chaves[i] = no->chaves[i + 1];
            }
            no->num_chaves--;
            return true;
        } else {
            // Caso 1b: Remoção em nó interno
            NoB *filhoEsq = no->filhos[idx];
            NoB *filhoDir = no->filhos[idx + 1];
            
            if (filhoEsq->num_chaves >= ORDEM/2) {
                // Subcaso 1b-i: Pegar predecessor
                RegistroIndice pred = getPredecessor(no, idx);
                no->chaves[idx] = pred;
                return removerRec(filhoEsq, pred.codigo);
            } else if (filhoDir->num_chaves >= ORDEM/2) {
                // Subcaso 1b-ii: Pegar sucessor
                RegistroIndice succ = getSuccessor(no, idx + 1);
                no->chaves[idx] = succ;
                return removerRec(filhoDir, succ.codigo);
            } else {
                // Subcaso 1b-iii: Concatenar
                concatenar(no, idx, filhoEsq, filhoDir);
                return removerRec(filhoEsq, codigo);
            }
        }
    } else {
        // Caso 2: A chave não está neste nó
        if (no->eh_folha) {
            return false; // Chave não encontrada
        }
        
        bool ultimoFilho = (idx == no->num_chaves);
        NoB *filho = no->filhos[idx];
        
        if (filho->num_chaves < ORDEM/2) {
            // Precisa balancear
            NoB *irmaoEsq = (idx > 0) ? no->filhos[idx - 1] : NULL;
            NoB *irmaoDir = (idx < no->num_chaves) ? no->filhos[idx + 1] : NULL;
            
            if (irmaoEsq && irmaoEsq->num_chaves >= ORDEM/2) {
                // Subcaso 2a-i: Redistribuir com irmão esquerdo
                redistribuir(no, idx - 1, filho, irmaoEsq, true);
            } else if (irmaoDir && irmaoDir->num_chaves >= ORDEM/2) {
                // Subcaso 2a-ii: Redistribuir com irmão direito
                redistribuir(no, idx, filho, irmaoDir, false);
            } else {
                // Subcaso 2b: Concatenar
                if (idx > 0) {
                    concatenar(no, idx - 1, irmaoEsq, filho);
                    filho = irmaoEsq;
                } else {
                    concatenar(no, idx, filho, irmaoDir);
                }
            }
        }
        
        return removerRec(filho, codigo);
    }
}

// Função pública de remoção
bool removerDaArvoreB(int codigo) {
    if (!raiz) return false;
    
    bool result = removerRec(raiz, codigo);
    
    // Se a raiz ficou vazia, ajustar a árvore
    if (raiz->num_chaves == 0 && !raiz->eh_folha) {
        NoB *novaRaiz = raiz->filhos[0];
        free(raiz);
        raiz = novaRaiz;
    }
    
    return result;

}
