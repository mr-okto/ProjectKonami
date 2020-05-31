#pragma once

#include <memory>
#include <istream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <DbData.hpp>

namespace dbo = Wt::Dbo;

template< class DBConnector >
class DbSession {
 private:
  bool is_connected_{};
  dbo::Session db_session_;
  std::unique_ptr<dbo::Transaction> transaction_;
  static std::string get_var(const std::string &name, const boost::property_tree::ptree &pt);
 public:
  DbSession() = default;
  bool connect(const std::string &host, uint32_t port, const std::string &user,
               const std::string &password, const std::string &db_name);
  bool connect(std::basic_istream<boost::property_tree::ptree::key_type::value_type> &json_config);
  bool connect(const std::string &json_config_fname);
  void disconnect();
  DbSession(const DbSession &) = delete;
  ~DbSession();
  void start_transaction();
  bool end_transaction();
  template<class C>
  dbo::ptr<C> add(std::unique_ptr<C> ptr);
  template<class C>
  dbo::ptr<C> get_by_id(long long int id);
  template<class C>
  dbo::Query<dbo::ptr<C>> find();
  template<class C>
  dbo::Query<C> raw_query(const std::string & sql);
  static std::string dbconfig_filename(int argc, char **argv);

};

template<class DBConnector>
DbSession<DBConnector>::~DbSession() {
  if (is_connected_) {
    disconnect();
  }
}

template<class DBConnector>
bool DbSession<DBConnector>::connect(const std::string &host,
                                     uint32_t port,
                                     const std::string &user,
                                     const std::string &password,
                                     const std::string &db_name) {
  unsigned long len_limit = host.size() + user.size() + password.size() + db_name.size() + 64;
  auto buf = new char[len_limit];
  snprintf(buf, len_limit, "host=%s user=%s password=%s port=%u dbname=%s",
           host.c_str(), user.c_str(), password.c_str(), port, db_name.c_str());
  std::unique_ptr<DBConnector> db_connector{new DBConnector()};
  try {
    db_connector->connect(buf);
  }
  catch (Wt::Dbo::Exception &e) {
    std::cerr << e.what();
    delete[] buf;
    return false;
  }
  delete[] buf;
  if (is_connected_) {
    disconnect();
  }
  is_connected_ = true;
  try {
    db_session_.setConnection(std::move(db_connector));
    db_session_.mapClass<DialogueDbData>("dialogue");
    db_session_.mapClass<MessageDbData>("message");
    db_session_.mapClass<UserDbData>("user");
    db_session_.mapClass<PictureDbData>("picture");
    db_session_.mapClass<ContentDbData>("content");
  }
  catch (Wt::Dbo::Exception &e) {
    std::cerr << e.what();
    is_connected_ = false;
    return false;
  }
  return true;
}

template<class DBConnector>
bool DbSession<DBConnector>::connect(std::basic_istream<boost::property_tree::ptree::key_type::value_type> &json_config) {
  boost::property_tree::ptree pt;
  try {
    boost::property_tree::read_json(json_config, pt);
    std::string host = get_var("host", pt);
    uint32_t port = 0;
    try {
      port = std::stoul(get_var("port", pt));
    }
    catch (std::logic_error &e) {
      std::cerr << e.what();
      return false;
    }
    std::string user = get_var("db_user", pt);
    std::string password = get_var("db_password", pt);
    std::string db_name = get_var("db_name", pt);
    return connect(host, port, user, password, db_name);
  }
  catch (boost::property_tree::ptree_error &e) {
    std::cerr << e.what();
    return false;
  }

}

template<class DBConnector>
bool DbSession<DBConnector>::connect(const std::string &json_config_fname) {
  std::ifstream f_in;
  f_in.open(json_config_fname);
  bool result = (bool) f_in;
  if (result) {
     result = connect(f_in);
  } else {
    std::cerr << "Unable to open DB config file: \"" << json_config_fname << "\"" << std::endl;
  }
  f_in.close();
  return result;
}

template<class DBConnector>
std::string DbSession<DBConnector>::get_var(const std::string &name, const boost::property_tree::ptree &pt) {
  std::string result = pt.get<std::string>(name);
  if (!result.empty() && result[0] == '$') {
    char *val = std::getenv(result.c_str() + 1);
    if (val) {
      result = val;
    }
  }
  return result;
}

template<class DBConnector>
void DbSession<DBConnector>::disconnect() {
  if (transaction_) {
    transaction_->commit();
    transaction_.reset();
  }
  db_session_.flush();
};

template<class DBConnector>
template<class C>
dbo::ptr<C> DbSession<DBConnector>::add(std::unique_ptr<C> ptr) {
  if (not transaction_) {
    // Disposable transaction
    dbo::Transaction transaction(db_session_);
    return transaction.session().add(std::move(ptr));
  }
  auto result = db_session_.add(std::move(ptr));
  return result;
}

template<class DBConnector>
template<class C>
dbo::ptr<C> DbSession<DBConnector>::get_by_id(IdType id) {
  if (not transaction_) {
    // Disposable transaction
    dbo::Transaction transaction(db_session_);
    return transaction.session().find<C>().where("id = ?").bind(id);
  }
  return db_session_.find<C>().where("id = ?").bind(id);
}

template<class DBConnector>
void DbSession<DBConnector>::start_transaction() {
  if (not transaction_) {
    transaction_ = std::make_unique<dbo::Transaction>(db_session_);
  }
}

template<class DBConnector>
bool DbSession<DBConnector>::end_transaction() {
  if (transaction_) {
    transaction_->commit();
    transaction_.reset();
    return true;
  }
  return false;
}

template<class DBConnector>
template<class C>
dbo::Query<dbo::ptr<C>>  DbSession<DBConnector>::find() {
  if (not transaction_) {
    dbo::Transaction transaction(db_session_);
    return transaction.session().find<C>();
  }
  return db_session_.find<C>();
}

template<class DBConnector>
template<class C>
dbo::Query<C> DbSession<DBConnector>::raw_query(const std::string &sql) {
  if (not transaction_) {
    dbo::Transaction transaction(db_session_);
    return transaction.session().query<C>(sql);
  }
  return db_session_.query<C>(sql);
}

// Detects "--db *.json" option
template<class DBConnector>
std::string DbSession<DBConnector>::dbconfig_filename(int argc, char **argv) {
  std::string fname("db_conf.json");
  char **end_ptr = argv + argc;
  auto cmp = [](const char *x) { return strcmp(x, "--db") == 0; };
  auto ptr = std::find_if(argv, end_ptr, cmp);
  if (ptr != end_ptr && ++ptr != end_ptr && boost::algorithm::ends_with(*ptr, ".json")) {
    fname = *ptr;
  }
  return fname;
}
