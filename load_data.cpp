#include <string>
#include <iostream>
#include <fstream>

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <functional>

using Poco::Data::Session;
using Poco::Data::Statement;

auto main() -> int
{

    Poco::Data::MySQL::Connector::registerConnector();

    std::string _connection_string;
    _connection_string+="host=";
    _connection_string+=std::getenv("DB_HOST");
    _connection_string+=";port=";
    _connection_string+=std::getenv("DB_PORT");
    _connection_string+=";user=";
    _connection_string+=std::getenv("DB_LOGIN");
    _connection_string+=";db=";
    _connection_string+=std::getenv("DB_DATABASE");
    _connection_string+=";password=";
    _connection_string+=std::getenv("DB_PASSWORD");
    std::cout << "connection string:" << _connection_string << std::endl;

    Poco::Data::Session session(
        Poco::Data::SessionFactory::instance().create(
            Poco::Data::MySQL::Connector::KEY, _connection_string));
    std::cout << "session created" << std::endl;
    try
    {
        // https://www.onlinedatagenerator.com/
        std::string json;
        std::ifstream is("data.json");
        std::istream_iterator<char> eos;
        std::istream_iterator<char> iit(is);
        while (iit != eos)
            json.push_back(*(iit++));
        is.close();

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(json);
        Poco::JSON::Array::Ptr arr = result.extract<Poco::JSON::Array::Ptr>();

        size_t i{0};
        for (i = 0; i < arr->size(); ++i)
        {
            Poco::JSON::Object::Ptr object = arr->getObject(i);
            std::string my_id = object->getValue<std::string>("my_id");
            std::string first_name = object->getValue<std::string>("first_name");
            std::string last_name = object->getValue<std::string>("last_name");
            std::string addres = object->getValue<std::string>("addres");
            std::string login = object->getValue<std::string>("login");
            std::string password = object->getValue<std::string>("password");

            std::string key;
            key += my_id;
            size_t shard_number = std::hash<std::string>{}(key)%(3);

            std::string result = "-- sharding:";
            result += std::to_string(shard_number);
            std::string sharding_hint = result;
            std::string select_str = "INSERT INTO User (my_id,first_name,last_name,addres,login,password) VALUES(?, ?, ?, ?, ?, ?) ";
            select_str += sharding_hint;

            Poco::Data::Statement insert(session);
            insert << select_str,
                Poco::Data::Keywords::use(my_id),
                Poco::Data::Keywords::use(first_name),
                Poco::Data::Keywords::use(last_name),
                Poco::Data::Keywords::use(addres),
                Poco::Data::Keywords::use(login),
                Poco::Data::Keywords::use(password);

            insert.execute();
            if(i%100==0) std::cout << ".";
        }

        std::cout << "Inserted " << i << " records" << std::endl; 
       
    }
    catch (Poco::Data::MySQL::ConnectionException &e)
    {
        std::cout << "connection:" << e.what() << std::endl;
    }
    catch (Poco::Data::MySQL::StatementException &e)
    {

        std::cout << "statement:" << e.what() << std::endl;
    }
    catch (std::exception* ex)
    {
        std::cout << "exception:" << ex->what() << std::endl;
    }
    return 1;
}