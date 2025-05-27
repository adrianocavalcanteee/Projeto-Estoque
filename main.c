// === main.c ===
#include "utils.h"
#include "arvoreb.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    int opcao, codigo;
    inicializarArvoreB();

    do {
        printf("\n1 - Inserir\n2 - Buscar\n3 - Alterar\n4 - Listar\n5 - Mesclar Dados\n0 - Sair\nOpcao: ");

        if (scanf("%d", &opcao) != 1) {
            limparBuffer();
            continue;
        }
        limparBuffer();

        switch (opcao) {
            case 1: salvarTenis(lerTenis()); break;
            case 2:
                printf("Codigo: ");
                if (scanf("%d", &codigo) == 1) {
                    limparBuffer();
                    exibirTenis(buscarTenis(codigo));
                } else {
                    limparBuffer();
                    printf("Codigo invalido!\n");
                }
                break;
            case 3:
                printf("Codigo: ");
                if (scanf("%d", &codigo) == 1) {
                    limparBuffer();
                    alterarTenis(codigo);
                } else {
                    limparBuffer();
                    printf("Codigo invalido!\n");
                }
                break;
            case 4: listarTenis(); break;
            case 5: mesclarDados(); break;
            case 0: printf("Encerrando o programa.\n"); break;
            default: printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}
