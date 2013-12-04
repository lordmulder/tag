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

#ifndef TAG_TYPES_H_INCLUDED
#define TAG_TYPES_H_INCLUDED

#include <cstring>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
// Types
///////////////////////////////////////////////////////////////////////////////

typedef enum
{
	TAG_TYPE_STRING = 0,
	TAG_TYPE_NUMBER = 1,
	TAG_TYPE_DATE   = 2
}
TagType;

///////////////////////////////////////////////////////////////////////////////
// Date
///////////////////////////////////////////////////////////////////////////////

class TagDate
{
public:
	TagDate(const unsigned int y, const unsigned int m = 0, const unsigned int d = 0)
	:
		m_y(y), m_m(m), m_d(d)
	{
		/*nothing to do*/
	}

	inline const unsigned int getY(void) const { return m_y; }
	inline const unsigned int getM(void) const { return m_m; }
	inline const unsigned int getD(void) const { return m_d; }
	
private:
	const unsigned int m_y;
	const unsigned int m_m;
	const unsigned int m_d;
};

///////////////////////////////////////////////////////////////////////////////
// Tag Data
///////////////////////////////////////////////////////////////////////////////

// Base class for all data classes
class TagData
{
public:
	TagData(void) {}
	virtual ~TagData(void) {}

	virtual const char        *toString(void) const = 0;
	virtual const unsigned int toNumber(void) const = 0;
	virtual const TagDate     &toDate(void)   const = 0;
	
	virtual const TagType type(void) = 0;

private:
	TagData &operator=(const TagData &rhs) { throw std::runtime_error("Unimplemented!");}
};

// String data
class TagData_String : public TagData
{
public:
	TagData_String(const char *str)                      { m_string = (str != NULL) ? _strdup(str) : NULL; }
	virtual ~TagData_String(void)                        { if(m_string != NULL) { free(m_string); m_string = NULL; } }

	virtual const char *toString(void)        const      { return m_string; }
	virtual const unsigned int toNumber(void) const      { throw std::runtime_error("Invalid type requested!"); }
	virtual const TagDate &toDate(void)       const      { throw std::runtime_error("Invalid type requested!"); }

	virtual const TagType type(void)                     { return TAG_TYPE_STRING; }

private:
	char *m_string;
	TagData_String &operator=(const TagData_String &rhs) { throw std::runtime_error("Unimplemented!");}
};

// Numeric data
class TagData_Number : public TagData
{
public:
	TagData_Number(const unsigned int x)                 : m_number(x) {}
	virtual ~TagData_Number(void)                        { /*nothing to do*/ }

	virtual const char *toString(void)        const      { throw std::runtime_error("Invalid type requested!"); }
	virtual const unsigned int toNumber(void) const      { return m_number; }
	virtual const TagDate &toDate(void)       const      { throw std::runtime_error("Invalid type requested!"); }

	virtual const TagType type(void)                     { return TAG_TYPE_NUMBER; }

private:
	const unsigned int m_number;
	TagData_Number &operator=(const TagData_Number &rhs) { throw std::runtime_error("Unimplemented!");}
};

// Date data
class TagData_Date : public TagData
{
public:
	TagData_Date(const unsigned int y, const unsigned int m = 0, const unsigned int d = 0) : m_date(y, m, d) {}
	virtual ~TagData_Date(void)                      { /*nothing to do*/ }

	virtual const char *toString(void)        const  { throw std::runtime_error("Invalid type requested!"); }
	virtual const unsigned int toNumber(void) const  { throw std::runtime_error("Invalid type requested!"); }
	virtual const TagDate &toDate(void )      const  { return m_date; }

	virtual const TagType type(void)                 { return TAG_TYPE_DATE; }

private:
	const TagDate m_date;
	TagData_Date &operator=(const TagData_Date &rhs) { throw std::runtime_error("Unimplemented!");}
};

///////////////////////////////////////////////////////////////////////////////
// Tag Item
///////////////////////////////////////////////////////////////////////////////

class TagItem
{
public:
	static TagItem *fromString(const char* key, const char *str)
	{
		return new TagItem(key, new TagData_String(str));
	}

	static TagItem *fromNumber(const char* key, const unsigned int x)
	{
		return new TagItem(key, new TagData_Number(x));
	}

	static TagItem *fromDate(const char* key, const unsigned int y, const unsigned int m = 0, const unsigned int d = 0)
	{
		return new TagItem(key, new TagData_Date(y, m ,d));
	}

	inline const char    *getTagKey(void)  const { return m_key; }
	inline const TagData *getTagData(void) const { return m_data; }

	~TagItem(void)
	{
		delete m_data; m_data = NULL;
		free(m_key);   m_key  = NULL;
	}

private:
	TagItem(const char* key, const TagData *data)
	{
		if((key != NULL) && (data != NULL))
		{
			m_key = _strdup(key);
			m_data = data;
		}
		else
		{
			throw std::runtime_error("Invalid arguments!");
		}
	}

	char *m_key;
	const TagData *m_data;

	TagItem &operator=(const TagItem &rhs) { throw std::runtime_error("Unimplemented!");}
};

#endif //TAG_TYPES_H_INCLUDED
