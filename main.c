#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void mostrarMenu() {
    printf("\n=== Sistema de Tenis ===\n");
    printf("1. Inserir novo tenis\n");
    printf("2. Buscar tenis\n");
    printf("3. Alterar tenis\n");
    printf("4. Listar todos\n");
    printf("5. Mesclar arquivo\n");
    printf("0. Sair\n");
    printf("Escolha: ");
}

int main() {
    if (!inicializarSistema()) {
        fprintf(stderr, "Falha ao inicializar sistema!\n");
        return EXIT_FAILURE;
    }

    int opcao;
    do {
        mostrarMenu();
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1: {
                Tenis t = lerTenis();
                if (salvarTenis(&t)) {
                    printf("Tenis salvo com sucesso!\n");
                } else {
                    printf("Falha ao salvar tenis!\n");
                }
                break;
            }
            case 2: {
                printf("Codigo: ");
                int codigo;
                scanf("%d", &codigo);
                limparBuffer();
                
                Tenis resultado;
                if (buscarTenis(codigo, &resultado)) {
                    exibirTenis(&resultado);
                } else {
                    printf("Tenis nao encontrado!\n");
                }
                break;
            }
            case 3: {
                printf("Codigo para alterar: ");
                int codigo;
                scanf("%d", &codigo);
                limparBuffer();
                
                if (alterarTenis(codigo)) {
                    printf("Tenis alterado com sucesso!\n");
                } else {
                    printf("Falha ao alterar tenis!\n");
                }
                break;
            }
            case 4:
                listarTenis();
                break;
            case 5: {
                    printf("\n=== Mesclar Arquivo ===\n");
                    printf("Esta operação irá mesclar o arquivo 'novos_tenis.txt' da pasta dados.\n");
                    printf("Deseja continuar? (s/n): ");
                    
                    char confirmacao;
                    scanf(" %c", &confirmacao);
                    limparBuffer();
                    
                    if (confirmacao == 's' || confirmacao == 'S') {
                        if (mesclarArquivoNovosTenis()) {
                            printf("Operação concluída com sucesso!\n");
                        }
                    } else {
                        printf("Operação cancelada.\n");
                    }
                    break;
                }
            
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    finalizarSistema();
    return EXIT_SUCCESS;
}