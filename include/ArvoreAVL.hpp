#ifndef _ARVOREAVL_HPP_
#define _ARVOREAVL_HPP_

#include "node.hpp"
#include <iostream>
#include <algorithm> // std::max
#include <cstddef>   // std::size_t
#include <memory>    // std::unique_ptr, std::make_unique, std::move

namespace sc {

/**
 * @brief Node extended specifically for AVL balancing (tracks height).
 */
template <typename Key, typename Value = Key>
struct AVLNode : public Node<Key, Value> {
  int height{ 1 }; //!< Height of the subtree rooted at this node.

  explicit AVLNode(const Key& k, const Value& v = Value{})
      : Node<Key, Value>(k, v)
      {}
};

/**
 * @brief AVL Tree class with concrete rebalancing operations.
 */
template <typename Key = int, typename Value = Key>
class ArvoreAVL {
private:
  using NodePtr = AVLNode<Key, Value>*;

  // Root node owned by std::unique_ptr
  std::unique_ptr<AVLNode<Key, Value>> m_root;
  std::size_t m_size{ 0 };
  std::size_t m_rotacoes{ 0 }; //!< Rotation counter for metrics

  //=== Private Helper Methods for Balancing
  int get_height(NodePtr node) const {
    return node ? node->height : 0;
  }

  int get_balance_factor(NodePtr node) const {
    if (!node) return 0;
    return get_height(static_cast<NodePtr>(node->left.get())) - 
           get_height(static_cast<NodePtr>(node->right.get()));
  }

  void update_height(NodePtr node) {
    if (node) {
      node->height = 1 + std::max(get_height(static_cast<NodePtr>(node->left.get())),
                                  get_height(static_cast<NodePtr>(node->right.get())));
    }
  }

  //=== Rotation Operations
  std::unique_ptr<AVLNode<Key, Value>> rotacionar_direita(std::unique_ptr<AVLNode<Key, Value>> y) {
    m_rotacoes++;
    auto x = std::unique_ptr<AVLNode<Key, Value>>(static_cast<AVLNode<Key, Value>*>(y->left.release()));
    auto T2 = std::move(x->right);

    x->right = std::move(y);
    x->right->left = std::move(T2);

    update_height(static_cast<NodePtr>(x->right.get()));
    update_height(x.get());

    return x;
  }

  std::unique_ptr<AVLNode<Key, Value>> rotacionar_esquerda(std::unique_ptr<AVLNode<Key, Value>> x) {
    m_rotacoes++;
    auto y = std::unique_ptr<AVLNode<Key, Value>>(static_cast<AVLNode<Key, Value>*>(x->right.release()));
    auto T2 = std::move(y->left);

    y->left = std::move(x);
    y->left->right = std::move(T2);

    update_height(static_cast<NodePtr>(y->left.get()));
    update_height(y.get());

    return y;
  }

  // Helper de rebalanceamento
  std::unique_ptr<AVLNode<Key, Value>> rebalancear(std::unique_ptr<AVLNode<Key, Value>> node) {
    if (!node) return nullptr;

    update_height(node.get());
    int balance = get_balance_factor(node.get());

    // Caso Esquerda-Esquerda
    if (balance > 1 && get_balance_factor(static_cast<NodePtr>(node->left.get())) >= 0) {
      return rotacionar_direita(std::move(node));
    }

    // Caso Esquerda-Direita
    if (balance > 1 && get_balance_factor(static_cast<NodePtr>(node->left.get())) < 0) {
      node->left = rotacionar_esquerda(std::unique_ptr<AVLNode<Key, Value>>(
          static_cast<AVLNode<Key, Value>*>(node->left.release())));
      return rotacionar_direita(std::move(node));
    }

    // Caso Direita-Direita
    if (balance < -1 && get_balance_factor(static_cast<NodePtr>(node->right.get())) <= 0) {
      return rotacionar_esquerda(std::move(node));
    }

    // Caso Direita-Esquerda
    if (balance < -1 && get_balance_factor(static_cast<NodePtr>(node->right.get())) > 0) {
      node->right = rotacionar_direita(std::unique_ptr<AVLNode<Key, Value>>(
          static_cast<AVLNode<Key, Value>*>(node->right.release())));
      return rotacionar_esquerda(std::move(node));
    }

    return node;
  }

