#include <iostream>
#include <fstream>
#include <cstring> // Para strcmp, strncpy, etc.
#include <cstdio>  // Para remove, rename, arquivos C-style
#include <cstdlib> // Para exit, malloc, free
#include <limits>  // Para numeric_limits

using namespace std;

// --- ESTRUTURAS DE DADOS ---

// Estrutura que representa cada registro de atleta.
struct Registro {
    int id;
    char name[200];
    char team[150];
    char games[150];
    int year;
    char season[20];
    bool removido;

    Registro() : id(0), year(0), removido(false) {
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

// --- PROTÓTIPOS DAS FUNÇÕES ---

// Funções do menu e operações em arquivo
void menuPrincipal();
void converterCSVparaBinario(const char* csvPath, const char* binPath);
void inserirRegistro(const char* binPath);
void visualizarRegistros(const char* binPath);
void alterarRegistro(const char* binPath);
void trocarRegistros(const char* binPath);
void imprimirTodos(const char* binPath);

// Funções de manipulação de arquivo (baixo nível)
void lerRegistro(fstream& arquivo, Registro& reg, long pos);
void escreverRegistro(fstream& arquivo, const Registro& reg, long pos);
int contarRegistros(const char* binPath);

// --- ALGORITMOS IMPLEMENTADOS MANUALMENTE ---

// 1. Quicksort para ordenar os blocos em memória
void trocar(Registro* a, Registro* b);
int particionar(Registro arr[], int baixo, int alto);
void quicksort(Registro arr[], int baixo, int alto);

// 2. Min-Heap para a intercalação dos blocos
struct MinHeap {
    HeapItem* harr; // Ponteiro para o array de elementos no heap
    int capacidade; // Capacidade máxima do min-heap
    int tamanho;    // Tamanho atual do min-heap
};
MinHeap* criarMinHeap(int capacidade);
void minHeapify(MinHeap* minHeap, int idx);
HeapItem extrairMin(MinHeap* minHeap);
void inserirMinHeap(MinHeap* minHeap, HeapItem item);
void destruirMinHeap(MinHeap* minHeap);

// 3. Ordenação Externa (Merge Sort)
void mergeSortExterno(const char* binPath);

// --- FUNÇÃO PRINCIPAL ---

int main() {
    menuPrincipal();
    return 0;
}

void menuPrincipal() {
    const char* csvPath = "dados.csv";
    const char* binPath = "dados.bin";
    int opcao;

    do {
        cout << "\n--- Menu de Operacoes \n";
        cout << "1. Converter CSV para Binario\n";
        cout << "2. Inserir um registro em uma posicao\n";
        cout << "3. Visualizar registros em um intervalo\n";
        cout << "4. Alterar um registro em uma posicao\n";
        cout << "5. Trocar dois registros de posicao\n";
        cout << "6. Imprimir todos os registros\n";
        cout << "7. Ordenar arquivo por ID (Merge Sort Externo)\n";
        cout << "0. Sair\n";
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        while (cin.fail()) {
            cout << "Entrada invalida. Por favor, insira um numero." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Escolha uma opcao: ";
            cin >> opcao;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpa o buffer de entrada

        switch (opcao) {
            case 1: converterCSVparaBinario(csvPath, binPath); break;
            case 2: inserirRegistro(binPath); break;
            case 3: visualizarRegistros(binPath); break;
            case 4: alterarRegistro(binPath); break;
            case 5: trocarRegistros(binPath); break;
            case 6: imprimirTodos(binPath); break;
            case 7: mergeSortExterno(binPath); break;
            case 0: cout << "Saindo do programa..." << endl; break;
            default: cout << "Opcao invalida! Tente novamente." << endl; break;
        }
    } while (opcao != 0);
}

// --- IMPLEMENTAÇÃO DO QUICKSORT ---

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

// --- IMPLEMENTAÇÃO DO MIN-HEAP ---

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
        itemVazio.registro.id = -1;
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
        cout << "Erro: Heap cheio." << endl;
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


// --- PARSER DE CSV E CONVERSÃO ---

void parseCSVLineManual(char* linha, char campos[6][200]) {
    int campoAtual = 0;
    int charAtual = 0;
    bool dentroDeAspas = false;

    for (int i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == '"') {
            dentroDeAspas = !dentroDeAspas;
        } else if (linha[i] == ',' && !dentroDeAspas) {
            campos[campoAtual][charAtual] = '\0';
            campoAtual++;
            charAtual = 0;
            if (campoAtual >= 6) break;
        } else {
            campos[campoAtual][charAtual] = linha[i];
            charAtual++;
        }
    }
    campos[campoAtual][charAtual] = '\0';
}

void converterCSVparaBinario(const char* csvPath, const char* binPath) {
    FILE* csv = fopen(csvPath, "r");
    if (!csv) {
        cerr << "Erro: Nao foi possivel abrir o arquivo CSV '" << csvPath << "'" << endl;
        return;
    }

    ofstream bin(binPath, ios::binary);
    if (!bin.is_open()) {
        cerr << "Erro: Nao foi possivel criar o arquivo binario '" << binPath << "'" << endl;
        fclose(csv);
        return;
    }

    char linha[1024];
    fgets(linha, sizeof(linha), csv); // Ignora cabeçalho

    while (fgets(linha, sizeof(linha), csv)) {
        linha[strcspn(linha, "\r\n")] = 0;

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
    cout << "=> Conversao concluida com sucesso!" << endl;
}


// --- FUNÇÕES DE OPERAÇÃO NO ARQUIVO BINÁRIO ---

void lerRegistro(fstream& arquivo, Registro& reg, long pos) {
    arquivo.seekg(pos * sizeof(Registro));
    arquivo.read(reinterpret_cast<char*>(&reg), sizeof(Registro));
}

void escreverRegistro(fstream& arquivo, const Registro& reg, long pos) {
    arquivo.seekp(pos * sizeof(Registro));
    arquivo.write(reinterpret_cast<const char*>(&reg), sizeof(Registro));
}

int contarRegistros(const char* binPath) {
    ifstream arquivo(binPath, ios::binary | ios::ate);
    if (!arquivo.is_open()) return 0;
    return arquivo.tellg() / sizeof(Registro);
}

void imprimirTodos(const char* binPath) {
    fstream bin(binPath, ios::binary | ios::in);
    if (!bin.is_open()) {
        cerr << "Erro ao abrir arquivo binario!" << endl;
        return;
    }
    int total = contarRegistros(binPath);
    if (total == 0) {
        cout << "Arquivo vazio." << endl;
        return;
    }
    
    cout << "\n--- Imprimindo todos os registros ---\n";
    for (int i = 0; i < total; i++) {
        Registro reg;
        lerRegistro(bin, reg, i);
        cout << "[" << i << "] ID: " << reg.id 
             << ", Nome: " << reg.name
             << ", Equipe: " << reg.team
             << ", Jogos: " << reg.games
             << ", Ano: " << reg.year
             << ", Temporada: " << reg.season << endl;
    }
}

void inserirRegistro(const char* binPath) {
    fstream bin(binPath, ios::binary | ios::in | ios::out);
    if (!bin.is_open()) {
        cerr << "Erro ao abrir arquivo binario para insercao!" << endl;
        return;
    }

    Registro novo;
    int pos;
    cout << "Digite os dados do novo registro:\n";
    cout << "ID: "; cin >> novo.id;
    cout << "Nome: "; cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.getline(novo.name, sizeof(novo.name));
    cout << "Equipe: "; cin.getline(novo.team, sizeof(novo.team));
    cout << "Jogos: "; cin.getline(novo.games, sizeof(novo.games));
    cout << "Ano: "; cin >> novo.year;
    cout << "Temporada: "; cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.getline(novo.season, sizeof(novo.season));
    cout << "Posicao para inserir: "; cin >> pos;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpa buffer para o próximo menu

    int totalRegistros = contarRegistros(binPath);
    if (pos < 0) pos = 0;

    if (pos >= totalRegistros) {
        escreverRegistro(bin, novo, totalRegistros);
        cout << "Registro inserido no final do arquivo (posicao " << totalRegistros << ")." << endl;
    } else {
        Registro temp;
        for (int i = totalRegistros - 1; i >= pos; i--) {
            lerRegistro(bin, temp, i);
            escreverRegistro(bin, temp, i + 1);
        }
        escreverRegistro(bin, novo, pos);
        cout << "Registro inserido na posicao " << pos << "." << endl;
    }
}

void visualizarRegistros(const char* binPath) {
    int inicio, fim;
    cout << "Posicao inicial: "; cin >> inicio;
    cout << "Posicao final: "; cin >> fim;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    fstream bin(binPath, ios::binary | ios::in);
    if (!bin.is_open()) {
        cerr << "Erro ao abrir arquivo binario!" << endl;
        return;
    }

    int total = contarRegistros(binPath);
    if (inicio < 0 || fim < inicio || inicio >= total) {
        cout << "Intervalo invalido." << endl;
        return;
    }
    if (fim >= total) fim = total -1;

    cout << "\n--- Exibindo Registros de " << inicio << " a " << fim << " ---\n";
    for (int i = inicio; i <= fim; i++) {
        Registro reg;
        lerRegistro(bin, reg, i);
        cout << "[" << i << "] ID: " << reg.id 
             << ", Nome: " << reg.name
             << ", Ano: " << reg.year << endl;
    }
}

void alterarRegistro(const char* binPath) {
    fstream bin(binPath, ios::binary | ios::in | ios::out);
    if (!bin.is_open()) {
        cerr << "Erro ao abrir arquivo binario!" << endl;
        return;
    }

    int pos;
    cout << "Posicao do registro a ser alterado: "; cin >> pos;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    int total = contarRegistros(binPath);
    if (pos < 0 || pos >= total) {
        cout << "Posicao invalida." << endl;
        return;
    }

    Registro reg;
    cout << "Digite os novos dados:\n";
    cout << "ID: "; cin >> reg.id;
    cout << "Nome: "; cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.getline(reg.name, sizeof(reg.name));
    cout << "Equipe: "; cin.getline(reg.team, sizeof(reg.team));
    cout << "Jogos: "; cin.getline(reg.games, sizeof(reg.games));
    cout << "Ano: "; cin >> reg.year;
    cout << "Temporada: "; cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.getline(reg.season, sizeof(reg.season));

    escreverRegistro(bin, reg, pos);
    cout << "Registro na posicao " << pos << " alterado." << endl;
}

void trocarRegistros(const char* binPath) {
    fstream bin(binPath, ios::binary | ios::in | ios::out);
    if (!bin.is_open()) {
        cerr << "Erro ao abrir arquivo binario!" << endl;
        return;
    }

    int pos1, pos2;
    cout << "Primeira posicao: "; cin >> pos1;
    cout << "Segunda posicao: "; cin >> pos2;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    int total = contarRegistros(binPath);
    if (pos1 < 0 || pos1 >= total || pos2 < 0 || pos2 >= total) {
        cout << "Uma ou ambas as posicoes sao invalidas." << endl;
        return;
    }

    Registro reg1, reg2;
    lerRegistro(bin, reg1, pos1);
    lerRegistro(bin, reg2, pos2);
    
    escreverRegistro(bin, reg1, pos2);
    escreverRegistro(bin, reg2, pos1);

    cout << "Registros das posicoes " << pos1 << " e " << pos2 << " trocados." << endl;
}

// --- IMPLEMENTAÇÃO DA ORDENAÇÃO EXTERNA ---

void mergeSortExterno(const char* binPath) {
    int totalRegistros = contarRegistros(binPath);
    if (totalRegistros <= 1) {
        cout << "Arquivo ja esta ordenado ou vazio." << endl;
        return;
    }

    const int TAM_BLOCO = 5000;
    int numBlocos = (totalRegistros + TAM_BLOCO - 1) / TAM_BLOCO;

    cout << "Iniciando Fase 1: Criacao de " << numBlocos << " blocos ordenados..." << endl;
    Registro* bloco = new Registro[TAM_BLOCO];
    ifstream entrada(binPath, ios::binary);
    
    for (int i = 0; i < numBlocos; i++) {
        int registrosNesteBloco = min(TAM_BLOCO, totalRegistros - i * TAM_BLOCO);
        entrada.read(reinterpret_cast<char*>(bloco), registrosNesteBloco * sizeof(Registro));

        quicksort(bloco, 0, registrosNesteBloco - 1);
        
        char nomeArquivoBloco[20];
        sprintf(nomeArquivoBloco, "bloco_%d.bin", i);
        ofstream saida(nomeArquivoBloco, ios::binary);
        saida.write(reinterpret_cast<const char*>(bloco), registrosNesteBloco * sizeof(Registro));
    }
    delete[] bloco;
    entrada.close();

    cout << "Iniciando Fase 2: Intercalacao dos blocos..." << endl;
    ifstream* entradasBlocos = new ifstream[numBlocos];
    for (int i = 0; i < numBlocos; i++) {
        char nomeArquivoBloco[20];
        sprintf(nomeArquivoBloco, "bloco_%d.bin", i);
        entradasBlocos[i].open(nomeArquivoBloco, ios::binary);
    }
    
    ofstream saidaFinal("ordenado_final.bin", ios::binary);
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
    for (int i = 0; i < numBlocos; i++) {
        entradasBlocos[i].close();
        char nomeArquivoBloco[20];
        sprintf(nomeArquivoBloco, "bloco_%d.bin", i);
        remove(nomeArquivoBloco);
    }
    delete[] entradasBlocos;
    saidaFinal.close();

    remove(binPath);
    rename("ordenado_final.bin", binPath);
    cout << "=> Arquivo ordenado com sucesso!!" << endl;
}
