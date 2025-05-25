// === utils.c ===
#include "utils.h"
#include "arvoreb.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define VERIFICAR_INTEGRIDADE

#define REGISTRO_TAM 90

Tenis lerTenis() {
    Tenis t = {0};
    
    do {
        printf("Codigo: ");
        if (scanf("%d", &t.codigo) != 1 || t.codigo <= 0) {
            printf("Codigo invalido! Deve ser um numero positivo.\n");
            limparBuffer();
            continue;
        }
        
        // Verifica se código já existe
        #ifdef VERIFICAR_INTEGRIDADE
        if (buscarNaArvoreB(t.codigo) != -1) {
            printf("Erro: Codigo %d ja existe!\n", t.codigo);
            t.codigo = 0;
            continue;
        }
        #endif
        
        break;
    } while (1);
    limparBuffer();
    
    printf("Marca: ");
    fgets(t.marca, sizeof(t.marca), stdin);
    t.marca[strcspn(t.marca, "\n")] = '\0';
    
    printf("Modelo: ");
    fgets(t.modelo, sizeof(t.modelo), stdin);
    t.modelo[strcspn(t.modelo, "\n")] = '\0';
    
    do {
        printf("Preco: ");
        if (scanf("%f", &t.preco) != 1 || t.preco <= 0) {
            printf("Preco invalido! Deve ser um valor positivo.\n");
            limparBuffer();
            continue;
        }
        break;
    } while (1);
    limparBuffer();
    
    do {
        printf("Tamanho: ");
        if (scanf("%d", &t.tamanho) != 1 || t.tamanho <= 0) {
            printf("Tamanho invalido! Deve ser um numero positivo.\n");
            limparBuffer();
            continue;
        }
        break;
    } while (1);
    limparBuffer();
    
    do {
        printf("Quantidade: ");
        if (scanf("%d", &t.quantidade) != 1 || t.quantidade < 0) {
            printf("Quantidade invalida! Deve ser um numero nao negativo.\n");
            limparBuffer();
            continue;
        }
        break;
    } while (1);
    limparBuffer();
    
    return t;
}
void exibirTenis(Tenis t) {
    if(t.codigo != 0) {
        printf("Codigo: %d | Marca: %s | Modelo: %s | Preco: %.2f | Tamanho: %d | Quantidade: %d\n",
               t.codigo, 
               t.marca, 
               t.modelo, 
               t.preco, 
               t.tamanho, 
               t.quantidade);
    }
}
char *formatarTenis(Tenis t) {
    static char linha[REGISTRO_TAM + 1] = {0}; // Inicializa com zeros
    snprintf(linha, sizeof(linha), "%05d|%-20s|%-20s|%07.2f|%02d|%02d",
             t.codigo, t.marca, t.modelo, t.preco, t.tamanho, t.quantidade);
    return linha;
}
Tenis lerLinhaRegistro(char *linha) {
    Tenis t = {0};
    sscanf(linha, "%5d|%20[^|]|%20[^|]|%f|%2d|%2d",
           &t.codigo, t.marca, t.modelo, &t.preco, &t.tamanho, &t.quantidade);
    return t;
}

