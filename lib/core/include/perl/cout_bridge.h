/* Copyright (c) 1997-2015
   Ewgenij Gawrilow, Michael Joswig (Technische Universitaet Berlin, Germany)
   http://www.polymake.org

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version: http://www.gnu.org/licenses/gpl.txt.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
--------------------------------------------------------------------------------
*/
#ifndef POLYMAKE_PERL_COUT_BRIDGE_H
#define POLYMAKE_PERL_COUT_BRIDGE_H

#include "polymake/perl/glue.h"
#include <iostream>

namespace pm { namespace perl { namespace glue {

class ostreambuf_bridge
   : public std::streambuf {
public:
   ostreambuf_bridge(pTHX_ GV* gv_arg);
protected:
   int sync();
   int_type overflow(int_type c=traits_type::eof());
private:
   bool handover(bool with_sync);

   PerlInterpreter* pi;
   GV* gv;
   char buf[1024];
};

} } }

#endif // POLYMAKE_PERL_COUT_BRIDGE_H

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
