#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdbool.h>

#define MARCA_TAM 21
#define MODELO_TAM 21
#define REGISTRO_TAM 90

typedef struct {
    int codigo;
    char marca[MARCA_TAM];
    char modelo[MODELO_TAM];
    float preco;
    int tamanho;
    int quantidade;
} Tenis;

// Interface das funções
void limparBuffer();
Tenis lerTenis();
void exibirTenis(const Tenis *t);
bool salvarTenis(Tenis *t);
bool buscarTenis(int codigo, Tenis *resultado);
bool alterarTenis(int codigo);
void listarTenis();
bool inicializarSistema();
void finalizarSistema();
bool mesclarArquivo(const char *novoArquivo);
bool mesclarArquivoNovosTenis();
bool buscarTenisPorTexto(const char *texto, Tenis *resultados, int *quantidade);

void limparString(const char *origem, char *destino);

#endif