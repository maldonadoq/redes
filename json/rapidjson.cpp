#include <iostream>
#include <fstream>
#include <cstdlib>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>

int main()
{
    using namespace rapidjson;

    std::ifstream ifs { R"(/home/maldonado/Network/json/alice.json)"};
    if ( !ifs.is_open() )
    {
        std::cerr << "Could not open file for reading!\n";
        return EXIT_FAILURE;
    }

    IStreamWrapper isw { ifs };
    std::cout << ifs << '\n';
    return 0;
}