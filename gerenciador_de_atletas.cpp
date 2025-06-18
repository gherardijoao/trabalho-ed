/*
 * Projeto Prático: Tratamento de Arquivos e Ordenação em Memória Secundária
 * Equipe:
 * - JONATHAN MARTINS CABRAL
 * - THALES MAIA MENDONÇA NEGRÃO
 * - JOÃO AMANCIO GHERARDI
 *
 * Data: 18 de Junho de 2025
 *
 * Descrição: Este arquivo de implementação (`.cpp`) contém as definições
 * dos métodos da classe `GerenciadorDeAtletas`. Aqui são implementadas
 * as funcionalidades de manipulação do arquivo binário, incluindo
 * a conversão de CSV para binário, inserção, visualização, alteração,
 * e troca de registros. Também inclui a implementação do algoritmo
 * de ordenação externa (Merge Sort Multi-way Crescente) e a função
 * de verificação de ordenação do arquivo resultante.
 */

 
 #include "gerenciador_de_atletas.h"
 #include "algoritmos.h"
 #include <iostream>
 #include <cstring>
 #include <cstdio>
 #include <limits>
 #include <algorithm> 
 
 // Implementação dos métodos da classe GerenciadorDeAtletas 
 
 // Construtor: inicializa o caminho do arquivo binário.
 GerenciadorDeAtletas::GerenciadorDeAtletas(const std::string& caminho) {
     this->caminhoArquivoBin = caminho;
 }
 
 // Lê um único registro do arquivo na posição especificada.
 void GerenciadorDeAtletas::lerRegistro(std::fstream& arquivo, Registro& reg, long pos) {
     arquivo.seekg(pos * sizeof(Registro)); // Posiciona o ponteiro de leitura
     arquivo.read(reinterpret_cast<char*>(&reg), sizeof(Registro)); // Lê o registro
 }
 
 // Escreve um único registro no arquivo na posição especificada.
 void GerenciadorDeAtletas::escreverRegistro(std::fstream& arquivo, const Registro& reg, long pos) {
     arquivo.seekp(pos * sizeof(Registro)); // Posiciona o ponteiro de escrita
     arquivo.write(reinterpret_cast<const char*>(&reg), sizeof(Registro)); // Escreve o registro
 }
 
 // Conta o número total de registros no arquivo binário.
 int GerenciadorDeAtletas::contarRegistros() {
     std::ifstream arquivo(this->caminhoArquivoBin, std::ios::binary | std::ios::ate);
     if (!arquivo.is_open()) return 0; // Retorna 0 se o arquivo não puder ser aberto
     long long tamanhoBytes = arquivo.tellg(); // Obtém o tamanho do arquivo em bytes
     arquivo.close();
     return tamanhoBytes / sizeof(Registro); // Calcula o número de registros
 }
 
 // Converte dados de um arquivo CSV para um arquivo binário.
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
     fgets(linha, sizeof(linha), csv); // Ignora a linha de cabeçalho do CSV
 
     // Lê cada linha do CSV, faz o parse e escreve no arquivo binário
     while (fgets(linha, sizeof(linha), csv)) {
         linha[strcspn(linha, "\r\n")] = 0; // Remove caracteres de quebra de linha
 
         char campos[6][200] = {0};
         parseCSVLineManual(linha, campos); // Processa a linha CSV
 
         Registro reg; // Cria um registro e preenche com os dados parseados
         if(strlen(campos[0]) > 0) reg.id = atoi(campos[0]); else reg.id = 0;
         strncpy(reg.name, campos[1], sizeof(reg.name) - 1);
         strncpy(reg.team, campos[2], sizeof(reg.team) - 1);
         strncpy(reg.games, campos[3], sizeof(reg.games) - 1);
         if(strlen(campos[4]) > 0) reg.year = atoi(campos[4]); else reg.year = 0;
         strncpy(reg.season, campos[5], sizeof(reg.season) - 1);
         
         bin.write(reinterpret_cast<const char*>(&reg), sizeof(Registro)); // Escreve o registro no binário
     }
 
     fclose(csv);
     bin.close();
     std::cout << "=> Conversao concluida com sucesso!" << std::endl;
 }
 
 // Imprime todos os registros armazenados no arquivo binário.
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
     // Percorre e imprime cada registro
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
 
 // Insere um novo registro no arquivo binário em uma posição específica.
 void GerenciadorDeAtletas::inserirRegistro() {
     std::fstream bin(this->caminhoArquivoBin, std::ios::binary | std::ios::in | std::ios::out);
     if (!bin.is_open()) {
         std::cerr << "Erro ao abrir arquivo binario para insercao!" << std::endl;
         return;
     }
 
     Registro novo;
     int pos;
     // Solicita os dados do novo registro ao usuário
     std::cout << "Digite os dados do novo registro:\n";
     std::cout << "ID: "; std::cin >> novo.id;
     std::cout << "Nome: "; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cin.getline(novo.name, sizeof(novo.name));
     std::cout << "Equipe: "; std::cin.getline(novo.team, sizeof(novo.team));
     std::cout << "Jogos: "; std::cin.getline(novo.games, sizeof(novo.games));
     std::cout << "Ano: "; std::cin >> novo.year;
     std::cout << "Temporada: "; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cin.getline(novo.season, sizeof(novo.season));
     std::cout << "Posicao para inserir: "; std::cin >> pos;
 
     int totalRegistros = contarRegistros();
     if (pos < 0) pos = 0; // Garante que a posição não seja negativa
 
     if (pos >= totalRegistros) {
         escreverRegistro(bin, novo, totalRegistros); // Insere no final se a posição for maior ou igual ao total
         std::cout << "Registro inserido no final do arquivo (posicao " << totalRegistros << ")." << std::endl;
     } else {
         // Desloca os registros existentes para abrir espaço na posição desejada
         for (int i = totalRegistros - 1; i >= pos; i--) {
             Registro temp;
             lerRegistro(bin, temp, i); // Lê o registro da posição atual
             escreverRegistro(bin, temp, i + 1); // Escreve na próxima posição
         }
         escreverRegistro(bin, novo, pos); // Insere o novo registro na posição livre
         std::cout << "Registro inserido na posicao " << pos << "." << std::endl;
     }
     bin.close();
 }
 
 // Visualiza registros dentro de um intervalo de posições especificado.
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
     // Valida o intervalo de posições
     if (inicio < 0 || fim < inicio || inicio >= total) {
         std::cout << "Intervalo invalido." << std::endl;
         bin.close();
         return;
     }
     if (fim >= total) fim = total -1; // Ajusta o fim para não exceder o total de registros
 
     std::cout << "\n--- Exibindo Registros de " << inicio << " a " << fim << " ---\n";
     // Lê e imprime registros no intervalo
     for (int i = inicio; i <= fim; i++) {
         Registro reg;
         lerRegistro(bin, reg, i);
         std::cout << "[" << i << "] ID: " << reg.id 
                   << ", Nome: " << reg.name
                   << ", Ano: " << reg.year << std::endl;
     }
     bin.close();
 }
 
 // Altera os dados de um registro existente em uma posição específica.
 void GerenciadorDeAtletas::alterarRegistro() {
     std::fstream bin(this->caminhoArquivoBin, std::ios::binary | std::ios::in | std::ios::out);
     if (!bin.is_open()) {
         std::cerr << "Erro ao abrir arquivo binario!" << std::endl;
         return;
     }
 
     int pos;
     std::cout << "Posicao do registro a ser alterado: "; std::cin >> pos;
     
     int total = contarRegistros();
     // Valida a posição
     if (pos < 0 || pos >= total) {
         std::cout << "Posicao invalida." << std::endl;
         bin.close();
         return;
     }
 
     Registro reg;
     // Solicita os novos dados ao usuário
     std::cout << "Digite os novos dados:\n";
     std::cout << "ID: "; std::cin >> reg.id;
     std::cout << "Nome: "; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cin.getline(reg.name, sizeof(reg.name));
     std::cout << "Equipe: "; std::cin.getline(reg.team, sizeof(reg.team));
     std::cout << "Jogos: "; std::cin.getline(reg.games, sizeof(reg.games));
     std::cout << "Ano: "; std::cin >> reg.year;
     std::cout << "Temporada: "; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cin.getline(reg.season, sizeof(reg.season));
 
     escreverRegistro(bin, reg, pos); // Sobrescreve o registro na posição
     std::cout << "Registro na posicao " << pos << " alterado." << std::endl;
     bin.close();
 }
 
 // Troca dois registros de posição no arquivo binário.
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
     // Valida as posições
     if (pos1 < 0 || pos1 >= total || pos2 < 0 || pos2 >= total) {
         std::cout << "Uma ou ambas as posicoes sao invalidas." << std::endl;
         bin.close();
         return;
     }
 
     Registro reg1, reg2;
     lerRegistro(bin, reg1, pos1); // Lê o registro da primeira posição
     lerRegistro(bin, reg2, pos2); // Lê o registro da segunda posição
     
     escreverRegistro(bin, reg1, pos2); // Escreve o primeiro no lugar do segundo
     escreverRegistro(bin, reg2, pos1); // Escreve o segundo no lugar do primeiro
 
     std::cout << "Registros das posicoes " << pos1 << " e " << pos2 << " trocados." << std::endl;
     bin.close();
 }
 
 // Implementa o algoritmo de Ordenação Externa (Merge Sort Multi-way Crescente).
 void GerenciadorDeAtletas::mergeSortExterno() {
     int totalRegistros = contarRegistros();
     if (totalRegistros <= 1) {
         std::cout << "Arquivo ja esta ordenado ou vazio." << std::endl;
         return;
     }
 
     const int TAM_BLOCO = 5000; // Tamanho do bloco para ordenação em memória
     int numBlocos = (totalRegistros + TAM_BLOCO - 1) / TAM_BLOCO;
 
     std::cout << "Iniciando Fase 1: Criacao de " << numBlocos << " blocos ordenados..." << std::endl;
     Registro* bloco = new Registro[TAM_BLOCO]; // Buffer para um bloco
     std::ifstream entrada(this->caminhoArquivoBin, std::ios::binary);
     
     // Divide o arquivo em blocos, ordena cada bloco e salva em arquivos temporários
     for (int i = 0; i < numBlocos; i++) {
         int registrosNesteBloco = std::min(TAM_BLOCO, totalRegistros - i * TAM_BLOCO);
         entrada.read(reinterpret_cast<char*>(bloco), registrosNesteBloco * sizeof(Registro));
 
         mergesort(bloco, 0, registrosNesteBloco - 1); // Ordena o bloco em memória
         
         char nomeArquivoBloco[20];
         sprintf(nomeArquivoBloco, "bloco_%d.bin", i);
         std::ofstream saidaBloco(nomeArquivoBloco, std::ios::binary);
         saidaBloco.write(reinterpret_cast<const char*>(bloco), registrosNesteBloco * sizeof(Registro));
         saidaBloco.close();
     }
     delete[] bloco;
     entrada.close();
 
     std::cout << "Iniciando Fase 2: Intercalacao dos blocos..." << std::endl;
     std::ifstream* entradasBlocos = new std::ifstream[numBlocos]; // Arrays para abrir todos os blocos
     for (int i = 0; i < numBlocos; i++) {
         char nomeArquivoBloco[20];
         sprintf(nomeArquivoBloco, "bloco_%d.bin", i);
         entradasBlocos[i].open(nomeArquivoBloco, std::ios::binary);
     }
     
     std::string nomeSaidaFinal = "ordenado_final.bin";
     std::ofstream saidaFinal(nomeSaidaFinal, std::ios::binary);
     MinHeap* heap = criarMinHeap(numBlocos); // Cria o Min-Heap para a intercalação multi-way
     
     // Lê o primeiro registro de cada bloco e insere no heap
     for (int i = 0; i < numBlocos; i++) {
         HeapItem item;
         if (entradasBlocos[i].read(reinterpret_cast<char*>(&item.registro), sizeof(Registro))) {
             item.origem = i; // Guarda de qual bloco o item veio
             inserirMinHeap(heap, item);
         }
     }
 
     // Intercala os registros usando o heap até que todos os blocos sejam lidos
     while (heap->tamanho > 0) {
         HeapItem minItem = extrairMin(heap); // Obtém o menor registro de todos os blocos
         saidaFinal.write(reinterpret_cast<const char*>(&minItem.registro), sizeof(Registro)); // Escreve no arquivo final
 
         HeapItem proximoItem;
         // Tenta ler o próximo registro do bloco de onde o item veio
         if (entradasBlocos[minItem.origem].read(reinterpret_cast<char*>(&proximoItem.registro), sizeof(Registro))) {
             proximoItem.origem = minItem.origem;
             inserirMinHeap(heap, proximoItem); // Insere o próximo registro no heap
         }
     }
 
     destruirMinHeap(heap);
     saidaFinal.close();
 
     // Fecha e remove os arquivos de blocos temporários
     for (int i = 0; i < numBlocos; i++) {
         entradasBlocos[i].close();
         char nomeArquivoBloco[20];
         sprintf(nomeArquivoBloco, "bloco_%d.bin", i);
         remove(nomeArquivoBloco);
     }
     delete[] entradasBlocos;
 
     remove(this->caminhoArquivoBin.c_str()); // Remove o arquivo original desordenado
     rename(nomeSaidaFinal.c_str(), this->caminhoArquivoBin.c_str()); // Renomeia o arquivo ordenado para o nome original
     std::cout << "=> Arquivo ordenado com sucesso!" << std::endl;
 }
 
 // Verifica se o arquivo binário está ordenado por ID de forma crescente.
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
     lerRegistro(bin, regAnterior, 0); // Lê o primeiro registro
 
     // Compara cada registro com o anterior para verificar a ordem
     for (int i = 1; i < total; i++) {
         lerRegistro(bin, regAtual, i);
         if (regAtual.id < regAnterior.id) {
             std::cout << "ERRO DE ORDENACAO! Na posicao " << i << ", o ID " << regAtual.id 
                       << " eh menor que o ID anterior " << regAnterior.id << "." << std::endl;
             bin.close();
             return;
         }
         regAnterior = regAtual; // Atualiza o registro anterior
     }
 
     std::cout << "=> Verificacao concluida. O arquivo esta perfeitamente ordenado por ID!" << std::endl;
     bin.close();
 }