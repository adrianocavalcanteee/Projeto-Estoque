// === utils.h ===
#ifndef UTILS_H
#define UTILS_H

typedef struct {
    int codigo;
    char marca[20];
    char modelo[20];
    float preco;
    int tamanho;
    int quantidade;
} Tenis;

Tenis lerTenis();
void exibirTenis(Tenis t);
void salvarTenis(Tenis t);
Tenis buscarTenis(int codigo);
void alterarTenis(int codigo);
void removerTenis(int codigo);
void listarTenis();
Tenis lerLinhaRegistro(char *linha);
char *formatarTenis(Tenis t);

#endif