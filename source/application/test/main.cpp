#include "app.h"

int main(int argc, char* argv[])
{
    core::App app;

    const int return_code = app.run("test", 1280, 720, argc, argv);

    return return_code;
}
