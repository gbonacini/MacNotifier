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

#include <macnotify.hpp>

namespace  macnotify{

    using std::cerr;
    using std::endl;
    using std::stringstream;
    using std::to_string;
    using std::string;
    using std::strtoul;
    using std::copy;
    using std::fill;
    using std::regex_match;
    using std::get;
    using std::make_tuple;
    using std::atomic_bool;
    using std::fstream;
    using std::ios;
    using std::array;
    using std::thread;
    using std::make_unique;

    using macnotifycfg::MacString;

    int         Dumper::pipefd[2]  = {-1, -1};
    MacNotify*  Dumper::macNotify  = nullptr;

    Dumper::Dumper(MacNotify* mn){
        Dumper::macNotify = mn;
        if (pipe(pipefd) < 0)
            throw string("Error opening pipe");
    }
    
    Dumper::~Dumper(void){
        close(pipefd[0]);
        close(pipefd[1]);
    }

    Dumper&  Dumper::getInst(MacNotify* mn) noexcept{
        static Dumper pipeInst(mn);

        return pipeInst;
    }

    void Dumper::dump(void) noexcept{
         macNotify->dumpEvents();
    }

    int Dumper::getReadFd(void) noexcept{
         return pipefd[0];
    }

    int Dumper::getWriteFd(void) noexcept{
         return pipefd[1];
    }

    atomic_bool MacNotify::stop(false);
    
    MacNotify::MacNotify(string cfg)
        : nlSock{-1}, debug{false}, lookupCapability{false}, 
          group_addr{}, config{cfg}, dumper{nullptr}
    {
        static_cast<void>(Dumper::getInst(this));

        dumper             =  make_unique<thread>( [](){ 
                                    PipeMessage buff;
                                    while(read(Dumper::getReadFd(), reinterpret_cast<void*>(buff.data()), buff.size())){
                                           Dumper::dump();
                                    }
                              });
        dumper->detach();
        
        group_addr.nl_family = AF_NETLINK;
        group_addr.nl_pid    = getpid();
        group_addr.nl_groups = RTMGRP_NEIGH;

        static_cast<void>(sigfillset(&sigactInt.sa_mask));
        static_cast<void>(sigfillset(&sigactUsr1.sa_mask));
        sigdelset(&sigactUsr1.sa_mask,  SIGUSR1);
        sigdelset(&sigactInt.sa_mask,   SIGINT);
        if(sigprocmask(0, nullptr, &sigsetBck) != 0)
            throw string("MacNotify: Error getting the signal mask.");
        sigactUsr1.sa_flags          = 0;
        sigactUsr1.sa_handler        = [](int){ write(Dumper::getWriteFd(), reinterpret_cast<const void*>(pipeMsg.data()), pipeMsg.size());};

        sigactInt.sa_flags           = 0;
        sigactInt.sa_handler         = [](int){ MacNotify::stop = true; };

        if(sigaction(SIGINT, &sigactInt, nullptr) != 0)
            throw string("MacNotify: setting SIGUSR1 hdlr.");

        if(sigaction(SIGUSR1, &sigactUsr1, nullptr) != 0)
            throw string("MacNotify: setting SIGINT hdlr.");

        try{
           config.loadConfig();
           cerr << "Logfile: " << config.getLogFile() << " LogLevel: " << config.getLogLevel() << endl;
           Logger::getInst(config.getLogFile(), config.getLogLevel());
           timeout = config.getTimeout();
           macLookup.loadLookupDb(config.getLookupDbFile());
        }catch(string& err){
           cerr << "Error loading conf: " << err << endl;
           exit(1);
        }
    }

    int MacNotify::init(void) noexcept{
        int                err              =  -1;
        constexpr size_t   group_addr_len   = sizeof(group_addr);
        nlSock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        if(nlSock >= 0) {
            if(bind(nlSock, reinterpret_cast<Sockaddr *>(&group_addr), group_addr_len) < 0) {
                Logger::getInst().procLogger(ERRORS, "Error: binding netlink socket");
                return err;
            }
        }else{
              Logger::getInst().procLogger(ERRORS, "Error: opening netlink socket"); 
              return err;
        }
    
        return nlSock;
    }

