bin_PROGRAMS     = macnotifier
dist_man_MANS    = ../doc/macnotifier.1
# dist_bin_SCRIPTS = 

nobase_include_HEADERS       = ../include/anyexcept.hpp ../include/configFile.hpp ../include/macnotify.hpp ../include/maclookup.hpp ../include/macnotifier.hpp ../include/parseCmdLine.hpp ../include/logger.hpp
macnotifier_SOURCES          = ./configFile.cpp ./macnotifier.cpp ./macnotify.cpp ./maclookup.cpp ./parseCmdLine.cpp ./logger.cpp
macnotifier_CPPFLAGS         = ${LUA_INCLUDE}
macnotifier_LDADD            = ${LUA_LIB}
