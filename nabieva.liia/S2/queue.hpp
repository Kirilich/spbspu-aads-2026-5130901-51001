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

  const T& front() const {
    if (empty()) {
      throw std::logic_error("Queue is empty");
    }
    return data[head];
  }
  const T& back() const {
    if (empty()) {
      throw std::runtime_error("Queue is empty");
    }
    size_t last;
    if (tail == 0) {
      last = capacity - 1;
    }
    else {
      last = tail - 1;
    }
    return data[last];
  }

  bool empty() const {
    return count == 0;
  }
  size_t size() const {
    return count;
  }

private:
  T* data;
  size_t capacity;
  size_t head;
  size_t tail;
  size_t count;
};

#endif
