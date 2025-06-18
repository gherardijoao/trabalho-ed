/*
 * Projeto Prático: Tratamento de Arquivos e Ordenação em Memória Secundária
 * Equipe:
 * - JONATHAN MARTINS CABRAL
 * - THALES MAIA MENDONÇA NEGRÃO
 * - JOÃO AMANCIO GHERARDI
 *
 * Data: 18 de Junho de 2025
 *
 * Descrição: Este arquivo de cabeçalho (`.h`) contém as declarações de todas as
 * funções auxiliares e estruturas de dados relacionadas aos algoritmos
 * de ordenação. Inclui as definições para o parser de linhas CSV,
 * o algoritmo Merge Sort em memória (usado para ordenar blocos internos),
 * a estrutura Min-Heap (essencial para a intercalação multi-way),
 * e a interface da função principal para o Merge Sort Multi-way Crescente.
 */


#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "gerenciador_de_atletas.h" // Precisa das definições de Registro e HeapItem

// Parser de CSV
void parseCSVLineManual(char* linha, char campos[6][200]);

// Algoritmos implementados manualmente
// 1. Mergesort
void merge(Registro arr[], int esq, int meio, int dir);
void mergesort(Registro arr[], int esq, int dir);

// 2. Min-Heap
MinHeap* criarMinHeap(int capacidade);
void minHeapify(MinHeap* minHeap, int idx);
HeapItem extrairMin(MinHeap* minHeap);
void inserirMinHeap(MinHeap* minHeap, HeapItem item);
void destruirMinHeap(MinHeap* minHeap);

#endif // ALGORITMOS_H
