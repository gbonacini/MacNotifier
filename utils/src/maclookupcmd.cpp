// -----------------------------------------------------------------
// maclookup - an utility to obtain the company name associated to a
//             given MAC address.
// Copyright (C) 2018  Gabriele Bonacini
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
// -----------------------------------------------------------------

#include <maclookupcmd.hpp>

using namespace macnotify;
using namespace parCmdLine;
using std::string;
using std::cerr;
using std::cout;
using std::cin;
using std::getline;
using std::endl;

const std::string STD_FILL_DEVICE           =  "/dev/null";

int main(int argc, char** argv){
    const char         flags[] = "c:il:hV";
    int                ret     = 0;

    ParseCmdLine pcl(argc, argv, flags);
    if(pcl.getErrorState())
         paramError(argv[0], "Invalid Command Line.");

    if(pcl.isSet('V')){
       if(argc != 2) paramError(argv[0], "-V parameter must be present alone.");
       versionInfo();
    }

    if(pcl.isSet('h')){
       if(argc != 2) 
            paramError(argv[0], "-h parameter must be present alone.");
       else 
            paramError(argv[0], nullptr);
    }

    if(pcl.isSet('i') && pcl.isSet('l'))
            paramError(argv[0], "-i and -l are mutually exclusive.");

    if(!pcl.isSet('i') && !pcl.isSet('l'))
            paramError(argv[0], "one of -i or -l must be specified.");

    if(!pcl.isSet('c'))
        paramError(argv[0], "You must specify -c with the csv lookup file path.");

    string cfg = pcl.getValue('c');

    MacLookup  macLookup;
    macLookup.loadLookupDb(cfg);
    string     buff,
               resp;

    if(pcl.isSet('l')){
            if(!MacLookup::normalizeMacString(pcl.getValue('l'), buff)){
                cerr << "Invalid MAC Address format" << endl;
            }else{
                if(macLookup.lookupMAcAddr(buff, resp)){
                    cout << resp << endl;
                }else{
                    cerr << "Not Available" << endl;
                    ret  =  1;
                }
            }
    }

    if(pcl.isSet('i')){
        for(string line; getline(cin, line); ) {
            if(line.compare("end") == 0)
               break;

            if(!MacLookup::normalizeMacString(line, buff)){
                cerr << "Invalid MAC Address format" << endl;
                continue;
            }
            
            if(macLookup.lookupMAcAddr(buff, resp))
                cout << resp << endl;
            else
                cout << "Not Available" << endl;
        }
    }

    return ret;
}

void paramError(const char* progname, const char* err) noexcept{

   if(err != nullptr) cerr << err << endl << endl;

   cerr << progname << " - try to enumerate users on a openssh server using a dictionary. "
                    << " GBonacini - (C) 2018   "                                      << endl
        << "Syntax: "                                                                  << endl
        << "       "  << progname << " [ -c lookup_csv_db ] [-i] | [-l mac_addr ]"     << endl 
        << "       "              << " | [-h] | [-V]  "                                << endl 
        << "       "  << "-c MAC addresses csv lookup db with path."                   << endl 
        << "       "  << "-i Interactive mode."                                        << endl 
        << "       "  << "-l cmd line mode: it resolves the specified mac and exits."  << endl                                   << endl 
        << "       "  << "-h print this help message."                                 << endl 
        << "       "  << "-V version information."                                     << endl;

   exit(1);
}

void versionInfo(void) noexcept{
   cerr << PACKAGE << " version: " VERSION << endl;
   exit(1);
}
