///////////////////////////////////////////////////////////////////////////////
// Simple Tag Creator
// Copyright (C) 2004-2013 LoRd_MuldeR <MuldeR2@GMX.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version, but always including the *additional*
// restrictions defined in the "License.txt" file.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// http://www.gnu.org/licenses/gpl-2.0.txt
///////////////////////////////////////////////////////////////////////////////

#ifndef TAG_KEYS_H_INCLUDED
#define TAG_KEYS_H_INCLUDED

///////////////////////////////////////////////////////////////////////////////
// Supported Tag keys
///////////////////////////////////////////////////////////////////////////////

static const struct
{
	const char *key;
	const TagType type;
	const char *info;
}
g_tagSpec[]
{
	{ "Album",           TAG_TYPE_STRING , "Album name "                   },
	{ "Artist",          TAG_TYPE_STRING , "Performing artist"             },
	{ "Comment",         TAG_TYPE_STRING , "User comments"                 },
	{ "Composer",        TAG_TYPE_STRING , "Name of the original composer" },
	{ "Copyright",       TAG_TYPE_STRING , "Copyright holder"              },
	{ "Genre",           TAG_TYPE_STRING , "Genre, normally English terms" },
	{ "Language",        TAG_TYPE_STRING , "Used Language for music/words" },
	{ "Media",           TAG_TYPE_STRING , "Source media"                  },
	{ "Publisher",       TAG_TYPE_STRING , "Record label or publisher"     },
	{ "Record Date",     TAG_TYPE_DATE   , "Record date"                   },
	{ "Record Location", TAG_TYPE_STRING , "Record location"               },
	{ "Subtitle",        TAG_TYPE_STRING , "Additional sub title"          },
	{ "Title",           TAG_TYPE_STRING , "Music piece title"             },
	{ "Track",           TAG_TYPE_NUMBER , "Track Number"                  },
	{ "Year",            TAG_TYPE_DATE   , "Year"                          },
	{ NULL, ((TagType)-1) }
};

#endif //TAG_KEYS_H_INCLUDED
