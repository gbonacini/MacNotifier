#!/usr/bin/env bash

# 
# Requires as parameters the csv files from IEEE website (large, medium and small MAC blocks lookup files)
# Example:
# ./createDb.sh oui.large.csv mam.medium.csv oui36.small.csv 
#

[[ "$#" -ne "3" ]] && { echo "Expected 3 input file, provided : $#" ;
                        echo "Syntax:"
                        echo "$0 file_large.csv file_medium.csv file_small.csv";
                        exit 1; }

[[ -f $1 ]] || { echo "Not found: $1" ; exit 1; }
[[ -f $2 ]] || { echo "Not found: $2" ; exit 1; }
[[ -f $3 ]] || { echo "Not found: $3" ; exit 1; }

cat  $*                                                                                    | \
     grep -v 'Registry,Assignment,Organization Name,Organization Address'                  | \
     grep -v 'IEEE Registration Authority'                                                 | \
     awk -F',' '{ printf "%s;%s;\n",$2,$3 }'                                               | \
     tr -d '"'   > mac_db.csv

exit 0