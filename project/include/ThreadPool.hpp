
#ifndef MESSENGER_PROJECT_THREADPOOL_HPP
#define MESSENGER_PROJECT_THREADPOOL_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <atomic>
#include <future>

namespace utils {
    template <typename T>
    class MutexedQueue {
    private:
        std::queue<T> queue_;
        std::mutex mutex_;

    public:
        [[nodiscard]] bool is_empty() const {
            std::unique_lock<std::mutex> lock(this->mutex_);
            return queue_.empty();
        }
        
        bool enqueue(const T& val);
        bool dequeue(T& val);
    };
}

class ThreadPool {
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    
    std::vector<std::unique_ptr<std::thread>> threads_;
    std::vector<std::shared_ptr<std::atomic<bool>>> flags_;
    utils::MutexedQueue<std::function<void(int id)> *> queue_;
    
    std::atomic<bool> is_done_;
    std::atomic<bool> is_stop_;
    std::atomic<int> n_waiting_;

    void init() { this->n_waiting_ = 0; this->is_stop_ = false; this->is_done_ = false; }

    void set_thread(int i) {};

public:
    explicit ThreadPool(int n_threads) { 
        init();
        resize(n_threads);
    };

    // deleted
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool & operator=(const ThreadPool &) = delete;
    ThreadPool & operator=(ThreadPool &&) = delete;
    
    // the destructor waits for all the functions in the queue to be finished
    ~ThreadPool() {
        stop(true);
    }
    
    // get the number of running threads in the pool
    int size() { return static_cast<int>(threads_.size()); }

    // number of idle threads
    int n_idle() { return n_waiting_; }

    std::thread & get_thread(int i) { return *this->threads_[i]; }

    // change the number of threads in the pool
    // should be called from one thread, otherwise be careful to not interleave, also with this->stop()
    // n_threads must be >= 0
    void resize(int n_threads) {};

    // pops a functional wrapper to the original function
    std::function<void(int)> dequeue() {};

    // run the user's function that excepts argument int - id of the running thread. returned value is templatized
    // operator returns std::future, where the user can get the result and rethrow the catched exceptins
    template<typename F>
    auto enqueue(F && f) ->std::future<decltype(f(0))> {
        auto pck = std::make_shared<std::packaged_task<decltype(f(0))(int)>>(std::forward<F>(f));
        return pck->get_future();
    };

    // wait for all computing threads to finish and stop all threads
    // may be called asynchronously to not pause the calling thread while waiting
    // if isWait == true, all the functions in the queue are run, otherwise the queue is cleared without running the functions
    void stop(bool isWait = false) {};

};


#endif //MESSENGER_PROJECT_THREADPOOL_HPP
