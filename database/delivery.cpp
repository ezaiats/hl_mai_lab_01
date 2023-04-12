#include "delivery.h"
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
    void Delivery::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Delivery` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`track_number` VARCHAR(256) NOT NULL,"
                        << "`carrier` VARCHAR(256) NOT NULL,"
                        << "`status` INT NOT NULL,"
                        << "`time` DATETIME NOT NULL,"
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

    Poco::JSON::Object::Ptr Delivery::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("id", _id);
        root->set("track_number", _track_number);
        root->set("carrier", _carrier);
        root->set("status", _status);
        root->set("time", _time);
        return root;
    }

    Delivery Delivery::fromJSON(const std::string &str)
    {
        Delivery delivery;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        delivery.id() = object->getValue<long>("id");
        delivery.track_number() = object->getValue<std::string>("track_number");
        delivery.carrier() = object->getValue<std::string>("carrier");
        delivery.status() = object->getValue<int>("status");
        delivery.time() = object->getValue<time_t>("time");
        return delivery;
    }

    void Delivery::save_to_mysql()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Delivery (track_number,carrier,status,time) VALUES(?, ?, ?, NOW())",
                use(_track_number),
                use(_carrier),
                use(_status);
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

    std::vector<Delivery> Delivery::search_login_in(std::string login_in)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::vector<Delivery> result;
            Delivery a;

            select << "SELECT Post.id, Post.track_number, carrier, status, time FROM Post INNER JOIN Delivery ON Post.track_number=Delivery.track_number WHERE login_in=? GROUP BY Delivery.track_number",
                    into(a._id),
                    into(a._track_number),
                    into(a._carrier),
                    into(a._status),
                    into(a._time),
                    use(login_in),
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

    std::vector<Delivery> Delivery::search_login_out(std::string login_out)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::vector<Delivery> result;
            Delivery a;

            select << "SELECT Post.id, Post.track_number, carrier, status, time FROM Post INNER JOIN Delivery ON Post.track_number=Delivery.track_number WHERE login_out=? GROUP BY Delivery.track_number",
                    into(a._id),
                    into(a._track_number),
                    into(a._carrier),
                    into(a._status),
                    into(a._time),
                    use(login_out),
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

    void Delivery::change_status(int status, std::string track_number)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::string carrier;
            select << "SELECT carrier FROM Delivery WHERE track_number=? GROUP BY track_number",
                    into(carrier),
                    use(track_number);
            select.execute();

            Poco::Data::Statement insert(session);
            insert << "INSERT INTO Delivery (track_number,carrier,status,time) VALUES(?, ?, ?, NOW())",
                    use(track_number),
                    use(carrier),
                    use(status);
            insert.execute();
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

    bool Delivery::check_track_number(std::string track_number)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::string tmp;
            select << "SELECT track_number FROM Post WHERE track_number=?",
                into(tmp),
                use(track_number);
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

    long Delivery::get_id() const
    {
        return _id;
    }

    const std::string &Delivery::get_track_number() const
    {
        return _track_number;
    }

    const std::string &Delivery::get_carrier() const
    {
        return _carrier;
    }

    const int &Delivery::get_status() const
    {
        return _status;
    }

    const time_t &Delivery::get_time() const
    {
        return _time;
    }

    long &Delivery::id()
    {
        return _id;
    }

    std::string &Delivery::track_number()
    {
        return _track_number;
    }

    std::string &Delivery::carrier()
    {
        return _carrier;
    }

    int &Delivery::status()
    {
        return _status;
    }

    time_t &Delivery::time()
    {
        return _time;
    }
}