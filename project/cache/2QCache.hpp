#pragma once

#include "BaseCache.hpp"
#include "FifoCache.hpp"
#include "LRUCache.hpp"

template <typename Key, typename Value>
class Q2Cache
{
  public:
    Q2Cache(int size);
    ~Q2Cache() = default;

    void Put(Key key, Value value);
    Value Get(const Key &key);

  private:
    BaseNoAlgCache<Key, Value, FIFOCacheStrategy<Key>> InCache;
    BaseNoAlgCache<Key, Value, FIFOCacheStrategy<Key>> OutCache;
    BaseNoAlgCache<Key, Value, LRUCachePolicy<Key>> HotLRU;
};

template <typename Key, typename Value>
Q2Cache<Key, Value>::Q2Cache(int size) {
    int fifo_size = int(size * 0.4);
    int LRU_size = size - fifo_size * 2;

    auto ElDeleteInCache = [=](Key key, Value val) {
      this->OutCache.Put(key, val);
    };
    InCache.SetElDeleteHook(ElDeleteInCache);

    auto ElTouchedOutCache = [=](Key key, Value val) {
      this->HotLRU.Put(key, val);
    };
    OutCache.SetElTouchHook(ElTouchedOutCache);

    InCache.Resize(fifo_size);
    OutCache.Resize(fifo_size);
    HotLRU.Resize(LRU_size);
}

template <typename Key, typename Value>
void Q2Cache<Key, Value>::Put(Key key, Value value) {
    InCache.Put(key, value);
}

template <typename Key, typename Value>
Value Q2Cache<Key, Value>::Get(const Key &key) {
    Value result;
    try {
        result = HotLRU.Get(key);
        return result;
    } catch (const std::range_error& e) {}

    try {
        result = InCache.Get(key);
        return result;
    } catch (const std::range_error& e) {}

    try {
        result = OutCache.Get(key);
        return result;
    } catch (const std::range_error& e) {}

    throw std::range_error{"No such element in the cache"};
}
