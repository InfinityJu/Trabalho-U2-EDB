#ifndef _ARVORERN_HPP_
#define _ARVORERN_HPP_

#include "node.hpp"
#include <iostream>
#include <algorithm> // std::max
#include <cstddef>   // std::size_t
#include <memory>    // std::unique_ptr, std::make_unique, std::move

namespace sc {

enum class Color { RED, BLACK };

/**
 * @brief Node extended specifically for Red-Black Tree.
 */
template <typename Key, typename Value = Key>
struct RBNode : public Node<Key, Value> {
  Color color{ Color::RED };

  explicit RBNode(const Key& k, const Value& v = Value{}, Color c = Color::RED)
      : Node<Key, Value>(k, v)
      , color{ c }
      {}
};

/**
 * @brief Red-Black Tree class (Left-Leaning Red-Black variant).
 */
template <typename Key = int, typename Value = Key>
class ArvoreRN {
private:
  using NodePtr = RBNode<Key, Value>*;

  std::unique_ptr<RBNode<Key, Value>> m_root;
  std::size_t m_size{ 0 };
  std::size_t m_rotacoes{ 0 }; //!< Rotation counter for metrics

  //=== Private Helper Methods for Colors and Balancing
  bool is_red(NodePtr node) const noexcept {
    if (!node) return false;
    return node->color == Color::RED;
  }

  void invert_colors(NodePtr node) {
    if (!node) return;
    node->color = (node->color == Color::RED) ? Color::BLACK : Color::RED;
    if (node->left) {
      auto left_ptr = static_cast<NodePtr>(node->left.get());
      left_ptr->color = (left_ptr->color == Color::RED) ? Color::BLACK : Color::RED;
    }
    if (node->right) {
      auto right_ptr = static_cast<NodePtr>(node->right.get());
      right_ptr->color = (right_ptr->color == Color::RED) ? Color::BLACK : Color::RED;
    }
  }

  //=== Rotation Operations
  std::unique_ptr<RBNode<Key, Value>> rotacionar_esquerda(std::unique_ptr<RBNode<Key, Value>> h) {
    m_rotacoes++;
    auto x = std::unique_ptr<RBNode<Key, Value>>(static_cast<RBNode<Key, Value>*>(h->right.release()));
    h->right = std::move(x->left);
    
    x->color = h->color;
    h->color = Color::RED;
    
    x->left = std::move(h);
    return x;
  }

  std::unique_ptr<RBNode<Key, Value>> rotacionar_direita(std::unique_ptr<RBNode<Key, Value>> h) {
    m_rotacoes++;
    auto x = std::unique_ptr<RBNode<Key, Value>>(static_cast<RBNode<Key, Value>*>(h->left.release()));
    h->left = std::move(x->right);

    x->color = h->color;
    h->color = Color::RED;

    x->right = std::move(h);
    return x;
  }

  //=== Helper de inserção LLRB
  std::unique_ptr<RBNode<Key, Value>> inserir_helper(std::unique_ptr<RBNode<Key, Value>> h,
                                                     const Key& key, const Value& value, bool& inseriu) {
    if (!h) {
      inseriu = true;
      return std::make_unique<RBNode<Key, Value>>(key, value, Color::RED);
    }

    if (key < h->key) {
      h->left = inserir_helper(std::unique_ptr<RBNode<Key, Value>>(
          static_cast<RBNode<Key, Value>*>(h->left.release())), key, value, inseriu);
    } else if (key > h->key) {
      h->right = inserir_helper(std::unique_ptr<RBNode<Key, Value>>(
          static_cast<RBNode<Key, Value>*>(h->right.release())), key, value, inseriu);
    } else {
      inseriu = false; // Chave existente
      return h;
    }

    // Corrigir links vermelhos inclinados à direita
    if (is_red(static_cast<NodePtr>(h->right.get())) && !is_red(static_cast<NodePtr>(h->left.get()))) {
      h = rotacionar_esquerda(std::move(h));
    }
    // Corrigir dois links vermelhos seguidos à esquerda
    if (is_red(static_cast<NodePtr>(h->left.get())) && 
        is_red(static_cast<NodePtr>(static_cast<NodePtr>(h->left.get())->left.get()))) {
      h = rotacionar_direita(std::move(h));
    }
    // Inverter cores se ambos os filhos forem vermelhos (Split de nó 4)
    if (is_red(static_cast<NodePtr>(h->left.get())) && is_red(static_cast<NodePtr>(h->right.get()))) {
      invert_colors(h.get());
    }

    return h;
  }

