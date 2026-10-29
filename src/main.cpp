#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include "../include/ArvoreAVL.hpp"
#include "../include/ArvoreRN.hpp"

// Converte uma string individual para inteiro lançando exceções se a entrada for inválida
int converter_para_int(const std::string& token) {
    std::size_t pos_processada = 0;
    int valor = std::stoi(token, &pos_processada);
    
    // Se não leu a string inteira (ex: "2-x", "abc"), o token é inválido
    if (pos_processada != token.size()) {
        throw std::invalid_argument("Entrada inválida contendo caracteres não numéricos: " + token);
    }
    return valor;
}

// Lê uma linha de chaves separadas por espaço do usuário
std::vector<int> ler_chaves() {
    std::string linha;
    std::getline(std::cin, linha);
    
    std::stringstream ss(linha);
    std::string token;
    std::vector<int> chaves;

    while (ss >> token) {
        chaves.push_back(converter_para_int(token));
    }

    if (chaves.empty()) {
        throw std::invalid_argument("Nenhuma chave foi informada.");
    }

    return chaves;
}

// Lê apenas uma opção numérica no menu com tratamento de erros
int ler_opcao_menu() {
    std::string linha;
    std::getline(std::cin, linha);

    std::stringstream ss(linha);
    std::string token;
    
    if (!(ss >> token)) {
        throw std::invalid_argument("Nenhuma opção foi digitada.");
    }

    int opcao = converter_para_int(token);

    std::string extra;
    if (ss >> extra) {
        throw std::invalid_argument("Digite apenas uma opção válida por vez.");
    }

    return opcao;
}

void menu_avl() {
    sc::ArvoreAVL<int> avl;

    while (true) {
        std::cout << "\n--- Menu Árvore AVL ---\n";
        std::cout << "1. Inserir chave(s)\n";
        std::cout << "2. Remover chave(s)\n";
        std::cout << "3. Buscar chave(s)\n";
        std::cout << "4. Imprimir em-ordem\n";
        std::cout << "5. Imprimir estrutura visual\n";
        std::cout << "6. Exibir métricas (Tamanho, Altura, Rotações)\n";
        std::cout << "0. Voltar ao menu principal\n";
        std::cout << "Escolha: ";

        try {
            int opcao = ler_opcao_menu();

            if (opcao == 0) break;

            if (opcao == 1) {
                std::cout << "Digite a(s) chave(s) a ser(em) inserida(s) (separadas por espaço): ";
                std::vector<int> chaves = ler_chaves();
                for (int c : chaves) {
                    if (avl.inserir(c)) {
                        std::cout << "Chave " << c << " inserida com sucesso.\n";
                    } else {
                        std::cout << "Chave " << c << " já existe na árvore.\n";
                    }
                }
            } else if (opcao == 2) {
                std::cout << "Digite a(s) chave(s) a ser(em) removida(s) (separadas por espaço): ";
                std::vector<int> chaves = ler_chaves();
                for (int c : chaves) {
                    if (avl.remover(c)) {
                        std::cout << "Chave " << c << " removida com sucesso.\n";
                    } else {
                        std::cout << "Chave " << c << " não encontrada.\n";
                    }
                }
            } else if (opcao == 3) {
                std::cout << "Digite a(s) chave(s) a buscar (separadas por espaço): ";
                std::vector<int> chaves = ler_chaves();
                for (int c : chaves) {
                    if (avl.buscar(c) != nullptr) {
                        std::cout << "Chave " << c << " encontrada!\n";
                    } else {
                        std::cout << "Chave " << c << " não encontrada.\n";
                    }
                }
            } else if (opcao == 4) {
                std::cout << "Em ordem: ";
                avl.em_ordem();
                std::cout << "\n";
            } else if (opcao == 5) {
                std::cout << "Estrutura da árvore:\n";
                avl.imprimir_estrutura();
            } else if (opcao == 6) {
                std::cout << "Tamanho: " << avl.size() << "\n";
                std::cout << "Altura: " << avl.get_altura() << "\n";
                std::cout << "Rotações: " << avl.get_rotacoes() << "\n";
            } else {
                std::cout << "[ERRO] Opção inválida!\n";
            }
        } catch (const std::exception& e) {
            std::cout << "[EXCEÇÃO CAPTURADA] " << e.what() << "\n";
        }
    }
}

