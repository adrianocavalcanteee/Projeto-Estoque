// === utils.c ===
#include "utils.h"
#include "arvoreb.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define REGISTRO_TAM 90

Tenis lerTenis() {
    Tenis t;
    printf("Codigo: ");
    scanf("%d", &t.codigo);
    while(getchar() != '\n'); // Limpa buffer
    
    printf("Marca: ");
    fgets(t.marca, sizeof(t.marca), stdin);
    t.marca[strcspn(t.marca, "\n")] = '\0';
    
    printf("Modelo: ");
    fgets(t.modelo, sizeof(t.modelo), stdin);
    t.modelo[strcspn(t.modelo, "\n")] = '\0';
    
    printf("Preco: ");
    while(scanf("%f", &t.preco) != 1) { // Validação do preço
        printf("Preco invalido. Digite novamente: ");
        while(getchar() != '\n'); // Limpa buffer
    }
    while(getchar() != '\n'); // Limpa buffer
    
    printf("Tamanho: ");
    scanf("%d", &t.tamanho);
    while(getchar() != '\n'); // Limpa buffer
    
    printf("Quantidade: ");
    scanf("%d", &t.quantidade);
    while(getchar() != '\n'); // Limpa buffer
    
    return t;
}

void exibirTenis(Tenis t) {
    if (t.codigo != 0) {
        printf("Codigo: %d | Marca: %s | Modelo: %s | Preco: %.2f | Tamanho: %d | Quantidade: %d\n",
               t.codigo, t.marca, t.modelo, t.preco, t.tamanho, t.quantidade);
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
    // Verifica se o código já existe
    if (buscarNaArvoreB(t.codigo) != -1) {
        printf("Erro: Codigo %d ja existe!\n", t.codigo);
        return;
    }

    FILE *f = fopen("dados/dados.txt", "a+");
    if (!f) {
        perror("Erro ao abrir dados/dados.txt");
        return;
    }
    
    fseek(f, 0, SEEK_END);
    long pos = ftell(f);
    
    // Verifica se há espaço suficiente
    if (fprintf(f, "%s\n", formatarTenis(t)) < 0) {
        perror("Erro ao escrever no arquivo");
        fclose(f);
        return;
    }
    
    fclose(f);

    RegistroIndice reg = { t.codigo, pos };
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
    
    // Verifica se há índices
    if(totalIndices == 0) {
        printf("Nenhum tenis cadastrado.\n");
        return;
    }
    
    // Ordenação
    for(int i = 0; i < totalIndices - 1; i++) {
        for(int j = i + 1; j < totalIndices; j++) {
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
    
    // Remove duplicatas
    int lastCode = -1;
    for(int i = 0; i < totalIndices; i++) {
        if(indices[i].codigo == lastCode) continue;
        
        fseek(f, indices[i].pos, SEEK_SET);
        char linha[REGISTRO_TAM + 2] = {0}; // Inicializa com zeros
        
        if(fgets(linha, sizeof(linha), f)) {
            // Remove possíveis lixos no final
            linha[strcspn(linha, "\n")] = '\0';
            
            Tenis t = lerLinhaRegistro(linha);
            if(t.codigo != 0) {
                exibirTenis(t);
                lastCode = t.codigo;
            }
        }
    }
    fclose(f);
}