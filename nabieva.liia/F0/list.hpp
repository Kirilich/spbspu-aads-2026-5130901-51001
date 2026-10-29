#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>
#include <iterator>
#include <stdexcept>

namespace nabieva
{
  template< class T >
  class List;
  template< class T >
  class ListConstIterator;
  namespace detail
  {
    template< class T >
    struct ListNode
    {
      T value_;
      ListNode* next_;

      ListNode(const T& value, ListNode* next) :
        value_(value),
        next_(next)
      {
      }
    };
  }
  template< class T >
  class ListIterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    ListIterator() :
      node_(nullptr)
    {
    }

    bool operator==(const ListIterator& other) const
    {
      return node_ == other.node_;
    }

    bool operator!=(const ListIterator& other) const
    {
      return !(*this == other);
    }

    bool operator==(const ListConstIterator< T >& other) const
    {
      return ListConstIterator< T >(*this) == other;
    }

    bool operator!=(const ListConstIterator< T >& other) const
    {
      return !(*this == other);
    }


    T& operator*() const
    {
      if (node_ == nullptr)
      {
        throw std::out_of_range("нельзя получить значение конечного итератора.");
      }

      return node_->value_;
    }

    T* operator->() const
    {
      return &operator*();
    }

    ListIterator& operator++()
    {
      if (node_ == nullptr)
      {
        throw std::out_of_range("нельзя передвинуть конечный итератор.");
      }

      node_ = node_->next_;
      return *this;
    }

    ListIterator operator++(int)
    {
      const ListIterator previous = *this;
      ++(*this);
      return previous;
    }

  private:
    friend class List< T >;
    friend class ListConstIterator< T >;
    using Node = detail::ListNode< T >;

    Node* node_;

    explicit ListIterator(Node* node) :
      node_(node)
    {
    }
  };

  template< class T >
  class ListConstIterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    ListConstIterator() :
      node_(nullptr)
    {
    }

    ListConstIterator(const ListIterator< T >& other) :
      node_(other.node_)
    {
    }

    bool operator==(const ListConstIterator& other) const
    {
      return node_ == other.node_;
    }

    bool operator!=(const ListConstIterator& other) const
    {
      return !(*this == other);
    }

    const T& operator*() const
    {
      if (node_ == nullptr)
      {
        throw std::out_of_range("нельзя получить значение конечного итератора.");
      }

      return node_->value_;
    }

    const T* operator->() const
    {
      return &operator*();
    }

    ListConstIterator& operator++()
    {
      if (node_ == nullptr)
      {
        throw std::out_of_range("нельзя передвинуть конечный итератор.");
      }

      node_ = node_->next_;
      return *this;
    }

    ListConstIterator operator++(int)
    {
      const ListConstIterator previous = *this;
      ++(*this);
      return previous;
    }

  private:
    friend class List< T >;

    using Node = detail::ListNode< T >;

    const Node* node_;

    explicit ListConstIterator(const Node* node) :
      node_(node)
    {
    }
  };

  template< class T >
  class List
  {
  public:
    using iterator = ListIterator< T >;
    using const_iterator = ListConstIterator< T >;
    List() :
      head_(nullptr)
    {
    }

    List(const List& other) :
      head_(nullptr)
    {
      const Node* current = other.head_;
      Node* tail = nullptr;

      try
      {
        while (current != nullptr)
        {
          Node* const newNode = new Node(current->value_, nullptr);

          if (head_ == nullptr)
          {
            head_ = newNode;
          }
          else
          {
            tail->next_ = newNode;
          }

          tail = newNode;
          current = current->next_;
        }
      }
      catch (...)
      {
        clear();
        throw;
      }
    }

    List(List&& other) noexcept :
      head_(other.head_)
    {
      other.head_ = nullptr;
    }

    ~List()
    {
      clear();
    }

    List& operator=(const List& other)
    {
      if (this != &other)
      {
        List copy(other);
        clear();
        head_ = copy.head_;
        copy.head_ = nullptr;
      }

      return *this;
    }

    List& operator=(List&& other) noexcept
    {
      if (this != &other)
      {
        clear();
        head_ = other.head_;
        other.head_ = nullptr;
      }

      return *this;
    }

    bool isEmpty() const
    {
      return head_ == nullptr;
    }

    void pushFront(const T& value)
    {
      Node* const newNode = new Node(value, head_);
      head_ = newNode;
    }
    T& getFront()
    {
      if (isEmpty())
      {
        throw std::out_of_range("список пуст.");
      }

      return head_->value_;
    }

    void popFront()
    {
      if (isEmpty())
      {
        throw std::out_of_range("список пуст.");
      }

      Node* const removed = head_;
      head_ = head_->next_;
      delete removed;
    }

    iterator insertAfter(const_iterator position, const T& value)
    {
      Node* const previous = findNode(position);
      Node* const newNode = new Node(value, previous->next_);
      previous->next_ = newNode;
      return iterator(newNode);
    }

    iterator eraseAfter(const_iterator position)
    {
      Node* const previous = findNode(position);
      Node* const removed = previous->next_;

      if (removed == nullptr)
      {
        throw std::out_of_range("после этой позиции нет элемента.");
      }

      previous->next_ = removed->next_;
      delete removed;
      return iterator(previous->next_);
    }

    const T& getFront() const
    {
      if (isEmpty())
      {
        throw std::out_of_range("список пуст.");
      }

      return head_->value_;
    }
    void clear()
    {
      while (head_ != nullptr)
      {
        Node* const removed = head_;
        head_ = head_->next_;
        delete removed;
      }
    }

    iterator begin()
    {
      return iterator(head_);
    }

    iterator end()
    {
      return iterator(nullptr);
    }

    const_iterator begin() const
    {
      return const_iterator(head_);
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

  private:
    using Node = detail::ListNode< T >;

    Node* head_;
    Node* findNode(const_iterator position)
    {
      if (position.node_ == nullptr)
      {
        throw std::out_of_range("конечный итератор нельзя использовать как позицию.");
      }

      Node* current = head_;

      while (current != nullptr)
      {
        if (current == position.node_)
        {
          return current;
        }

        current = current->next_;
      }

      throw std::invalid_argument("итератор не принадлежит этому списку.");
    }
  };
}

#endif
