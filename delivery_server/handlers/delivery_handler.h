#ifndef USEHANDLER_H
#define USEHANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <iostream>
#include <fstream>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../../database/delivery.h"
#include "../../helper.h"

static bool hasSubstr(const std::string &str, const std::string &substr)
{
    if (str.size() < substr.size())
        return false;
    for (size_t i = 0; i <= str.size() - substr.size(); ++i)
    {
        bool ok{true};
        for (size_t j = 0; ok && (j < substr.size()); ++j)
            ok = (str[i + j] == substr[j]);
        if (ok)
            return true;
    }
    return false;
}

class DeliveryHandler : public HTTPRequestHandler
{
private:
    std::string _format;

    bool check_track_number(const std::string &track_number, std::string &reason)
    {
        bool result = database::Delivery::check_track_number(track_number);
        if (result == false)
        {
            reason = "Post not found";
            return false;
        }

        return true;
    }

public:
    DeliveryHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        try
        {
            if (hasSubstr(request.getURI(), "/search_login_in") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET))
            {
                std::string login = form.get("login_in");
                auto results = database::Delivery::search_login_in(login);
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                for (size_t i = 0; i != results.size(); i++)
                {
                    root->set(std::to_string(i), std::to_string(results[i].get_id()) + results[i].get_track_number() + results[i].get_carrier() + std::to_string(results[i].get_status()) + "\n");
                }
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;
            }
            else if (hasSubstr(request.getURI(), "/search_login_out") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET))
            {
                std::string login = form.get("login_out");
                auto results = database::Delivery::search_login_out(login);
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                for (size_t i = 0; i != results.size(); i++)
                {
                    root->set(std::to_string(i), std::to_string(results[i].get_id()) + results[i].get_track_number() + results[i].get_carrier() + std::to_string(results[i].get_status()) + "\n");
                }
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;
            }
            else if (hasSubstr(request.getURI(), "/change_status") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST))
            {
                std::string track_number = form.get("track_number");
                int status = std::stoi(form.get("status"));
                bool check_result = true;
                std::string message;
                std::string reason;
                if (!check_track_number(track_number, reason))
                {
                    check_result = false;
                    message += reason;
                    message += "<br>";
                }
                if (check_result)
                {
                    database::Delivery::change_status(status, track_number);
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    response.send();
                    return;
                }
                else
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                    std::ostream &ostr = response.send();
                    ostr << message;
                    response.send();
                    return;
                }
            }
            else if (hasSubstr(request.getURI(), "/create_new_delivery") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST))
            {
                if (form.has("track_number") && form.has("carrier"))
                {
                    database::Delivery delivery;
                    delivery.track_number() = form.get("track_number");
                    delivery.carrier() = form.get("carrier");
                    delivery.status() = 0;
                    bool check_result = true;
                    std::string message;
                    std::string reason;
                    if (!check_track_number(delivery.get_track_number(), reason))
                    {
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }
                    if (check_result)
                    {
                        delivery.save_to_mysql();
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");
                        std::ostream &ostr = response.send();
                        ostr << delivery.get_id();
                        return;
                    }
                    else
                    {
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                        std::ostream &ostr = response.send();
                        ostr << message;
                        response.send();
                        return;
                    }
                }
            }
        }
        catch (...)
        {
        }

        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/not_found");
        root->set("title", "Internal exception");
        root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        root->set("detail", "request ot found");
        root->set("instance", "/delivery");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }
};
#endif