    void MacNotify::updateLastViewTime(MacAddr mac)  noexcept{
         auto elem = macEvents.find(mac);
         if(elem != macEvents.end())
             get<LAST_VIEWED>(elem->second) = time(nullptr);
    }

    void MacNotify::updateLastTriggerExeTim(MacAddr mac) noexcept{
         auto elem = macEvents.find(mac);
         if(elem != macEvents.end())
             get<LAST_TRIGGER_EXE>(elem->second) = time(nullptr);
    }

    bool MacNotify::setTimeout(long int tout) noexcept{
         if(tout < -1)
            return false;

         timeout  =  tout;
         return true;
    }

    bool  MacNotify::execTriggers(const MacString condition) const noexcept{
        try{
            if(debug) Logger::getInst().procLogger(DEBUG, {"execTriggers: cond: ", condition});
            if(config.isTriggered(condition)){
                 if(debug) Logger::getInst().procLogger(DEBUG, "execTriggers : found trigger");
                 string cmd;
                 for(auto trg : config.getTriggers(condition)){
                      cmd.clear();
                      cmd.append(trg.at(0)).append(" ").append(trg.at(1)).append(" ");
                      if(trg.size() >= 3)
                          cmd.append(trg.at(2));
                      if(debug) Logger::getInst().procLogger(DEBUG, {"execTriggers : Executing: ", cmd});
                      int ret = system(cmd.c_str());
                      if( ret != 0){
                          Logger::getInst().procLogger(ERRORS, {"Error Executing Trigger: ", to_string(ret)});
                          return false;
                      } 
                 }
            }

            return true;

        }catch(...){
           cerr << "MacNotifyConfig::execTriggers error processing triggers" << endl;
        }

        return false;
    }

    bool MacNotify::handleEvent(MacArray data) noexcept{
         bool   ret             =  true;

         try{
             string mac             =  MacLookup::getMac(data),
                    lookupString;

             if(macEvents.find(mac) != macEvents.end()){
                 updateLastViewTime(mac);
                 if(isTriggerable(mac)){
                     updateLastTriggerExeTim(mac);
                     execTriggers(mac);
                     Logger::getInst().procLogger(DEBUG, {"Exec MAC :", mac});   
                 }else{
                     if(debug) Logger::getInst().procLogger(DEBUG, {"No Exec MAC :", mac});   
                 }
             }else{
                 time_t now = time(nullptr);
                 macEvents.emplace(mac, make_tuple(now, now));    
                 execTriggers(mac);

                 if(debug) 
                     Logger::getInst().procLogger(DEBUG, {"Added and Exec MAC :", mac});   
             }

             if(macLookup.lookupMAcAddr(mac, lookupString)){
                 if( macEvents.find(lookupString) != macEvents.end()){
                     updateLastViewTime(lookupString);
                     if(isTriggerable(lookupString)){
                         execTriggers(lookupString);
                         Logger::getInst().procLogger(DEBUG, {"Lookup Exec String :", lookupString});   
                     }else{
                         if(debug) Logger::getInst().procLogger(DEBUG, {"No Exec Lookup MAC :", lookupString});   
                     }
                 }else{
                     time_t now = time(nullptr);
                     macEvents.emplace(lookupString, make_tuple(now, now));    
                     execTriggers(lookupString);

                     if(debug) 
                         Logger::getInst().procLogger(DEBUG, {"Added And Exec Lookup MAC :", lookupString});   
                 }
             }
         }catch(...){
             ret = false;
         }
         return ret;
    }

