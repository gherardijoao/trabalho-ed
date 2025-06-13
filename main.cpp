#include "gerenciador_de_atletas.h"
#include <iostream>
#include <limits>
#include <string>

void menuPrincipal() {
    GerenciadorDeAtletas gerenciador("dados.bin");
    const std::string csvPath = "dados.csv";
    int opcao;

    do {
        std::cout << "\n--- Menu de Operacoes ---\n";
        std::cout << "1. Converter CSV para Binario\n";
        std::cout << "2. Inserir um registro\n";
        std::cout << "3. Visualizar registros\n";
        std::cout << "4. Alterar um registro\n";
        std::cout << "5. Trocar dois registros\n";
        std::cout << "6. Imprimir todos os registros\n";
        std::cout << "7. Ordenar arquivo por ID\n";
        std::cout << "8. Verificar ordenacao do arquivo\n"; // Nova opção
        std::cout << "0. Sair\n";
        std::cout << "Escolha uma opcao: ";
        std::cin >> opcao;

        while (std::cin.fail()) {
            std::cout << "Entrada invalida. Por favor, insira um numero." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Escolha uma opcao: ";
            std::cin >> opcao;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (opcao) {
            case 1: gerenciador.converterCSVparaBinario(csvPath); break;
            case 2: gerenciador.inserirRegistro(); break;
            case 3: gerenciador.visualizarRegistros(); break;
            case 4: gerenciador.alterarRegistro(); break;
            case 5: gerenciador.trocarRegistros(); break;
            case 6: gerenciador.imprimirTodos(); break;
            case 7: gerenciador.mergeSortExterno(); break;
            case 8: gerenciador.verificarOrdenacao(); break; // Chama a nova função
            case 0: std::cout << "Saindo do programa..." << std::endl; break;
            default: std::cout << "Opcao invalida! Tente novamente." << std::endl; break;
        }
    } while (opcao != 0);
}

int main() {
    menuPrincipal();
    return 0;
}
