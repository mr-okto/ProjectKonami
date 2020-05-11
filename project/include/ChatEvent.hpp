#pragma once

#include <Wt/WString.h>
#include <Wt/WEvent.h>
#include <Wt/WSignal.h>
#include <DialogueService.hpp>

#include <cstdint>
#include <string>
#include <functional>

class ChatEvent {
public:
    // types
    enum Type {
      SIGN_UP,
      SIGN_IN,
      SIGN_OUT,
      NEW_MSG,
      NEW_DIALOGUE,
    };

    ChatEvent(Type type, uint32_t user_id, std::string data = std::string())
        : type_(type),
          user_id_(user_id),
          data_(data),
          dialogue_id_(-1) { }

    ChatEvent(const chat::Message& message, const Wt::WString& username, Type type = Type::NEW_MSG)
        : type_(type),
          username_(username),
          dialogue_id_(message.dialogue_id),
          message_(message) {}

    ChatEvent(Type type, const Wt::WString& username)
        : type_(type),
          username_(username) {}

    Type type() const { return type_; }


private:
  Type type_;
  uint32_t object_id_;
  uint32_t user_id_;
  int dialogue_id_;
  Wt::WString data_;
  Wt::WString username_;
  chat::Message message_;

    ChatEvent(Type type,
                  const Wt::WString& username,
                  uint32_t user_id,
                  const Wt::WString& data = Wt::WString::Empty)
        : type_(type),
          username_(username),
          data_(data),
          user_id_(user_id),
          dialogue_id_(-1) { }

  friend class ChatServer;
  friend class ChatWidget;
};

typedef std::function<void (const ChatEvent&)> ChatEventCallback;
