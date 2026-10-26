#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <memory>  // std::unique_ptr, std::move
#include <string>

namespace sc {

/**
 * @brief Generic Binary Tree Node using std::unique_ptr for child management.
 * 
 * @tparam Key Key type stored in the node.
 * @tparam Value Value type associated with the key.
 */
template <typename Key, typename Value = Key>
struct Node {
  Key key;
  Value value;
  std::unique_ptr<Node<Key, Value>> left;
  std::unique_ptr<Node<Key, Value>> right;

  explicit Node(const Key& k, const Value& v = Value{})
      : key  { k }
      , value{ v }
      , left { nullptr }
      , right{ nullptr }
      {}

  /// Helper booleans
  [[nodiscard]] bool is_leaf()   const noexcept { return left == nullptr && right == nullptr; }
  [[nodiscard]] bool one_kid()   const noexcept { return left != nullptr ^  right != nullptr; }
  [[nodiscard]] bool full_node() const noexcept { return left != nullptr && right != nullptr; }

};

} // namespace sc

#endif