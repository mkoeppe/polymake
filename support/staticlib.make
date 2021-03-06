#  Copyright (c) 1997-2015
#  Ewgenij Gawrilow, Michael Joswig (Technische Universitaet Berlin, Germany)
#  http://www.polymake.org
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by the
#  Free Software Foundation; either version 2, or (at your option) any
#  later version: http://www.gnu.org/licenses/gpl.txt.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#-------------------------------------------------------------------------------

#  Building static libraries out of bundled third-party software

SourceDir := $(firstword ${ExtensionTop} ${ProjectTop})/staticlib/$(notdir ${CURDIR})

include ${SourceDir}/Makefile.inc

Cflags += ${CflagsSuppressWarnings}
CXXflags += ${CflagsSuppressWarnings}

Archive := $(addsuffix $A, ${OwnLibrary})
LibObjects := $(addsuffix $O, ${LibModules})

.INTERMEDIATE: ${LibObjects}

${Archive} : ${LibObjects}
	${AR} -rc $@ $^
	${RANLIB} $@

compile : ${Archive}

clean::
	rm -f *$A *$O ${ExtraCLEAN}
