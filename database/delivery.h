#ifndef AUTHOR_H
#define AUTHOR_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>
#include <ctime>

namespace database
{
    class Delivery{
        private:
            long _id;
            std::string _track_number;
            std::string _carrier;
            int _status;
            time_t _time;

        public:
            static Delivery fromJSON(const std::string & str);
            Poco::JSON::Object::Ptr toJSON() const;

            long               get_id() const;
            const std::string &get_track_number() const;
            const std::string &get_carrier() const;
            const int         &get_status() const;
            const time_t      &get_time() const;

            long        &id();
            std::string &track_number();
            std::string &carrier();
            int         &status();
            time_t      &time();

            static void init();
            static std::vector<Delivery> search_login_in(std::string login_in);
            static std::vector<Delivery> search_login_out(std::string login_out);
            static void change_status(int status, std::string track_number);
            static bool check_track_number(std::string track_number);
            void save_to_mysql();
    };
}

#endif