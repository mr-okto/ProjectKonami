#pragma once

#include <ctime>
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <Wt/Dbo/Dbo.h>

namespace dbo = Wt::Dbo;

class DialogueDbData;
class MessageDbData;
class PictureDbData;
class UserDbData;
class ContentDbData;

typedef dbo::ptr<DialogueDbData> DialoguePtr;
typedef dbo::ptr<MessageDbData> MessagePtr;
typedef dbo::ptr<PictureDbData> PicturePtr;
typedef dbo::ptr<UserDbData> UserPtr;
typedef dbo::ptr<ContentDbData> ContentPtr;
typedef long long IdType;
typedef dbo::collection<dbo::ptr<DialogueDbData>> Dialogues;
typedef dbo::collection<dbo::ptr<MessageDbData>> Messages;
typedef dbo::collection<dbo::ptr<PictureDbData>> Pictures;
typedef dbo::collection<dbo::ptr<UserDbData>> Users;
typedef dbo::collection<dbo::ptr<ContentDbData>> ContentElements;

namespace Wt::Dbo {
  // Auto-increment id fields
  template<>
  struct dbo_traits<DialogueDbData> : public dbo_default_traits {
    static const char *surrogateIdField() {
      return "id";
    }
  };
  template<>
  struct dbo_traits<MessageDbData> : public dbo_default_traits {
    static const char *surrogateIdField() {
      return "id";
    }
  };
  template<>
  struct dbo_traits<PictureDbData> : public dbo_default_traits {
    static const char *surrogateIdField() {
      return "id";
    }
  };
  template<>
  struct dbo_traits<UserDbData> : public dbo_default_traits {
    static const char *surrogateIdField() {
      return "id";
    }
  };
}


class DialogueDbData {
 public:
  dbo::collection<UserPtr> members_;
  dbo::collection<MessagePtr> messages_;
  template<class Action>
  void persist(Action& a) {
    dbo::hasMany(a, messages_, dbo::ManyToOne, "dialogue");
    dbo::hasMany(a, members_, dbo::ManyToMany, "dialogue_members");
  }
};

class MessageDbData {
 public:
  std::string text_;
  std::time_t creation_dt_;
  bool is_read_;
  DialoguePtr dialogue_;
  UserPtr author_;
  dbo::collection<ContentPtr> content_elements_;

  template<class Action>
  void persist(Action& a) {
    dbo::field(a, text_, "text");
    dbo::field(a, creation_dt_, "creation_dt");
    dbo::field(a, is_read_, "is_read");
    dbo::belongsTo(a, dialogue_, "dialogue");
    dbo::belongsTo(a, author_, "author");
    dbo::hasMany(a, content_elements_, dbo::ManyToOne, "message");
  }
};

class PictureDbData {
 public:
  std::string path_;
  int access_lvl_;
  UserPtr user_;

  template<class Action>
  void persist(Action& a) {
    dbo::field(a, path_, "path");
    dbo::field(a, access_lvl_, "access_lvl");
    dbo::belongsTo(a, user_, "user");
  }
};

class UserDbData {
 public:
  enum Gender {
    MALE,
    FEMALE,
    NON_BINARY
  };
  std::string username_;
  std::string display_name_;
  std::string pwd_hash_;
  Gender gender_;
  dbo::collection<DialoguePtr> dialogues_;
  dbo::collection<PicturePtr> pictures_;

  template<class Action>
  void persist(Action& a) {
    dbo::field(a, username_, "username");
    dbo::field(a, display_name_, "display_name");
    dbo::field(a, pwd_hash_, "pwd_hash");
    dbo::field(a, gender_, "gender");
    dbo::hasMany(a, dialogues_,  dbo::ManyToMany, "dialogue_members");
    dbo::hasMany(a, pictures_, dbo::ManyToOne, "user");
  }
};

class ContentDbData {
 public:
  enum Type {
    IMAGE,
    VIDEO,
    DOCUMENT,
    OTHER
  };
  Type type_;
  std::string file_path_;
  std::string metadata_;
  MessagePtr message_;

  template<class Action>
  void persist(Action& a) {
    dbo::field(a, type_, "content_type");
    dbo::field(a, file_path_, "file_path");
    dbo::field(a, metadata_, "metadata");
    dbo::belongsTo(a, message_, "message");
  }
};
