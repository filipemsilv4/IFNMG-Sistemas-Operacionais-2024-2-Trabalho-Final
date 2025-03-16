// Huffman.cpp
#include "Huffman.h"
#include <iostream>

Huffman::Huffman() : root(nullptr) {}

Huffman::~Huffman() {
    clearTree(root);
}

// Libera a memória utilizada pela árvore
void Huffman::clearTree(Node* node) {
    if (node) {
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
}

// Construção da tabela de frequência a partir dos dados
void Huffman::buildFrequencyTable(const std::string &data) {
    for (char ch : data) {
        frequencyTable[ch]++;
    }
}

// Construção da árvore de Huffman utilizando uma fila de prioridades
void Huffman::buildTree() {
    std::vector<Node*> nodes;
    for (auto &pair : frequencyTable) {
        nodes.push_back(new Node(pair.first, pair.second));
    }
    // Ordena os nós por frequência e, em caso de empate, pelo valor do caractere
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

// Geração da tabela de códigos (caminho da raiz até cada folha)
void Huffman::buildCodeTable(Node* node, const std::string &str) {
    if (!node)
        return;
    if (!node->left && !node->right) {
        codeTable[node->ch] = str;
    }
    buildCodeTable(node->left, str + "0");
    buildCodeTable(node->right, str + "1");
}

// Codificação dos dados utilizando a tabela de códigos
std::string Huffman::encode(const std::string &data) {
    std::string encodedStr;
    for (char ch : data) {
        encodedStr += codeTable[ch];
    }
    return encodedStr;
}

// Decodificação dos dados a partir da árvore de Huffman
std::string Huffman::decode(const std::string &encodedData, Node* root) {
    std::string decodedStr;
    Node* current = root;
    for (char bit : encodedData) {
        if (bit == '0')
            current = current->left;
        else
            current = current->right;
        // Se chegamos a uma folha, obtém o caractere
        if (!current->left && !current->right) {
            decodedStr += current->ch;
            current = root;
        }
    }
    return decodedStr;
}
