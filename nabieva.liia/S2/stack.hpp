#ifndef STACK_HPP
#define STACK_HPP

#include <cstddef>

template<typename T>
class Stack {
public:
  Stack();
  Stack(const Stack& other);
  ~Stack();

  Stack& operator=(const Stack& other);

  void push(const T& rhs);
  T pop();

  const T& top() const;

  bool empty() const;
  size_t size() const;

private:
  T* data;
  size_t capacity;
  size_t count;
};

#endif
