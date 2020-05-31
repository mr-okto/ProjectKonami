#pragma once

#include "BaseCache.hpp"

template <typename Key>
class LRUCachePolicy : public NoAlgCacheFunc<Key>
{
  public:
    LRUCachePolicy() = default;
    ~LRUCachePolicy() = default;

    void Insert(const Key &key) override;
    void Touch(const Key &key) override;
    void Delete(const Key &) override;
    const Key &DelElFull() const override;

  private:
    std::list<Key> lru_queue;
    std::unordered_map<Key, typename std::list<Key>::iterator> key_finder;
};

template <typename Key>
void LRUCachePolicy<Key>::Insert(const Key &key) {
    lru_queue.emplace_front(key);
    key_finder[key] = lru_queue.begin();
}

template <typename Key>
void LRUCachePolicy<Key>::Touch(const Key &key) {
    lru_queue.splice(lru_queue.begin(), lru_queue, key_finder[key]);
}

template <typename Key>
void LRUCachePolicy<Key>::Delete(const Key &key) {
    key_finder.erase(lru_queue.back());
    lru_queue.pop_back();
}

template <typename Key>
const Key &LRUCachePolicy<Key>::DelElFull() const {
    return lru_queue.back();
}
