#ifndef MESSENGER_PROJECT_SESSIONMANAGER_HPP
#define MESSENGER_PROJECT_SESSIONMANAGER_HPP

#include <vector>
#include <stdint.h>
#include <memory>

#include "Scheduler.hpp"

class Session;

class SessionManager {
private:
    std::vector<std::unique_ptr<Session>> active_sessions_;
    std::vector<std::unique_ptr<Session>> inactive_sessions_;
    std::vector<std::unique_ptr<Session>> recently_closed_sessions_;

    Scheduler scheduler_;

public:
    SessionManager() = default;
    SessionManager(const SessionManager&) = default;
    ~SessionManager() = default;

    // вызывается на каждом запросе пользователя
    // обновляется time_point у сессии
    bool validate_session(const std::string& token) {return false;};

    bool add_session(const Session& s) { return false;}; // вызывается модулем Auth при успешной авторизации (Session создается в Auth)
    bool add_session(Session&& s) {return false;};// вызывается модулем Auth при успешной авторизации (Session создается в Auth)
    bool add_session(std::unique_ptr<Session> s) { return false;};

    bool close_session(const Session& s) {return false;}; // вызывается при принудительном завершении сессии
    bool close_session(const std::string& token) {return false;};
    bool close_session(uint32_t user_id) {return false;};


    std::vector<uint32_t> get_online_users(); // вызывается ядром
    uint32_t get_id_by_token(std::string& token);
    
private:
    void check_inactive_sessions(); // пробегается по списку автивных сессий, если (time::now - s.time_point) > 5 min ---> s.deactivate
    void delete_inactive_sessions();  //

};


#endif //MESSENGER_PROJECT_SESSIONMANAGER_HPP
