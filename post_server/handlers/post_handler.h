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

#include "../../database/post.h"
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

class PostHandler : public HTTPRequestHandler
{

private:
    std::string _format;

    bool check_login(const std::string &login_in, const std::string &login_out, std::string &reason)
    {
        bool result = database::Post::check_login(login_in) && database::Post::check_login(login_out);
        if (result == false)
        {
            reason = "Login not found";
            return false;
        }

        else if (login_in == login_out)
        {
            reason = "Logins match";
            return false;
        }

        return true;
    }

    bool check_weight(const float weight, std::string &reason)
    {
        if (weight <= 0.)
        {
            reason = "Weight cannot be negative number";
            return false;
        }

        return true;
    }

public:
    PostHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        try
        {
            if (hasSubstr(request.getURI(), "/user_posts") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET))
            {
                std::string login = form.get("login_in");
                auto results = database::Post::get_user_posts(login);
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                for (size_t i = 0; i != results.size(); i++)
                {
                    root->set(std::to_string(i), results[i].get_login_in() + results[i].get_login_out() + results[i].get_address_in() + results[i].get_address_out() + std::to_string(results[i].get_weight()) + results[i].get_track_number() + "\n");
                }
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;
            }
            else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
            {
                if (form.has("login_in") && form.has("login_out") && form.has("address_in") && form.has("address_out") && form.has("weight"))
                {
                    database::Post post;
                    post.login_in() = form.get("login_in");
                    post.login_out() = form.get("login_out");
                    post.address_in() = form.get("address_in");
                    post.address_out() = form.get("address_out");
                    post.weight() = atof(form.get("weight").c_str());
                    post.track_number() = post.login_in() + std::to_string(std::rand());

                    bool check_result = true;
                    std::string message;
                    std::string reason;

                    if (!check_login(post.get_login_in(), post.get_login_out(), reason))
                    {
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }

                    if (!check_weight(post.get_weight(), reason))
                    {
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }

                    if (check_result)
                    {
                        post.save_to_mysql();
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");
                        std::ostream &ostr = response.send();
                        ostr << post.get_track_number();
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
        root->set("instance", "/post");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

};
#endif