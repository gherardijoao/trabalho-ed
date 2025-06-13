#include "gerenciador_de_atletas.h"
#include "algoritmos.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <limits>
#include <algorithm> // Para std::min

// --- IMPLEMENTAÇÃO DOS MÉTODOS DA CLASSE GerenciadorDeAtletas ---

GerenciadorDeAtletas::GerenciadorDeAtletas(const std::string& caminho) {
    this->caminhoArquivoBin = caminho;
}

void GerenciadorDeAtletas::lerRegistro(std::fstream& arquivo, Registro& reg, long pos) {
    arquivo.seekg(pos * sizeof(Registro));
    arquivo.read(reinterpret_cast<char*>(&reg), sizeof(Registro));
}

void GerenciadorDeAtletas::escreverRegistro(std::fstream& arquivo, const Registro& reg, long pos) {
    arquivo.seekp(pos * sizeof(Registro));
    arquivo.write(reinterpret_cast<const char*>(&reg), sizeof(Registro));
}

int GerenciadorDeAtletas::contarRegistros() {
    std::ifstream arquivo(this->caminhoArquivoBin, std::ios::binary | std::ios::ate);
    if (!arquivo.is_open()) return 0;
    long long tamanhoBytes = arquivo.tellg();
    arquivo.close();
    return tamanhoBytes / sizeof(Registro);
}

void GerenciadorDeAtletas::converterCSVparaBinario(const std::string& caminhoCSV) {
    FILE* csv = fopen(caminhoCSV.c_str(), "r");
    if (!csv) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo CSV '" << caminhoCSV << "'" << std::endl;
        return;
    }

    std::ofstream bin(this->caminhoArquivoBin, std::ios::binary);
    if (!bin.is_open()) {
        std::cerr << "Erro: Nao foi possivel criar o arquivo binario '" << this->caminhoArquivoBin << "'" << std::endl;
        fclose(csv);
        return;
    }

    char linha[1024];
    fgets(linha, sizeof(linha), csv); // Ignora cabeçalho

    while (fgets(linha, sizeof(linha), csv)) {
        linha[strcspn(linha, "\r\n")] = 0; // Remove quebra de linha

        char campos[6][200] = {0};
        parseCSVLineManual(linha, campos);
        
        Registro reg;
        if(strlen(campos[0]) > 0) reg.id = atoi(campos[0]); else reg.id = 0;
        strncpy(reg.name, campos[1], sizeof(reg.name) - 1);
        strncpy(reg.team, campos[2], sizeof(reg.team) - 1);
        strncpy(reg.games, campos[3], sizeof(reg.games) - 1);
        if(strlen(campos[4]) > 0) reg.year = atoi(campos[4]); else reg.year = 0;
        strncpy(reg.season, campos[5], sizeof(reg.season) - 1);
        
        bin.write(reinterpret_cast<const char*>(&reg), sizeof(Registro));
    }

    fclose(csv);
    bin.close();
    std::cout << "=> Conversao concluida com sucesso!" << std::endl;
}

void GerenciadorDeAtletas::imprimirTodos() {
    std::fstream bin(this->caminhoArquivoBin, std::ios::binary | std::ios::in);
    if (!bin.is_open()) {
        std::cerr << "Erro ao abrir arquivo binario!" << std::endl;
        return;
    }
    int total = contarRegistros();
    if (total == 0) {
        std::cout << "Arquivo vazio." << std::endl;
        bin.close();
        return;
    }
    
    std::cout << "\n--- Imprimindo todos os registros ---\n";
    for (int i = 0; i < total; i++) {
        Registro reg;
        lerRegistro(bin, reg, i);
        std::cout << "[" << i << "] ID: " << reg.id 
             << ", Nome: " << reg.name
             << ", Equipe: " << reg.team
             << ", Ano: " << reg.year << std::endl;
    }
    bin.close();
}

