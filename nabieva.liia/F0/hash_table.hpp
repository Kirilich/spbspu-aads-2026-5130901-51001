#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <cstddef>
#include <functional>
#include <limits>
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
    explicit HashTable(std::size_t bucketCount) :
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

    HashTable(const HashTable& other) = delete;

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
      if (has(key))
      {
        return false;
      }

      const Entry prepared(key, value);
      const std::size_t maximum = std::numeric_limits< std::size_t >::max();
      const std::size_t limit = bucketCount_ - bucketCount_ / 4;
      if ((size_ == maximum) || ((size_ >= limit) && (bucketCount_ > maximum / 2)))
      {
        throw std::length_error("хэш-таблица слишком велика.");
      }
      if (size_ >= limit)
      {
        rehash(bucketCount_ * 2);
      }

      const std::size_t index = hash_(prepared.first) % bucketCount_;
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
    const Value* find(const Key& key) const
    {
      const std::size_t index = hash_(key) % bucketCount_;
      const Bucket& bucket = buckets_[index];
      typename Bucket::const_iterator position = bucket.cbegin();

      while (position != bucket.cend())
      {
        if (equal_(position->first, key))
        {
          return &position->second;
        }

        ++position;
      }

      return nullptr;
    }

    bool has(const Key& key) const
    {
      return find(key) != nullptr;
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
  };
}

#endif
