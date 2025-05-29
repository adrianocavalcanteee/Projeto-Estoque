#include "utils.h"
#include "arvoreb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(a) _mkdir(a)
#else
#include <sys/stat.h>
#define MKDIR(a) mkdir(a, 0700)
#endif
#ifdef _WIN32
#include <ctype.h>

char *strcasestr(const char *haystack, const char *needle) {
    if (!*needle) return (char *)haystack;
    
    for (; *haystack; haystack++) {
        if (toupper(*haystack) == toupper(*needle)) {
            const char *h = haystack, *n = needle;
            for (; *h && *n; h++, n++) {
                if (toupper(*h) != toupper(*n)) {
                    break;
                }
            }
            if (!*n) return (char *)haystack;
        }
    }
    return NULL;
}
#endif

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

Tenis lerTenis() {
    Tenis t = {0};
    
    do {
        printf("Codigo: ");
        if (scanf("%d", &t.codigo) != 1) {
            limparBuffer();
            printf("Codigo invalido! Digite um numero.\n");
            continue;
        }
        limparBuffer();
    } while (t.codigo <= 0);

    do {
        printf("Marca (max %d chars): ", MARCA_TAM-1);
        if (!fgets(t.marca, MARCA_TAM, stdin)) {
            limparBuffer();
            continue;
        }
        t.marca[strcspn(t.marca, "\n")] = '\0';
    } while (strlen(t.marca) == 0);

    do {
        printf("Modelo (max %d chars): ", MODELO_TAM-1);
        if (!fgets(t.modelo, MODELO_TAM, stdin)) {
            limparBuffer();
            continue;
        }
        t.modelo[strcspn(t.modelo, "\n")] = '\0';
    } while (strlen(t.modelo) == 0);

    do {
        printf("Preco: ");
        if (scanf("%f", &t.preco) != 1) {
            limparBuffer();
            printf("Preco invalido! Digite um numero.\n");
            continue;
        }
        limparBuffer();
    } while (t.preco <= 0);

    do {
        printf("Tamanho: ");
        if (scanf("%d", &t.tamanho) != 1) {
            limparBuffer();
            printf("Tamanho invalido! Digite um numero.\n");
            continue;
        }
        limparBuffer();
    } while (t.tamanho <= 0);

    do {
        printf("Quantidade: ");
        if (scanf("%d", &t.quantidade) != 1) {
            limparBuffer();
            printf("Quantidade invalida! Digite um numero.\n");
            continue;
        }
        limparBuffer();
    } while (t.quantidade <= 0);

    return t;
}
// Implemente limparString primeiro
void limparString(const char *origem, char *destino) {
    int i = 0, j = 0;
    while (origem[i] != '\0' && j < MARCA_TAM-1) {
        if (isprint((unsigned char)origem[i])) {
            destino[j++] = origem[i];
        }
        i++;
    }
    destino[j] = '\0';
}


void exibirTenis(const Tenis *t) {
    if (t == NULL) return;
    
    // Limpar caracteres não-ASCII
    char marca_limpa[MARCA_TAM];
    char modelo_limpo[MODELO_TAM];
    limparString(t->marca, marca_limpa);
    limparString(t->modelo, modelo_limpo);
    
    printf("%05d | %-20s | %-20s | %7.2f | %2d | %2d\n",
           t->codigo, marca_limpa, modelo_limpo, 
           t->preco, t->tamanho, t->quantidade);
}
static bool criarDiretorioSeNecessario() {
    if (MKDIR("dados") != 0 && errno != EEXIST) {
        perror("Erro ao criar diretorio");
        return false;
    }
    return true;
}

