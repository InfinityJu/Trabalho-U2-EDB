#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip>
#include <cassert>

#include "../include/ArvoreAVL.hpp"
#include "../include/ArvoreRN.hpp"

// Evita a eliminação de código morto (Dead Code Elimination) no -O2
volatile const void* g_dummy_ptr = nullptr;

std::vector<int> gerar_dados(std::size_t n, const std::string& ordem) {
    std::vector<int> v(n);
    std::iota(v.begin(), v.end(), 1);

    if (ordem == "decrescente") {
        std::reverse(v.begin(), v.end());
    } else if (ordem == "nao_ordenada") {
        std::mt19937 g(42); // Semente fixa
        std::shuffle(v.begin(), v.end(), g);
    }
    return v;
}

void executar_testes_borda() {
    std::cout << "========================================================\n";
    std::cout << " RUNNING: Testes de Casos de Borda e Regras da Imagem\n";
    std::cout << "========================================================\n";

    // --- AVL ---
    {
        sc::ArvoreAVL<int> avl;
        assert(avl.empty());
        assert(avl.get_altura() == 0);
        assert(avl.buscar(10) == nullptr);
        assert(!avl.remover(10));

        avl.inserir(50);
        assert(avl.size() == 1);
        assert(avl.get_altura() == 1);
        assert(avl.buscar(50) != nullptr);

        avl.inserir(30);
        avl.inserir(70);
        avl.inserir(20);
        avl.inserir(40);

        assert(avl.remover(20));
        assert(avl.buscar(20) == nullptr);
        assert(avl.remover(30));
        assert(avl.buscar(30) == nullptr);
        assert(avl.remover(50));
        assert(avl.buscar(50) == nullptr);
        assert(!avl.remover(999));
    }

    // --- Rubro-Negra ---
    {
        sc::ArvoreRN<int> rn;
        assert(rn.empty());
        assert(rn.get_altura() == 0);
        assert(rn.buscar(10) == nullptr);
        assert(!rn.remover(10));

        rn.inserir(50);
        assert(rn.size() == 1);
        assert(rn.get_altura() == 1);
        assert(rn.buscar(50) != nullptr);

        rn.inserir(30);
        rn.inserir(70);
        rn.inserir(20);
        rn.inserir(40);

        assert(rn.remover(20));
        assert(rn.buscar(20) == nullptr);
        assert(!rn.remover(999));
    }

    std::cout << " [OK] Todos os testes de borda passaram com sucesso!\n\n";
}

