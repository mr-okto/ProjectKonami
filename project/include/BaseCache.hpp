#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

template <typename Key>
class CacheFunc
{
  public:
    virtual ~CacheFunc() {}

    // All caches functionality
    virtual void Insert(const Key &key) = 0;
    virtual void Touch(const Key &key) = 0;
    virtual void Delete(const Key &key) = 0;

    // Some el delete then cache full
    virtual const Key &DelElFull() const = 0;
};

template <typename Key>
class NoAlgCacheFunc : public CacheFunc<Key>
{
  public:
    NoAlgCacheFunc() = default;
    ~NoAlgCacheFunc() override = default;

    void Insert(const Key &key) override;
    void Touch(const Key &key) override {};
    void Delete(const Key &key) override;
    const Key &DelElFull() const;

  private:
    std::unordered_set<Key> key_storage;
};

template <typename Key>
void NoAlgCacheFunc<Key>::Insert(const Key &key) {
    key_storage.emplace(key);
}

template <typename Key>
void NoAlgCacheFunc<Key>::Delete(const Key &key) {
    key_storage.erase(key);
}

template <typename Key>
const Key &NoAlgCacheFunc<Key>::DelElFull() const {
    return *key_storage.cbegin();
}

//Base class for other algorithm
template <typename Key, typename Value, typename CacheStrategy = NoAlgCacheFunc<Key>>
class BaseNoAlgCache {
  public:
    BaseNoAlgCache();
    BaseNoAlgCache(size_t size);
    ~BaseNoAlgCache();

    void Put(const Key &key, const Value &value);
    const Value &Get(const Key &key) const;
    bool Cached(const Key &key) const;
    size_t Size() const;
    void Resize(size_t size);

    typename std::unordered_map<Key, Value>::const_iterator
    begin() const;

    typename std::unordered_map<Key, Value>::const_iterator
    end() const;

    void SetElTouchHook(std::function<void(Key, Value)> function);
    void SetElDeleteHook(std::function<void(Key, Value)> function);

  protected:
    void Insert(const Key &key, const Value &value);
    void Delete(const Key &key);
    void Update(const Key &key, const Value &value);

    typename std::unordered_map<Key, Value>::const_iterator
    FindElem(const Key &key) const;

    static void mockHock(Key key, Value value) {}
  private:
    mutable std::mutex cache_mutex;
    mutable CacheStrategy cache_func;

    std::unordered_map<Key, Value> items_map;
    size_t max_size{};

    std::function<void(Key, Value)> ElTouchHook;
    std::function<void(Key, Value)> ElDeleteHook;
};

template <typename Key, typename Value, typename CacheStrategy>
void BaseNoAlgCache<Key, Value, CacheStrategy>::Insert(const Key &key, const Value &value) {
    cache_func.Insert(key);
    items_map.emplace(std::make_pair(key, value));
}

template <typename Key, typename Value, typename CacheStrategy>
void BaseNoAlgCache<Key, Value, CacheStrategy>::Delete(const Key &key) {
    cache_func.Delete(key);
    auto elem_it = FindElem(key);
    ElDeleteHook(key, elem_it->second);
    items_map.erase(elem_it);
}

template <typename Key, typename Value, typename CacheStrategy>
void BaseNoAlgCache<Key, Value, CacheStrategy>::Update(const Key &key, const Value &value) {
    cache_func.Touch(key);
    items_map[key] = value;
}

template <typename Key, typename Value, typename CacheStrategy>
typename std::unordered_map<Key, Value>::const_iterator
BaseNoAlgCache<Key, Value, CacheStrategy>::FindElem(const Key &key) const {
    return items_map.find(key);
}

template <typename Key, typename Value, typename CacheStrategy>
BaseNoAlgCache<Key, Value, CacheStrategy>::~BaseNoAlgCache() {
    std::lock_guard<std::mutex> lock{cache_mutex};

    for (auto it = begin(); it != end(); ++it) {
        cache_func.Delete(it->first);
    }

    items_map.clear();
}

template <typename Key, typename Value, typename CacheStrategy>
size_t BaseNoAlgCache<Key, Value, CacheStrategy>::Size() const {
    std::lock_guard<std::mutex> lock{cache_mutex};
    return items_map.size();
}

template <typename Key, typename Value, typename CacheStrategy>
bool BaseNoAlgCache<Key, Value, CacheStrategy>::Cached(const Key &key) const {
    std::lock_guard<std::mutex> lock{cache_mutex};
    return FindElem(key) != items_map.end();
}

template <typename Key, typename Value, typename CacheStrategy>
void BaseNoAlgCache<Key, Value, CacheStrategy>::Put(const Key &key, const Value &value) {
    typename std::lock_guard<std::mutex> lock{cache_mutex};
    auto elem_it = FindElem(key);

    if (elem_it == items_map.end()) {
        if (items_map.size() + 1 > max_size) {
            auto disp_candidate_key = cache_func.DelElFull();
            Delete(disp_candidate_key);
        }
        Insert(key, value);
    }
    else {
        Update(key, value);
    }
}

template <typename Key, typename Value, typename CacheStrategy>
const Value &BaseNoAlgCache<Key, Value, CacheStrategy>::Get(const Key &key) const {
    typename std::lock_guard<std::mutex> lock{cache_mutex};
    auto elem_it = FindElem(key);

    if (elem_it == items_map.end()) {
        throw std::range_error{"No such element in the cache"}; // TODO
    }
    cache_func.Touch(key);
    ElTouchHook(key, elem_it->second);

    return elem_it->second;
}

template <typename Key, typename Value, typename CacheStrategy>
typename std::unordered_map<Key, Value>::const_iterator
BaseNoAlgCache<Key, Value, CacheStrategy>::begin() const {
    return items_map.begin();
}

template <typename Key, typename Value, typename CacheStrategy>
typename std::unordered_map<Key, Value>::const_iterator
BaseNoAlgCache<Key, Value, CacheStrategy>::end() const {
    return items_map.end();
}

template <typename Key, typename Value, typename CacheStrategy>
void BaseNoAlgCache<Key, Value, CacheStrategy>::Resize(size_t size) {
    max_size = size;
}

template <typename Key, typename Value, typename CacheStrategy>
BaseNoAlgCache<Key, Value, CacheStrategy>::BaseNoAlgCache() {
    ElTouchHook = mockHock;
    ElDeleteHook = mockHock;
}

template <typename Key, typename Value, typename CacheStrategy>
BaseNoAlgCache<Key, Value, CacheStrategy>::BaseNoAlgCache(size_t size) : max_size(size) {
    ElTouchHook = mockHock;
    ElDeleteHook = mockHock;
}

template <typename Key, typename Value, typename CacheStrategy>
void BaseNoAlgCache<Key, Value, CacheStrategy>::SetElTouchHook(
    std::function<void(Key, Value)> function) {
    ElTouchHook = function;
}


template <typename Key, typename Value, typename CacheStrategy>
void BaseNoAlgCache<Key, Value, CacheStrategy>::SetElDeleteHook(
    std::function<void(Key, Value)> function) {
    ElDeleteHook = function;
}
