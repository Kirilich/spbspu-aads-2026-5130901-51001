#ifndef NODE_HPP
#define NODE_HPP

namespace nabieva {
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
}

#endif
