# This file is part of GPaste.
#
# Copyright 2012 Marc-Antoine Perennou <Marc-Antoine@Perennou.com>
#
# GPaste is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# GPaste is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GPaste.  If not, see <http://www.gnu.org/licenses/>.

bin_PROGRAMS += \
	bin/gpaste \
	$(NULL)

bin_gpaste_SOURCES = \
	src/gpaste/gpaste.c \
	$(NULL)

bin_gpaste_CFLAGS = \
	-I$(srcdir)/libgpaste/client/ \
	$(AM_CFLAGS) \
	$(NULL)

if ENABLE_APPLET
bin_gpaste_CFLAGS += \
	-DENABLE_APPLET \
	$(NULL)
endif

bin_gpaste_LDADD = \
	$(libgpaste_client_la_file) \
	$(NULL)
