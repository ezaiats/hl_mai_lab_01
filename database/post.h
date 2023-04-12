#ifndef AUTHOR_H
#define AUTHOR_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Post{
        private:
            long _id;
            std::string _login_in;
            std::string _login_out;
            std::string _address_in;
            std::string _address_out;
            float _weight;
            std::string _track_number;

        public:

            static Post fromJSON(const std::string & str);

            long               get_id() const;
            const std::string &get_login_in() const;
            const std::string &get_login_out() const;
            const std::string &get_address_in() const;
            const std::string &get_address_out() const;
            const float &get_weight() const;
            const std::string &get_track_number() const;

            long&        id();
            std::string &login_in();
            std::string &login_out();
            std::string &address_in();
            std::string &address_out();
            float &weight();
            std::string &track_number();

            static void init();
            static std::vector<Post> get_user_posts(std::string login_in);
            static bool check_login(std::string login);
            void save_to_mysql();

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif