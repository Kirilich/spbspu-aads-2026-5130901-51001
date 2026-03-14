#ifndef LIST_HPP
#define LIST_HPP

namespace nabieva
{
  template< class T >
  struct Node
  {
    T data;
    Node* next;
    Node(const T& value):
      data(value),
      next(nullptr)
    {}
  };

  template< class T > class List;

  template < class T >
  class LIter {
    friend class List< T >;
  private:
    Node<T>* node;
  public:
    LIter(Node<T>* n = nullptr):
      node(n)
    {}

    T& operator*()
    {
      return node->data;
    }
    T* operator->()
    {
      return &(node->data);
    }

    LIter& operator++()
    {
      node = node->next;
      return *this;
    }
    LIter<T> operator++(int)
    {
      LIter<T> temp = *this;
      node = node->next;
      return temp;
    }

    bool operator!=(const LIter& other) const
    {
      return node != other.node;
    }
    bool operator==(LIter<T> other) const
    {
      return node == other.node;
    }
  };

  template < class T >
  class LCIter {
    friend class List< T >;
  private:
    const Node<T>* node;
  public:
    LCIter(const Node<T>* n = nullptr):
      node(n)
    {}

    const T& operator*() const
    {
      return node->data;
    }
    const T* operator->() const
    {
      return &(node->data);
    }

    LCIter& operator++()
    {
      node = node->next;
      return *this;
    }
    LIter<T> operator++(int)
    {
      LIter<T> temp = *this;
      node = node->next;
      return temp;
    }

    bool operator!=(const LCIter& other) const
    {
      return node != other.node;
    }
    bool operator==(const LIter<T> other) const
    {
      return node == other.node;
    }
  };

  template < class T >
  class List {
  private:
    Node<T>* head;
  public:
    List():
      head(nullptr)
    {}
  };
}

#endif
