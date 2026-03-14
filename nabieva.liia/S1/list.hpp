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
    bool operator==(const LCIter<T> other) const
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

    LIter<T> begin()
    {
      return LIter<T>(head);
    }

    LIter<T> end()
    {
      return LIter<T>(nullptr);
    }

    T& front()
    {
      return head->data;
    }

    const T& front() const
    {
      return head->data;
    }

    T& back()
    {
      Node<T>* current = head;
      while (current->next)
      {
        current = current->next;
      }
      return current->data;
    }

    const T& back() const
    {
      Node<T>* current = head;
      while (current->next)
      {
        current = current->next;
      }
      return current->data;
    }

    void push_front(const T& value)
    {
      Node<T>* node = new Node<T>(value);
      node->next = head;
      head = node;
    }

    void push_back(const T& value)
    {
      Node<T>* node = new Node<T>(value);
      if (!head)
      {
        head = node;
        return;
      }
      Node<T>* current = head;
      while (current->next)
      {
        current = current->next;
      }
      current->next = node;
    }

    void pop_front()
    {
      if (!head)
      {
        return;
      }

      Node<T>* tmp = head;
      head = head->next;
      delete tmp;
    }

    void pop_back()
    {
      if (!head)
      {
        return;
      }
      if (!head->next)
      {
        delete head;
        head = nullptr;
        return;
      }
      Node<T>* current = head;
      while (current->next->next)
      {
        current = current->next;
      }
      delete current->next;
      current->next = nullptr;
    }


    bool empty() const
    {
      return head == nullptr;
    }

    ~List()
    {
      clear();
    }

    void clear()
    {
      while (head)
      {
        pop_front();
      }
    }
    List(const List& other)
    {
      head = nullptr;
      Node<T>* current = other.head;
      while (current)
      {
        push_back(current->data);
        current = current->next;
      }
    }

    List& operator=(const List& other)
    {
      if (this != &other)
      {
        clear();
        Node<T>* current = other.head;
        while (current)
        {
          push_back(current->data);
          current = current->next;
        }
      }
      return *this;
    }

    LCIter<T> cbegin() const
    {
      return LCIter<T>(head);
    }

    LCIter<T> cend() const
    {
      return LCIter<T>(nullptr);
    }
  };
}

#endif
