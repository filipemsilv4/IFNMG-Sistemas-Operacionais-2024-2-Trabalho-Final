"""
Script para testar o programa Huffman.
Este script gera 10 casos de teste (arquivos) com tamanhos entre 256 bytes e 8 MB,
cada um com diferentes padrões de repetição, e para cada caso:
  - Comprime o arquivo original usando o programa 'huffman'
  - Descomprime o arquivo compactado
  - Compara o arquivo descomprimido com o original

Após executar os 10 casos, o script exibe os resultados e pergunta se deseja:
  [E]ncerrar e apagar todos os arquivos gerados ou
  [G]erar mais 10 casos.

Requisitos:
  - O executável 'huffman' deve estar compilado e presente no mesmo diretório.
  - Python 3.x
"""

import os
import random
import string
import subprocess
import sys

# Lista global para armazenar os nomes dos arquivos gerados (input, .huff e descomprimidos)
generated_files = []

def generate_test_case(test_index):
    """
    Gera um arquivo de teste com conteúdo repetitivo.
    O arquivo terá tamanho aleatório entre 256 bytes e 8 MB.
    Um padrão base é criado e repetido para formar o conteúdo.
    Retorna o nome do arquivo gerado.
    """
    # Define tamanho mínimo e máximo (em bytes)
    min_size = 256
    max_size = 8 * 1024 * 1024  # 8 MB

    # Escolhe um tamanho aleatório dentro do intervalo
    size = random.randint(min_size, max_size)

    # Escolhe um tamanho para o padrão base (entre 10 e 100 bytes)
    base_pattern_length = random.randint(10, 100)
    # Gera um padrão base com caracteres aleatórios (letras e dígitos)
    base_pattern = ''.join(random.choices(string.ascii_letters + string.digits, k=base_pattern_length))

    # Calcula quantas repetições são necessárias
    repetitions = size // len(base_pattern) + 1

    # Gera o conteúdo repetindo o padrão e trunca para o tamanho exato
    content = (base_pattern * repetitions)[:size]

    # Nome do arquivo de teste
    filename = f"testcase_{test_index}.txt"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(content)
    
    # Armazena o arquivo gerado
    generated_files.append(filename)
    return filename

def run_command(command):
    """
    Executa um comando via subprocess e retorna o código de retorno.
    """
    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return result.returncode, result.stdout.decode('utf-8'), result.stderr.decode('utf-8')

def process_test_case(test_index):
    """
    Gera, comprime, descomprime e verifica um caso de teste.
    Exibe o resultado em uma linha com:
      "Teste X | Original: Y bytes | Comprimido: Z bytes | OK/FAIL"
    """
    # Gerar arquivo de teste
    input_filename = generate_test_case(test_index)
    # Define nomes para os arquivos comprimido e descomprimido
    compressed_filename = f"testcase_{test_index}.huff"
    decompressed_filename = f"testcase_{test_index}_decompressed.txt"
    
    generated_files.extend([compressed_filename, decompressed_filename])
    
    # Comprimir: comando: ./huffman C <input> <compressed>
    cmd_compress = f"./huffman C {input_filename} {compressed_filename}"
    ret, out, err = run_command(cmd_compress)
    if ret != 0:
        print(f"Teste {test_index} | Erro na compressão: {err.strip()}")
        return False
    
    # Descomprimir: comando: ./huffman D <compressed> <decompressed>
    cmd_decompress = f"./huffman D {compressed_filename} {decompressed_filename}"
    ret, out, err = run_command(cmd_decompress)
    if ret != 0:
        print(f"Teste {test_index} | Erro na descompressão: {err.strip()}")
        return False
    
    # Obter tamanhos dos arquivos
    original_size = os.path.getsize(input_filename)
    compressed_size = os.path.getsize(compressed_filename)
    
    # Comparar arquivos originais e descomprimidos
    with open(input_filename, "r", encoding="utf-8") as f:
        original_content = f.read()
    with open(decompressed_filename, "r", encoding="utf-8") as f:
        decompressed_content = f.read()
    
    if original_content == decompressed_content:
        status = "✅"
    else:
        status = "❌"
    
    print(f"Teste {test_index:2d} | Original: {original_size:8d} bytes | Comprimido: {compressed_size:8d} bytes | {status}")
    return original_content == decompressed_content

def delete_generated_files():
    """
    Apaga todos os arquivos gerados listados em generated_files.
    """
    for filename in set(generated_files):
        try:
            if os.path.exists(filename):
                os.remove(filename)
        except Exception as e:
            print(f"Erro ao apagar {filename}: {e}")

def main():
    test_count = 10
    current_batch = 1

    while True:
        print(f"\n=== Processando lote {current_batch}: {test_count} casos de teste ===")
        successes = 0
        for i in range(1, test_count + 1):
            # Use um índice global único para os arquivos (batch * test_count + i)
            test_index = (current_batch - 1) * test_count + i
            if process_test_case(test_index):
                successes += 1

        print(f"\nLote {current_batch} concluído: {successes} de {test_count} testes OK.")

        # Pergunta ao usuário se deseja gerar mais casos ou encerrar e apagar os arquivos
        opcao = input("\nDigite 'E' para encerrar e apagar todos os arquivos gerados ou 'G' para gerar mais 10 casos: ").strip().upper()
        if opcao == 'E':
            delete_generated_files()
            print("Todos os arquivos gerados foram apagados. Encerrando...")
            break
        elif opcao == 'G':
            current_batch += 1
            continue
        else:
            print("Opção inválida. Encerrando e apagando arquivos.")
            delete_generated_files()
            break

if __name__ == "__main__":
    main()