  std::size_t get_altura_helper(NodePtr node) const {
    if (!node) return 0;
    std::size_t alt_esq = get_altura_helper(static_cast<NodePtr>(node->left.get()));
    std::size_t alt_dir = get_altura_helper(static_cast<NodePtr>(node->right.get()));
    return 1 + std::max(alt_esq, alt_dir);
  }

public:
  //=== Constructors & Destructor
  ArvoreRN() : m_root{ nullptr }, m_size{ 0 }, m_rotacoes{ 0 } {}
  ~ArvoreRN() = default;

  //=== Public Core API
  bool inserir(const Key& key, const Value& value = Value{}) {
    bool inseriu = false;
    m_root = inserir_helper(std::move(m_root), key, value, inseriu);
    if (m_root) {
      m_root->color = Color::BLACK; // A raiz sempre é preta
    }
    if (inseriu) m_size++;
    return inseriu;
  }

  const Value* buscar(const Key& key) const {
    RBNode<Key, Value>* current = m_root.get();
    while (current != nullptr) {
      if (key == current->key) return &(current->value);
      if (key < current->key) {
        current = static_cast<RBNode<Key, Value>*>(current->left.get());
      } else {
        current = static_cast<RBNode<Key, Value>*>(current->right.get());
      }
    }
    return nullptr;
  }

  void em_ordem(std::ostream& os = std::cout) const {
    em_ordem_helper(m_root.get(), os);
  }

  void imprimir_estrutura(std::ostream& os = std::cout) const {
    if (!m_root) {
      os << "Árvore vazia.\n";
      return;
    }
    imprimir_helper(m_root.get(), "", true, true, os);
  }

  [[nodiscard]] std::size_t size()         const noexcept { return m_size;                          }
  [[nodiscard]] bool empty()               const noexcept { return m_size == 0;                     }
  [[nodiscard]] std::size_t get_altura()   const          { return get_altura_helper(m_root.get()); }
  [[nodiscard]] std::size_t get_rotacoes() const noexcept { return m_rotacoes;                      }

private:
  void em_ordem_helper(NodePtr node, std::ostream& os) const {
    if (!node) return;
    em_ordem_helper(static_cast<NodePtr>(node->left.get()), os);
    os << node->key << " ";
    em_ordem_helper(static_cast<NodePtr>(node->right.get()), os);
  }

  void imprimir_helper(NodePtr node, const std::string& prefix, bool is_tail, bool is_root, std::ostream& os) const {
    if (!node) return;

    std::string cor_str = (node->color == Color::RED) ? "R" : "B";
    std::string lado_str = is_root ? "[Root] " : (is_tail ? "[D] " : "[E] ");

    os << prefix;
    if (!is_root) {
      os << (is_tail ? "└── " : "├── ");
    } else {
      os << "└── ";
    }
    os << lado_str << node->key << " (" << cor_str << ")\n";

    NodePtr left = static_cast<NodePtr>(node->left.get());
    NodePtr right = static_cast<NodePtr>(node->right.get());

    if (left || right) {
      std::string new_prefix = prefix + (is_root ? "    " : (is_tail ? "    " : "│   "));
      
      // Se existir filho esquerdo, imprime como não-tail (├──) se houver filho direito, ou tail se não houver
      if (left) {
        imprimir_helper(left, new_prefix, right == nullptr, false, os);
      }
      
      // Filho direito é sempre a última ramificação do nó atual (└──)
      if (right) {
        imprimir_helper(right, new_prefix, true, false, os);
      }
    }
  }
};

} // namespace sc

#endif // _ARVORERN_HPP_