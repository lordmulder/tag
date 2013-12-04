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

#include "parser.h"
#include "types.h"

#include <cstdio>
#include <cstring>
#include <stdexcept>

//Macros
#define LOG(...) fprintf(stderr, __VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////
// Supported Tag key
///////////////////////////////////////////////////////////////////////////////

static const struct
{
	const char *key;
	const TagType type;
}
g_tagSpec[]
{
	{ "Artist", TAG_TYPE_STRING },
	{ "Year",   TAG_TYPE_DATE },
	{ NULL, ((TagType)-1) }
};

///////////////////////////////////////////////////////////////////////////////
// Helper functions
///////////////////////////////////////////////////////////////////////////////

static inline bool is_space(const char c)
{
	return ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'));
}

static void trim(char **str)
{
	size_t len = strlen(*str);
	while((len > 0) && is_space((*str)[--len]))
	{
		(*str)[len] = '\0';
	}
	while(is_space((*str)[0]))
	{
		(*str)++;
	}
}

///////////////////////////////////////////////////////////////////////////////
// CLI Parser
///////////////////////////////////////////////////////////////////////////////

bool TagParser::parse(int argc, char* argv[], std::vector<TagItem*> &items)
{
	for(int i = 3; i < argc; i++)
	{
		char *key = _strdup(argv[i]);
		char *val = strchr(key, '=');

		if(val == NULL)
		{
			LOG("Separator is missing in tag specification:\n%s\n\n", argv[i]);
			free(key);
			return false;
		}

		*val++ = '\0';
		trim(&key); trim(&val);

		if(!(val[0] && key[0]))
		{
			LOG("Key or value is empty in tag specification:\n\"%s\" = \"%s\"\n\n", key, val);
			free(key);
			return false;
		}

		bool ok = false;

		for(int j = 0; g_tagSpec[j].key != NULL; j++)
		{
			if(_stricmp(key, g_tagSpec[j].key) == 0)
			{
				switch(g_tagSpec[j].type)
				{
				case TAG_TYPE_STRING:
					ok = parseString(g_tagSpec[j].key, val, items);
					break;
				case TAG_TYPE_NUMBER:
					ok = parseNumber(g_tagSpec[j].key, val, items);
					break;
				case TAG_TYPE_DATE:
					ok = parseDate  (g_tagSpec[j].key, val, items);
					break;
				default:
					throw std::runtime_error("Bad tag type!");
				}
				
				if(!ok)
				{
					LOG("Tag specification contains a malformed value:\n\"%s\" = \"%s\"\n\n", key, val);
					free(key);
					return false;
				}
			}
		}

		if(!ok)
		{
			LOG("Tag specification uses an unknown key:\n\"%s\" = \"%s\"\n\n", key, val);
			free(key);
			return false;
		}

		free(key);
	}
	
	return true;
}

bool TagParser::parseString(const char *key, const char *value, std::vector<TagItem*> &items)
{
	if(value && value[0])
	{
		items.push_back(TagItem::fromString(key, value));
		return true;
	}
	return false;
}

bool TagParser::parseNumber(const char *key, const char *value, std::vector<TagItem*> &items)
{
	if(value && value[0])
	{
		unsigned int number = 0;
		if(sscanf(value, "%u", &number) == 1)
		{
			items.push_back(TagItem::fromNumber(key, number));
			return true;
		}
	}
	return false;
}

bool TagParser::parseDate(const char *key, const char *value, std::vector<TagItem*> &items)
{
	if(value && value[0])
	{
		unsigned int y = 0, m = 0, d = 0;
		if(sscanf(value, "%u-%u-%u", &y, &m, &d) == 3)
		{
			if((m <= 12) && (d <= 31))
			{
				items.push_back(TagItem::fromDate(key, y, m, d));
				return true;
			}
		}
		else if(sscanf(value, "%u-%u", &y, &m) == 2)
		{
			if((y <= 9999) && (m <= 12))
			{
				items.push_back(TagItem::fromDate(key, y, m));
				return true;
			}
		}
		else if(sscanf(value, "%u", &y) == 1)
		{
			if(y <= 9999)
			{
				items.push_back(TagItem::fromDate(key, y));
				return true;
			}
		}
	}
	return false;
}
