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

#ifndef  ____MACNOTIF___LOGGER___
#define  ____MACNOTIF___LOGGER___

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <initializer_list>
#include <chrono>
#include <ctime>
#include <mutex>

#include <time.h>

#include <anyexcept.hpp>

namespace macnotify{

const std::string MACNOTIF_DEF_LOG  =  "/tmp/macnotif.log";

enum  LOGLEVEL    { SYSEVENTS, ERRORS, WARNINGS, DEBUG };

enum  STDLOGDEF   { STD_TBUFF_SIZE = 128 };

using LogLevMap = std::map<std::string, LOGLEVEL>;

class Logger{
    private:
        mutable      std::ofstream   mainLogStr;
        mutable      std::mutex      logFileMtx;
        static const LogLevMap       logLevelLookup;
        uint32_t                     logLevel;

        explicit Logger(const std::string& fileName, uint32_t llevel);

    public:

        static LOGLEVEL logLevelFromString(const std::string& llevel)                noexcept;

        static Logger& getInst(const std::string& logFile = MACNOTIF_DEF_LOG,
                               uint32_t llevel            = ERRORS)                  noexcept;

        Logger(Logger const&)              = delete;             
        Logger(Logger&&)                   = delete;                  
        Logger& operator=(Logger const&)   = delete; 
        Logger& operator=(Logger &&)       = delete; 

        ~Logger(void);

        void         procLogger(uint32_t logLevel, const std::string& msg)      const anyexcept;
        void         procLogger(uint32_t logLevel, 
                                const std::initializer_list<std::string>& msg)  const anyexcept;
        static bool  isValid(const std::string& newLev)                               noexcept;
        bool         setLevel(const std::string& newLev)                              noexcept;
};

} // End Namespace

#endif