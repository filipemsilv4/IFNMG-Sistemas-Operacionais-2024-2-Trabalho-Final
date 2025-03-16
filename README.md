# Trabalho Prático - Compressão e Descompressão com Algoritmo de Huffman

## Descrição

Este projeto implementa um algoritmo de compressão e descompressão de arquivos utilizando o método de Huffman. O algoritmo cria uma árvore baseada na frequência dos caracteres de um arquivo, gera uma tabela de códigos binários e utiliza essa tabela para converter os dados originais em uma sequência compactada. Na descompressão, a árvore é reconstruída para recuperar o arquivo original.

## Estrutura do Projeto

- **Huffman.h**: Declara as estruturas, a classe e os métodos essenciais.
- **Huffman.cpp**: Implementa as funções para construir a árvore de Huffman, gerar a tabela de códigos e realizar a codificação/decodificação.
- **main.cpp**: Gerencia a leitura dos arquivos, execução dos processos de compressão/descompressão e gravação dos resultados.
- **Makefile**: Script para compilar o projeto e facilitar a execução dos modos de compressão (`exeC`) e descompressão (`exeD`).

## Como Usar o Programa

### Compilação

Antes de executar o programa, é necessário compilá-lo. Para isso, utilize o **Makefile**:

```sh
make
```

Isso gerará o executável `huffman`.

### Compressão

Para comprimir um arquivo de entrada, use o seguinte comando:

```sh
./huffman C <arquivo_entrada> <arquivo_saida>
```

Por exemplo, para comprimir um arquivo `input.txt` e gerar `output.huff`:

```sh
./huffman C input.txt output.huff
```

### Descompressão

Para descomprimir um arquivo compactado e restaurar o original, use:

```sh
./huffman D <arquivo_compactado> <arquivo_saida>
```

Por exemplo, para descomprimir `output.huff` e restaurar `output.txt`:

```sh
./huffman D output.huff output.txt
```

## Funcionamento do Algoritmo

### 1. Construção da Tabela de Frequência

O método `buildFrequencyTable` percorre os dados do arquivo e conta a frequência de cada caractere. Essa tabela é usada para construir a árvore de Huffman.

```cpp
// Em Huffman.cpp
void Huffman::buildFrequencyTable(const std::string &data) {
    for (char ch : data) {
        frequencyTable[ch]++;
    }
}
```

### 2. Construção da Árvore de Huffman

A árvore de Huffman é construída utilizando uma fila de prioridades. Cada nó armazena um caractere e sua frequência, e em caso de empate, o nó com menor valor ASCII tem prioridade. Essa estratégia garante uma ordenação determinística.

```cpp
// Trecho de Huffman.h
struct CompareNode {
    bool operator()(Node* l, Node* r) {
        if (l->freq == r->freq) {
            return l->ch > r->ch; // Tie-breaker: menor valor ASCII tem maior prioridade
        }
        return l->freq > r->freq;
    }
};
```

No método `buildTree`, os nós são inicialmente inseridos em um vetor, ordenados e depois adicionados a uma fila de prioridades. Os nós de menor frequência são extraídos e combinados até formar a árvore completa.

```cpp
// Em Huffman.cpp
void Huffman::buildTree() {
    std::vector<Node*> nodes;
    for (auto &pair : frequencyTable) {
        nodes.push_back(new Node(pair.first, pair.second));
    }
    std::sort(nodes.begin(), nodes.end(), [](Node* a, Node* b) {
        if (a->freq == b->freq)
            return a->ch < b->ch;
        return a->freq < b->freq;
    });
    
    std::priority_queue<Node*, std::vector<Node*>, CompareNode> minHeap;
    for (auto node : nodes) {
        minHeap.push(node);
    }
    
    while (minHeap.size() > 1) {
        Node* left = minHeap.top();
        minHeap.pop();
        Node* right = minHeap.top();
        minHeap.pop();
        Node* newNode = new Node('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        minHeap.push(newNode);
    }
    if (!minHeap.empty()) {
        root = minHeap.top();
    }
}
```

### 3. Geração da Tabela de Códigos

Após construir a árvore, o método `buildCodeTable` percorre-a recursivamente para gerar os códigos binários para cada caractere. Cada folha recebe um código formado pelo caminho percorrido (0 para o ramo esquerdo e 1 para o direito).