void GerenciadorDeAtletas::inserirRegistro() {
    std::fstream bin(this->caminhoArquivoBin, std::ios::binary | std::ios::in | std::ios::out);
    if (!bin.is_open()) {
        std::cerr << "Erro ao abrir arquivo binario para insercao!" << std::endl;
        return;
    }

    Registro novo;
    int pos;
    std::cout << "Digite os dados do novo registro:\n";
    std::cout << "ID: "; std::cin >> novo.id;
    std::cout << "Nome: "; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cin.getline(novo.name, sizeof(novo.name));
    std::cout << "Equipe: "; std::cin.getline(novo.team, sizeof(novo.team));
    std::cout << "Jogos: "; std::cin.getline(novo.games, sizeof(novo.games));
    std::cout << "Ano: "; std::cin >> novo.year;
    std::cout << "Temporada: "; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cin.getline(novo.season, sizeof(novo.season));
    std::cout << "Posicao para inserir: "; std::cin >> pos;

    int totalRegistros = contarRegistros();
    if (pos < 0) pos = 0;

    if (pos >= totalRegistros) {
        escreverRegistro(bin, novo, totalRegistros);
        std::cout << "Registro inserido no final do arquivo (posicao " << totalRegistros << ")." << std::endl;
    } else {
        // Desloca os registros para abrir espaço
        for (int i = totalRegistros - 1; i >= pos; i--) {
            Registro temp;
            lerRegistro(bin, temp, i);
            escreverRegistro(bin, temp, i + 1);
        }
        escreverRegistro(bin, novo, pos);
        std::cout << "Registro inserido na posicao " << pos << "." << std::endl;
    }
    bin.close();
}

void GerenciadorDeAtletas::visualizarRegistros() {
    int inicio, fim;
    std::cout << "Posicao inicial: "; std::cin >> inicio;
    std::cout << "Posicao final: "; std::cin >> fim;

    std::fstream bin(this->caminhoArquivoBin, std::ios::binary | std::ios::in);
    if (!bin.is_open()) {
        std::cerr << "Erro ao abrir arquivo binario!" << std::endl;
        return;
    }

    int total = contarRegistros();
    if (inicio < 0 || fim < inicio || inicio >= total) {
        std::cout << "Intervalo invalido." << std::endl;
        bin.close();
        return;
    }
    if (fim >= total) fim = total -1;

    std::cout << "\n--- Exibindo Registros de " << inicio << " a " << fim << " ---\n";
    for (int i = inicio; i <= fim; i++) {
        Registro reg;
        lerRegistro(bin, reg, i);
        std::cout << "[" << i << "] ID: " << reg.id 
             << ", Nome: " << reg.name
             << ", Ano: " << reg.year << std::endl;
    }
    bin.close();
}

void GerenciadorDeAtletas::alterarRegistro() {
    std::fstream bin(this->caminhoArquivoBin, std::ios::binary | std::ios::in | std::ios::out);
    if (!bin.is_open()) {
        std::cerr << "Erro ao abrir arquivo binario!" << std::endl;
        return;
    }

    int pos;
    std::cout << "Posicao do registro a ser alterado: "; std::cin >> pos;
    
    int total = contarRegistros();
    if (pos < 0 || pos >= total) {
        std::cout << "Posicao invalida." << std::endl;
        bin.close();
        return;
    }

    Registro reg;
    std::cout << "Digite os novos dados:\n";
    std::cout << "ID: "; std::cin >> reg.id;
    std::cout << "Nome: "; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cin.getline(reg.name, sizeof(reg.name));
    std::cout << "Equipe: "; std::cin.getline(reg.team, sizeof(reg.team));
    std::cout << "Jogos: "; std::cin.getline(reg.games, sizeof(reg.games));
    std::cout << "Ano: "; std::cin >> reg.year;
    std::cout << "Temporada: "; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cin.getline(reg.season, sizeof(reg.season));

    escreverRegistro(bin, reg, pos);
    std::cout << "Registro na posicao " << pos << " alterado." << std::endl;
    bin.close();
}

void GerenciadorDeAtletas::trocarRegistros() {
    std::fstream bin(this->caminhoArquivoBin, std::ios::binary | std::ios::in | std::ios::out);
    if (!bin.is_open()) {
        std::cerr << "Erro ao abrir arquivo binario!" << std::endl;
        return;
    }

    int pos1, pos2;
    std::cout << "Primeira posicao: "; std::cin >> pos1;
    std::cout << "Segunda posicao: "; std::cin >> pos2;
    
    int total = contarRegistros();
    if (pos1 < 0 || pos1 >= total || pos2 < 0 || pos2 >= total) {
        std::cout << "Uma ou ambas as posicoes sao invalidas." << std::endl;
        bin.close();
        return;
    }

    Registro reg1, reg2;
    lerRegistro(bin, reg1, pos1);
    lerRegistro(bin, reg2, pos2);
    
    escreverRegistro(bin, reg1, pos2);
    escreverRegistro(bin, reg2, pos1);

    std::cout << "Registros das posicoes " << pos1 << " e " << pos2 << " trocados." << std::endl;
    bin.close();
}