void executar_testes_comparativos(std::size_t n) {
    std::cout << "=========================================================================\n";
    std::cout << " BUSTER TEST SUITE - TAMANHO DO CONJUNTO (N) = " << n << "\n";
    std::cout << "=========================================================================\n";

    std::vector<std::string> ordens = {"crescente", "decrescente", "nao_ordenada"};

    std::vector<std::pair<std::string, std::vector<int>>> cenarios;
    for (const auto& o : ordens) {
        cenarios.push_back({o, gerar_dados(n, o)});
    }

    // Número de repetições inversamente proporcional a N para garantir precisão e estabilidade
    std::size_t repeticoes = 1;
    if (n <= 100) repeticoes = 1000;
    else if (n <= 1000) repeticoes = 100;
    else if (n <= 10000) repeticoes = 10;

    std::cout << std::left 
              << std::setw(15) << "Ordem" 
              << std::setw(8)  << "Estrut." 
              << std::setw(10) << "Altura" 
              << std::setw(12) << "Rotacoes" 
              << std::setw(15) << "Ins. (us)" 
              << std::setw(15) << "Busca (us)" 
              << std::setw(15) << "Rem. (us)" << "\n";
    std::cout << std::string(90, '-') << "\n";

    for (const auto& cenario : cenarios) {
        const auto& nome_ordem = cenario.first;
        const auto& dados = cenario.second;

        // --- TESTANDO AVL ---
        {
            std::size_t altura_final = 0;
            std::size_t rotacoes_totais = 0;

            double tempo_ins_acumulado = 0;
            double tempo_busca_acumulado = 0;
            double tempo_rem_acumulado = 0;

            for (std::size_t r = 0; r < repeticoes; ++r) {
                sc::ArvoreAVL<int> avl;

                // 1. Inserção
                auto t_start = std::chrono::high_resolution_clock::now();
                for (int x : dados) avl.inserir(x);
                auto t_end = std::chrono::high_resolution_clock::now();
                tempo_ins_acumulado += std::chrono::duration<double, std::micro>(t_end - t_start).count();

                altura_final = avl.get_altura();
                rotacoes_totais = avl.get_rotacoes();

                // 2. Busca (existente + inexistente)
                t_start = std::chrono::high_resolution_clock::now();
                for (int x : dados) {
                    g_dummy_ptr = avl.buscar(x);
                }
                // Busca de elementos inexistentes
                g_dummy_ptr = avl.buscar(-1);
                g_dummy_ptr = avl.buscar(n + 100);
                t_end = std::chrono::high_resolution_clock::now();
                tempo_busca_acumulado += std::chrono::duration<double, std::micro>(t_end - t_start).count();

                // 3. Remoção
                t_start = std::chrono::high_resolution_clock::now();
                for (int x : dados) avl.remover(x);
                // Tentativa de remoção inexistente
                avl.remover(-1);
                t_end = std::chrono::high_resolution_clock::now();
                tempo_rem_acumulado += std::chrono::duration<double, std::micro>(t_end - t_start).count();
            }

            std::cout << std::left 
                      << std::setw(15) << nome_ordem 
                      << std::setw(8)  << "AVL" 
                      << std::setw(10) << altura_final 
                      << std::setw(12) << rotacoes_totais 
                      << std::setw(15) << std::fixed << std::setprecision(2) << (tempo_ins_acumulado / repeticoes)
                      << std::setw(15) << (tempo_busca_acumulado / repeticoes)
                      << std::setw(15) << (tempo_rem_acumulado / repeticoes) << "\n";
        }

        // --- TESTANDO RUBRO-NEGRA ---
        {
            std::size_t altura_final = 0;
            std::size_t rotacoes_totais = 0;

            double tempo_ins_acumulado = 0;
            double tempo_busca_acumulado = 0;
            double tempo_rem_acumulado = 0;

            for (std::size_t r = 0; r < repeticoes; ++r) {
                sc::ArvoreRN<int> rn;

                // 1. Inserção
                auto t_start = std::chrono::high_resolution_clock::now();
                for (int x : dados) rn.inserir(x);
                auto t_end = std::chrono::high_resolution_clock::now();
                tempo_ins_acumulado += std::chrono::duration<double, std::micro>(t_end - t_start).count();

                altura_final = rn.get_altura();
                rotacoes_totais = rn.get_rotacoes();

                // 2. Busca (existente + inexistente)
                t_start = std::chrono::high_resolution_clock::now();
                for (int x : dados) {
                    g_dummy_ptr = rn.buscar(x);
                }
                // Busca de elementos inexistentes
                g_dummy_ptr = rn.buscar(-1);
                g_dummy_ptr = rn.buscar(n + 100);
                t_end = std::chrono::high_resolution_clock::now();
                tempo_busca_acumulado += std::chrono::duration<double, std::micro>(t_end - t_start).count();

                // 3. Remoção
                t_start = std::chrono::high_resolution_clock::now();
                for (int x : dados) rn.remover(x);
                // Tentativa de remoção inexistente
                rn.remover(-1);
                t_end = std::chrono::high_resolution_clock::now();
                tempo_rem_acumulado += std::chrono::duration<double, std::micro>(t_end - t_start).count();
            }

            std::cout << std::left 
                      << std::setw(15) << nome_ordem 
                      << std::setw(8)  << "RN" 
                      << std::setw(10) << altura_final 
                      << std::setw(12) << rotacoes_totais 
                      << std::setw(15) << std::fixed << std::setprecision(2) << (tempo_ins_acumulado / repeticoes)
                      << std::setw(15) << (tempo_busca_acumulado / repeticoes)
                      << std::setw(15) << (tempo_rem_acumulado / repeticoes) << "\n";
        }
        std::cout << std::string(90, '-') << "\n";
    }
    std::cout << "\n";
}

int main() {
    executar_testes_borda();

    executar_testes_comparativos(100);
    executar_testes_comparativos(1000);
    executar_testes_comparativos(10000);
    executar_testes_comparativos(100000);

    return 0;
}