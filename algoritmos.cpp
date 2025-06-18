/*
 * Projeto Prático: Tratamento de Arquivos e Ordenação em Memória Secundária
 * Equipe:
 * - JONATHAN MARTINS CABRAL
 * - THALES MAIA MENDONÇA NEGRÃO
 * - JOÃO AMANCIO GHERARDI
 *
 * Data: 18 de Junho de 2025
 *
 * Descrição: Este arquivo de implementação (`.cpp`) contém o código-fonte
 * das funções auxiliares e dos algoritmos de ordenação. Aqui estão as
 * implementações do parser manual de linhas CSV, o algoritmo Merge Sort
 * para ordenação em memória, e as funções que compõem a estrutura e
 * operações do Min-Heap, que é crucial para a fase de intercalação
 * do algoritmo de ordenação externa (Merge Sort Multi-way Crescente).
 */


#include "algoritmos.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

// Implementação das funções auxiliares e algoritmos 

void parseCSVLineManual(char* linha, char campos[6][200]) {
    int campoAtual = 0;
    int charAtual = 0;
    bool dentroDeAspas = false;
    int i = 0;

    
    while (linha[i] != '\0' && campoAtual < 6) {
        if (linha[i] == '"') {
            dentroDeAspas = !dentroDeAspas;
        } else if (linha[i] == ',' && !dentroDeAspas) {
            campos[campoAtual][charAtual] = '\0'; 
            campoAtual++;
            charAtual = 0;
        } else {
            campos[campoAtual][charAtual] = linha[i];
            charAtual++;
        }
        i++;
    }
    campos[campoAtual][charAtual] = '\0';
}

// Mergesort
void merge(Registro arr[], int esq, int meio, int dir) {
    int i, j, k;
    int n1 = meio - esq + 1;
    int n2 = dir - meio;

    // Cria arrays temporários
    Registro* L = new Registro[n1];
    Registro* R = new Registro[n2];

    // Copia dados para os arrays temporários L[] e R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[esq + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[meio + 1 + j];

    // Intercala os arrays temporários de volta em arr[esq..dir]
    i = 0; 
    j = 0; 
    k = esq; 
    while (i < n1 && j < n2) {
        if (L[i].id <= R[j].id) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copia os elementos restantes de L[], se houver algum
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copia os elementos restantes de R[], se houver algum
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
    
    // Libera a memória dos arrays temporários
    delete[] L;
    delete[] R;
}

void mergesort(Registro arr[], int esq, int dir) {
    if (esq < dir) {
        int meio = esq + (dir - esq) / 2;

        // Ordena a primeira e a segunda metade
        mergesort(arr, esq, meio);
        mergesort(arr, meio + 1, dir);

        // Intercala as metades ordenadas
        merge(arr, esq, meio, dir);
    }
}

// Cria e inicializa uma nova estrutura MinHeap com a capacidade especificada.
MinHeap* criarMinHeap(int capacidade) {
    MinHeap* minHeap = (MinHeap*) malloc(sizeof(MinHeap)); // Aloca memória para a estrutura do heap
    minHeap->capacidade = capacidade; // Define a capacidade máxima
    minHeap->tamanho = 0; // Inicializa o tamanho atual como zero
    minHeap->harr = (HeapItem*) malloc(capacidade * sizeof(HeapItem)); // Aloca memória para os elementos do heap
    return minHeap;
}

// Libera a memória alocada para a estrutura MinHeap e seus elementos.
void destruirMinHeap(MinHeap* minHeap) {
    free(minHeap->harr); // Libera o array de elementos
    free(minHeap); // Libera a própria estrutura do heap
}

// Restaura a propriedade de min-heap a partir de um nó específico (idx).
void minHeapify(MinHeap* minHeap, int idx) {
    int menor = idx; 
    int esq = 2 * idx + 1;
    int dir = 2 * idx + 2; 

    // Verifica se o filho esquerdo existe e é menor
    if (esq < minHeap->tamanho && minHeap->harr[esq].registro.id < minHeap->harr[menor].registro.id)
        menor = esq;

    // Verifica se o filho direito existe e é menor que o atual "menor"
    if (dir < minHeap->tamanho && minHeap->harr[dir].registro.id < minHeap->harr[menor].registro.id)
        menor = dir;

    // Se o menor não for o nó atual, troca e chama recursivamente
    if (menor != idx) {
        HeapItem temp = minHeap->harr[menor];
        minHeap->harr[menor] = minHeap->harr[idx];
        minHeap->harr[idx] = temp;
        minHeapify(minHeap, menor); // Continua heapify na subárvore afetada
    }
}

// Remove e retorna o menor elemento (raiz) do min-heap.
HeapItem extrairMin(MinHeap* minHeap) {
    // Retorna item vazio se o heap estiver vazio
    if (minHeap->tamanho <= 0) {
        HeapItem itemVazio;
        itemVazio.registro.id = -1; 
        return itemVazio;
    }
    // Caso especial: apenas um elemento no heap
    if (minHeap->tamanho == 1) {
        minHeap->tamanho--;
        return minHeap->harr[0];
    }

    HeapItem raiz = minHeap->harr[0]; // Salva a raiz (menor elemento)
    minHeap->harr[0] = minHeap->harr[minHeap->tamanho - 1]; // Move o último elemento para a raiz
    minHeap->tamanho--; // Diminui o tamanho do heap
    minHeapify(minHeap, 0); // Restaura a propriedade do heap a partir da nova raiz

    return raiz; // Retorna o menor elemento
}

// Insere um novo item no min-heap, mantendo a propriedade do heap.
void inserirMinHeap(MinHeap* minHeap, HeapItem item) {
    // Verifica se o heap está cheio
    if (minHeap->tamanho == minHeap->capacidade) {
        std::cout << "Erro: Heap cheio." << std::endl;
        return;
    }

    minHeap->tamanho++; // Aumenta o tamanho do heap
    int i = minHeap->tamanho - 1; // Posição para o novo item

    minHeap->harr[i] = item; // Coloca o novo item na última posição

    // Corrige a posição do item subindo na árvore, se necessário
    while (i != 0 && minHeap->harr[(i - 1) / 2].registro.id > minHeap->harr[i].registro.id) {
       HeapItem temp = minHeap->harr[(i - 1) / 2];
       minHeap->harr[(i - 1) / 2] = minHeap->harr[i];
       minHeap->harr[i] = temp;
       i = (i - 1) / 2; // Move para a posição do pai
    }
}
