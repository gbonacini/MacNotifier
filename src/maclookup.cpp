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

#include <maclookup.hpp>

namespace  macnotify{

    using std::fstream;
    using std::stringstream;
    using std::string;
    using std::getline;
    using std::cerr;
    using std::endl;
    using std::regex;
    using std::ios_base;
    using std::uppercase;
    using std::hex;
    using std::setfill;
    using std::setw;

    void MacLookup::loadLookupDb(const MacString& dbfile) anyexcept{
        fstream   lookUpFile;
        lookUpFile.open(dbfile, ios_base::in);
        if(!lookUpFile.is_open())
            throw string("MacLookup::loadLookupDb : Can't open Lookup Db.");

        for(string line; getline(lookUpFile, line); ) {
            stringstream      lineStream(line);
            string            key,
                              value;

            getline(lineStream, key,   STD_SEPARATOR);
            getline(lineStream, value, STD_SEPARATOR);

            if(key.size() == 0 || value.size() == 0)
                 throw string("MacLookup::loadLookupDb : empty field detected.");

            if(key.size() != BIG_KEY_LEN    && 
               key.size() != MEDIUM_KEY_LEN && 
               key.size() != SMALL_KEY_LEN)
                 throw string("MacLookup::loadLookupDb : invalid key size.");

            if(value.size() > MAX_OWNER_NAME_LEN)
                 throw string("MacLookup::loadLookupDb : value too long.");

            try{
                lookUpMac.emplace(key, value);
            }catch(...){
                 throw string("MacLookup::loadLookupDb : can't update lookup map.");
            }
        }

        lookUpFile.close();
    }

    bool MacLookup::lookupMAcAddr(const MacString& mac, string& lookUp) noexcept{

        for(auto mLen: MAC_CAT_ARRAY){
            string Oui = mac.substr(0, mLen);
            if(lookUpMac.find(Oui) != lookUpMac.end()){
                 lookUp=lookUpMac[Oui];
                 return true;
            }
        }

        lookUp="NoLookup";
        return false;
    }

    void MacLookup::printMac(MacArray& aMac) noexcept{
         cerr  << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[0]) << ":" 
               << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[1]) << ":" 
               << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[2]) << ":" 
               << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[3]) << ":" 
               << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[4]) << ":" 
               << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[5]) << endl;
    }

    string MacLookup::printMacStr(MacArray& aMac) noexcept{
         try{
             stringstream macstream;
             macstream  << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[0]) << ":" 
                   << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[1]) << ":" 
                   << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[2]) << ":" 
                   << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[3]) << ":" 
                   << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[4]) << ":" 
                   << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[5]) << endl;
               return macstream.str();
         }catch(...){
               return string(ERR_MAC_CSV);
         }
    }

    string MacLookup::getMac(MacArray& aMac) noexcept{
         try{
              stringstream macstream;
              macstream  << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[0]) 
                         << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[1]) 
                         << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[2])
                         << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[3])
                         << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[4]) 
                         << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(aMac[5]);
               return macstream.str();
         }catch(...){
               return string(ERR_MAC_STRING);
         }
    }

    bool MacLookup::normalizeMacString(const MacString& orig, string&  dest)  noexcept{
          const regex  macAddrRegexp{MAC_REGEX};

          if(!regex_match(orig, macAddrRegexp))
              return false;

          dest.clear();

          for(auto ch : orig) 
              if(ch != ':')
                 dest.push_back(toupper(ch));

          return true; 
    }

} // End Namespace