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

#ifndef TAG_PARSER_H_INCLUDED
#define TAG_PARSER_H_INCLUDED

#include <vector>

class TagItem;

class TagParser
{
public:
	static bool parse(int argc, char* argv[], std::vector<TagItem*> &items);

private:
	static bool parseString(const char *key, const char *value, std::vector<TagItem*> &items);
	static bool parseNumber(const char *key, const char *value, std::vector<TagItem*> &items);
	static bool parseDate  (const char *key, const char *value, std::vector<TagItem*> &items);
};

#endif //TAG_PARSER_H_INCLUDED
