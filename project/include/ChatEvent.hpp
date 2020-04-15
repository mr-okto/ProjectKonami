#include <cstdint>
#include <string>
#include <functional>

enum CallbackAction {
  NEW_MSG,
  NEW_DIALOGUE,
};

struct ChatEvent {
  CallbackAction action;
  uint32_t object_id;
  uint32_t user_id;
  std::string data;
};

typedef std::function<void (const ChatEvent&) > ChatEventCallback;
