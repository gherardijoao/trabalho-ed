# Projeto Prático: Tratamento de Arquivos e Ordenação em Memória Secundária

Este documento fornece informações essenciais para a compilação, execução e entendimento do projeto prático de Tratamento de Arquivos e Ordenação em Memória Secundária.

---

## 1. Descrição do Projeto

Este projeto implementa um sistema em **C++** capaz de:

- Converter arquivos **CSV** em formato **binário**.
- Realizar diversas operações de manipulação de registros diretamente no arquivo binário (em memória secundária).

A funcionalidade principal é a **ordenação externa dos registros por ID**, utilizando um **algoritmo Merge Sort Multi-way Crescente**, projetado para lidar com grandes volumes de dados que não cabem integralmente na memória RAM.

### Operações suportadas:

- Conversão de CSV para binário.
- Inserção de novos registros em posições específicas.
- Visualização de registros em um intervalo.
- Alteração de dados de um registro.
- Troca de posição entre dois registros.
- Impressão de todos os registros.
- Ordenação externa do arquivo.
- Verificação da ordenação do arquivo.

---

## 2. Membros da Equipe

- **Jonathan Martins Cabral**
- **Thales Maia Mendonça Negrão**
- **João Amancio Gherardi**

---

## 3. Pré-requisitos

Para compilar e executar este projeto, você precisará de:

- Um compilador **C++** compatível com **C++11** (ou superior), como o **G++**.

  Para instalá-lo no Ubuntu/Debian:

  ```bash
  sudo apt update && sudo apt install build-essential
  ```

- **GNU Make** (geralmente já incluído com o `build-essential`).

---

## 4. Estrutura do Projeto

O projeto é organizado nos seguintes arquivos:

- `main.cpp`: Contém a função principal e o menu de interação com o usuário.
- `gerenciador_de_atletas.h`: Define as estruturas de dados principais (`Registro`, `HeapItem`, `MinHeap`) e a interface da classe `GerenciadorDeAtletas`.
- `gerenciador_de_atletas.cpp`: Implementa os métodos da classe `GerenciadorDeAtletas` (manipulação de arquivos, conversão, ordenação externa e verificação).
- `algoritmos.h`: Declarações de funções auxiliares de parsing e dos algoritmos de ordenação (**Merge Sort em memória** e funções do **Min-Heap**).
- `algoritmos.cpp`: Implementa as funções declaradas em `algoritmos.h`.
- `Makefile`: Script para automatizar o processo de compilação.
- `dados.csv`: Arquivo entrada csv com os dados.

---

## 5. Detalhes de Compilação

Para compilar o projeto, navegue até o diretório raiz do projeto no terminal e execute:

```bash
make
```

Este comando usará o **Makefile** para compilar todos os arquivos `.cpp` e gerar o executável final.

### Flags de Compilação Utilizadas (`CXXFLAGS` no Makefile):

- `-std=c++11`: Garante a compatibilidade com o padrão **C++11**.
- `-Wall -Wextra`: Ativa avisos adicionais do compilador para identificar possíveis problemas e melhorar a qualidade do código.
- `-g`: Inclui informações de depuração, útil para ferramentas como **GDB**.

---

## 6. Detalhes de Execução

Após a compilação bem-sucedida, execute o programa com:

```bash
./projeto_ED
```

O programa apresentará um **menu de opções** no terminal.

### Arquivos de Entrada e Saída:

- O programa espera um arquivo CSV chamado **`dados.csv`** no mesmo diretório do executável para a conversão inicial.
- O arquivo binário gerado e manipulado será nomeado **`dados.bin`**.
- Durante a ordenação externa, arquivos temporários com o prefixo **`bloco_`** serão criados e posteriormente removidos automaticamente.

---

## 7. Limpeza (`clean`)

Para remover todos os arquivos objeto (`.o`) e o executável gerado, utilize o comando:

```bash
make clean
```
