#ifndef _ARVOREAVL_HPP_
#define _ARVOREAVL_HPP_

#include "node.hpp"
#include <iostream>
#include <algorithm> // std::max
#include <cstddef>   // std::size_t

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
    if (node->is_leaf){
        return 0;
    } else {
        return get_height(static_cast<NodePtr>(node->left)) - get_height(static_cast<NodePtr>(node->right.get()));
    }
  }

  void update_height(NodePtr node) {
    if (node) {
      node->height = 1 + std::max(get_height(static_cast<NodePtr>(node->left.get())),
                                  get_height(static_cast<NodePtr>(node->right.get())));
    }
  }

  //=== Rotation Operations
  NodePtr rotacionar_direita(NodePtr y) {
    m_rotacoes++;
    // Perform rotation using std::unique_ptr releases and moves
    auto x = std::unique_ptr<AVLNode<Key, Value>>(static_cast<AVLNode<Key, Value>*>(y->left.release()));
    auto T2 = std::move(x->right);

    x->right = std::unique_ptr<AVLNode<Key, Value>>(y);
    x->right->left = std::move(T2);

    // Update parents
    if (x->right->left) {
      x->right->left->parent = x->right.get();
    }
    x->right->parent = x.get();

    update_height(x->right.get());
    update_height(x.get());

    return x.release();
  }

  NodePtr rotacionar_esquerda(NodePtr x) {
    m_rotacoes++;
    auto y = std::unique_ptr<AVLNode<Key, Value>>(static_cast<AVLNode<Key, Value>*>(x->right.release()));
    auto T2 = std::move(y->left);

    y->left = std::unique_ptr<AVLNode<Key, Value>>(x);
    y->left->right = std::move(T2);

    if (y->left->right) {
      y->left->right->parent = y->left.get();
    }
    y->left->parent = y.get();

    update_height(y->left.get());
    update_height(y.get());

    return y.release();
  }

public:
  //=== Constructors & Destructor
  ArvoreAVL() : m_root{ nullptr }, m_size{ 0 }, m_rotacoes{ 0 } {}
  ~ArvoreAVL() = default;

  //=== Public Core API
  bool inserir(const Key& key, const Value& value = Value{}) {
    // Insertion implementation logic
    return true;
  }

  bool remover(const Key& key) {
    // Removal implementation logic
    return true;
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
    imprimir_helper(m_root.get(), "", true, os);
  }

  [[nodiscard]] std::size_t size()         const noexcept { return m_size;                   }
  [[nodiscard]] bool empty()               const noexcept { return m_size == 0;              }
  [[nodiscard]] std::size_t get_altura()   const          { return get_height(m_root.get()); }
  [[nodiscard]] std::size_t get_rotacoes() const noexcept { return m_rotacoes;               }

private:
  void em_ordem_helper(NodePtr node, std::ostream& os) const {
    if (!node) return;
    em_ordem_helper(static_cast<NodePtr>(node->left.get()), os);
    os << node->key << " ";
    em_ordem_helper(static_cast<NodePtr>(node->right.get()), os);
  }

  void imprimir_helper(NodePtr node, const std::string& prefix, bool is_tail, std::ostream& os) const {
    if (!node) return;
    os << prefix << (is_tail ? "└── " : "├── ") << node->key << " (h:" << node->height << ")\n";
    
    NodePtr left = static_cast<NodePtr>(node->left.get());
    NodePtr right = static_cast<NodePtr>(node->right.get());

    if (left || right) {
      if (left) imprimir_helper(left, prefix + (is_tail ? "    " : "│   "), !right, os);
      if (right) imprimir_helper(right, prefix + (is_tail ? "    " : "│   "), true, os);
    }
  }
};

} // namespace sc

#endif // _ARVOREAVL_HPP_