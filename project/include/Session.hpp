#ifndef MESSENGER_PROJECT_SESSION_HPP
#define MESSENGER_PROJECT_SESSION_HPP

#include <string>
#include <chrono>
#include <utility>

#include "ChatEvent.hpp"

struct Session {
public:
    enum Status {
        Active,
        Inactive,
    };

    std::string username_;
    std::string session_id_;
    uint32_t user_id_;
    std::chrono::time_point<std::chrono::system_clock> time_point_;
    Status status_;

    ChatEventCallback callback_;
    std::string cookie_;

public:
    Session() = default;
    Session(const Session& s) = default;
    ~Session() = default;

    bool is_active() const { return status_ == Status::Active; };

    void activate() { status_ = Status::Active; };
    void deactivate() { status_ = Status::Inactive; };

    uint32_t id() const { return user_id_;};
    Status status() const { return status_;};
    std::chrono::time_point<std::chrono::system_clock> time_point() const { return time_point_;};

    void set_callback(const ChatEventCallback& callback) { callback_ = callback; }
};


#endif //MESSENGER_PROJECT_SESSION_HPP
