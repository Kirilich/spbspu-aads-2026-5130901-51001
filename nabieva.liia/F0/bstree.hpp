#ifndef BSTREE_HPP
#define BSTREE_HPP

#include <cstddef>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace nabieva
{
  template< class Key, class Value, class Compare >
  class BSTree;
  namespace detail
  {
    template< class Key, class Value >
    struct TreeNode
    {
      std::pair< const Key, Value > data_;
      TreeNode* left_;
      TreeNode* right_;
      TreeNode* parent_;
      int height_;

      TreeNode(const Key& key, const Value& value, TreeNode* parent) :
        data_(key, value),
        left_(nullptr),
        right_(nullptr),
        parent_(parent),
        height_(1)
      {
      }
    };
  }
  template< class Key, class Value >
  class TreeConstIterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair< const Key, Value >;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;

    TreeConstIterator() :
      node_(nullptr)
    {
    }

    bool operator==(const TreeConstIterator& other) const
    {
      return node_ == other.node_;
    }

    bool operator!=(const TreeConstIterator& other) const
    {
      return !(*this == other);
    }

    const value_type& operator*() const
    {
      if (node_ == nullptr)
      {
        throw std::out_of_range("нельзя получить значение конечного итератора.");
      }

      return node_->data_;
    }

    const value_type* operator->() const
    {
      return &operator*();
    }

    TreeConstIterator& operator++()
    {
      if (node_ == nullptr)
      {
        throw std::out_of_range("нельзя передвинуть конечный итератор.");
      }

      if (node_->right_ != nullptr)
      {
        node_ = node_->right_;

        while (node_->left_ != nullptr)
        {
          node_ = node_->left_;
        }
      }
      else
      {
        const Node* parent = node_->parent_;

        while ((parent != nullptr) && (node_ == parent->right_))
        {
          node_ = parent;
          parent = parent->parent_;
        }

        node_ = parent;
      }

      return *this;
    }

    TreeConstIterator operator++(int)
    {
      const TreeConstIterator previous = *this;
      ++(*this);
      return previous;
    }

  private:
    template< class K, class V, class C >
    friend class BSTree;

    using Node = detail::TreeNode< Key, Value >;

    const Node* node_;

    explicit TreeConstIterator(const Node* node) :
      node_(node)
    {
    }
  };

  template< class Key, class Value, class Compare = std::less< Key > >
  class BSTree
  {
  public:
    using const_iterator = TreeConstIterator< Key, Value >;
    BSTree() :
      root_(nullptr),
      size_(0),
      compare_()
    {
    }

    BSTree(const BSTree& other) = delete;

    ~BSTree()
    {
      clear();
    }

    BSTree& operator=(const BSTree& other) = delete;

    void swap(BSTree& other)
    {
      std::swap(compare_, other.compare_);
      std::swap(root_, other.root_);
      std::swap(size_, other.size_);
    }

    bool isEmpty() const
    {
      return root_ == nullptr;
    }

    std::size_t getSize() const
    {
      return size_;
    }
    int getHeight() const
    {
      return getNodeHeight(root_);
    }
    bool add(const Key& key, const Value& value)
    {
      Node* current = root_;
      Node* previous = nullptr;
      bool insertLeft = false;

      while (current != nullptr)
      {
        previous = current;

        if (compare_(key, current->data_.first))
        {
          current = current->left_;
          insertLeft = true;
        }
        else if (compare_(current->data_.first, key))
        {
          current = current->right_;
          insertLeft = false;
        }
        else
        {
          return false;
        }
      }

      Node* const newNode = new Node(key, value, previous);

      if (previous == nullptr)
      {
        root_ = newNode;
      }
      else if (insertLeft)
      {
        previous->left_ = newNode;
      }
      else
      {
        previous->right_ = newNode;
      }

      ++size_;
      rebalanceFrom(previous);
      return true;
    }
    bool remove(const Key& key)
    {
      Node* removed = root_;

      while (removed != nullptr)
      {
        if (compare_(key, removed->data_.first))
        {
          removed = removed->left_;
        }
        else if (compare_(removed->data_.first, key))
        {
          removed = removed->right_;
        }
        else
        {
          break;
        }
      }

      if (removed == nullptr)
      {
        return false;
      }

      Node* start = removed->parent_;
      if (removed->left_ == nullptr)
      {
        replaceNode(removed, removed->right_);
      }
      else if (removed->right_ == nullptr)
      {
        replaceNode(removed, removed->left_);
      }
      else
      {
        Node* next = removed->right_;

        while (next->left_ != nullptr)
        {
          next = next->left_;
        }

        if (next->parent_ != removed)
        {
          start = next->parent_;
          replaceNode(next, next->right_);
          next->right_ = removed->right_;
          next->right_->parent_ = next;
        }
        else
        {
          start = next;
        }

        replaceNode(removed, next);
        next->left_ = removed->left_;
        next->left_->parent_ = next;
        updateHeight(next);
      }

      delete removed;
      --size_;
      rebalanceFrom(start);
      return true;
    }
    Value* find(const Key& key)
    {
      Node* current = root_;

      while (current != nullptr)
      {
        if (compare_(key, current->data_.first))
        {
          current = current->left_;
        }
        else if (compare_(current->data_.first, key))
        {
          current = current->right_;
        }
        else
        {
          return &current->data_.second;
        }
      }

      return nullptr;
    }
    const Value* find(const Key& key) const
    {
      const Node* current = root_;

      while (current != nullptr)
      {
        if (compare_(key, current->data_.first))
        {
          current = current->left_;
        }
        else if (compare_(current->data_.first, key))
        {
          current = current->right_;
        }
        else
        {
          return &current->data_.second;
        }
      }

      return nullptr;
    }
    const_iterator begin() const
    {
      const Node* current = root_;

      if (current == nullptr)
      {
        return end();
      }

      while (current->left_ != nullptr)
      {
        current = current->left_;
      }

      return const_iterator(current);
    }

    const_iterator end() const
    {
      return const_iterator(nullptr);
    }

    const_iterator cbegin() const
    {
      return begin();
    }

    const_iterator cend() const
    {
      return end();
    }
    void clear()
    {
      Node* current = root_;

      while (current != nullptr)
      {
        if (current->left_ != nullptr)
        {
          current = current->left_;
        }
        else if (current->right_ != nullptr)
        {
          current = current->right_;
        }
        else
        {
          Node* const parent = current->parent_;

          if (parent != nullptr)
          {
            if (parent->left_ == current)
            {
              parent->left_ = nullptr;
            }
            else
            {
              parent->right_ = nullptr;
            }
          }

          delete current;
          current = parent;
        }
      }

      root_ = nullptr;
      size_ = 0;
    }
  private:
    using Node = detail::TreeNode< Key, Value >;
    static int getNodeHeight(const Node* node)
    {
      return (node == nullptr) ? 0 : node->height_;
    }

    static void updateHeight(Node* node)
    {
      const int left = getNodeHeight(node->left_);
      const int right = getNodeHeight(node->right_);
      node->height_ = 1 + ((left > right) ? left : right);
    }

    static int getBalance(const Node* node)
    {
      return getNodeHeight(node->left_) - getNodeHeight(node->right_);
    }

    Node* rotateLeft(Node* node)
    {
      Node* const next = node->right_;
      node->right_ = next->left_;
      if (node->right_ != nullptr)
      {
        node->right_->parent_ = node;
      }
      replaceNode(node, next);
      next->left_ = node;
      node->parent_ = next;
      updateHeight(node);
      updateHeight(next);
      return next;
    }

    Node* rotateRight(Node* node)
    {
      Node* const next = node->left_;
      node->left_ = next->right_;
      if (node->left_ != nullptr)
      {
        node->left_->parent_ = node;
      }
      replaceNode(node, next);
      next->right_ = node;
      node->parent_ = next;
      updateHeight(node);
      updateHeight(next);
      return next;
    }

    void rebalanceFrom(Node* node)
    {
      while (node != nullptr)
      {
        updateHeight(node);
        const int balance = getBalance(node);
        if (balance > 1)
        {
          if (getBalance(node->left_) < 0)
          {
            rotateLeft(node->left_);
          }
          node = rotateRight(node);
        }
        else if (balance < -1)
        {
          if (getBalance(node->right_) > 0)
          {
            rotateRight(node->right_);
          }
          node = rotateLeft(node);
        }
        node = node->parent_;
      }
    }

    void replaceNode(Node* oldNode, Node* newNode)
    {
      Node* const parent = oldNode->parent_;

      if (parent == nullptr)
      {
        root_ = newNode;
      }
      else if (parent->left_ == oldNode)
      {
        parent->left_ = newNode;
      }
      else
      {
        parent->right_ = newNode;
      }

      if (newNode != nullptr)
      {
        newNode->parent_ = parent;
      }
    }
    Node* root_;
    std::size_t size_;
    Compare compare_;
  };
}

#endif
