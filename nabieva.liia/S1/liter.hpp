#ifndef LITER_HPP
#define LITER_HPP
#include "node.hpp"
#include <stdexcept>

namespace nabieva
{
  template < class T > class List;

  template < class T >
  class LIter {
    friend class List< T >;
  private:
    Node<T>* node;
  public:
    LIter(Node<T>* n = nullptr):
      node(n)
    {
    }

    T& operator*()
    {
      if (!node) throw std::logic_error("dereferencing end iterator");
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
    bool operator==(const LIter& other) const
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
    {
    }

    const T& operator*() const
    {
      if (!node) throw std::logic_error("dereferencing end iterator");
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
    LCIter<T> operator++(int)
    {
      LCIter<T> temp = *this;
      node = node->next;
      return temp;
    }

    bool operator!=(const LCIter& other) const
    {
      return node != other.node;
    }
    bool operator==(const LCIter& other) const
    {
      return node == other.node;
    }
  };
}

#endif