void GerenciadorDeAtletas::mergeSortExterno() {
    int totalRegistros = contarRegistros();
    if (totalRegistros <= 1) {
        std::cout << "Arquivo ja esta ordenado ou vazio." << std::endl;
        return;
    }

    const int TAM_BLOCO = 5000;
    int numBlocos = (totalRegistros + TAM_BLOCO - 1) / TAM_BLOCO;

    std::cout << "Iniciando Fase 1: Criacao de " << numBlocos << " blocos ordenados..." << std::endl;
    Registro* bloco = new Registro[TAM_BLOCO];
    std::ifstream entrada(this->caminhoArquivoBin, std::ios::binary);
    
    for (int i = 0; i < numBlocos; i++) {
        int registrosNesteBloco = std::min(TAM_BLOCO, totalRegistros - i * TAM_BLOCO);
        entrada.read(reinterpret_cast<char*>(bloco), registrosNesteBloco * sizeof(Registro));

        quicksort(bloco, 0, registrosNesteBloco - 1);
        
        char nomeArquivoBloco[20];
        sprintf(nomeArquivoBloco, "bloco_%d.bin", i);
        std::ofstream saidaBloco(nomeArquivoBloco, std::ios::binary);
        saidaBloco.write(reinterpret_cast<const char*>(bloco), registrosNesteBloco * sizeof(Registro));
        saidaBloco.close();
    }
    delete[] bloco;
    entrada.close();

    std::cout << "Iniciando Fase 2: Intercalacao dos blocos..." << std::endl;
    std::ifstream* entradasBlocos = new std::ifstream[numBlocos];
    for (int i = 0; i < numBlocos; i++) {
        char nomeArquivoBloco[20];
        sprintf(nomeArquivoBloco, "bloco_%d.bin", i);
        entradasBlocos[i].open(nomeArquivoBloco, std::ios::binary);
    }
    
    std::string nomeSaidaFinal = "ordenado_final.bin";
    std::ofstream saidaFinal(nomeSaidaFinal, std::ios::binary);
    MinHeap* heap = criarMinHeap(numBlocos);
    
    for (int i = 0; i < numBlocos; i++) {
        HeapItem item;
        if (entradasBlocos[i].read(reinterpret_cast<char*>(&item.registro), sizeof(Registro))) {
            item.origem = i;
            inserirMinHeap(heap, item);
        }
    }

    while (heap->tamanho > 0) {
        HeapItem minItem = extrairMin(heap);
        saidaFinal.write(reinterpret_cast<const char*>(&minItem.registro), sizeof(Registro));

        HeapItem proximoItem;
        if (entradasBlocos[minItem.origem].read(reinterpret_cast<char*>(&proximoItem.registro), sizeof(Registro))) {
            proximoItem.origem = minItem.origem;
            inserirMinHeap(heap, proximoItem);
        }
    }

    destruirMinHeap(heap);
    saidaFinal.close();

    for (int i = 0; i < numBlocos; i++) {
        entradasBlocos[i].close();
        char nomeArquivoBloco[20];
        sprintf(nomeArquivoBloco, "bloco_%d.bin", i);
        remove(nomeArquivoBloco);
    }
    delete[] entradasBlocos;

    remove(this->caminhoArquivoBin.c_str());
    rename(nomeSaidaFinal.c_str(), this->caminhoArquivoBin.c_str());
    std::cout << "=> Arquivo ordenado com sucesso!" << std::endl;
}

void GerenciadorDeAtletas::verificarOrdenacao() {
    std::cout << "\n--- Iniciando verificacao de ordenacao ---\n";
    std::fstream bin(this->caminhoArquivoBin, std::ios::binary | std::ios::in);
    if (!bin.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo para verificacao." << std::endl;
        return;
    }

    int total = contarRegistros();
    if (total < 2) {
        std::cout << "=> Arquivo ordenado (contem menos de 2 registros)." << std::endl;
        bin.close();
        return;
    }

    Registro regAnterior, regAtual;
    lerRegistro(bin, regAnterior, 0);

    for (int i = 1; i < total; i++) {
        lerRegistro(bin, regAtual, i);
        if (regAtual.id < regAnterior.id) {
            std::cout << "ERRO DE ORDENACAO! Na posicao " << i << ", o ID " << regAtual.id 
                      << " eh menor que o ID anterior " << regAnterior.id << "." << std::endl;
            bin.close();
            return;
        }
        regAnterior = regAtual;
    }

    std::cout << "=> Verificacao concluida. O arquivo esta perfeitamente ordenado por ID!" << std::endl;
    bin.close();
}
