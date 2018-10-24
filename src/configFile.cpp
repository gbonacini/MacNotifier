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

#include <configFile.hpp>

namespace macnotifycfg {

    using std::string;
    using std::vector;
    using std::cerr;
    using std::endl;
    using std::pair;
    using std::stringstream;
    using std::getline;

    using macnotify::LOGLEVEL;
    using macnotify::Logger;

    MacNotifyConfig::MacNotifyConfig(string cfg)
                      :     luaState{nullptr},
                            configFile{cfg},
                            logFile{"/tmp/macnotifier.log"},
                            lookupfile{""},
                            dumpfile{"/tmp/macnotify.dump"},
                            timeout{60},
                            triggersLoader{[&](string& key, string& val){
                                 vector<string>  buff;
                                 stringstream    sstream(val);
                                 Stat            fstat;

                                 cerr << "triggersLoader: key: " << key << " value: " << val << endl;

                                 string act;
                                 getline(sstream, act, ';');

                                 if(act.empty())
                                     throw string("Invalid action in trigger's list.");

                                 cerr << "Map Loader Element action: [" << act << "]" << endl;

                                 if(stat(act.c_str(), &fstat) != 0)
                                     throw string("Missing Executable/Script:").append(act);

                                 if((fstat.st_mode & S_IFMT ) != S_IFREG)
                                     throw string("Invalid Executable/Script File Type");

                                 buff.push_back(act);
                                 buff.push_back(key);

                                 string param;
                                 while(getline(sstream, param, ';')){
                                     cerr <<  "Map Loader Element param: " << param << endl;
                                     buff.push_back(param);
                                 }

                                 if(triggersMap.find(key) == triggersMap.end())
                                     triggersMap.insert(pair<string, vector<vector<string>>>(key, {buff}));
                                 else
                                     triggersMap[key].push_back(buff);
                             }} 
    {
        luaState = luaL_newstate();
        if(luaState == nullptr)
            throw string("MacNotifyConfig::MacNotifyConfig - lua parser");

        luaL_openlibs(luaState);
    }

    void MacNotifyConfig::cleanConfig(void) noexcept{
         lua_close(luaState);
         luaState         =  nullptr;
    }

    const string&  MacNotifyConfig::getLogFile(void) const noexcept{
         return  logFile;
    }

    const string&  MacNotifyConfig::getDumpFile(void) const noexcept{
         return  dumpfile;
    }

    LOGLEVEL  MacNotifyConfig::getLogLevel(void) const noexcept{
         return  logLevel;
    }

    long int  MacNotifyConfig::getTimeout(void) const noexcept{
         return timeout;
    }

    const string&  MacNotifyConfig::getLookupDbFile(void) const noexcept{
         return  lookupfile;
    }

    const  AssociatedTriggers&  MacNotifyConfig::getTriggers(const MacString mac)  const noexcept{
         return triggersMap.find(mac)->second;
    }

    bool  MacNotifyConfig::isTriggered(const MacString condition)  const noexcept{
         return triggersMap.find(condition) == triggersMap.end() ? false : true;
    }

    MacNotifyConfig::~MacNotifyConfig(void){
          if(luaState != nullptr) cleanConfig();
    }    

    void MacNotifyConfig::loadConfig(void) anyexcept{

        if(luaL_loadfile(luaState, configFile.c_str()) != 0)
            throw string("invalid file.");
    
        if(lua_pcall(luaState, 0, 0, 0) != 0)
            throw string("syntax error.");

        lua_getglobal(luaState, "dumpfile");  
        if(lua_isnil(luaState, -1) == 1 ){
            cerr << "Invalid dump file: default will be used" << endl; 
        }else{
            if(lua_isstring(luaState, -1) == 0)
                throw string("'dumpfile' flag: invalid type.");
            dumpfile  =   lua_tostring(luaState, -1);
        }
        lua_pop(luaState, 1);

        lua_getglobal(luaState, "logfile");  
        if(lua_isnil(luaState, -1) == 1 ){
            cerr << "Invalid log file: default will be used" << endl; 
        }else{
            if(lua_isstring(luaState, -1) == 0)
                throw string("'logfile' flag: invalid type.");
            logFile  =   lua_tostring(luaState, -1);
        }
        lua_pop(luaState, 1);

        lua_getglobal(luaState, "loglevel");  
        if(lua_isnil(luaState, -1) == 1 ){
            cerr << "Invalid log file: default will be used" << endl; 
            logLevel  =  LOGLEVEL::ERRORS;
        }else{
            if(lua_isstring(luaState, -1) == 0)
                throw string("'loglevel' flag: invalid type.");
            logLevel  =   Logger::logLevelFromString(lua_tostring(luaState, -1));
        }
        lua_pop(luaState, 1);

        lua_getglobal(luaState, "lookupfile");  
        if(lua_isnil(luaState, -1) == 1 ){ 
            throw string("'lookupfile' flag: not present.");
        }else{
            if(lua_isstring(luaState, -1) == 0)
                throw string("'lookupfile' flag: invalid type.");
            lookupfile  =   lua_tostring(luaState, -1);
        }
        Stat   fstat;
        if(stat(lookupfile.c_str(), &fstat) != 0)
             throw string("Missing Lookup File: ").append(lookupfile);
        if((fstat.st_mode & S_IFMT ) != S_IFREG)
             throw string("Invalid Lookup File.");

        lua_pop(luaState, 1);

        lua_getglobal(luaState, "timeout");  
        if(lua_isnil(luaState, -1) == 0 ){
            long int buff         = -2;
            int      indicator;
            
            buff  =  lua_tointegerx(luaState, -1, &indicator);
            if(indicator == 0)
                throw string("'timeout' flag: invalid value.");
            if(buff < -1)
                throw string("'timeout' flag: invalid negative value (only -1 is valid)");

            timeout = buff;
        }
        lua_pop(luaState, 1);

        loadLuaGenericAssociativeTable("triggersByMac",     triggersLoader);
        loadLuaGenericAssociativeTable("triggersByCompany", triggersLoader);

        cerr <<  "Configuration loaded." << endl;
    }

    void  MacNotifyConfig::loadLuaGenericAssociativeTable(const SectionName section,
                                                          MapLoader clbk)               anyexcept{
        size_t len;
        string key,
               value;

        lua_getglobal(luaState, section.c_str());  
        lua_pushnil(luaState);  

        while (lua_next(luaState, -2) != 0){
            key    =  lua_tolstring(luaState, -2, &len);
            value  =  lua_tolstring(luaState, -1, &len);

            cerr << "LUA loader : key: " << key << " val: " << value << endl;
    
            clbk(key, value);
            lua_pop(luaState, 1);
        }
    }

} // End Namespace
