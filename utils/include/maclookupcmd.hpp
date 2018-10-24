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

#ifndef MACLOOKUP___CMD_HPP
#define MACLOOKUP___CMD_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <string>

#include <config.h> 
#include <anyexcept.hpp>
#include <maclookup.hpp>
#include <parseCmdLine.hpp>

void paramError(const char* progname, const char* err) noexcept __attribute__ ((noreturn));
void versionInfo(void) noexcept __attribute__ ((noreturn));

#endif