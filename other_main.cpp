
#include <optional>
#include "other_server/http_other_server.h"

int main(int argc, char*argv[]) 
{
    HTTPOtherWebServer app;
    return app.run(argc, argv);
}