void salvarTenis(Tenis t) {
    if (t.codigo == 0) {
        printf("Erro: Tenis invalido para salvar!\n");
        return;
    }

    FILE *f = fopen("dados/dados.txt", "a+");
    if (!f) {
        if (mkdir("dados", 0755) == -1) {
            perror("Erro ao criar diretorio");
            return;
        }
        f = fopen("dados/dados.txt", "a+");
        if (!f) {
            perror("Erro ao criar arquivo de dados");
            return;
        }
    }
    
    fseek(f, 0, SEEK_END);
    long pos = ftell(f);
    
    char registro[REGISTRO_TAM + 2]; // +2 para \n e \0
    snprintf(registro, sizeof(registro), "%s\n", formatarTenis(t));
    
    if (fwrite(registro, sizeof(char), strlen(registro), f) != strlen(registro)) {
        perror("Erro ao escrever no arquivo");
        fclose(f);
        return;
    }
    
    fclose(f);

    RegistroIndice reg = {t.codigo, pos};
    inserirNaArvoreB(reg);
    printf("Tenis inserido com sucesso!\n");
}
Tenis buscarTenis(int codigo) {
    Tenis t = {0};
    long pos = buscarNaArvoreB(codigo);
    if (pos == -1) {
        printf("Tenis com codigo %d nao encontrado.\n", codigo);
        return t;
    }
    
    FILE *f = fopen("dados/dados.txt", "r");
    if (!f) {
        perror("Erro ao abrir dados/dados.txt");
        return t;
    }
    
    fseek(f, pos, SEEK_SET);
    char linha[REGISTRO_TAM + 2];
    if (fgets(linha, sizeof(linha), f)) {
        t = lerLinhaRegistro(linha);
        if (t.codigo != codigo) { // Verificação adicional
            printf("Inconsistencia encontrada no registro!\n");
            t.codigo = 0;
        }
    }
    fclose(f);
    
    return t;
}
void alterarTenis(int codigo) {
    long pos = buscarNaArvoreB(codigo);
    if (pos == -1) {
        printf("Tenis nao encontrado.\n");
        return;
    }
    FILE *f = fopen("dados/dados.txt", "r+");
    if (!f) {
        perror("Erro ao abrir dados/dados.txt");
        return;
    }
    fseek(f, pos, SEEK_SET);
    fprintf(f, "%-90s", "");
    fclose(f);

    Tenis novo = lerTenis();
    f = fopen("dados/dados.txt", "a");
    if (!f) {
        perror("Erro ao abrir dados/dados.txt para append");
        return;
    }
    long novaPos = ftell(f);
    fprintf(f, "%s\n", formatarTenis(novo));
    fclose(f);

    for (int i = 0; i < totalIndices; i++) {
        if (indices[i].codigo == codigo) {
            indices[i].pos = novaPos;
            break;
        }
    }
    salvarIndice();
    printf("Tenis alterado com sucesso!\n");
}

void removerTenis(int codigo) {
    long pos = buscarNaArvoreB(codigo);
    if (pos == -1) {
        printf("Tenis nao encontrado.\n");
        return;
    }
    FILE *f = fopen("dados/dados.txt", "r+");
    if (!f) {
        perror("Erro ao abrir dados/dados.txt");
        return;
    }
    fseek(f, pos, SEEK_SET);
    fprintf(f, "%-90s", "");
    fclose(f);
    removerDaArvoreB(codigo);
    printf("Tenis removido com sucesso!\n");
}

void listarTenis() {
    printf("\n=== Lista de Tenis Ordenada ===\n");
    
    if(totalIndices == 0) {
        printf("Nenhum tenis cadastrado.\n");
        return;
    }
    
    // Ordenação por código
    for(int i = 0; i < totalIndices-1; i++) {
        for(int j = i+1; j < totalIndices; j++) {
            if(indices[i].codigo > indices[j].codigo) {
                RegistroIndice temp = indices[i];
                indices[i] = indices[j];
                indices[j] = temp;
            }
        }
    }
    
    FILE *f = fopen("dados/dados.txt", "r");
    if(!f) {
        perror("Erro ao abrir arquivo de dados");
        return;
    }
    
    int ultimoCodigo = -1;
    for(int i = 0; i < totalIndices; i++) {
        // Pula códigos duplicados
        if(indices[i].codigo == ultimoCodigo) continue;
        
        fseek(f, indices[i].pos, SEEK_SET);
        char linha[REGISTRO_TAM + 2] = {0}; // Buffer zerado
        
        if(fgets(linha, sizeof(linha), f)) {
            linha[strcspn(linha, "\n")] = '\0'; // Remove newline
            
            Tenis t = lerLinhaRegistro(linha);
            if(t.codigo != 0) {
                exibirTenis(t);
                ultimoCodigo = t.codigo;
            }
        }
    }
    fclose(f);
}