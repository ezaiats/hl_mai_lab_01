#include "post.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
    void Post::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Post` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`login_in` VARCHAR(256) NOT NULL,"
                        << "`login_out` VARCHAR(256) NOT NULL,"
                        << "`address_in` VARCHAR(256) NOT NULL,"
                        << "`address_out` VARCHAR(256) NOT NULL,"
                        << "`weight` FLOAT NOT NULL,"
                        << "`track_number` VARCHAR(256) NOT NULL UNIQUE,"
                        << "PRIMARY KEY (`id`));",
                now;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Post::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("login_in", _login_in);
        root->set("login_out", _login_out);
        root->set("address_in", _address_in);
        root->set("address_out", _address_out);
        root->set("weight", _weight);
        root->set("track_number", _track_number);

        return root;
    }

    Post Post::fromJSON(const std::string &str)
    {
        Post post;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        post.id() = object->getValue<long>("id");
        post.login_in() = object->getValue<std::string>("login_in");
        post.login_out() = object->getValue<std::string>("login_out");
        post.address_in() = object->getValue<std::string>("address_in");
        post.address_in() = object->getValue<std::string>("address_out");
        post.weight() = object->getValue<float>("weight");
        post.track_number() = object->getValue<std::string>("track_number");

        return post;
    }

    std::vector<Post> Post::get_user_posts(std::string login)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::vector<Post> result;
            Post a;
            select << "SELECT id, login_in, login_out, address_in, address_out, weight, track_number FROM Post WHERE login_in=?",
                into(a._id),
                into(a._login_in),
                into(a._login_out),
                into(a._address_in),
                into(a._address_out),
                into(a._weight),
                into(a._track_number),
                use(login),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    bool Post::check_login(std::string login)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::string tmp;
            select << "SELECT login FROM User WHERE login=?",
                into(tmp),
                use(login);
            select.execute();
            if (tmp.empty())
            {
                return false;
            }
            return true;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void Post::save_to_mysql()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Post (login_in,login_out,address_in,address_out,weight,track_number) VALUES(?, ?, ?, ?, ?, ?)",
                use(_login_in),
                use(_login_out),
                use(_address_in),
                use(_address_out),
                use(_weight),
                use(_track_number);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    const std::string &Post::get_login_in() const
    {
        return _login_in;
    }

    const std::string &Post::get_login_out() const
    {
        return _login_out;
    }

    long Post::get_id() const
    {
        return _id;
    }

    const std::string &Post::get_address_in() const
    {
        return _address_in;
    }

    const std::string &Post::get_address_out() const
    {
        return _address_out;
    }

    const float &Post::get_weight() const
    {
        return _weight;
    }

    const std::string &Post::get_track_number() const
    {
        return _track_number;
    }

    long &Post::id()
    {
        return _id;
    }

    std::string &Post::login_in()
    {
        return _login_in;
    }

    std::string &Post::login_out()
    {
        return _login_out;
    }

    std::string &Post::address_in()
    {
        return _address_in;
    }

    std::string &Post::address_out()
    {
        return _address_out;
    }

    float &Post::weight()
    {
        return _weight;
    }

    std::string &Post::track_number()
    {
        return _track_number;
    }
}