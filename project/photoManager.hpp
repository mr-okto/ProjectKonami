#include <iostream>
#include <vector>

#include "models.hpp"

template <class Image>
class PhotoManager {
 public:
    PhotoManager();
    ~PhotoManager();
    int update_profile_pic(const User& user);
    int upload_to_db(User user, std::string path);
    std::string get_from_db(User user);
};

template<class Image>
PhotoManager<Image>::PhotoManager() {

}

template<class Image>
PhotoManager<Image>::~PhotoManager() {

}

template<class Image>
int PhotoManager<Image>::update_profile_pic(const User &user) {
    return 0;
}

template<class Image>
int PhotoManager<Image>::upload_to_db(User user, std::string path) {
    return 0;
}

template<class Image>
std::string PhotoManager<Image>::get_from_db(User user) {
    return std::__cxx11::string();
}
