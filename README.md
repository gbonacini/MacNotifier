Description:
============

Macnotifier is an utility permitting to start automatically programs or scripts associated to a specific MAC Addresses or company IDs (as it appear in the IEEE registers used to create the DB containing the MAC adresses blocks assignees), when they are notified to the system running this program. To efficientlt catch the incoming notification, a Linux kernel Netlink socket is used.  

* Possible uses: 

- Automatic execution of tasks when the computer running the program is present in specific locations (office, home, etc ), binding the MAC addresses of machine presents for sure in that locations ( a server, a NAS, etc);
- Automatic notifications of the presence of some specific machine in your network;
- Automatically test/pentest any specific MACs/Class of MACs as soos as they are recognized;

Some other utilities are provided ( MAC Lookup tool , some small scripts to convert the IEEE DBs, etc). 


DISCLAIMER:
===========

This code has been posted for information and educational purposes. The intrusion in systems and services without the owner's authorisation is illegal. The misuse of the information and the software in this repo can result in criminal charges brought against the perpetrator of the crime. Any actions and or activities related to the material contained within this Git Repository is solely your responsibility. The author will not be held responsible in the event any criminal charges be brought against any individuals misusing the information or the software in this website to break the law.

Prerequisites:
==============

The program is intended to be used in a Linux environment and it is tested on various Linux distributions:

- Ubuntu 17.10;

using, as compiler, one in this list:

- gcc version7.2.0 (Ubuntu 7.2.0-8ubuntu3.2);

Installation:
=============

- launch the configure script:
  ./configure
- Compile the program:
  make
- Install the program and the man page:
  sudo make install

Instructions:
=============

- a MAC addresses lookup file must be created from IEEE registers. Detailed instructions are present int the maclookup utility README (./utils directory);
- a LUA configuration file must be provided. An example is presento in ./test directory (macnotifier.lua):
<BR>
--[[ Flag:           dumpfile<BR>
     Type:           string<BR>
     Synopsis:       Name and path of MACs/Company dump file. Format:<BR>
                     mac_or_company_name;last_view_epoch;last_trigger_epoch;last_vied_date;last_trigger_date;<BR>
                     [default: "/tmp/macnotify.dump"]<BR>
     Valid values:   A valid file name with path<BR>
--]]<BR>
dumpfile = "/home/bg/macnotify.dump"<BR>
<BR>
--[[ Flag:           logfile<BR>
     Type:           string<BR>
     Synopsis:       Name and path of main program log file.<BR>
                     [default: "/tmp/macnotifier.log"]<BR>
     Valid values:   A valid file name with path<BR>
--]]<BR>
logfile = "/home/bg/macnotifier.log"<BR>
<BR>
--[[ Flag:           loglevel<BR>
     Type:           string<BR>
     Synopsis:       Max log level<BR>
                     -d flag temporary switch on/off the DEBUG entries, overriding this setting.<BR>
                     [default: "errors"]<BR>
     Valid values:   sysevents, errors, warnings, debug <BR>
--]]<BR>
loglevel = "debug"<BR>
<BR>
--[[ Flag:           MAC Addresses Lookup DB File<BR>
     Type:           string<BR>
     Synopsis:       Name and path of the csv file with the MAC Lookup DB<BR>
     Valid values:   A valid file name with path<BR>
--]]<BR>
lookupfile = "/home/bg/_Netlink_Mac/data/mac_db.csv"<BR>
<BR>
--[[ Flag:           timeout  <BR>
     Type:           unsigned integer<BR>
     Synopsis:       Minimum number of waiting seconds before re-execute<BR>
                     the triggers for  MAC address or a registered name<BR>
                     of a MAC address range. <BR>
                     - 0  ( zero ) means always re-execute;<BR>
                     - -1 (minus one) means once.<BR>
                     [default: 60]<BR>
     Valid values:   an unsigned integer, from 0<BR>
--]]<BR>
timeout = 120<BR>
<BR>
--[[ Flag:           triggers (by MAC Address)<BR>
     Type:           associative array string(key)/string(csv value) <BR>
     Synopsis:       List of MAC addresses -> action triggered.<BR>
                     The key is MAC addres in string format (two HEX digits for field, ':' as separator ):<BR>
                         0C:FF:01:31:AC:01<BR>
                     The value is a csv string, the first field is the triggered program, <BR>
                     the other fields (optionals) are the executable's parameters.<BR>
                     "exe_or_script:par1,par2,...,parn"<BR>
                     Example:<BR>
                     "printmac:-d"<BR>

     Valid values:   Zero, one or more pairs string/csv string, as described.<BR>
--]]<BR>
triggersByMac = {["005056C00008"] = "/home/bg/_Netlink_Mac/test/printMac.sh;&;",<BR>
                 ["005056F6D10D"] = "/home/bg/_Netlink_Mac/test/printMac.sh;&;"  }<BR>
<BR>
--[[ Flag:           triggers (by Company)<BR>
     Type:           associative array string(key)/string(csv value) <BR>
     Synopsis:       List companies -> action triggered.<BR>
                     The key is a company name in string format (ended with ':' as separator ):<BR>
                         Apple             <BR>
                     The value is a csv string, the first field is the triggered program, <BR>
                     the other fields (optionals) are the executable's parameters.<BR>
                     "exe_or_script:par1,par2,...,parn"<BR>
                     Example:<BR>
                     "printmac:-d"<BR>

     Valid values:   Zero, one or more pairs string/csv string, as described.<BR>
--]]<BR>
triggersByCompany = {["VMware"] = "/home/bg/_Netlink_Mac/test/printCompany.sh;&;"}<BR>

Documentation:
==============

Man pages and examples are present in the distribution.
