###########################################################################
#   Copyright (C) 1998-2019 by authors (see AUTHORS.txt)                  #
#                                                                         #
#   This file is part of LuxMark.                                         #
#                                                                         #
#   LuxMark is free software; you can redistribute it and/or modify       #
#   it under the terms of the GNU General Public License as published by  #
#   the Free Software Foundation; either version 3 of the License, or     #
#   (at your option) any later version.                                   #
#                                                                         #
#   LuxMark is distributed in the hope that it will be useful,            #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#   GNU General Public License for more details.                          #
#                                                                         #
#   You should have received a copy of the GNU General Public License     #
#   along with this program.  If not, see <http://www.gnu.org/licenses/>. #
#                                                                         #
#   LuxMark website: https://www.luxcorerender.org                        #
###########################################################################

# Generally useful routines:

if (AdjustToolFlags_MODULE_INCLUDED)
	return()
else()
	set(AdjustToolFlags_MODULE_INCLUDED TRUE)
endif()

function(AdjustToolFlags) # ...vars [ADDITIONS ...val][REMOVALS ...regex]

	# Parse arguments:

	set(variables "")
	set(additions "")
	set(removals "")
	set(dstlist "variables")
	foreach (arg IN ITEMS ${ARGN})

		if (arg STREQUAL "ADDITIONS")
			set(dstlist additions)
		elseif (arg STREQUAL "REMOVALS")
			set(dstlist removals)
		else()
			list(APPEND ${dstlist} ${arg})
		endif()

	endforeach()

	# Whitespace-separated strings to cmake lists:
	string(REGEX REPLACE "[ \t]+" ";" removals "${removals}")
	string(REGEX REPLACE "[ \t]+" ";" additions "${additions}")

	foreach (variable IN LISTS variables)

		string(REGEX REPLACE "[ \t]+" ";" sclist "${${variable}}")

		foreach (pattern IN LISTS removals)
			list(FILTER sclist EXCLUDE REGEX ${pattern})
		endforeach()

		list(APPEND sclist ${additions})
		list(REMOVE_DUPLICATES sclist)

		# Join list with whitespace:
		string(REPLACE ";" " " readable "${sclist}")

		# Figure out whether the variable is cached yet and act accordingly
		# (if we don't cache it in, 'set' only creates a local that shadows
		# the cache variable, and in that case won't see it in the GUI):
		get_property(helpstring CACHE ${variable} PROPERTY HELPSTRING)
		if (helpstring)
			set(${variable} ${readable} CACHE STRING "${helpstring}" FORCE)
		else()
			set(${variable} ${readable} PARENT_SCOPE)
		endif()

	endforeach()

endfunction()