  // Helpers recursivos para inserção e remoção
  std::unique_ptr<AVLNode<Key, Value>> inserir_helper(std::unique_ptr<AVLNode<Key, Value>> node,
                                                      const Key& key, const Value& value, bool& inseriu) {
    if (!node) {
      inseriu = true;
      return std::make_unique<AVLNode<Key, Value>>(key, value);
    }

    if (key < node->key) {
      node->left = inserir_helper(std::unique_ptr<AVLNode<Key, Value>>(
          static_cast<AVLNode<Key, Value>*>(node->left.release())), key, value, inseriu);
    } else if (key > node->key) {
      node->right = inserir_helper(std::unique_ptr<AVLNode<Key, Value>>(
          static_cast<AVLNode<Key, Value>*>(node->right.release())), key, value, inseriu);
    } else {
      inseriu = false; // Chave duplicada
      return node;
    }

    return rebalancear(std::move(node));
  }

  NodePtr encontrar_minimo(NodePtr node) const {
    NodePtr current = node;
    while (current && current->left) {
      current = static_cast<NodePtr>(current->left.get());
    }
    return current;
  }

  std::unique_ptr<AVLNode<Key, Value>> remover_helper(std::unique_ptr<AVLNode<Key, Value>> node,
                                                      const Key& key, bool& removeu) {
    if (!node) {
      removeu = false;
      return nullptr;
    }

    if (key < node->key) {
      node->left = remover_helper(std::unique_ptr<AVLNode<Key, Value>>(
          static_cast<AVLNode<Key, Value>*>(node->left.release())), key, removeu);
    } else if (key > node->key) {
      node->right = remover_helper(std::unique_ptr<AVLNode<Key, Value>>(
          static_cast<AVLNode<Key, Value>*>(node->right.release())), key, removeu);
    } else {
      removeu = true;

      // Nó com um filho ou nenhum
      if (!node->left) {
        return std::unique_ptr<AVLNode<Key, Value>>(
            static_cast<AVLNode<Key, Value>*>(node->right.release()));
      } else if (!node->right) {
        return std::unique_ptr<AVLNode<Key, Value>>(
            static_cast<AVLNode<Key, Value>*>(node->left.release()));
      }

      // Nó com dois filhos
      NodePtr temp = encontrar_minimo(static_cast<NodePtr>(node->right.get()));
      node->key = temp->key;
      node->value = temp->value;

      bool sub_removeu = false;
      node->right = remover_helper(std::unique_ptr<AVLNode<Key, Value>>(
          static_cast<AVLNode<Key, Value>*>(node->right.release())), temp->key, sub_removeu);
    }

    return rebalancear(std::move(node));
  }

public:
  //=== Constructors & Destructor
  ArvoreAVL() : m_root{ nullptr }, m_size{ 0 }, m_rotacoes{ 0 } {}
  ~ArvoreAVL() = default;

  //=== Public Core API
  bool inserir(const Key& key, const Value& value = Value{}) {
    bool inseriu = false;
    m_root = inserir_helper(std::move(m_root), key, value, inseriu);
    if (inseriu) m_size++;
    return inseriu;
  }

  bool remover(const Key& key) {
    bool removeu = false;
    m_root = remover_helper(std::move(m_root), key, removeu);
    if (removeu) m_size--;
    return removeu;
  }

  const Value* buscar(const Key& key) const {
    AVLNode<Key, Value>* current = m_root.get();
    while (current != nullptr) {
      if (key == current->key) return &(current->value);
      if (key < current->key) {
        current = static_cast<AVLNode<Key, Value>*>(current->left.get());
      } else {
        current = static_cast<AVLNode<Key, Value>*>(current->right.get());
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

  std::size_t size()         const noexcept { return m_size;                   }
  bool empty()               const noexcept { return m_size == 0;              }
  std::size_t get_altura()   const          { return get_height(m_root.get()); }
  std::size_t get_rotacoes() const noexcept { return m_rotacoes;               }

private:
  void em_ordem_helper(NodePtr node, std::ostream& os) const {
    if (!node) return;
    em_ordem_helper(static_cast<NodePtr>(node->left.get()), os);
    os << node->key << " ";
    em_ordem_helper(static_cast<NodePtr>(node->right.get()), os);
  }

  void imprimir_helper(NodePtr node, const std::string& prefix, bool is_tail, bool is_root, std::ostream& os) const {
    if (!node) return;

    std::string lado_str = is_root ? "[Root] " : (is_tail ? "[D] " : "[E] ");

    os << prefix;
    if (!is_root) {
      os << (is_tail ? "└── " : "├── ");
    } else {
      os << "└── ";
    }
    os << lado_str << node->key << " (h:" << node->height << ")\n";

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

#endif // _ARVOREAVL_HPP_