    bool MacNotify::dumpEvents(void) const noexcept{
        bool    ret         =  true;
        fstream dumpFile;

        try{
            dumpFile.open(config.getDumpFile(), ios::out);
            for(auto elem : macEvents){
                static array<char, DATE_BUFF_SIZE> buff,
                                                   buff2;
                fill(buff.begin(),  buff.end(),  0);
                fill(buff2.begin(), buff2.end(), 0);
                Tm      tmbuff,
                        tmbuff2;
                strftime(buff.data(), buff.size(), "%Y-%m-%d:%H:%M:%S", localtime_r(&(get<LAST_VIEWED>(elem.second)), &tmbuff));
                strftime(buff2.data(), buff2.size(), "%Y-%m-%d:%H:%M:%S", localtime_r(&(get<LAST_TRIGGER_EXE>(elem.second)), &tmbuff2));
                dumpFile << elem.first << ";" <<  get<LAST_VIEWED>(elem.second)
                                       << ";" <<  get<LAST_TRIGGER_EXE>(elem.second)
                                       << ";" <<  reinterpret_cast<const char*>(buff.data())
                                       << ";" <<  reinterpret_cast<const char*>(buff2.data())
                                       << ";" <<  endl;
            }
        }catch(...){
            ret = false;
        }

        dumpFile.close();

        return ret;
    }

    bool MacNotify::isTriggerable(MacAddr mac) noexcept{
         if(timeout == 0)
                 return true;

         auto    elem = macEvents.find(mac);

         if(timeout == -1) 
                 return elem == macEvents.end() ? true : false;

         time_t  now  = time(nullptr),
                 last = get<LAST_TRIGGER_EXE>(elem->second);

         if(elem != macEvents.end()){
             if((now - last) >= timeout)
                 return true;
             else
                 return false;
         }

         return false;
    }

    void MacNotify::stopLoop(void)  noexcept{
        MacNotify::stop = true;
    }

    void MacNotify::restartLoop(void) noexcept{
        MacNotify::stop = false;
    }

    void MacNotify::loop(void)  noexcept{
        ssize_t             received     =  0;
    
        MacArray            buff,
                            data;
    
        Nlmsgndr            *hdr         =  reinterpret_cast<Nlmsgndr*>(buff.data());
        Ndmsg               *nd          =  reinterpret_cast<Ndmsg*>(hdr + 1);
        Nlattr              *attr        =  nullptr;
    
        while(!stop){
            received = recv(nlSock, buff.data(), buff.size(), 0);

            if(debug) Logger::getInst().procLogger(DEBUG, {"Received: ", to_string(received)});
    
	        switch (hdr->nlmsg_type) {
		    case RTM_NEWNEIGH:
                if(debug) Logger::getInst().procLogger(DEBUG, "Message type: RTM_NEWNEIGH");
			    attr = reinterpret_cast<Nlattr*>(nd + 1);
			    while ((reinterpret_cast<uint8_t*>(attr) - buff.data()) < hdr->nlmsg_len) {
                    if(debug) Logger::getInst().procLogger(DEBUG, "Parsing resp");
    
                    copy(reinterpret_cast<uint8_t *>(attr) + NLA_HDRLEN, 
                         reinterpret_cast<uint8_t *>(attr) + NLA_HDRLEN + STD_MAC_SIZE,
                         data.data());
     
				    if (attr->nla_type == NDA_LLADDR && nd->ndm_state == NUD_REACHABLE) {
                        if(debug) Logger::getInst().procLogger(DEBUG, MacLookup::printMacStr(data));
                        handleEvent(data);
				    }
				    attr = reinterpret_cast<Nlattr*>(reinterpret_cast<uint8_t*>(attr) + NLA_ALIGN(attr->nla_len));
			    }
		    break;

		    case RTM_DELNEIGH:
		    case RTM_GETNEIGH:
                if(debug) Logger::getInst().procLogger(DEBUG, "Message type: RTM_DELNEIGH/RTM_GETNEIGH");
		    break;
		    default:
                Logger::getInst().procLogger(ERRORS, {"Unknown message type:", to_string(hdr->nlmsg_type)});
	        }
        }
    }

    void MacNotify::setDebug(bool onOff) noexcept{
         debug = onOff;
    }

} //End Namespace