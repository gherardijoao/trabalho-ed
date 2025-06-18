/*
 * Projeto Prático: Tratamento de Arquivos e Ordenação em Memória Secundária
 * Equipe:
 * - JONATHAN MARTINS CABRAL
 * - THALES MAIA MENDONÇA NEGRÃO
 * - JOÃO AMANCIO GHERARDI
 *
 * Data: 18 de Junho de 2025
 *
 * Descrição: Este é o arquivo principal do projeto. Ele contém a função `main()`
 * e o menu interativo que permite ao usuário acessar todas as funcionalidades
 * do sistema, incluindo a conversão de CSV para binário, as operações de
 * manipulação de registros no arquivo binário e a execução do algoritmo de
 * ordenação externa (Merge Sort Multi-way Crescente).
 */

 #include "gerenciador_de_atletas.h"
 #include <iostream>  
 #include <limits>   
 #include <string>    
 
 // Função que exibe o menu e gerencia as operações
 void menuPrincipal() {
     GerenciadorDeAtletas gerenciador("dados.bin"); 
     const std::string csvPath = "dados.csv";      
     int opcao;                                   
 
     do {
         // Exibe o menu de opções
         std::cout << "\n--- Menu de Operacoes ---\n";
         std::cout << "1. Converter CSV para Binario\n";
         std::cout << "2. Inserir um registro\n";
         std::cout << "3. Visualizar registros\n";
         std::cout << "4. Alterar um registro\n";
         std::cout << "5. Trocar dois registros\n";
         std::cout << "6. Imprimir todos os registros\n";
         std::cout << "7. Ordenar arquivo por ID\n";
         std::cout << "8. Verificar ordenacao do arquivo\n";
         std::cout << "0. Sair\n";
         std::cout << "Escolha uma opcao: ";
         std::cin >> opcao; 
 
         // Tratamento de entrada inválida
         while (std::cin.fail()) {
             std::cout << "Entrada invalida. Por favor, insira um numero." << std::endl;
             std::cin.clear();
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
             std::cout << "Escolha uma opcao: ";
             std::cin >> opcao;
         }
         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpa o buffer de entrada
 
         // Executa a ação baseada na opção
         switch (opcao) {
             case 1: gerenciador.converterCSVparaBinario(csvPath); break;
             case 2: gerenciador.inserirRegistro(); break;
             case 3: gerenciador.visualizarRegistros(); break;
             case 4: gerenciador.alterarRegistro(); break;
             case 5: gerenciador.trocarRegistros(); break;
             case 6: gerenciador.imprimirTodos(); break;
             case 7: gerenciador.mergeSortExterno(); break;
             case 8: gerenciador.verificarOrdenacao(); break;
             case 0: std::cout << "Saindo do programa..." << std::endl; break;
             default: std::cout << "Opcao invalida! Tente novamente." << std::endl; break;
         }
     } while (opcao != 0); // Continua até o usuário sair
 }
 
 // Função principal do programa
 int main() {
     menuPrincipal();
     return 0;        
 }