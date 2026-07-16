#include <drogon/drogon.h>
int main() {
    drogon::app().loadConfigFile("test_cors.json").run();
    return 0;
}
