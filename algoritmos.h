#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "gerenciador_de_atletas.h" // Precisa das definições de Registro e HeapItem

// Parser de CSV
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

#endif // ALGORITMOS_H
