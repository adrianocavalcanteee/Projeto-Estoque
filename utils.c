#include "utils.h"
#include "arvoreb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>  // Para mkdir no Windows
#else
#include <sys/stat.h>  // Para mkdir no Linux/macOS
#endif

#define REGISTRO_TAM 90

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

Tenis lerTenis() {
    Tenis t = {0};

    printf("Codigo: ");
    scanf("%d", &t.codigo);
    limparBuffer();

    printf("Marca: ");
    fgets(t.marca, sizeof(t.marca), stdin);
    t.marca[strcspn(t.marca, "\n")] = '\0';

    printf("Modelo: ");
    fgets(t.modelo, sizeof(t.modelo), stdin);
    t.modelo[strcspn(t.modelo, "\n")] = '\0';

    printf("Preco: ");
    scanf("%f", &t.preco);
    limparBuffer();

    printf("Tamanho: ");
    scanf("%d", &t.tamanho);
    limparBuffer();

    printf("Quantidade: ");
    scanf("%d", &t.quantidade);
    limparBuffer();

    return t;
}

void exibirTenis(Tenis t) {
    printf("Codigo: %d | Marca: %s | Modelo: %s | Preco: %.2f | Tamanho: %d | Quantidade: %d\n",
           t.codigo, t.marca, t.modelo, t.preco, t.tamanho, t.quantidade);
}

char *formatarTenis(Tenis t) {
    static char linha[REGISTRO_TAM + 1];
    snprintf(linha, sizeof(linha), "%05d|%-20s|%-20s|%07.2f|%02d|%02d",
             t.codigo, t.marca, t.modelo, t.preco, t.tamanho, t.quantidade);
    return linha;
}

// ✅ CORRIGIDA: agora suporta campos de tamanho variável
Tenis lerLinhaRegistro(char *linha) {
    Tenis t = {0};
    sscanf(linha, "%d|%[^|]|%[^|]|%f|%d|%d",
           &t.codigo, t.marca, t.modelo, &t.preco, &t.tamanho, &t.quantidade);
    return t;
}

void salvarTenis(Tenis t) {
    FILE *f = fopen("dados/dados.txt", "a+");
    if (!f) {
        mkdir("dados");  // Compatível com Windows
        f = fopen("dados/dados.txt", "a+");
        if (!f) {
            perror("Erro ao criar arquivo de dados");
            return;
        }
    }

    fseek(f, 0, SEEK_END);
    long pos = ftell(f);
    fprintf(f, "%s\n", formatarTenis(t));
    fclose(f);

    RegistroIndice reg = {t.codigo, pos};
    inserirNaArvoreB(reg);
    printf("Tenis salvo com sucesso!\n");
}

Tenis buscarTenis(int codigo) {
    long pos = buscarNaArvoreB(codigo);
    Tenis t = {0};
    if (pos == -1) return t;

    FILE *f = fopen("dados/dados.txt", "r");
    if (!f) return t;

    fseek(f, pos, SEEK_SET);
    char linha[REGISTRO_TAM + 2];
    fgets(linha, sizeof(linha), f);
    fclose(f);

    t = lerLinhaRegistro(linha);
    return t;
}

void alterarTenis(int codigo) {
    long pos = buscarNaArvoreB(codigo);
    if (pos == -1) {
        printf("Codigo nao encontrado.\n");
        return;
    }

    Tenis novo = lerTenis();
    FILE *f = fopen("dados/dados.txt", "a");
    if (!f) return;

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

void listarTenis() {
    printf("\n=== Lista de Tenis ===\n");

    if (totalIndices == 0) {
        printf("Nenhum tenis cadastrado.\n");
        return;
    }

    FILE *f = fopen("dados/dados.txt", "r");
    if (!f) {
        perror("Erro ao abrir arquivo de dados");
        return;
    }

    for (int i = 0; i < totalIndices; i++) {
        fseek(f, indices[i].pos, SEEK_SET);
        char linha[REGISTRO_TAM + 2] = {0};
        if (fgets(linha, sizeof(linha), f)) {
            linha[strcspn(linha, "\n")] = '\0';
            Tenis t = lerLinhaRegistro(linha);
            if (t.codigo > 0) {
                exibirTenis(t);
            }
        }
    }

    fclose(f);
}

void mesclarDados() {
    FILE *f1 = fopen("dados/dados.txt", "r");
    FILE *f2 = fopen("dados/novos.txt", "r");
    if (!f1 && !f2) {
        printf("Nenhum dos arquivos foi encontrado.\n");
        return;
    }

    Tenis todos[2000];
    int count = 0;

    char linha[REGISTRO_TAM + 2];
    if (f1) {
        while (fgets(linha, sizeof(linha), f1)) {
            Tenis t = lerLinhaRegistro(linha);
            todos[count++] = t;
        }
        fclose(f1);
    }

    if (f2) {
        while (fgets(linha, sizeof(linha), f2)) {
            Tenis t = lerLinhaRegistro(linha);
            int duplicado = 0;
            for (int i = 0; i < count; i++) {
                if (todos[i].codigo == t.codigo) {
                    duplicado = 1;
                    break;
                }
            }
            if (!duplicado) {
                todos[count++] = t;
            }
        }
        fclose(f2);
    }

    // Ordena os registros por código
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (todos[j].codigo > todos[j + 1].codigo) {
                Tenis tmp = todos[j];
                todos[j] = todos[j + 1];
                todos[j + 1] = tmp;
            }
        }
    }

    FILE *fout = fopen("dados/dados.txt", "w");
    if (!fout) {
        perror("Erro ao abrir dados.txt para escrita");
        return;
    }

    totalIndices = 0;
    for (int i = 0; i < count; i++) {
        long pos = ftell(fout);
        fprintf(fout, "%s\n", formatarTenis(todos[i]));
        indices[totalIndices].codigo = todos[i].codigo;
        indices[totalIndices].pos = pos;
        totalIndices++;
    }

    fclose(fout);
    salvarIndice();

    printf("Mesclagem concluida: %d registros unificados e ordenados.\n", count);
}
