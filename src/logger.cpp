// -----------------------------------------------------------------
// macnotifier - an utility to launch automatically programs or 
//               scripts associated to a specific MAC Address
//               or company id when it is advertised.
// Copyright (C) 2018  Gabriele Bonacini
//
// This program is free software for no profit use; you can redistribute 
// it and/or modify it under the terms of the GNU General Public License 
// as published by the Free Software Foundation; either version 2 of 
// the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
// A commercial license is also available for a lucrative use.
// -----------------------------------------------------------------

#include<logger.hpp>

using std::string;
using std::time_t;
using std::cerr;
using std::endl;
using std::fstream;
using std::initializer_list;
using std::chrono::system_clock;
using std::strftime;
using std::localtime;

namespace macnotify{

Logger& Logger::getInst(const string& logFile,  uint32_t llevel)  noexcept{
    static Logger logInst(logFile, llevel);

    return logInst;
}

const LogLevMap Logger::logLevelLookup = {{"sysevents", SYSEVENTS}, {"errors", ERRORS}, {"warnings", WARNINGS}, {"debug", DEBUG}};

Logger::Logger(const string&  fileName, uint32_t llevel)
    : logLevel{llevel}
{
     mainLogStr.open(fileName, fstream::out | fstream::app);
     procLogger(DEBUG, string("LogFile: ").append(fileName));
}

Logger::~Logger(void){
   mainLogStr.close();
}

bool Logger::isValid(const string& newLev) noexcept{
    if(logLevelLookup.find(newLev) != logLevelLookup.end())
        return true;
    return false;
}

LOGLEVEL Logger::logLevelFromString(const string& llevel) noexcept{
    auto lev  =  Logger::logLevelLookup.find(llevel);
    if(lev != logLevelLookup.end())
         return lev->second;
    return ERRORS;
}

bool Logger::setLevel(const string& newLev)  noexcept {
    if(isValid(newLev)){
        logLevel = static_cast<uint32_t>(Logger::logLevelLookup.find(newLev)->second);
        return true;
    }
    return false;
}

void Logger::procLogger(uint32_t logLev, const string& msg) const anyexcept{
    system_clock::time_point evTime = system_clock::now();
    time_t                   evTt   = system_clock::to_time_t(evTime);
    char                     timeBuff[STD_TBUFF_SIZE];

    strftime(timeBuff, sizeof(timeBuff), "%A %c", localtime(&evTt));

    logFileMtx.lock();

    if(logLev <= logLevel){
        if(mainLogStr.is_open())
           mainLogStr << "[MACNotif : " << timeBuff << "]  " << msg << endl;
        else
           cerr       << "[MACNotif : " << timeBuff << "]  " << msg << endl;
    }

    logFileMtx.unlock();
}

void Logger::procLogger(uint32_t logLev, const initializer_list<string>& msg) const anyexcept{
    system_clock::time_point evTime = system_clock::now();
    time_t                   evTt   = system_clock::to_time_t(evTime);
    char                     timeBuff[STD_TBUFF_SIZE];

    strftime(timeBuff, sizeof(timeBuff), "%A %c", localtime(&evTt));

    logFileMtx.lock();

    if(logLev <= logLevel){
        if(mainLogStr.is_open()){
            mainLogStr  << "[MACNotif : " << timeBuff << "]  ";
            for(auto el : msg)
                mainLogStr << el;
            mainLogStr  << endl;
        }else{
            cerr        << "[MACNotif : " << timeBuff << "]  ";
            for(auto el : msg)
                cerr << el;
            cerr  << endl;
        }
    }

    logFileMtx.unlock();
}

} // End Namespace
