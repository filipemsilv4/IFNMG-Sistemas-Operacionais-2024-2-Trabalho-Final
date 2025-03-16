// Huffman.h
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <algorithm>

// Estrutura do nó da árvore de Huffman
struct Node {
    char ch;
    int freq;
    Node *left, *right;
    Node(char character, int frequency) : ch(character), freq(frequency), left(nullptr), right(nullptr) {}
};

// Função de comparação para a fila de prioridades (mínima) com tie-breaker
struct CompareNode {
    bool operator()(Node* l, Node* r) {
        if (l->freq == r->freq) {
            return l->ch > r->ch; // menor valor ASCII tem maior prioridade
        }
        return l->freq > r->freq;
    }
};

class Huffman {
public:
    Huffman();
    ~Huffman();
    
    // Métodos para construção da árvore e tabelas
    void buildFrequencyTable(const std::string &data);
    void buildTree();
    void buildCodeTable(Node* node, const std::string &str);
    
    // Métodos de codificação e decodificação
    std::string encode(const std::string &data);
    std::string decode(const std::string &encodedData, Node* root);
    void clearTree(Node* node);

    // Tabelas e árvore
    std::unordered_map<char, int> frequencyTable;
    std::unordered_map<char, std::string> codeTable;
    Node* root;
};

#endif // HUFFMAN_H
