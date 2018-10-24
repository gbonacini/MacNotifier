Description:
============

Maclookup is an utility permitting to obtain the company name associated to a MAC address, creating a DB from the IEEE registration CSV archives.

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

At this point, the creation of the lookupdb is necessary. To complete this step, some CSV archives from the IEEE web portal are required. So, download the file named "IEEE MA-L Assignments (.csv)", "IEEE MA-M Assignments (.csv)" and " IEEE MA-S Assignments (.csv)" from this page:

https://standards.ieee.org/products-services/regauth/index.html

and process these file with the script:

scripts/createDb.sh

A file named: 

mac_db.csv

will be created in the current directory: this file will be processed to create the internal lookup db.

Example:

createDb.sh oui.large.csv mam.medium.csv oui36.small.csv 

Now, to use the utilitiy,  the db file must be specified with the -c option:

maclookup -c ../data/mac_db.csv -l 60:03:08:a5:33:d4

then you can specify -l (command line, single MAC lookup ) or -i (interactive mode: it resolves MACs until the command 'end' close the program).

An example of interactive mode:

/src/maclookup -c ../data/mac_db.csv -i

The command line mode returns 0 in case of successful lookup, 1 in case of error.
