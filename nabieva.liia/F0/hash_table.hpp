#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <cstddef>
#include <functional>
#include <limits>
#include <iterator>
#include <stdexcept>
#include <utility>
#include "list.hpp"

namespace nabieva
{
  template< class Key, class Value, class Hash = std::hash< Key >,
      class Equal = std::equal_to< Key > >
  class HashTable
  {
  public:
    class ConstIterator
    {
    public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = std::pair< const Key, Value >;
      using difference_type = std::ptrdiff_t;
      using pointer = const value_type*;
      using reference = const value_type&;

      ConstIterator():
        buckets_(nullptr),
        bucketCount_(0),
        bucketIndex_(0),
        position_()
      {
      }

      bool operator==(const ConstIterator& other) const
      {
        return (buckets_ == other.buckets_) && (bucketIndex_ == other.bucketIndex_)
            && (position_ == other.position_);
      }

      bool operator!=(const ConstIterator& other) const
      {
        return !(*this == other);
      }

      const value_type& operator*() const
      {
        if (bucketIndex_ == bucketCount_)
        {
          throw std::out_of_range("нельзя получить значение конечного итератора.");
        }
        return *position_;
      }

      const value_type* operator->() const
      {
        return &operator*();
      }

      ConstIterator& operator++()
      {
        if (bucketIndex_ == bucketCount_)
        {
          throw std::out_of_range("нельзя передвинуть конечный итератор.");
        }

        ++position_;
        while (position_ == buckets_[bucketIndex_].cend())
        {
          ++bucketIndex_;
          if (bucketIndex_ == bucketCount_)
          {
            break;
          }
          position_ = buckets_[bucketIndex_].cbegin();
        }
        return *this;
      }

      ConstIterator operator++(int)
      {
        const ConstIterator previous = *this;
        ++(*this);
        return previous;
      }

    private:
      friend class HashTable;
      using Bucket = List< value_type >;

      const Bucket* buckets_;
      std::size_t bucketCount_;
      std::size_t bucketIndex_;
      typename Bucket::const_iterator position_;

      ConstIterator(const Bucket* buckets, std::size_t bucketCount, std::size_t bucketIndex,
          typename Bucket::const_iterator position):
        buckets_(buckets),
        bucketCount_(bucketCount),
        bucketIndex_(bucketIndex),
        position_(position)
      {
      }
    };

    using const_iterator = ConstIterator;

    HashTable(const HashTable& other) = delete;

    explicit HashTable(std::size_t bucketCount):
      buckets_(nullptr),
      bucketCount_(bucketCount),
      size_(0),
      hash_(),
      equal_()
    {
      if (bucketCount_ == 0)
      {
        throw std::invalid_argument("количество корзин должно быть положительным.");
      }

      buckets_ = new Bucket[bucketCount_];
    }

    ~HashTable()
    {
      delete[] buckets_;
    }

    HashTable& operator=(const HashTable& other) = delete;

    void swap(HashTable& other)
    {
      std::swap(hash_, other.hash_);
      std::swap(equal_, other.equal_);
      std::swap(buckets_, other.buckets_);
      std::swap(bucketCount_, other.bucketCount_);
      std::swap(size_, other.size_);
    }

    bool isEmpty() const
    {
      return size_ == 0;
    }

    std::size_t getSize() const
    {
      return size_;
    }
    std::size_t getBucketCount() const
    {
      return bucketCount_;
    }

    void rehash(std::size_t bucketCount)
    {
      HashTable prepared(bucketCount);
      prepared.hash_ = hash_;
      prepared.equal_ = equal_;

      for (std::size_t index = 0; index < bucketCount_; ++index)
      {
        for (typename Bucket::const_iterator current = buckets_[index].cbegin();
            current != buckets_[index].cend(); ++current)
        {
          const std::size_t destination = prepared.hash_(current->first) % bucketCount;
          prepared.buckets_[destination].pushFront(*current);
          ++prepared.size_;
        }
      }

      swap(prepared);
    }

    bool add(const Key& key, const Value& value)
    {
      std::size_t index = hash_(key) % bucketCount_;
      if (findInBucket(key, index) != buckets_[index].cend())
      {
        return false;
      }

      const Entry prepared(key, value);
      constexpr std::size_t maximum = std::numeric_limits< std::size_t >::max();
      const std::size_t loadDivisor = 4;
      const std::size_t growthFactor = 2;
      const std::size_t limit = bucketCount_ - bucketCount_ / loadDivisor;
      if ((size_ == maximum) || ((size_ >= limit) && (bucketCount_ > maximum / growthFactor)))
      {
        throw std::length_error("хэш-таблица слишком велика.");
      }
      if (size_ >= limit)
      {
        rehash(bucketCount_ * growthFactor);
        index = hash_(prepared.first) % bucketCount_;
      }

      buckets_[index].pushFront(prepared);
      ++size_;
      return true;
    }
    bool remove(const Key& key)
    {
      const std::size_t index = hash_(key) % bucketCount_;
      Bucket& bucket = buckets_[index];

      if (bucket.isEmpty())
      {
        return false;
      }

      if (equal_(bucket.getFront().first, key))
      {
        bucket.popFront();
        --size_;
        return true;
      }

      typename Bucket::iterator previous = bucket.begin();
      typename Bucket::iterator current = previous;
      ++current;

      while (current != bucket.end())
      {
        if (equal_(current->first, key))
        {
          bucket.eraseAfter(previous);
          --size_;
          return true;
        }

        previous = current;
        ++current;
      }

      return false;
    }
    ConstIterator find(const Key& key) const
    {
      const std::size_t index = hash_(key) % bucketCount_;
      const typename Bucket::const_iterator position = findInBucket(key, index);
      if (position == buckets_[index].cend())
      {
        return end();
      }
      return ConstIterator(buckets_, bucketCount_, index, position);
    }

    bool has(const Key& key) const
    {
      return find(key) != end();
    }

    ConstIterator begin() const
    {
      for (std::size_t index = 0; index < bucketCount_; ++index)
      {
        if (!buckets_[index].isEmpty())
        {
          return ConstIterator(buckets_, bucketCount_, index, buckets_[index].cbegin());
        }
      }
      return end();
    }

    ConstIterator end() const
    {
      return ConstIterator(buckets_, bucketCount_, bucketCount_, typename Bucket::const_iterator());
    }

    ConstIterator cbegin() const
    {
      return begin();
    }

    ConstIterator cend() const
    {
      return end();
    }

    List< Key > getKeys() const
    {
      List< Key > keys;

      for (std::size_t index = 0; index < bucketCount_; ++index)
      {
        const Bucket& bucket = buckets_[index];
        typename Bucket::const_iterator position = bucket.cbegin();

        while (position != bucket.cend())
        {
          keys.pushFront(position->first);
          ++position;
        }
      }

      return keys;
    }

  private:
    using Entry = std::pair< const Key, Value >;
    using Bucket = List< Entry >;

    Bucket* buckets_;
    std::size_t bucketCount_;
    std::size_t size_;
    Hash hash_;
    Equal equal_;

    typename Bucket::const_iterator findInBucket(const Key& key, std::size_t index) const
    {
      const Bucket& bucket = buckets_[index];
      typename Bucket::const_iterator position = bucket.cbegin();
      while (position != bucket.cend())
      {
        if (equal_(position->first, key))
        {
          return position;
        }
        ++position;
      }
      return bucket.cend();
    }
  };
}

#endif
