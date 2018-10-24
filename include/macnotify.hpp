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

#ifndef _MACNOTIFIER____HPP_
#define _MACNOTIFIER____HPP_

// Nelink/Socket
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <tuple>
#include <array>
#include <string>
#include <algorithm> 
#include <regex>
#include <atomic>
#include <thread>

#include <logger.hpp>
#include <configFile.hpp>
#include <maclookup.hpp>

namespace  macnotify{

    using Sockaddr_Nl    =   struct  sockaddr_nl;
    using Sockaddr       =   struct  sockaddr;
    using Nlmsgndr       =   struct  nlmsghdr;
    using Ndmsg          =   struct  ndmsg;
    using Nlattr         =   struct  nlattr; 

    enum  MACNOTIFYEVENTSCONSTS  { LAST_VIEWED=0,      LAST_TRIGGER_EXE=1  };

    enum  MACNOTIFYGENERICCONSTS { DATE_BUFF_SIZE=64,  STD_PIPE_MSG_SIZE=1 };

    using PipeMessage    = std::array<char, STD_PIPE_MSG_SIZE>;
    const PipeMessage      pipeMsg = {'X'};

    using MacAddr        =   std::string;
    using LastView       =   time_t; 
    using LastTriggerExe =   time_t; 
    using MacEvents      =   std::map<MacAddr, std::tuple<LastView, LastTriggerExe> >;
    using Sigaction      =   struct sigaction;
    using Tm             =   struct tm;

    class MacNotify{
        public:
             MacNotify(std::string cfg);
             ~MacNotify(void)                                                 = default;

             int  init(void)                                                  noexcept;
             void loop(void)                                                  noexcept;
             void stopLoop(void)                                              noexcept;
             void restartLoop(void)                                           noexcept;
             void setDebug(bool onOff)                                        noexcept;
             bool setTimeout(long int tout)                                   noexcept;
             bool dumpEvents(void)                                      const noexcept;

        private:
             int                                  nlSock;
             bool                                 debug,
                                                  lookupCapability;
             Sockaddr_Nl                          group_addr;
             MacEvents                            macEvents;

             macnotifycfg::MacNotifyConfig        config;
             MacLookup                            macLookup;
             long int                             timeout;
             sigset_t                             sigsetBck;
             Sigaction                            sigactUsr1,
                                                  sigactInt;
             static std::atomic_bool              stop;
             std::unique_ptr<std::thread>         dumper;

             void updateLastViewTime(MacAddr mac)                             noexcept;
             void updateLastTriggerExeTim(MacAddr mac)                        noexcept;
             bool handleEvent(MacArray data)                                  noexcept;
             bool isTriggerable(MacAddr mac)                                  noexcept;
             bool execTriggers(const macnotifycfg::MacString condition) const noexcept;
    };

    class Dumper{
        private:
             static int                 pipefd[2];
             static MacNotify*          macNotify;

             Dumper(MacNotify* mn);

        public:
             ~Dumper(void);

             Dumper(Dumper const&)                      = delete;             
             Dumper(Dumper&&)                           = delete;                  
             Dumper& operator=(Dumper const&)           = delete; 
             Dumper& operator=(Dumper &&)               = delete; 

             static Dumper&  getInst(MacNotify* mn)     noexcept;
             static int      getWriteFd(void)           noexcept;
             static int      getReadFd(void)            noexcept;
             static void     dump(void)                 noexcept;

    };

} // End Namespace

#endif