#pragma once

#include "BaseCache.hpp"

template <typename Key>
class FIFOCacheStrategy : public NoAlgCacheFunc<Key>
{
  public:
    FIFOCacheStrategy() = default;
    ~FIFOCacheStrategy() override = default;

    void Insert(const Key &key) override;
    void Touch(const Key &key) override {}
    void Delete(const Key &key) override;

    const Key &DelElFull() const override;

  private:
    std::list<Key> fifo_queue;
};

template <typename Key>
void FIFOCacheStrategy<Key>::Insert(const Key &key) {
    fifo_queue.emplace_front(key);
}

template <typename Key>
void FIFOCacheStrategy<Key>::Delete(const Key &key) {
    fifo_queue.pop_back();
}

template <typename Key>
const Key &FIFOCacheStrategy<Key>::DelElFull() const {
    return fifo_queue.back();
}
