# Trabalho-U2-EDB

## Introdução
 Este repositório é uma implementação dos algoritmos de árvore rubro-negra e de árvore AVL.

 O programa permite inserir chaves, remover chaves, buscar chaves, imprimir em ordem, imprimr estrutra visual e exibir métricas (tamanho, altura e rotações) em ambas as estruturas.

## Compilando e Rodando
 Para compilar este projeto, abra o terminal na pasta principal e execute o comando:

 ```bash
 g++ -std=c++17 -Iinclude src/main.cpp -o main
 ```

 Após isso, o arquivo main.exe aparecerá na pasta e pode ser rodado livremente.

## Testes Automatizados e Comparativos
 O projeto possui uma suíte de testes (`tests/testes.cpp`) que avalia o desempenho da Árvore AVL e da Rubro-Negra para conjuntos de elementos em ordens crescente, decrescente e não ordenada.
 
 O programa analisa a altura final, a quantidade de rotações e os tempos de inserção, busca e remoção em microsssegundos em grupos de 100, 1000, 10000 e 100000 elementos.

 Há também um arquivo chamado `test_results.txt` com os resultados inicialmente obtidos quando a suíte de testes foi rodada na minha máquina pela primeira vez, para referência e comparação do usuário com seus próprios resultados.

 Para compilar os testes, abra o terminal na pasta principal e execute o comando:

 ```bash
 g++ -std=c++17 -O2 -Iinclude tests/testes.cpp -o testes
 ```

 Após isso, o arquivo testes.exe aparecerá na pasta e pode ser rodado livremente.