bool salvarTenis(Tenis *t) {
    if (t == NULL) return false;

    if (!criarDiretorioSeNecessario()) {
        return false;
    }

    // Verificar se código já existe
    if (buscarNaArvoreB(t->codigo) != -1) {
        printf("Erro: Codigo %d ja existe!\n", t->codigo);
        return false;
    }

    FILE *f = fopen("dados/dados.txt", "a+");
    if (!f) {
        perror("Erro ao abrir arquivo de dados");
        return false;
    }

    fseek(f, 0, SEEK_END);
    long pos = ftell(f);
    limparString(t->marca, t->marca);
    limparString(t->modelo, t->modelo);
    
    if (fprintf(f, "%05d|%20s|%20s|%07.2f|%02d|%02d\n",  
            t->codigo, t->marca, t->modelo, t->preco, t->tamanho, t->quantidade) < 0) {
        perror("Erro ao escrever no arquivo");
        fclose(f);
        return false;
    }
    
    if (fclose(f) != 0) {
        perror("Erro ao fechar arquivo");
        return false;
    }

    RegistroIndice reg = {t->codigo, pos};
    if (!inserirNaArvoreB(reg)) {
        printf("Erro ao atualizar indice!\n");
        return false;
    }

    return true;
}

bool buscarTenis(int codigo, Tenis *resultado) {
    if (resultado == NULL) return false;

    long pos = buscarNaArvoreB(codigo);
    if (pos == -1) {
        return false;
    }

    FILE *f = fopen("dados/dados.txt", "r");
    if (!f) {
        perror("Erro ao abrir arquivo de dados");
        return false;
    }

    fseek(f, pos, SEEK_SET);
    char linha[REGISTRO_TAM + 2];
    if (!fgets(linha, sizeof(linha), f)) {
        fclose(f);
        return false;
    }
    fclose(f);

    // Limpar estrutura antes de preencher
    memset(resultado, 0, sizeof(Tenis));
    
    // Corrigir o sscanf para ler corretamente
    if (sscanf(linha, "%d|%20[^|]|%20[^|]|%f|%d|%d",
        &resultado->codigo, resultado->marca, resultado->modelo,
        &resultado->preco, &resultado->tamanho, &resultado->quantidade) != 6) {
        printf("Erro ao ler registro do arquivo! Linha: %s\n", linha);
        return false;
}

// Limpar os campos após a leitura
        limparString(resultado->marca, resultado->marca);
        limparString(resultado->modelo, resultado->modelo);

        return true;
}

bool alterarTenis(int codigo) {
    Tenis antigo, novo;
    if (!buscarTenis(codigo, &antigo)) {
        printf("Tenis nao encontrado!\n");
        return false;
    }

    printf("Editando tenis:\n");
    exibirTenis(&antigo);
    printf("\nNovos dados:\n");
    novo = lerTenis();

    // Verificar se novo código já existe (se for diferente)
    if (novo.codigo != codigo && buscarNaArvoreB(novo.codigo) != -1) {
        printf("Erro: Codigo %d ja existe!\n", novo.codigo);
        return false;
    }

    FILE *f = fopen("dados/dados.txt", "r+");
    if (!f) {
        perror("Erro ao abrir arquivo para edicao");
        return false;
    }
    long pos = buscarNaArvoreB(codigo);
    fseek(f, pos, SEEK_SET);
    
    if (fprintf(f, "%05d|%-20s|%-20s|%07.2f|%02d|%02d",
               novo.codigo, novo.marca, novo.modelo, novo.preco, novo.tamanho, novo.quantidade) < 0) {
        perror("Erro ao escrever no arquivo");
        fclose(f);
        return false;
    }
    
    fclose(f);

    // Atualizar índice se código mudou
    if (codigo != novo.codigo) {
        removerDaArvoreB(codigo);
        RegistroIndice reg = {novo.codigo, pos};
        inserirNaArvoreB(reg);
    }

    return true;
}

void listarTenis() {
    printf("\n=== Lista de Tenis ===\n");
    printf("COD.  | MARCA               | MODELO              | PRECO   | TM | QTD\n");
    printf("----------------------------------------------------------------------\n");

    FILE *f = fopen("dados/dados.txt", "r");
    if (!f) {
        perror("Erro ao abrir arquivo de dados");
        return;
    }

    percorrerEmOrdem(raiz, f);
    fclose(f);
}

bool inicializarSistema() {
    if (!criarDiretorioSeNecessario()) {
        return false;
    }
    inicializarArvoreB();
    return true;
}

void finalizarSistema() {
    salvarIndice();
}

