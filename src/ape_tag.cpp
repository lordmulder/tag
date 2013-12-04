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

#include "ape_tag.h"
#include "types.h"

#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <vector>

//Macros
#define LOG(...) fprintf(stderr, __VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////
// APE structs
///////////////////////////////////////////////////////////////////////////////

static const char APE_ID[8] = { 'A', 'P', 'E', 'T', 'A', 'G', 'E', 'X' };

typedef struct 
{
	unsigned char id      [8];
	unsigned char version [4];
	unsigned char length  [4];
	unsigned char tagCount[4];
	unsigned char flags   [4];
	unsigned char reserved[8];
}
ape_header_t;

///////////////////////////////////////////////////////////////////////////////
// Helper functions
///////////////////////////////////////////////////////////////////////////////

inline static void write_uint32(unsigned char* dest, const unsigned int value)
{
	dest[0] = (unsigned char) ((((unsigned int)(0x000000FF)) & (value)) >>  0);
	dest[1] = (unsigned char) ((((unsigned int)(0x0000FF00)) & (value)) >>  8);
	dest[2] = (unsigned char) ((((unsigned int)(0x00FF0000)) & (value)) >> 16);
	dest[3] = (unsigned char) ((((unsigned int)(0xFF000000)) & (value)) >> 24);
}

inline static void append_uint32(std::vector<unsigned char> &dest, const unsigned int value)
{
	dest.resize(dest.size() + 4);
	unsigned char *const pos = &dest.data()[dest.size() - 4];
	write_uint32(pos, value);
}

inline static void append_nbytes(std::vector<unsigned char> &dest, const unsigned char *data, const size_t len)
{
	dest.resize(dest.size() + len);
	unsigned char *const pos = &dest.data()[dest.size() - len];
	memcpy(pos, data, len * sizeof(unsigned char));
}

inline static void date2string(const TagDate &date, char *buffer)
{
	if(date.getY() < 1)
	{
		throw std::runtime_error("Invalid date specification!");
	}
	
	if(date.getM() > 0)
	{
		if(date.getD() > 0)
		{
			sprintf(buffer, "%04u-%02u-%02u", date.getY(), date.getM(), date.getD());
		}
		else
		{
			sprintf(buffer, "%04u-%02u", date.getY(), date.getM());
		}
	}
	else
	{
		sprintf(buffer, "%04u", date.getY());
	}
}

inline static void init_header(ape_header_t *header, const size_t data_size, const size_t n_items, const bool is_footer)
{
	static const unsigned int flags_header = 0xA0000001;
	static const unsigned int flags_footer = 0x80000001;

	memset(header, 0, sizeof(ape_header_t));
	memcpy(&header->id[0], APE_ID, 8);

	write_uint32(&header->version [0], 2000);
	write_uint32(&header->length  [0], data_size + sizeof(ape_header_t));
	write_uint32(&header->tagCount[0], n_items);
	write_uint32(&header->flags   [0], is_footer ? flags_footer: flags_header);
}

///////////////////////////////////////////////////////////////////////////////
// CLI Parser
///////////////////////////////////////////////////////////////////////////////

bool ApeTagger::writeTags(FILE* file, const std::vector<TagItem*> &items)
{
	std::vector<unsigned char> tagData;

	//Write all tags to internal buffer
	for(std::vector<TagItem*>::const_iterator iter = items.cbegin(); iter != items.cend(); iter++)
	{
		if(!appendTag(tagData, *iter))
		{
			return false;
		}
	}
	LOG("\n");

	//Prepare the APE header
	ape_header_t header;
		
	//Write header
	init_header(&header, tagData.size(), items.size(), false);
	if(fwrite(&header, sizeof(ape_header_t), 1, file) != 1)
	{
		LOG("File operation has failed:\nUnable to write header to destination file!\n\n");
		return false;
	}

	//Write actual data
	if(fwrite(tagData.data(), sizeof(unsigned char), tagData.size(), file) != tagData.size())
	{
		LOG("File operation has failed:\nUnable to write tag data to destination file!\n\n");
		return false;
	}
	
	//Write footer
	init_header(&header, tagData.size(), items.size(), true);
	if(fwrite(&header, sizeof(ape_header_t), 1, file) != 1)
	{
		LOG("File operation has failed:\nUnable to write footer to destination file!\n\n");
		return false;
	}

	return true;
}

bool ApeTagger::appendTag(std::vector<unsigned char> &dest, TagItem* item)
{
	static const unsigned int flags_str = 0x00000001;
	static const unsigned int flags_bin = 0x00000003;

	char tempBuffer[32];

	const char *key = item->getTagKey();
	const char *str = NULL;

	//Set the data source
	switch(item->getTagData()->type())
	{
	case TAG_TYPE_STRING:
		str = item->getTagData()->toString();
		break;
	case TAG_TYPE_NUMBER:
		sprintf(tempBuffer, "%u", item->getTagData()->toNumber());
		str = &tempBuffer[0];
		break;
	case TAG_TYPE_DATE:
		date2string(item->getTagData()->toDate(), tempBuffer);
		str = &tempBuffer[0];
		break;
	default:
		throw std::runtime_error("Bad item type!");
	}

	//Determine length
	const size_t len = strlen(str);

	//Write length, flags, key and the data
	append_uint32(dest, len);
	append_uint32(dest, flags_str);
	append_nbytes(dest, ((const unsigned char*)key), strlen(key) + 1);
	append_nbytes(dest, ((const unsigned char*)str), len);

	//Logging
	LOG("%-11s : %s\n", key, str);

	return true;
}
