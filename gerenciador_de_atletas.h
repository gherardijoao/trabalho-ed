#ifndef GERENCIADOR_DE_ATLETAS_H
#define GERENCIADOR_DE_ATLETAS_H

#include <string>
#include <fstream>

// --- ESTRUTURAS DE DADOS ---

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


// --- CLASSE PRINCIPAL PARA GERENCIAMENTO DO ARQUIVO ---

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

#endif // GERENCIADOR_DE_ATLETAS_H