bool mesclarArquivoNovosTenis() {
    const char *nomeArquivo = "dados/novos_tenis.txt";
    
    printf("\n=== Mesclagem de Arquivo ===\n");
    printf("Verificando arquivo '%s'...\n", nomeArquivo);

    FILE *fNovo = fopen(nomeArquivo, "r");
    if (!fNovo) {
        printf("Arquivo '%s' nao encontrado na pasta dados.\n", nomeArquivo);
        printf("Certifique-se que o arquivo existe e tem este nome exato.\n");
        return false;
    }
    // Primeiro, processar novos registros para verificar códigos duplicados
    int registrosAdicionados = 0;
    int registrosIgnorados = 0;
    Tenis *novosTenis = NULL;
    int numNovos = 0;
    char linha[REGISTRO_TAM + 2];
    while (fgets(linha, sizeof(linha), fNovo)) {
        Tenis t;
        if (sscanf(linha, "%d|%20[^|]|%20[^|]|%f|%d|%d",
                  &t.codigo, t.marca, t.modelo, &t.preco, &t.tamanho, &t.quantidade) != 6) {
            printf("Formato invalido na linha: %s", linha);
            registrosIgnorados++;
            continue;
        }

        if (buscarNaArvoreB(t.codigo) != -1) {
            printf("Codigo %d ja existe - ignorando\n", t.codigo);
            registrosIgnorados++;
            continue;
        }

        // Adicionar à lista de novos tênis
        numNovos++;
        novosTenis = realloc(novosTenis, numNovos * sizeof(Tenis));
        novosTenis[numNovos-1] = t;
    }
    fclose(fNovo);

    if (numNovos == 0) {
        printf("Nenhum registro novo para adicionar.\n");
        free(novosTenis);
        return true;
    }

    // Agora abrir o arquivo principal em modo append
    FILE *fAtual = fopen("dados/dados.txt", "a+");
    if (!fAtual) {
        perror("Erro ao abrir arquivo de dados");
        free(novosTenis);
        return false;
    }

    // Posicionar no final do arquivo
    fseek(fAtual, 0, SEEK_END);
    
    // Adicionar os novos registros
    for (int i = 0; i < numNovos; i++) {
        long pos = ftell(fAtual);
        Tenis *t = &novosTenis[i];
        
        if (fprintf(fAtual, "%05d|%-20s|%-20s|%07.2f|%02d|%02d\n",
                t->codigo, t->marca, t->modelo, t->preco, t->tamanho, t->quantidade) < 0) {
            perror("Erro ao escrever no arquivo");
            fclose(fAtual);
            free(novosTenis);
            return false;
        }
        
        // Adicionar ao índice
        RegistroIndice reg = {t->codigo, pos};
        if (!inserirNaArvoreB(reg)) {
            printf("Erro ao atualizar indice para codigo %d!\n", t->codigo);
        } else {
            registrosAdicionados++;
        }
    }

    fclose(fAtual);
    free(novosTenis);

    printf("Mesclagem concluida:\n");
    printf("- Registros adicionados: %d\n", registrosAdicionados);
    printf("- Registros ignorados: %d\n", registrosIgnorados);

    return true;
}

bool mesclarArquivo(const char *nomeArquivo) {
    // Implementação mantida para compatibilidade
    return mesclarArquivoNovosTenis();
}
// Adicione esta função em utils.c
bool buscarTenisPorTexto(const char *texto, Tenis *resultados, int *quantidade) {
    if (texto == NULL || resultados == NULL || quantidade == NULL) {
        return false;
    }

    FILE *f = fopen("dados/dados.txt", "r");
    if (!f) {
        perror("Erro ao abrir arquivo de dados");
        return false;
    }

    *quantidade = 0;
    char linha[REGISTRO_TAM + 2];
    Tenis t;

    while (fgets(linha, sizeof(linha), f)) {
        memset(&t, 0, sizeof(Tenis));
        
        if (sscanf(linha, "%d|%20[^|]|%20[^|]|%f|%d|%d",
                  &t.codigo, t.marca, t.modelo, &t.preco, &t.tamanho, &t.quantidade) != 6) {
            continue;
        }

        if (strcasestr(t.marca, texto) != NULL || strcasestr(t.modelo, texto) != NULL) {
            resultados[*quantidade] = t;
            (*quantidade)++;
        }
    }

    fclose(f);
    return true;
}