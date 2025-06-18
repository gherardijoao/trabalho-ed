/*
 * Projeto Prático: Tratamento de Arquivos e Ordenação em Memória Secundária
 * Equipe:
 * - JONATHAN MARTINS CABRAL
 * - THALES MAIA MENDONÇA NEGRÃO
 * - JOÃO AMANCIO GHERARDI
 *
 * Data: 18 de Junho de 2025
 *
 * Descrição: Este arquivo de cabeçalho (`.h`) define as estruturas de dados
 * principais utilizadas no projeto, como `Registro` (para cada atleta),
 * `HeapItem` (para auxiliar no Min-Heap durante a ordenação), e a
 * declaração da classe `GerenciadorDeAtletas`. A classe `GerenciadorDeAtletas`
 * é responsável por encapsular as operações de manipulação do arquivo binário,
 * como leitura, escrita, inserção, visualização, alteração e troca de registros.
 */


#ifndef GERENCIADOR_DE_ATLETAS_H
#define GERENCIADOR_DE_ATLETAS_H

#include <string>
#include <fstream>

// Estruturas de dados

// Estrutura que representa cada registro de atleta.
struct Registro {
    int id;
    char name[200];
    char team[150];
    char games[150];
    int year;
    char season[20];

    Registro() : id(0), year(0) {
        name[0] = '\0';
        team[0] = '\0';
        games[0] = '\0';
        season[0] = '\0';
    }
};

// Estrutura para os itens do heap usado na intercalação (merge).
struct HeapItem {
    Registro registro;
    int origem; // Índice do arquivo de bloco de onde o registro veio.
};

// Estrutura para o Min-Heap (implementação manual)
struct MinHeap {
    HeapItem* harr;
    int capacidade;
    int tamanho;
};


// Classe principal para gereniamento do arquivo

class GerenciadorDeAtletas {
private:
    std::string caminhoArquivoBin;

    // Métodos auxiliares privados para manipulação de baixo nível
    void lerRegistro(std::fstream& arquivo, Registro& reg, long pos);
    void escreverRegistro(std::fstream& arquivo, const Registro& reg, long pos);
    int contarRegistros();

public:
    // Construtor
    GerenciadorDeAtletas(const std::string& caminho);

    // Métodos públicos para as operações do menu
    void converterCSVparaBinario(const std::string& caminhoCSV);
    void inserirRegistro();
    void visualizarRegistros();
    void alterarRegistro();
    void trocarRegistros();
    void imprimirTodos();
    void mergeSortExterno();
    void verificarOrdenacao(); // Função para validar se o arquivo está ordenado
};

#endif 
