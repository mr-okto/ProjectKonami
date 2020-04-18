#pragma once

#include <cstdint>
#include <string>
#include <functional>

class ChatEvent {
public:
    // types
    enum CallbackAction {
      SIGN_UP,
      SIGN_IN,
      SIGN_OUT,
      NEW_MSG,
      NEW_DIALOGUE,
    };

    ChatEvent(CallbackAction action, uint32_t user_id, std::string data = std::string())
        : action_(action),
          user_id_(user_id),
          data_(data)
          { }

    CallbackAction action() const { return action_; }


private:
  CallbackAction action_;
  uint32_t object_id_;
  uint32_t user_id_;
  std::string data_;
};

typedef std::function<void (const ChatEvent&)> ChatEventCallback;
