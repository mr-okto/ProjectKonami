#pragma once

#include <ctime>
#include <cstdint>
#include <vector>
#include <string>
#include <Wt/Dbo/Dbo.h>

namespace dbo = Wt::Dbo;

class DialogueModel;
class MessageModel;
class PictureModel;
class UserModel;

typedef dbo::ptr<DialogueModel> DialogueModelPtr;
typedef dbo::ptr<MessageModel> MessageModelPtr;
typedef dbo::ptr<PictureModel> PictureModelPtr;
typedef dbo::ptr<UserModel> UserModelPtr;
typedef long long IdType;
typedef dbo::collection<dbo::ptr<DialogueModel>> Dialogues;
typedef dbo::collection<dbo::ptr<MessageModel>> Messages;
typedef dbo::collection<dbo::ptr<PictureModel>> Pictures;
typedef dbo::collection<dbo::ptr<UserModel>> Users;

namespace Wt::Dbo {
  // Auto-increment id fields
  template<>
  struct dbo_traits<DialogueModel> : public dbo_default_traits {
    static const char *surrogateIdField() {
      return "id";
    }
  };
  template<>
  struct dbo_traits<MessageModel> : public dbo_default_traits {
    static const char *surrogateIdField() {
      return "id";
    }
  };
  template<>
  struct dbo_traits<PictureModel> : public dbo_default_traits {
    static const char *surrogateIdField() {
      return "id";
    }
  };
  template<>
  struct dbo_traits<UserModel> : public dbo_default_traits {
    static const char *surrogateIdField() {
      return "id";
    }
  };
}


class DialogueModel {
 public:
  dbo::collection<UserModelPtr> members_;
  dbo::collection<MessageModelPtr> messages_;
  template<class Action>
  void persist(Action& a) {
    dbo::hasMany(a, messages_, dbo::ManyToOne, "dialogue");
    dbo::hasMany(a, members_, dbo::ManyToMany, "dialogue_members");
  }
};

class MessageModel {
 public:
  std::string text_;
  std::time_t creation_dt_;
  bool is_read_;
  DialogueModelPtr dialogue_;
  UserModelPtr author_;

  template<class Action>
  void persist(Action& a) {
    dbo::field(a, text_, "text");
    dbo::field(a, creation_dt_, "creation_dt");
    dbo::field(a, is_read_, "is_read");
    dbo::belongsTo(a, dialogue_, "dialogue");
    dbo::belongsTo(a, author_, "author");
  }
};

class PictureModel {
 public:
  std::string path_;
  int access_lvl_;
  UserModelPtr user_;

  template<class Action>
  void persist(Action& a) {
    dbo::field(a, path_, "path");
    dbo::field(a, access_lvl_, "access_lvl");
    dbo::belongsTo(a, user_, "user");
  }
};

class UserModel {
 public:

  std::string username_;
  std::string pwd_hash_;
  dbo::collection<DialogueModelPtr> dialogues_;
  dbo::collection<PictureModelPtr> pictures_;

  template<class Action>
  void persist(Action& a) {
    dbo::field(a, username_, "username");
    dbo::field(a, pwd_hash_, "pwd_hash");
    dbo::hasMany(a, dialogues_,  dbo::ManyToMany, "dialogue_members");
    dbo::hasMany(a, pictures_, dbo::ManyToOne, "user");
  }
};
