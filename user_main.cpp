#include "my_service/http_web_server.h"

int main(int argc, char*argv[]) 
{
    std::cout << "Hello!!\n";
    HTTPWebServer app;
    return app.run(argc, argv);
}