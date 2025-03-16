// main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Huffman.h"

// Converte uma string de bits ("0" e "1") em um vetor de bytes (string binária)
// Também retorna a quantidade de bits não utilizados no último byte
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

// Converte um vetor de bytes (armazenado em uma string) de volta para a string de bits,
// removendo os bits não utilizados no último byte
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

// Função para realizar a compressão
void compressFile(const std::string &inputFile, const std::string &outputFile) {
    std::ifstream in(inputFile, std::ios::binary);
    if (!in) {
        std::cerr << "Erro ao abrir arquivo de entrada.\n";
        return;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string data = buffer.str();
    in.close();

    Huffman huffman;
    huffman.buildFrequencyTable(data);
    huffman.buildTree();
    huffman.buildCodeTable(huffman.root, "");

    std::string encodedBitString = huffman.encode(data);
    
    unsigned char unusedBits = 0;
    std::string encodedBytes = convertBitStringToBytes(encodedBitString, unusedBits);
    
    std::ofstream out(outputFile, std::ios::binary);
    if (!out) {
        std::cerr << "Erro ao abrir arquivo de saída.\n";
        return;
    }
    
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
    out.close();

    std::cout << "Compressão concluída. Dados compactados em " << outputFile << "\n";
}

// Função para realizar a descompressão
void decompressFile(const std::string &inputFile, const std::string &outputFile) {
    std::ifstream in(inputFile, std::ios::binary);
    if (!in) {
        std::cerr << "Erro ao abrir arquivo de entrada.\n";
        return;
    }
    
    // Lê a quantidade de bits não preenchidos no último byte (1 byte)
    unsigned char unusedBits = 0;
    in.read(reinterpret_cast<char*>(&unusedBits), sizeof(unsigned char));
    
    // Lendo a tabela de frequência
    int freqSize;
    in.read(reinterpret_cast<char*>(&freqSize), sizeof(int));
    Huffman huffman;
    for (int i = 0; i < freqSize; i++) {
        char ch;
        int freq;
        in.read(&ch, sizeof(char));
        in.read(reinterpret_cast<char*>(&freq), sizeof(int));
        huffman.frequencyTable[ch] = freq;
    }
    huffman.buildTree();
    huffman.buildCodeTable(huffman.root, "");
    
    // Lê o tamanho do vetor de bytes codificados
    int encodedByteLength;
    in.read(reinterpret_cast<char*>(&encodedByteLength), sizeof(int));
    
    // Lê os dados codificados
    std::string encodedBytes(encodedByteLength, ' ');
    in.read(&encodedBytes[0], encodedByteLength);
    in.close();
    
    // Converte os bytes de volta para a string de bits
    std::string encodedBitString = convertBytesToBitString(encodedBytes, unusedBits);
    
    std::string decodedData = huffman.decode(encodedBitString, huffman.root);
    std::ofstream out(outputFile, std::ios::binary);
    if (!out) {
        std::cerr << "Erro ao abrir arquivo de saída.\n";
        return;
    }
    out << decodedData;
    out.close();

    std::cout << "Descompressão concluída. Dados descompactados em " << outputFile << "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <C|D> <arquivo_entrada> <arquivo_saida>\n";
        return 1;
    }
    char mode = argv[1][0];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];

    if (mode == 'C' || mode == 'c') {
        compressFile(inputFile, outputFile);
    } else if (mode == 'D' || mode == 'd') {
        decompressFile(inputFile, outputFile);
    } else {
        std::cerr << "Modo inválido. Use 'C' para compressão ou 'D' para descompressão.\n";
        return 1;
    }
    return 0;
}
