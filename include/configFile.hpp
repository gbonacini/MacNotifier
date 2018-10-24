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

#ifndef _CONFIG_MAC_NOTIFIER___HPP
#define _CONFIG_MAC_NOTIFIER___HPP

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include <functional>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <utility>

#include <anyexcept.hpp>
#include <logger.hpp>

namespace macnotifycfg {

    using GenericKey          =  std::string;
    using GenericValue        =  std::string;
    using MapLoader           =  std::function<void(GenericKey&, GenericValue&)>;
    using MacString           =  std::string;
    using AssociatedTriggers  =  std::vector<std::vector<std::string>>;
    using TriggersMap         =  std::map<MacString, AssociatedTriggers>;
    using SectionName         =  std::string;
    using Stat                =  struct stat;

    class MacNotifyConfig{
        public:
           MacNotifyConfig(std::string cfg = "/etc/macnotifier.lua");
           ~MacNotifyConfig(void);
    
           void                        loadConfig(void)                                            anyexcept;
	       const std::string&          getLogFile(void)                                  const     noexcept;
	       const std::string&          getDumpFile(void)                                 const     noexcept;
	       macnotify::LOGLEVEL         getLogLevel(void)                                 const     noexcept;
	       time_t                      getTimeout(void)                                  const     noexcept;
	       const std::string&          getLookupDbFile(void)                             const     noexcept;
	       bool                        isTriggered(const MacString mac)                  const     noexcept;
	       const AssociatedTriggers&   getTriggers(const MacString mac)                  const     noexcept;

        private:
           lua_State                   *luaState;
           std::string                 configFile,
                                       logFile,
                                       lookupfile,
                                       dumpfile;
           long int                    timeout;
           MapLoader                   triggersLoader;
           TriggersMap                 triggersMap; 
           macnotify::LOGLEVEL         logLevel;

           void                        loadLuaGenericAssociativeTable(const SectionName section,
                                                                      MapLoader clbk)              anyexcept;

           void                        cleanConfig(void)                                           noexcept;
    };

} // End Namespace

#endif