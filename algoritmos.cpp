#include "algoritmos.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

// --- IMPLEMENTAÇÃO DAS FUNÇÕES AUXILIARES E ALGORITMOS ---

void parseCSVLineManual(char* linha, char campos[6][200]) {
    int campoAtual = 0;
    int charAtual = 0;
    bool dentroDeAspas = false;
    int i = 0;

    // Laço while, em conformidade com as regras mais rígidas.
    // Termina se chegar ao fim da string ou se já tiver lido os 6 campos.
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

// --- Mergesort ---
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
    i = 0; // Índice inicial do primeiro subarray
    j = 0; // Índice inicial do segundo subarray
    k = esq; // Índice inicial do subarray intercalado
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
        // Encontra o ponto do meio para evitar overflow
        int meio = esq + (dir - esq) / 2;

        // Ordena a primeira e a segunda metade
        mergesort(arr, esq, meio);
        mergesort(arr, meio + 1, dir);

        // Intercala as metades ordenadas
        merge(arr, esq, meio, dir);
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
