#ifndef MESSENGER_PROJECT_SESSION_HPP
#define MESSENGER_PROJECT_SESSION_HPP

#include <string>
#include <chrono>

class Session {
public:
    enum Status {
        Active,
        Inactive,
    };

private:
    std::string token_;
    uint32_t userID_;
    std::chrono::time_point<std::chrono::system_clock> time_point_;
    Status status_;

public:
    Session() = delete;
    Session(const Session& s) = default;
    ~Session() = default;

    explicit Session(uint32_t id, std::string&& token) :
        token_(token),
        userID_(id),
        time_point_(std::chrono::system_clock::now()),
        status_(Status::Active) {};

    bool operator==(const Session& rhs) const {};

    bool is_active() const { return status_ == Status::Active; };

    void activate() { status_ = Status::Active; };
    void deactivate() { status_ = Status::Inactive; };

    std::string token() const { return token_;};
    uint32_t id() const { return userID_;};
    Status status() const { return status_;};
    std::chrono::time_point<std::chrono::system_clock> time_point() const { return time_point_;};

};


#endif //MESSENGER_PROJECT_SESSION_HPP
