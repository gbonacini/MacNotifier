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

#include <macnotifier.hpp>
#include <macnotify.hpp>
#include <config.h> 

using namespace macnotify;
using namespace parCmdLine;
using std::array;
using std::string;
using std::cerr;
using std::endl;

const string STD_FILL_DEVICE    =  "/dev/null";

int main(int argc, char** argv){
    const char         flags[]       =  "c:dDhV";
    bool               debug         =  true;

    ParseCmdLine pcl(argc, argv, flags);
    if(pcl.getErrorState())
         paramError(argv[0], "Invalid Command Line.");

    if(pcl.isSet('V')){
       if(argc != 2) paramError(argv[0], "-V parameter must be present alone.");
       versionInfo();
    }

    if(pcl.isSet('d'))
        debug = false;

    if(pcl.isSet('h')){
       if(argc != 2) 
            paramError(argv[0], "-h parameter must be present alone.");
       else 
            paramError(argv[0], nullptr);
    }

    if(!pcl.isSet('c'))
        paramError(argv[0], "You must specify -c with the lua config file path.");

    string cfg = pcl.getValue('c');

    if(pcl.isSet('D'))
        daemonize();

    MacNotify macntf(cfg);

    macntf.init();
    macntf.setDebug(debug);
    macntf.loop();

    return 0;
}

void paramError(const char* progname, const char* err) noexcept{

   if(err != nullptr) cerr << err << endl << endl;

   cerr << progname   << " - try to enumerate users on a openssh server using a dictionary. "
                      << " GBonacini - (C) 2018   "                        << endl
        << "Syntax: "                                                      << endl
        << "       "  << progname << " [ -c lua_config_file ] [-D] [ -d] " << endl 
        << "       "              << " | [-h] | [-V]  "                    << endl 
        << "       "  << "-c Lua configuration file with path."            << endl 
        << "       "  << "-d debug mode."                                  << endl 
        << "       "  << "-D Start the program in daemon mode."            << endl 
        << "       "  << "-h print this help message."                     << endl 
        << "       "  << "-V version information."                         << endl;

   exit(1);
}

void versionInfo(void) noexcept{
   cerr << PACKAGE << " version: " VERSION << endl;
   exit(1);
}

int daemonize(void){
    int fdMax, 
        thisFd;

    switch(fork()){
        case -1:
            return -1;
        break;
        case 0:
                                  // Child: No action
        break;
        default:
            _exit(EXIT_SUCCESS);
    }

    if(setsid() == -1) return -1; // Session Leader

    switch(fork()){               // To be sure that the daemon isn't sessio leader
        case -1:
            return -1;
        break;
        case 0:
                                  // Child: No action
        break;
        default:
            _exit(EXIT_SUCCESS);
    }

    umask(0);                     // Clear umask
    if(chdir("/") != 0) throw string("chdir fails");

    fdMax=sysconf(_SC_OPEN_MAX);

    if(fdMax == -1) fdMax  =  8192;
        
    for(thisFd=0; thisFd<fdMax; thisFd++)
        close(fdMax);

    thisFd    =    open(STD_FILL_DEVICE.c_str(), O_RDWR);
    if(thisFd != STDIN_FILENO                 || 
           dup2(STDIN_FILENO, STDOUT_FILENO)  || 
           dup2(STDIN_FILENO, STDERR_FILENO)) 
       return -1;

    return 0;
}
