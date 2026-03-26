#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <cstddef>

template<typename T>
class Queue {
public:
  Queue():
    data(nullptr),
    capacity(0),
    head(0),
    tail(0),
    count(0)
  {}
  Queue(const Queue& other):
    data(nullptr),
    capacity(0),
    head(0),
    tail(0),
    count(0)
  {
    *this = other;
  }
  ~Queue()
  {
    delete[] data;
  }

  Queue& operator=(const Queue& other) {
    if (this != &other) {
      delete[] data;
      capacity = other.capacity;
      head = other.head;
      tail = other.tail;
      count = other.count;
      if (capacity > 0) {
        data = new T[capacity];
        for (size_t i = 0; i < count; ++i) {
          data[(head + i) % capacity] = other.data[(other.head + i) % other.capacity];
        }
      }
      else {
        data = nullptr;
      }
    }
    return *this;
  }

  void push(const T& rhs);
  T pop();

  const T& front() const
  {
    if (empty()) {
      throw std::logic_error("Queue is empty");
    }
    return data[head];
  }
  const T& back() const
  {
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

  bool empty() const
  {
    return count == 0;
  }
  size_t size() const
  {
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
