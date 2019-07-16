#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"

using std::string;
using std::vector;
using std::ifstream;
using std::istringstream;

class ProcessParser{
private:
    ifstream stream;
    public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static string getVmSize(string pid);
    static string getCpuPercent(string pid);
    static long int getSysUpTime();
    static string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getNumberOfCores();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static string PrintCpuStats(vector<string> values1, vector<string>values2);
    static bool isPidExisting(string pid);
};

// TODO: Define all of the above functions below:

string ProcessParser::getCmd(string pid)
{
    string cmd;
    ifstream stream;
    Util::getStream(Path::basePath() + pid + Path::cmdPath(), stream);

    if (stream.is_open())
    {
        getline(stream, cmd);
    }
    return cmd;
}

vector<string> ProcessParser::getPidList()
{
    vector<string> pidList;
    DIR* dir;
    dirent* entry;

    if ((dir = opendir(Path::basePath().c_str())) != NULL)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            if ((entry->d_type == DT_DIR) && Util::isNumber(entry->d_name))
            {
                pidList.push_back(entry->d_name);
            }
        }
        
        if (closedir(dir)) // Returns 0 on successful closure
        {
            throw std::runtime_error(std::strerror(errno));
        }
    }
    else
    {
        throw std::runtime_error(std::strerror(errno));
    }

    return pidList;
}

string ProcessParser::getVmSize(string pid)
{
    string line;
    string name = "VmData";
    string value;
    float result = 0.0f;
    ifstream stream;
    Util::getStream(Path::basePath() + pid + Path::statPath(), stream);

    if (stream.is_open())
    {
        while (std::getline(stream, line))
        {
            if (line.compare(0, name.size(), name) == 0)
            {
                istringstream buffer(line);
                string word;
                for(int i = 0; i <= 1; i++)
                {
                    word << buffer;
                }
                result = stof(word)/float(1024);
                break;
            }
        }
    }
    return to_string(result);
}

string ProcessParser::getCpuPercent(string pid)
{
    string line;
    ifstream stream;
    float result = 0.0f;
    Util::getStream(Path::basePath() + pid + Path::statPath(), stream);

    if (stream.is_open())
    {
        std::getline(stream, line);
        istringstream buffer(line);
        istream_iterator<string> beg(buffer), end;
        vector<string> values(beg, end);

        float utime = stof(ProcessParser::getProcUpTime(pid));
        float stime = stof(values[14]);
        float cutime = stof(values[15]);
        float cstime = stof(values[16]);
        float starttime = stof(values[21]);
        float uptime = ProcessParser::getSysUpTime();
        float freq = sysconf(_SC_CLK_TCK);
        float total_time = utime + stime + cutime + cstime;
        float seconds = uptime - (starttime/freq);
        result = 100.0*((total_time/freq)/seconds);
    }
    return to_string(result);
}

string ProcessParser::getProcUpTime(string pid)
{
    string line;
    ifstream stream;
    float result = 0.0f;
    Util::getStream(Path::basePath() + pid + Path::statPath(), stream);

    if (stream.is_open())
    {
        std::getline(stream, line);
        istringstream buffer(line);
        istream_iterator<string> beg(buffer), end;
        vector<string> values(beg, end);
        result = float(stof(values[13])/sysconf(_SC_CLK_TCK));
    }
    return to_string(result);
}

long int ProcessParser::getSysUpTime()
{
    ifstream stream;
    long int result = 0;
    Util::getStream(Path::basePath() + Path::upTimePath(), stream);

    if (stream.is_open())
    {
        std::getline(stream, line);
        istringstream buffer(line);
        istream_iterator<string> beg(buffer), end;
        vector<string> values(beg, end);
        result = stoi(values[0]);
    }
    return result;
}

string ProcessParser::getProcUser(string pid)
{
    ifstream stream;
    Util::getStream(Path::basePath() + pid + Path::statusPath(), stream);

    string line;
    string name = "Uid";
    string result = "";

    if (stream.is_open()) 
    {
        while (std::getline(stream, line))
        {
            if (line.compare(0, name.size(), name) == 0)
            {
                istringstream buffer(line);
                istream_iterator<string> beg(buffer), end;
                vector<string> values(beg, end);
                result = values[1];
                break;
            }
        }
    }

    ifstream stream2;
    Util::getStream("/etc/passwd", stream2);
    name = ("x:" + result);

    if (stream2.is_open())
    {
        while (std::getline(stream2, line))
        {
            if (line.find(name) != string::npos)
            {
                result = line.substr(0, line.find(":"));
            }
        }
    }
    return result;
}

int ProcessParser::getNumberOfCores()
{
    int result = 0;
    string line;
    ifstream stream;
    Util::getStream(Path::basePath() + "cpuinfo", stream);

    if (stream.is_open())
    {
        while (std::getline(stream, line))
        {
            istringstream buffer(line);
            istream_iterator<string> beg(buffer), end;
            vector<string> values(beg, end);
            result = stoi(values[3]);
        }
    }
    return result;
}

vector<string> ProcessParser::getSysCpuPercent(string coreNumber = "")
{
    string line;
    string name = "cpu" + coreNumber;
    ifstream stream;
    Util::getStream(Path::basePath() + Path::statPath(), stream);

    if (stream.is_open())
    {
        while (std::getline(stream, line))
        {
            if (line.compare(0, name.size(), name) == 0)
            {
                istringstream buffer(line);
                istream_iterator<string> beg(buf), end;
                vector<string> values(beg, end);
                return values;
            }
        }
    }
    return vector<string>();
}

string ProcessParser::PrintCpuStats(vector<string> values1, vector<string> values2)
{
    float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
    float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
    float totalTime = activeTime + idleTime;
    float result = 100.0*(activeTime / totalTime);
    return to_string(result);
}

float ProcessParser::getSysRamPercent()
{
    
}



string ProcessParser::getSysKernelVersion();