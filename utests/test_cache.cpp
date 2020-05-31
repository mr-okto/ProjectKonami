#include <gtest/gtest.h>
#include "2QCache.hpp"

template <typename Key, typename Value>
using q2_cache = BaseNoAlgCache<Key, Value, Q2Cache<Key, Value>>;

template <typename Key, typename Value>
using lru_cache = BaseNoAlgCache<Key, Value, LRUCachePolicy<Key>>;

template <typename Key, typename Value>
using fifo_cache = BaseNoAlgCache<Key, Value, FIFOCacheStrategy<Key>>;

TEST(FIFOCache, put_get_test) {
    fifo_cache<int, int> cache(2);

    cache.Put(1, 10);
    cache.Put(2, 20);

    EXPECT_EQ(cache.Size(), 2);
    EXPECT_EQ(cache.Get(1), 10);
    EXPECT_EQ(cache.Get(2), 20);
}

TEST(FIFOCache, replace_key) {
    fifo_cache<int, int> cache(2);

    cache.Put(2, 200);
    cache.Put(4, 400);
    cache.Put(3, 600);
    EXPECT_THROW(cache.Get(2), std::range_error);
    EXPECT_EQ(cache.Get(4), 400);
    EXPECT_EQ(cache.Get(3), 600);
}

TEST(FIFOCache, replace_val) {
    fifo_cache<int, int> cache(2);

    cache.Put(2, 200);
    cache.Put(4, 400);
    cache.Put(2, 600);
    EXPECT_EQ(cache.Get(2), 600);
    EXPECT_EQ(cache.Get(4), 400);
}

TEST(FIFOCache, miss_value) {
    fifo_cache<int, int> cache(2);

    cache.Put(1, 11);

    EXPECT_EQ(cache.Size(), 1);
    EXPECT_EQ(cache.Get(1), 11);
    EXPECT_THROW(cache.Get(2), std::range_error);
}

TEST(FIFOCache, replace_val_after_touch) {
    fifo_cache<int, int> cache(3);

    cache.Put(1, 10);
    cache.Put(2, 20);
    cache.Put(3, 30);
    EXPECT_EQ(cache.Get(1), 10);
    EXPECT_EQ(cache.Get(3), 30);
    cache.Put(4, 40);

    EXPECT_EQ(cache.Get(2), 20);
    EXPECT_THROW(cache.Get(1), std::range_error);
    EXPECT_EQ(cache.Get(3), 30);
    EXPECT_EQ(cache.Get(4), 40);
}

TEST(FIFOCache, full_work_test) {
    int test_size = 10;
    fifo_cache<int, int> cache(test_size);
    for (int i = 0; i < test_size; i++) {
        cache.Put(i, i);
    }
    for (int i = 0; i < test_size; i++) {
        EXPECT_EQ(cache.Get(i), i);
    }
    for (int i = 0; i < test_size / 2; i++) {
        cache.Put(i + cache.Size(), i + cache.Size());
    }
    for (int i = 0; i < test_size / 2; i++) {
        EXPECT_EQ(cache.Get(i + cache.Size()), i + cache.Size());
    }
    for (int i = test_size / 2; i < test_size; i++) {
        EXPECT_EQ(cache.Get(i), i);
    }
    for (int i = 0; i < test_size / 2; i++) {
        EXPECT_THROW(cache.Get(i), std::range_error);
    }
}

TEST(LRUcache, put_get_test) {
    lru_cache<int, int> cache(2);

    cache.Put(1, 10);
    cache.Put(2, 20);

    EXPECT_EQ(cache.Size(), 2);
    EXPECT_EQ(cache.Get(1), 10);
    EXPECT_EQ(cache.Get(2), 20);
}

TEST(LRUcache, replace_key) {
    lru_cache<int, int> cache(2);

    cache.Put(2, 200);
    cache.Put(4, 400);
    cache.Put(3, 600);
    EXPECT_THROW(cache.Get(2), std::range_error);
    EXPECT_EQ(cache.Get(4), 400);
    EXPECT_EQ(cache.Get(3), 600);
}

TEST(LRUcache, replace_val) {
    lru_cache<int, int> cache(2);

    cache.Put(2, 200);
    cache.Put(4, 400);
    cache.Put(2, 600);
    EXPECT_EQ(cache.Get(2), 600);
    EXPECT_EQ(cache.Get(4), 400);
}

