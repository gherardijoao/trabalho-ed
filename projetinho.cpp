#include <iostream>
#include <fstream>
#include <string>  // Usado na classe GerenciadorDeAtletas
#include <cstring> // Para strcmp, strncpy, etc.
#include <cstdio>  // Para remove, rename
#include <cstdlib> // Para exit, malloc, free
#include <limits>  // Para numeric_limits

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
};


// --- PROTÓTIPOS DAS FUNÇÕES AUXILIARES E ALGORITMOS ---

// Menu
void menuPrincipal();

// Parser de CSV (agora com while)
void parseCSVLineManual(char* linha, char campos[6][200]);

// Algoritmos implementados manualmente
// 1. Quicksort
void trocar(Registro* a, Registro* b);
int particionar(Registro arr[], int baixo, int alto);
void quicksort(Registro arr[], int baixo, int alto);

// 2. Min-Heap
MinHeap* criarMinHeap(int capacidade);
void minHeapify(MinHeap* minHeap, int idx);
HeapItem extrairMin(MinHeap* minHeap);
void inserirMinHeap(MinHeap* minHeap, HeapItem item);
void destruirMinHeap(MinHeap* minHeap);


// --- FUNÇÃO PRINCIPAL ---

int main() {
    menuPrincipal();
    return 0;
}

void menuPrincipal() {
    // Objeto que gerencia todas as operações no arquivo binário
    GerenciadorDeAtletas gerenciador("dados.bin");
    const std::string csvPath = "dados.csv";
    int opcao;

    do {
        std::cout << "\n--- Menu de Operacoes ---\n";
        std::cout << "1. Converter CSV para Binario\n";
        std::cout << "2. Inserir um registro em uma posicao\n";
        std::cout << "3. Visualizar registros em um intervalo\n";
        std::cout << "4. Alterar um registro em uma posicao\n";
        std::cout << "5. Trocar dois registros de posicao\n";
        std::cout << "6. Imprimir todos os registros\n";
        std::cout << "7. Ordenar arquivo por ID (Merge Sort Externo)\n";
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
            case 0: std::cout << "Saindo do programa..." << std::endl; break;
            default: std::cout << "Opcao invalida! Tente novamente." << std::endl; break;
        }
    } while (opcao != 0);
}

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
    return arquivo.tellg() / sizeof(Registro);
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
        Registro temp;
        for (int i = totalRegistros - 1; i >= pos; i--) {
            lerRegistro(bin, temp, i);
            escreverRegistro(bin, temp, i + 1);
        }
        escreverRegistro(bin, novo, pos);
        std::cout << "Registro inserido na posicao " << pos << "." << std::endl;
    }
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
        return;
    }

    Registro reg1, reg2;
    lerRegistro(bin, reg1, pos1);
    lerRegistro(bin, reg2, pos2);
    
    escreverRegistro(bin, reg1, pos2);
    escreverRegistro(bin, reg2, pos1);

    std::cout << "Registros das posicoes " << pos1 << " e " << pos2 << " trocados." << std::endl;
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


// --- IMPLEMENTAÇÃO DAS FUNÇÕES AUXILIARES E ALGORITMOS ---

void parseCSVLineManual(char* linha, char campos[6][200]) {
    int campoAtual = 0;
    int charAtual = 0;
    bool dentroDeAspas = false;
    int i = 0;

    while (linha[i] != '\0' && campoAtual < 6) {
        if (linha[i] == '"') {
            dentroDeAspas = !dentroDeAspas;
        } else if (linha[i] == ',' && !dentroDeAspas) {
            campos[campoAtual][charAtual] = '\0'; // Finaliza campo atual
            campoAtual++;
            charAtual = 0;
        } else {
            campos[campoAtual][charAtual] = linha[i];
            charAtual++;
        }
        i++; // Incrementa o contador de caracteres da linha
    }
    campos[campoAtual][charAtual] = '\0'; // Finaliza o último campo
}

// --- Quicksort ---
void trocar(Registro* a, Registro* b) {
    Registro temp = *a;
    *a = *b;
    *b = temp;
}

int particionar(Registro arr[], int baixo, int alto) {
    int pivo = arr[alto].id;
    int i = (baixo - 1);

    for (int j = baixo; j <= alto - 1; j++) {
        if (arr[j].id < pivo) {
            i++;
            trocar(&arr[i], &arr[j]);
        }
    }
    trocar(&arr[i + 1], &arr[alto]);
    return (i + 1);
}

void quicksort(Registro arr[], int baixo, int alto) {
    if (baixo < alto) {
        int pi = particionar(arr, baixo, alto);
        quicksort(arr, baixo, pi - 1);
        quicksort(arr, pi + 1, alto);
    }
}

// --- Min-Heap ---
MinHeap* criarMinHeap(int capacidade) {
    MinHeap* minHeap = (MinHeap*) malloc(sizeof(MinHeap));
    minHeap->capacidade = capacidade;
    minHeap->tamanho = 0;
    minHeap->harr = (HeapItem*) malloc(capacidade * sizeof(HeapItem));
    return minHeap;
}

void destruirMinHeap(MinHeap* minHeap) {
    free(minHeap->harr);
    free(minHeap);
}

void minHeapify(MinHeap* minHeap, int idx) {
    int menor = idx;
    int esq = 2 * idx + 1;
    int dir = 2 * idx + 2;

    if (esq < minHeap->tamanho && minHeap->harr[esq].registro.id < minHeap->harr[menor].registro.id)
        menor = esq;

    if (dir < minHeap->tamanho && minHeap->harr[dir].registro.id < minHeap->harr[menor].registro.id)
        menor = dir;

    if (menor != idx) {
        HeapItem temp = minHeap->harr[menor];
        minHeap->harr[menor] = minHeap->harr[idx];
        minHeap->harr[idx] = temp;
        minHeapify(minHeap, menor);
    }
}

HeapItem extrairMin(MinHeap* minHeap) {
    if (minHeap->tamanho <= 0) {
        HeapItem itemVazio;
        itemVazio.registro.id = -1; // Sinaliza erro/vazio
        return itemVazio;
    }
    if (minHeap->tamanho == 1) {
        minHeap->tamanho--;
        return minHeap->harr[0];
    }

    HeapItem raiz = minHeap->harr[0];
    minHeap->harr[0] = minHeap->harr[minHeap->tamanho - 1];
    minHeap->tamanho--;
    minHeapify(minHeap, 0);

    return raiz;
}

void inserirMinHeap(MinHeap* minHeap, HeapItem item) {
    if (minHeap->tamanho == minHeap->capacidade) {
        std::cout << "Erro: Heap cheio." << std::endl;
        return;
    }

    minHeap->tamanho++;
    int i = minHeap->tamanho - 1;
    minHeap->harr[i] = item;

    while (i != 0 && minHeap->harr[(i - 1) / 2].registro.id > minHeap->harr[i].registro.id) {
       HeapItem temp = minHeap->harr[(i - 1) / 2];
       minHeap->harr[(i - 1) / 2] = minHeap->harr[i];
       minHeap->harr[i] = temp;
       i = (i - 1) / 2;
    }
}