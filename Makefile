# Nome do compilador C++
CXX = g++

# Flags de compilação:
# -std=c++11: Usa o padrão C++11 (ou mais recente, como c++17)
# -Wall -Wextra: Ativa todos os avisos úteis do compilador (sinal de código de qualidade)
# -g: Inclui informações de depuração no executável
CXXFLAGS = -std=c++11 -Wall -Wextra -g

# Nome do programa executável final
TARGET = meu_programa

# Lista dos arquivos fonte (.cpp)
SOURCES = main.cpp gerenciador_de_atletas.cpp algoritmos.cpp

# Gera automaticamente a lista de arquivos objeto (.o) a partir da lista de fontes
OBJECTS = $(SOURCES:.cpp=.o)

# Regra padrão: compila o programa inteiro. É a primeira regra, então é executada por padrão.
all: $(TARGET)

# Regra para linkar: como criar o executável final a partir dos arquivos objeto
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Regra genérica para compilar: como criar um arquivo .o a partir de um arquivo .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra "clean": remove todos os arquivos gerados pela compilação
clean:
	rm -f $(OBJECTS) $(TARGET)
