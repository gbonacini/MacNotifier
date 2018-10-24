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

#ifndef _MACLOOKUP___HPP_
#define _MACLOOKUP___HPP_

#include <cstdint>
#include <ctype.h>
#include <string>
#include <array>
#include <map>
#include <iostream>
#include <fstream>
#include <regex>
#include <iomanip>

#include <anyexcept.hpp>

namespace  macnotify{

using MacPrefix    =  std::string;
using CompanyName  =  std::string;
using LookUpMap    =  std::map<MacPrefix, CompanyName>; 
using MacString    =  std::string;

enum MACLOOKUPCONST { STD_MAC_SIZE        = 6,
                      STD_BUFF_SIZE       = 4096,
                      SMALL_KEY_LEN       = 9,
                      MEDIUM_KEY_LEN      = 7,
                      BIG_KEY_LEN         = 6,
                      MAC_CATEGORIES      = 3,
                      MAX_OWNER_NAME_LEN  = 512 };

const std::array<size_t, MAC_CATEGORIES> MAC_CAT_ARRAY = {BIG_KEY_LEN, MEDIUM_KEY_LEN, SMALL_KEY_LEN};

const char            STD_SEPARATOR       =  ';';
const char * const    ERR_MAC_CSV         =  "00:00:00:00:00:00";
const char * const    ERR_MAC_STRING      =  "000000000000";
const std::string     MAC_REGEX           =  "^[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]:"
                                             "[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]:[0-9A-Fa-f][0-9A-Fa-f]$";

using MacArray=std::array<uint8_t, STD_BUFF_SIZE>;

class MacLookup{
    private:
         LookUpMap          lookUpMac;

    public:
         MacLookup(void)                                                      = default;
         ~MacLookup(void)                                                     = default;

                void        loadLookupDb(     const MacString&  dbfile)       anyexcept;
                bool        lookupMAcAddr(    const MacString&  mac,
                                              std::string&      lookUp)       noexcept;

         static bool         normalizeMacString(const MacString&  orig, 
                                                std::string&      dest)       noexcept;

         static void         printMac(          MacArray&         aMac)       noexcept;
         static std::string  printMacStr(       MacArray&         aMac)       noexcept;

         static std::string  getMac(MacArray& aMac)                           noexcept;
};

} // End Namespace 

#endif