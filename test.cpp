#include <string>
#include <iostream>
// #include <filesystem>
#include "constants.h"
#include <dirent.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>

using namespace std;

// int main()
// {   
//     for (const auto & entry : fs::directory_iterator(Path::basePath()))
//         std::cout << entry.path() << std::endl;
// }

vector<string> string2vector(string str)
{
    istringstream inputss(str);
    istream_iterator<string> beg(inputss), end;
    vector<string> ret(beg, end);
    return ret;
}

void testDirent() 
{
    DIR* dir;
        struct dirent* ent;

        if ((dir = opendir(Path::basePath().c_str())) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                cout << ent->d_name << endl;
            }
            closedir(dir);
        }
}

int main()
{
    string str = "Hey whats up?";
    vector<string> splitup = string2vector(str);

    for (auto word : splitup)
    {
        cout << word <<  endl;
    }
    return 0;
}