```cpp
// Em Huffman.cpp
void Huffman::buildCodeTable(Node* node, const std::string &str) {
    if (!node)
        return;
    if (!node->left && !node->right) {
        codeTable[node->ch] = str;
    }
    buildCodeTable(node->left, str + "0");
    buildCodeTable(node->right, str + "1");
}
```

### 4. Compressão dos Dados

Na compressão, o arquivo é lido e cada caractere é substituído pelo seu código da tabela. A sequência de bits resultante é convertida em bytes para armazenamento mais eficiente.

```cpp
// Em main.cpp
std::string encodedBitString = huffman.encode(data);
unsigned char unusedBits = 0;
std::string encodedBytes = convertBitStringToBytes(encodedBitString, unusedBits);
```

A função `convertBitStringToBytes` agrupa os bits em blocos de 8 e preenche o último byte com zeros, se necessário:

```cpp
std::string convertBitStringToBytes(const std::string &bitString, unsigned char &unusedBits) {
    std::string padded = bitString;
    int remainder = padded.size() % 8;
    if (remainder != 0) {
        unusedBits = 8 - remainder;
        padded.append(unusedBits, '0');
    } else {
        unusedBits = 0;
    }
    
    std::string byteString;
    for (size_t i = 0; i < padded.size(); i += 8) {
        unsigned char byte = 0;
        for (size_t j = 0; j < 8; j++) {
            byte <<= 1;
            if (padded[i+j] == '1')
                byte |= 1;
        }
        byteString.push_back(byte);
    }
    return byteString;
}
```

Os dados compactados, juntamente com a quantidade de bits não utilizados e a tabela de frequência, são gravados no arquivo de saída.

### 5. Descompressão dos Dados

Para descomprimir, o programa lê o arquivo compactado, recupera a quantidade de bits não utilizados, a tabela de frequência e os dados em formato de bytes. Em seguida, reconstrói a árvore e converte os bytes de volta para uma string de bits:

```cpp
std::string encodedBitString = convertBytesToBitString(encodedBytes, unusedBits);
std::string decodedData = huffman.decode(encodedBitString, huffman.root);
```

A função `convertBytesToBitString` converte cada byte em uma sequência de 8 bits e remove os bits de preenchimento:

```cpp
std::string convertBytesToBitString(const std::string &byteString, unsigned char unusedBits) {
    std::string bitString;
    for (size_t i = 0; i < byteString.size(); i++) {
        unsigned char byte = byteString[i];
        std::string bits;
        for (int j = 7; j >= 0; j--) {
            bits.push_back((byte & (1 << j)) ? '1' : '0');
        }
        bitString += bits;
    }
    if (unusedBits > 0 && !bitString.empty()) {
        bitString.erase(bitString.end()-unusedBits, bitString.end());
    }
    return bitString;
}
```

O método `decode` percorre a árvore conforme os bits são lidos, reconstruindo o texto original.

```cpp
// Em Huffman.cpp
std::string Huffman::decode(const std::string &encodedData, Node* root) {
    std::string decodedStr;
    Node* current = root;
    for (char bit : encodedData) {
        if (bit == '0')
            current = current->left;
        else
            current = current->right;
        if (!current->left && !current->right) {
            decodedStr += current->ch;
            current = root;
        }
    }
    return decodedStr;
}
```

## Testando o programa automaticamente

Para facilitar a validação da compressão e descompressão, há um script chamado `test_huffman.py` que gera casos de teste automaticamente.  

### Como usar o script de testes  

1. Certifique-se de que o executável `huffman` está no mesmo diretório que o script.
2. Execute o script com o comando:

   ```bash
   python3 test_huffman.py
   ```

3. O script irá:
   - Gerar 10 arquivos de teste aleatórios com tamanhos entre 256 bytes e 8 MB.
   - Comprimir e descomprimir cada arquivo usando `huffman`.
   - Verificar se os arquivos descomprimidos são idênticos aos originais.
   - Exibir os resultados de cada teste no formato:

     ```
     Teste  1 | Original:   1024 bytes | Comprimido:    512 bytes | ✅
     Teste  2 | Original:  524288 bytes | Comprimido:  200000 bytes | ✅
     ```

   - Perguntar se deseja gerar mais 10 casos ou encerrar e apagar todos os arquivos criados.

4. Para encerrar e excluir todos os arquivos temporários, digite `E` quando o script perguntar.