TEST(LRUcache, miss_value) {
    lru_cache<int, int> cache(2);

    cache.Put(1, 11);

    EXPECT_EQ(cache.Size(), 1);
    EXPECT_EQ(cache.Get(1), 11);
    EXPECT_THROW(cache.Get(2), std::range_error);
}

TEST(LRUcache, full_work_test) {
    int test_size = 10;
    lru_cache<int, int> cache(test_size);
    for (int i = 0; i < test_size; i++) {
        cache.Put(i, i);
    }
    for (int i = 0; i < test_size; i++) {
        EXPECT_EQ(cache.Get(i), i);
    }
    for (int i = 0; i < test_size / 2; i++) {
        cache.Put(i + cache.Size(), i + cache.Size());
    }
    for (int i = 0; i < test_size / 2; i++) {
        EXPECT_EQ(cache.Get(i + cache.Size()), i + cache.Size());
    }
    for (int i = test_size / 2; i < test_size; i++) {
        EXPECT_EQ(cache.Get(i), i);
    }
    for (int i = 0; i < test_size / 2; i++) {
        EXPECT_THROW(cache.Get(i), std::range_error);
    }
}

TEST(LRUcache, replace_val_after_touch) {
    lru_cache<int, int> cache(3);

    cache.Put(1, 10);
    cache.Put(2, 20);
    cache.Put(3, 30);
    EXPECT_EQ(cache.Get(1), 10);
    EXPECT_EQ(cache.Get(3), 30);
    cache.Put(4, 40);

    EXPECT_EQ(cache.Get(1), 10);
    EXPECT_THROW(cache.Get(2), std::range_error);
    EXPECT_EQ(cache.Get(3), 30);
    EXPECT_EQ(cache.Get(4), 40);
}

TEST(LRUcache, replace_val_after_touch2) {
    lru_cache<int, int> cache(5);

    cache.Put(1, 10);
    cache.Put(2, 20);
    cache.Put(3, 30);
    cache.Put(4, 40);
    cache.Put(5, 50);
    EXPECT_EQ(cache.Get(1), 10);
    EXPECT_EQ(cache.Get(3), 30);
    EXPECT_EQ(cache.Get(5), 50);

    cache.Put(6, 60);
    cache.Put(7, 70);

    EXPECT_EQ(cache.Get(1), 10);
    EXPECT_THROW(cache.Get(2), std::range_error);
    EXPECT_EQ(cache.Get(3), 30);
    EXPECT_THROW(cache.Get(4), std::range_error);
    EXPECT_EQ(cache.Get(5), 50);
    EXPECT_EQ(cache.Get(6), 60);
    EXPECT_EQ(cache.Get(7), 70);
}


TEST(Q2Cache, put_get) {
    Q2Cache<int, int> cache(5);

    cache.Put(1, 10);
    cache.Put(2, 20);
    cache.Put(3, 30);
    cache.Put(4, 40);

    EXPECT_EQ(cache.Get(1), 10);
    EXPECT_EQ(cache.Get(2), 20);
    EXPECT_EQ(cache.Get(3), 30);
    EXPECT_EQ(cache.Get(4), 40);
}

TEST(Q2Cache, miss_val) {
    Q2Cache<int, int> cache(5);

    cache.Put(1, 10);
    cache.Put(2, 20);
    cache.Put(3, 30);
    cache.Put(4, 40);
    cache.Put(5, 50);

    EXPECT_EQ(cache.Get(2), 20);
    EXPECT_EQ(cache.Get(3), 30);
    EXPECT_EQ(cache.Get(4), 40);
    EXPECT_EQ(cache.Get(5), 50);
    EXPECT_THROW(cache.Get(1), std::range_error);
}


TEST(Q2Cache, push_to_hot_path) {
    Q2Cache<int, int> cache(5);

    cache.Put(1, 10);
    cache.Put(2, 20);
    cache.Put(3, 30);
    cache.Put(4, 40);

    cache.Get(4);
    cache.Get(2);

    cache.Put(5, 50);
    cache.Put(6, 60);
    cache.Put(7, 70);
    cache.Put(8, 80);

    EXPECT_EQ(cache.Get(2), 20);
    EXPECT_THROW(cache.Get(1), std::range_error);
    EXPECT_THROW(cache.Get(3), std::range_error);
    EXPECT_THROW(cache.Get(4), std::range_error);
}