void menu_rn() {
    sc::ArvoreRN<int> rn;

    while (true) {
        std::cout << "\n--- Menu Árvore Rubro-Negra ---\n";
        std::cout << "1. Inserir chave(s)\n";
        std::cout << "2. Remover chave(s)\n";
        std::cout << "3. Buscar chave(s)\n";
        std::cout << "4. Imprimir em-ordem\n";
        std::cout << "5. Imprimir estrutura visual\n";
        std::cout << "6. Exibir métricas (Tamanho, Altura, Rotações)\n";
        std::cout << "0. Voltar ao menu principal\n";
        std::cout << "Escolha: ";

        try {
            int opcao = ler_opcao_menu();

            if (opcao == 0) break;

            if (opcao == 1) {
                std::cout << "Digite a(s) chave(s) a ser(em) inserida(s) (separadas por espaço): ";
                std::vector<int> chaves = ler_chaves();
                for (int c : chaves) {
                    if (rn.inserir(c)) {
                        std::cout << "Chave " << c << " inserida com sucesso.\n";
                    } else {
                        std::cout << "Chave " << c << " já existe na árvore.\n";
                    }
                }
            } else if (opcao == 2) {
                std::cout << "Digite a(s) chave(s) a ser(em) removida(s) (separadas por espaço): ";
                std::vector<int> chaves = ler_chaves();
                for (int c : chaves) {
                    if (rn.remover(c)) {
                        std::cout << "Chave " << c << " removida com sucesso.\n";
                    } else {
                        std::cout << "Chave " << c << " não encontrada.\n";
                    }
                }
            } else if (opcao == 3) {
                std::cout << "Digite a(s) chave(s) a buscar (separadas por espaço): ";
                std::vector<int> chaves = ler_chaves();
                for (int c : chaves) {
                    if (rn.buscar(c) != nullptr) {
                        std::cout << "Chave " << c << " encontrada!\n";
                    } else {
                        std::cout << "Chave " << c << " não encontrada.\n";
                    }
                }
            } else if (opcao == 4) {
                std::cout << "Em ordem: ";
                rn.em_ordem();
                std::cout << "\n";
            } else if (opcao == 5) {
                std::cout << "Estrutura da árvore:\n";
                rn.imprimir_estrutura();
            } else if (opcao == 6) {
                std::cout << "Tamanho: " << rn.size() << "\n";
                std::cout << "Altura: " << rn.get_altura() << "\n";
                std::cout << "Rotações: " << rn.get_rotacoes() << "\n";
            } else {
                std::cout << "[ERRO] Opção inválida!\n";
            }
        } catch (const std::exception& e) {
            std::cout << "[EXCEÇÃO CAPTURADA] " << e.what() << "\n";
        }
    }
}

int main() {
    while (true) {
        std::cout << "\n===============================\n";
        std::cout << " SELECIONE O TIPO DE ÁRVORE\n";
        std::cout << "===============================\n";
        std::cout << "1. Operar com Árvore AVL\n";
        std::cout << "2. Operar com Árvore Rubro-Negra\n";
        std::cout << "0. Sair\n";
        std::cout << "Opção: ";

        try {
            int opcao = ler_opcao_menu();

            if (opcao == 0) {
                std::cout << "Saindo...\n";
                break;
            }

            if (opcao == 1) {
                menu_avl();
            } else if (opcao == 2) {
                menu_rn();
            } else {
                std::cout << "[ERRO] Opção inválida!\n";
            }
        } catch (const std::exception& e) {
            std::cout << "[EXCEÇÃO CAPTURADA] " << e.what() << "\n";
        }
    }

    return 0;
}