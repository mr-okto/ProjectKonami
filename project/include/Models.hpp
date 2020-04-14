#pragma once

#include <ctime>
#include <cstdint>
#include <vector>
#include <string>

class DialogueModel;
class MessageModel;
class PictureModel;
class UserModel;

#define DialogueModelPtr DialogueModel*
#define MessageModelPtr MessageModel*
#define PictureModelPtr PictureModel*
#define UserModelPtr UserModel*


class DialogueModel {
 protected:
  uint32_t id_;
 public:
  std::vector<UserModelPtr> members_;
  std::vector<MessageModelPtr> messages_;
};

class MessageModel {
 protected:
  uint32_t id_;
 public:
  std::string text_;
  std::time_t creation_dt_;
  DialogueModelPtr dialogue_;
  UserModelPtr author_;
};

class PictureModel {
 protected:
  uint32_t id_;
 public:
  std::string path_;
  uint32_t access_lvl_;
  UserModelPtr user_;
};

class UserModel {
 protected:
  uint32_t id_;
 public:
  std::string username_;
  std::string pwd_hash_;
  std::vector<DialogueModelPtr> dialogues_;
  std::vector<PictureModelPtr> pictures_;
};
