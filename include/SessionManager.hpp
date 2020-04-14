#ifndef MESSENGER_PROJECT_SESSIONMANAGER_HPP
#define MESSENGER_PROJECT_SESSIONMANAGER_HPP

#include <vector>
#include <stdint-gcc.h>
#include <memory>


class Session;

//class ISessionManager {
//public:
//    virtual bool add_session(Session& s) = 0;
//    virtual bool close_session(Session& s) = 0;
//};

//class SessionManager : public ISessionManager {
class SessionManager {
private:

    std::vector<std::unique_ptr<Session>> active_sessions_;
    std::vector<std::unique_ptr<Session>> inactive_sessions_;
    std::vector<std::unique_ptr<Session>> recently_closed_sessions_;

public:
    SessionManager() = default;
    SessionManager(const SessionManager&) = default;
    ~SessionManager() = default;

    // вызывается на каждом запросе пользователя
    // обновляется time_point у сессии
    bool validate_session(const std::string& token) {return false;};

    bool add_session(const Session& s) { return false;}; // вызывается модулем Auth при успешной авторизации (Session создается в Auth)
    bool add_session(Session&& s) {return false;};
    bool close_session(Session& s) {return false;}; // вызывается при принудительном завершении сессии

    std::vector<uint32_t> get_online_users(); // вызывается ядром
    
private:
    void check_inactive_sessions(); // пробегается по списку автивных сессий, если (time::now - s.time_point) > 5 min ---> s.deactivate
    void delete_inactive_sessions();  //

};


#endif //MESSENGER_PROJECT_SESSIONMANAGER_HPP
