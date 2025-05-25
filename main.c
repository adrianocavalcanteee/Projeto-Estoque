// === main.c ===
#include "utils.h"
#include "arvoreb.h"
#include <stdio.h>

int main() {
    int opcao, codigo;
    inicializarArvoreB();
    do {
        printf("\n1 - Inserir\n2 - Buscar\n3 - Alterar\n4 - Remover\n5 - Listar\n0 - Sair\nOpcao: ");
        if (scanf("%d", &opcao) != 1) {
            // Limpa o buffer completamente em caso de erro
            while (getchar() != '\n');
            continue;
        }
        
        // Limpa o buffer completamente
        while (getchar() != '\n');
        
        switch (opcao) {
            case 1: salvarTenis(lerTenis()); break;
            case 2: 
                printf("Codigo: "); 
                scanf("%d", &codigo); 
                while (getchar() != '\n'); // Limpa buffer
                exibirTenis(buscarTenis(codigo)); 
                break;
            case 3: 
                printf("Codigo: "); 
                scanf("%d", &codigo); 
                while (getchar() != '\n'); // Limpa buffer
                alterarTenis(codigo); 
                break;
            case 4: 
                printf("Codigo: "); 
                scanf("%d", &codigo); 
                while (getchar() != '\n'); // Limpa buffer
                removerTenis(codigo); 
                break;
            case 5: listarTenis(); break;
            case 0: printf("Encerrando o programa.\n"); break;
            default: printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
    return 0;
}