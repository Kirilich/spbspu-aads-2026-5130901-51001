#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <cstddef>

template<typename T>
class Queue {
public:
  Queue();
  Queue(const Queue& other);
  ~Queue();

  Queue& operator=(const Queue& other);

  void push(const T& rhs);
  T pop();

  const T& front() const;
  const T& back() const;

  bool empty() const;
  size_t size() const;

private:
  T* data;
  size_t capacity;
  size_t head;
  size_t tail;
  size_t count;
};

#endif