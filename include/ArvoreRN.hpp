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

  std::unique_ptr<RBNode<Key, Value>> fix_up(std::unique_ptr<RBNode<Key, Value>> h) {
    if (is_red(static_cast<NodePtr>(h->right.get())) && !is_red(static_cast<NodePtr>(h->left.get()))) {
      h = rotacionar_esquerda(std::move(h));
    }
    if (is_red(static_cast<NodePtr>(h->left.get())) && 
        is_red(static_cast<NodePtr>(static_cast<NodePtr>(h->left.get())->left.get()))) {
      h = rotacionar_direita(std::move(h));
    }
    if (is_red(static_cast<NodePtr>(h->left.get())) && is_red(static_cast<NodePtr>(h->right.get()))) {
      invert_colors(h.get());
    }
    return h;
  }

  std::unique_ptr<RBNode<Key, Value>> move_red_left(std::unique_ptr<RBNode<Key, Value>> h) {
    invert_colors(h.get());
    if (is_red(static_cast<NodePtr>(static_cast<NodePtr>(h->right.get())->left.get()))) {
      h->right = rotacionar_direita(std::unique_ptr<RBNode<Key, Value>>(
          static_cast<RBNode<Key, Value>*>(h->right.release())));
      h = rotacionar_esquerda(std::move(h));
      invert_colors(h.get());
    }
    return h;
  }

  std::unique_ptr<RBNode<Key, Value>> move_red_right(std::unique_ptr<RBNode<Key, Value>> h) {
    invert_colors(h.get());
    if (is_red(static_cast<NodePtr>(static_cast<NodePtr>(h->left.get())->left.get()))) {
      h = rotacionar_direita(std::move(h));
      invert_colors(h.get());
    }
    return h;
  }

  NodePtr encontrar_minimo(NodePtr node) const {
    NodePtr current = node;
    while (current && current->left) {
      current = static_cast<NodePtr>(current->left.get());
    }
    return current;
  }

  std::unique_ptr<RBNode<Key, Value>> remover_minimo(std::unique_ptr<RBNode<Key, Value>> h) {
    if (!h->left) return nullptr;

    if (!is_red(static_cast<NodePtr>(h->left.get())) && 
        !is_red(static_cast<NodePtr>(static_cast<NodePtr>(h->left.get())->left.get()))) {
      h = move_red_left(std::move(h));
    }

    h->left = remover_minimo(std::unique_ptr<RBNode<Key, Value>>(
        static_cast<RBNode<Key, Value>*>(h->left.release())));

    return fix_up(std::move(h));
  }

  std::unique_ptr<RBNode<Key, Value>> remover_helper(std::unique_ptr<RBNode<Key, Value>> h,
                                                     const Key& key, bool& removeu) {
    if (!h) {
      removeu = false;
      return nullptr;
    }

    if (key < h->key) {
      if (h->left) {
        if (!is_red(static_cast<NodePtr>(h->left.get())) && 
            !is_red(static_cast<NodePtr>(static_cast<NodePtr>(h->left.get())->left.get()))) {
          h = move_red_left(std::move(h));
        }
        h->left = remover_helper(std::unique_ptr<RBNode<Key, Value>>(
            static_cast<RBNode<Key, Value>*>(h->left.release())), key, removeu);
      }
    } else {
      if (is_red(static_cast<NodePtr>(h->left.get()))) {
        h = rotacionar_direita(std::move(h));
      }

      if (key == h->key && !h->right) {
        removeu = true;
        return nullptr;
      }

      if (h->right) {
        if (!is_red(static_cast<NodePtr>(h->right.get())) && 
            !is_red(static_cast<NodePtr>(static_cast<NodePtr>(h->right.get())->left.get()))) {
          h = move_red_right(std::move(h));
        }

        if (key == h->key) {
          removeu = true;
          NodePtr min_node = encontrar_minimo(static_cast<NodePtr>(h->right.get()));
          h->key = min_node->key;
          h->value = min_node->value;
          h->right = remover_minimo(std::unique_ptr<RBNode<Key, Value>>(
              static_cast<RBNode<Key, Value>*>(h->right.release())));
        } else {
          h->right = remover_helper(std::unique_ptr<RBNode<Key, Value>>(
              static_cast<RBNode<Key, Value>*>(h->right.release())), key, removeu);
        }
      }
    }

    return fix_up(std::move(h));
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

    return fix_up(std::move(h));
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

  bool remover(const Key& key) {
    if (!m_root || buscar(key) == nullptr) return false;

    if (!is_red(static_cast<NodePtr>(m_root->left.get())) && 
        !is_red(static_cast<NodePtr>(m_root->right.get()))) {
      m_root->color = Color::RED;
    }

    bool removeu = false;
    m_root = remover_helper(std::move(m_root), key, removeu);

    if (m_root) {
      m_root->color = Color::BLACK;
    }

    if (removeu) m_size--;
    return removeu;
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

  std::size_t size()         const noexcept { return m_size;                          }
  bool empty()               const noexcept { return m_size == 0;                     }
  std::size_t get_altura()   const          { return get_altura_helper(m_root.get()); }
  std::size_t get_rotacoes() const noexcept { return m_rotacoes;                      }

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
      
      if (left) {
        imprimir_helper(left, new_prefix, right == nullptr, false, os);
      }
      if (right) {
        imprimir_helper(right, new_prefix, true, false, os);
      }
    }
  }
};

} // namespace sc

#endif // _ARVORERN_HPP_