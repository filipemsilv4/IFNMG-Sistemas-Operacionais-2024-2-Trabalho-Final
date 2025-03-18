# Trabalho Prático - Compressão e Descompressão com Algoritmo de Huffman


Este projeto implementa um compressor e descompressor de arquivos usando o algoritmo de Huffman. Este algoritmo cria códigos de tamanho variável para cada símbolo (caractere) baseados em sua frequência de ocorrência, resultando em uma representação mais eficiente dos dados.

![Algoritmo de Huffman](https://upload.wikimedia.org/wikipedia/commons/thumb/8/82/Huffman_tree_2.svg/440px-Huffman_tree_2.svg.png)

*Exemplo de uma árvore de Huffman*

## 🧠 Visão Conceitual do Algoritmo

1. **Análise de Frequência**: O algoritmo conta a ocorrência de cada caractere no arquivo.
2. **Construção da Árvore**: Usando as frequências, cria-se uma árvore onde caracteres mais frequentes ficam mais próximos da raiz.
3. **Geração da Tabela de Códigos**: Para cada caractere, gera-se um código único baseado em seu caminho na árvore.
4. **Compressão**: Os caracteres originais são substituídos por seus códigos binários.
5. **Descompressão**: Usando a árvore, converte-se os códigos de volta aos caracteres originais.

## 🔨 Estrutura do Projeto

```
huffman/
├── Huffman.h         # Declaração das estruturas e classe
├── Huffman.cpp       # Implementação do algoritmo de Huffman
├── main.cpp          # Gerenciamento de arquivos e interface do programa
├── Makefile          # Script para compilação
└── test_huffman.py   # Script para teste automático (opcional)
```

## 💻 Como Usar

### Compilação
```bash
make
```

### Compressão
```bash
./huffman C arquivo_entrada arquivo_comprimido.huff
```

### Descompressão
```bash
./huffman D arquivo_comprimido.huff arquivo_saida
```

## 🔍 Funcionamento Detalhado

### 1. Nó da Árvore de Huffman

Cada nó na árvore armazena:
- Um caractere
- Sua frequência
- Ponteiros para os filhos esquerdo e direito

```cpp
// De Huffman.h
struct Node {
    char ch;             // Caractere armazenado (só significativo em folhas)
    int freq;            // Frequência do caractere
    Node *left, *right;  // Ponteiros para subárvores
    // Construtor
    Node(char character, int frequency) : ch(character), freq(frequency), 
                                          left(nullptr), right(nullptr) {}
};
```

### 2. Tabela de Frequência

O primeiro passo é contar a frequência de cada caractere no arquivo:

```cpp
// De Huffman.cpp
void Huffman::buildFrequencyTable(const std::string &data) {
    for (char ch : data) {
        frequencyTable[ch]++;  // Incrementa contador para cada caractere
    }
}
```

**Visualização da Tabela de Frequência:**
```
Caractere | Frequência
----------|------------
   'a'    |     42
   'b'    |     15
   'c'    |     23
   ...    |     ...
```

### 3. Construção da Árvore de Huffman

A árvore é construída seguindo estes passos:

1. Criar um nó folha para cada caractere
2. Ordenar os nós por frequência (e valor ASCII em caso de empate)
3. Repetidamente extrair os dois nós de menor frequência e combiná-los em um novo nó
4. O último nó remanescente é a raiz da árvore

```cpp
// De Huffman.cpp
void Huffman::buildTree() {
    // Cria nós para cada caractere e sua frequência
    std::vector<Node*> nodes;
    for (auto &pair : frequencyTable) {
        nodes.push_back(new Node(pair.first, pair.second));
    }
    
    // Ordena os nós por frequência e valor ASCII (para determinismo)
    std::sort(nodes.begin(), nodes.end(), [](Node* a, Node* b) {
        if (a->freq == b->freq)
            return a->ch < b->ch;
        return a->freq < b->freq;
    });
    
    // Usa fila de prioridade para construir a árvore
    std::priority_queue<Node*, std::vector<Node*>, CompareNode> minHeap;
    for (auto node : nodes) {
        minHeap.push(node);
    }
    
    // Constrói a árvore combinando os dois nós de menor frequência
    while (minHeap.size() > 1) {
        Node* left = minHeap.top();
        minHeap.pop();
        Node* right = minHeap.top();
        minHeap.pop();
        
        // Cria novo nó interno com soma das frequências
        Node* newNode = new Node('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        minHeap.push(newNode);
    }
    
    // O último nó é a raiz da árvore
    if (!minHeap.empty()) {
        root = minHeap.top();
    }
}
```

**Visualização da Construção da Árvore:**
```
Etapa 1: Nós folha ordenados por frequência
[a:42] [c:23] [b:15] [d:10] [e:5]

Etapa 2: Combinamos os dois menores (e:5 + d:10 = 15)
[a:42] [c:23] [b:15] [interno:15]
                      /      \
                   [d:10]    [e:5]

Etapa 3: Continuamos combinando
[a:42] [c:23] [interno:30]
                /       \
           [b:15]    [interno:15]
                      /      \
                   [d:10]    [e:5]

... e assim por diante até termos apenas a raiz
```

### 4. Geração da Tabela de Códigos

Para cada caractere, percorremos a árvore da raiz até a folha:
- Caminho à esquerda = '0'
- Caminho à direita = '1'

```cpp
// De Huffman.cpp
void Huffman::buildCodeTable(Node* node, const std::string &str) {
    if (!node)
        return;
    
    // Se é um nó folha, armazena o código na tabela
    if (!node->left && !node->right) {
        codeTable[node->ch] = str;
    }
    
    // Recursivamente constrói os códigos para as subárvores
    buildCodeTable(node->left, str + "0");   // Adiciona '0' para caminho à esquerda
    buildCodeTable(node->right, str + "1");  // Adiciona '1' para caminho à direita
}
```

**Visualização da Tabela de Códigos:**
```
Caractere | Código Huffman
----------|---------------
   'a'    |      '0'
   'b'    |     '10'
   'c'    |    '110'
   't'    |   '1110'
   'e'    |   '1111'
```

### 5. Codificação dos Dados

Usando a tabela de códigos, substituímos cada caractere por seu código binário:

```cpp
// De Huffman.cpp
std::string Huffman::encode(const std::string &data) {
    std::string encodedStr;
    for (char ch : data) {
        encodedStr += codeTable[ch];  // Concatena o código para cada caractere
    }
    return encodedStr;
}
```

**Exemplo:** A string "abacate" seria codificada como:
```
'a' = '0'
'b' = '10'
'a' = '0'
'c' = '110'
'a' = '0'
't' = '1110'
'e' = '1111'
Resultado: "0100110011101111"
```

### 6. Armazenamento Otimizado

Como trabalhamos com bits individuais, precisamos convertê-los em bytes para armazenamento eficiente:

```cpp
// De main.cpp
std::string convertBitStringToBytes(const std::string &bitString, unsigned char &unusedBits) {
    // Preenche com zeros para completar o último byte se necessário
    std::string padded = bitString;
    int remainder = padded.size() % 8;
    if (remainder != 0) {
        unusedBits = 8 - remainder;
        padded.append(unusedBits, '0');
    } else {
        unusedBits = 0;
    }
    
    // Converte grupos de 8 bits para bytes
    std::string byteString;
    for (size_t i = 0; i < padded.size(); i += 8) {
        unsigned char byte = 0;
        for (size_t j = 0; j < 8; j++) {
            byte <<= 1;                  // Desloca à esquerda
            if (padded[i+j] == '1')
                byte |= 1;               // Define o bit menos significativo
        }
        byteString.push_back(byte);
    }
    return byteString;
}
```

**Visualização da Conversão:**
```
String de bits: "10110101001..."
Agrupando em bytes: "10110101" "00100001" ...
```

### 7. Formato do Arquivo Comprimido

O arquivo comprimido armazena:
1. **Bits não utilizados** (1 byte): Número de bits de preenchimento no último byte
2. **Tabela de frequência**:
   - Tamanho da tabela (4 bytes)
   - Pares (caractere, frequência) (5 bytes cada)
3. **Dados comprimidos**:
   - Tamanho da sequência de bytes comprimidos em bytes (4 bytes)
   - Sequência de bytes comprimidos

```cpp
// De main.cpp (função compressFile)
// Escreve a quantidade de bits não preenchidos no último byte (1 byte)
out.write(reinterpret_cast<char*>(&unusedBits), sizeof(unsigned char));

// Salva a tabela de frequência para futura descompressão
int freqSize = huffman.frequencyTable.size();
out.write(reinterpret_cast<char*>(&freqSize), sizeof(int));
for (auto pair : huffman.frequencyTable) {
    out.write(&pair.first, sizeof(char));
    out.write(reinterpret_cast<char*>(&pair.second), sizeof(int));
}

// Escreve o tamanho do vetor de bytes codificados
int encodedByteLength = encodedBytes.size();
out.write(reinterpret_cast<char*>(&encodedByteLength), sizeof(int));

// Escreve os dados codificados (em bytes)
out.write(encodedBytes.c_str(), encodedBytes.size());
```

**O que é `reinterpret_cast`?**
`reinterpret_cast` é um operador de C++ usado para conversão entre tipos de ponteiros incompatíveis. Aqui, estamos usando-o para tratar variáveis numéricas como sequências de bytes para armazenamento binário. Por exemplo:

```cpp
// De main.cpp
unsigned char unusedBits = 3;
// Converte ponteiro para unusedBits em ponteiro para char, 
// permitindo gravar os bytes brutos da variável
out.write(reinterpret_cast<char*>(&unusedBits), sizeof(unsigned char));
```

Isso permite escrever estruturas numéricas diretamente em arquivos binários.

### 8. Descompressão e Recuperação da Árvore

A chave para a descompressão está em reconstruir corretamente a árvore de Huffman. Este processo segue estes passos:

1. **Ler o cabeçalho do arquivo comprimido**:
   ```cpp
   // De main.cpp (função decompressFile)
   unsigned char unusedBits = 0;
   in.read(reinterpret_cast<char*>(&unusedBits), sizeof(unsigned char));
   ```

2. **Recuperar a tabela de frequência**:
   ```cpp
   // De main.cpp (função decompressFile)
   int freqSize;
   in.read(reinterpret_cast<char*>(&freqSize), sizeof(int));
   
   for (int i = 0; i < freqSize; i++) {
       char ch;
       int freq;
       in.read(&ch, sizeof(char));
       in.read(reinterpret_cast<char*>(&freq), sizeof(int));
       huffman.frequencyTable[ch] = freq;
   }
   ```

3. **Reconstruir a árvore** usando exatamente o mesmo algoritmo da compressão:
   ```cpp
   // De main.cpp (função decompressFile)
   huffman.buildTree();
   huffman.buildCodeTable(huffman.root, "");
   ```

O determinismo é garantido pelo critério de desempate na construção da árvore - quando dois nós têm a mesma frequência, o de menor valor ASCII terá prioridade. Isso assegura que a mesma árvore seja construída tanto na compressão quanto na descompressão.

**Importante**: A própria árvore não é armazenada no arquivo comprimido, apenas a tabela de frequência, que é suficiente para reconstruir a árvore com exatamente a mesma estrutura.

### 9. Processo de Decodificação

Após reconstruir a árvore, os bytes comprimidos são convertidos de volta à sequência de bits:

```cpp
// De main.cpp (função decompressFile)
// Converte os bytes de volta para a string de bits
std::string encodedBitString = convertBytesToBitString(encodedBytes, unusedBits);
```

Então, a sequência de bits é decodificada percorrendo a árvore:

```cpp
// De Huffman.cpp
std::string Huffman::decode(const std::string &encodedData, Node* root) {
    std::string decodedStr;
    Node* current = root;
    
    // Para cada bit da sequência codificada
    for (char bit : encodedData) {
        // Navega para esquerda (0) ou direita (1)
        if (bit == '0')
            current = current->left;
        else
            current = current->right;
        
        // Se chegamos a uma folha, encontramos um caractere
        if (!current->left && !current->right) {
            decodedStr += current->ch;  // Adiciona caractere recuperado
            current = root;             // Volta à raiz para o próximo caractere
        }
    }
    return decodedStr;
}
```

**Visualização da Decodificação:**
```
Sequência de bits: "0100110011101111"

Bit  | Caminho na árvore         | Caractere encontrado
-----|---------------------------|---------------------
'0'  | Raiz → Esquerda (folha)   | 'a'
'1'  | Raiz → Direita            | (continua)
'0'  | → Esquerda (folha)        | 'b'
'0'  | Raiz → Esquerda (folha)   | 'a'
'1'  | Raiz → Direita            | (continua)
'1'  | → Direita                 | (continua)
'0'  | → Esquerda (folha)        | 'c'
...  | ...                       | ...

Decodificado: "abacate"
```

## 📊 Desempenho e Considerações

- **Taxa de Compressão**: Varia conforme a distribuição de frequência dos caracteres. Textos com muitos caracteres repetidos geralmente têm melhor compressão.
- **Tempo de Execução**: O algoritmo tem complexidade O(n log n), onde n é o número de caracteres únicos no arquivo.
- **Uso de Memória**: Proporcional ao número de caracteres únicos, não ao tamanho total do arquivo.

## 🔄 Fluxo Completo do Programa

### Compressão:
1. Lê o arquivo de entrada
2. Cria a tabela de frequência
3. Constrói a árvore de Huffman
4. Gera a tabela de códigos binários
5. Codifica o texto original usando os códigos
6. Converte a sequência de bits em bytes
7. Salva o arquivo comprimido com cabeçalho e dados

### Descompressão:
1. Lê o arquivo comprimido
2. Extrai o cabeçalho (bits não utilizados e tabela de frequência)
3. Reconstrói a árvore de Huffman
4. Converte os bytes em sequência de bits
5. Decodifica a sequência de bits usando a árvore
6. Salva o texto decodificado no arquivo de saída

## 🧪 Testando o Programa

Você pode testar o programa manualmente comprimindo e descomprimindo arquivos, ou usar o script fornecido para teste automatizado.

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

