--[[ Flag:           dumpfile
     Type:           string
     Synopsis:       Name and path of MACs/Company dump file. Format:
                     mac_or_company_name;last_view_epoch;last_trigger_epoch;last_vied_date;last_trigger_date;
                     [default: "/tmp/macnotify.dump"]
     Valid values:   A valid file name with path
--]]
dumpfile = "/home/bg/macnotify.dump"

--[[ Flag:           logfile
     Type:           string
     Synopsis:       Name and path of main program log file.
                     [default: "/tmp/macnotifier.log"]
     Valid values:   A valid file name with path
--]]
logfile = "/home/bg/macnotifier.log"

--[[ Flag:           loglevel
     Type:           string
     Synopsis:       Max log level
                     -d flag temporary switch on/off the DEBUG entries, overriding this setting.
                     [default: "errors"]
     Valid values:   sysevents, errors, warnings, debug 
--]]
loglevel = "debug"

--[[ Flag:           MAC Addresses Lookup DB File
     Type:           string
     Synopsis:       Name and path of the csv file with the MAC Lookup DB
     Valid values:   A valid file name with path
--]]
lookupfile = "/home/bg/_Netlink_Mac/data/mac_db.csv"

--[[ Flag:           timeout  
     Type:           unsigned integer
     Synopsis:       Minimum number of waiting seconds before re-execute
                     the triggers for  MAC address or a registered name
                     of a MAC address range. 
                     - 0  ( zero ) means always re-execute;
                     - -1 (minus one) means once.
                     [default: 60]
     Valid values:   an unsigned integer, from 0
--]]
timeout = 120

--[[ Flag:           triggers (by MAC Address)
     Type:           associative array string(key)/string(csv value) 
     Synopsis:       List of MAC addresses -> action triggered.
                     The key is MAC addres in string format (two HEX digits for field, ':' as separator ):
                         0C:FF:01:31:AC:01
                     The value is a csv string, the first field is the triggered program, 
                     the other fields (optionals) are the executable's parameters.
                     "exe_or_script:par1,par2,...,parn"
                     Example:
                     "printmac:-d"

     Valid values:   Zero, one or more pairs string/csv string, as described.
--]]
triggersByMac = {["005056C00008"] = "/home/bg/_Netlink_Mac/test/printMac.sh;&;",
                 ["005056F6D10D"] = "/home/bg/_Netlink_Mac/test/printMac.sh;&;"  }

--[[ Flag:           triggers (by Company)
     Type:           associative array string(key)/string(csv value) 
     Synopsis:       List companies -> action triggered.
                     The key is a company name in string format (ended with ':' as separator ):
                         Apple             
                     The value is a csv string, the first field is the triggered program, 
                     the other fields (optionals) are the executable's parameters.
                     "exe_or_script:par1,par2,...,parn"
                     Example:
                     "printmac:-d"

     Valid values:   Zero, one or more pairs string/csv string, as described.
--]]
triggersByCompany = {["VMware"] = "/home/bg/_Netlink_Mac/test/printCompany.sh;&;"}
