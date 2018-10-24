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

#include <parseCmdLine.hpp>
#include <iostream>

using std::string;
using std::pair;

namespace parCmdLine{

    ParseCmdLine::ParseCmdLine(int argc, char **argv, const char* flags) 
         : errState{false}, unflaggedParams{false},
           argcRef{argc},   errString{""},
           unflaggedArgs{"noparams"}
    {
        tokenizeFlags(flags);
        parseArgs(argv, flags);
    }

    bool ParseCmdLine::isSet(char flag) const noexcept{
        if(flagsStatus.find(flag) != flagsStatus.end())
            if(flagsStatus.at(flag).isPresent)
                return true;
        
        return false;
    }

    bool ParseCmdLine::isLegal(char flag)  const noexcept{
        if(flagsStatus.find(flag) != flagsStatus.end())
                return true;

        return false;
    }

    bool ParseCmdLine::hasValue(char flag) const noexcept{
        if(flagsStatus.find(flag) != flagsStatus.end())
            if(flagsStatus.at(flag).hasValue)
                return true;

        return false;
    }

    bool  ParseCmdLine::hasUnflaggedPars(void) const noexcept{
        return unflaggedParams;
    }

    const string& ParseCmdLine::getUnflaggedPars(void) const noexcept{
        return unflaggedArgs;
    }

    const string& ParseCmdLine::getValue(char flag) const noexcept{
        if(flagsStatus.find(flag) != flagsStatus.end())
            if(flagsStatus.at(flag).hasValue)
                return flagsStatus.at(flag).value;

        return errString;
    }

    bool ParseCmdLine::setOn(char flag) noexcept{
        if(flagsStatus.find(flag) == flagsStatus.end())
            return false;

        if(flagsStatus[flag].isPresent)
            return false;

        flagsStatus[flag].isPresent         = true;
        return true;
    }

    bool ParseCmdLine::getErrorState() const noexcept{
        return errState;
    }

    bool ParseCmdLine::setErrorState(bool state) const noexcept{
        errState      =  state;
        return !state;
    }

    bool ParseCmdLine::parseArgs(char **argv, const char* flags) noexcept{
        char   c;

        while ((c = getopt (argcRef, argv, flags)) != -1){
            if(c != '?' && c > 0){
                if(flagsStatus.find(c) == flagsStatus.end())
                    return setErrorState(true);

                if(!setOn(c))
                    return setErrorState(true);

                if(flagsStatus[c].hasValue){
                    if(optarg == nullptr || optarg[0] == '-')
                        return setErrorState(true);

                    flagsStatus[c].value.insert(0, optarg);
                }
            }else if(c == '?' &&  c > 0){
                    return setErrorState(true);
            }else{
                break;
            }
        }

        if(argcRef > optind){
            unflaggedParams = true;
            unflaggedArgs   = argv[optind];
        }

        return true;
    }

    bool ParseCmdLine::tokenizeFlags(const char* flags) noexcept{
        int         pos                           =  0; 
        char        prevKey                       =  0; 

        while(flags[pos] != 0){
            if((flags[pos] >= 65 && flags[pos] <= 90) || ( flags[pos] >= 97 && flags[pos] <= 122)){
                prevKey                                  =  flags[pos];
                flagsStatus.insert(pair<char, ParseResult>(flags[pos], {false, false, ""}));
            }else if(flags[pos] == 58){
                if(prevKey == 0)
                    return setErrorState(true);

                flagsStatus[prevKey].hasValue     =  true;
            }else{
                return setErrorState(true);
            }
            pos++;
        }

        return true;
    }

} // End Namespace 
