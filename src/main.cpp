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

//CRT includes
#include <cstdlib>
#include <cstdio>
#include <stdexcept>
#include <csignal>

//Windows includes
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//Internal
#include "types.h"
#include "parser.h"
#include "ape_tag.h"
#include "unicode_support.h"

//Const
static const unsigned int TAG_VERSION_MAJOR = 1;
static const unsigned int TAG_VERSION_MINOR = 0;

//Macros
#define LOG(...) fprintf(stderr, __VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////
// Help screen
///////////////////////////////////////////////////////////////////////////////

void tag_help(void)
{
	LOG("APEv2 specifications by courtesy of Hydrogenaudio.org:\n");
	LOG("http://wiki.hydrogenaudio.org/index.php?title=APEv2_specification\n");
	LOG("\n\n");
	LOG("Usage:\n");
	LOG("   tag.exe <type> <file> [<tag 1> <tag 2> ... <tag n>]\n");
	LOG("\n");
	LOG("Options:\n");
	LOG("   type - The technical type of the meta tag to be added\n");
	LOG("   file - the media file to append the tag to\n");
	LOG("   tag  - meta tag item to be added in the \"key=value\" format\n");
	LOG("\n");
	LOG("Supported tag types:\n");
	LOG("   APE2 - APE Tag, version 2 (only type currently supported)\n");
	LOG("\n");
	LOG("Supported keys:\n");
	LOG("   Artist   <string>\n");
	LOG("   Album    <string>\n");
	LOG("   Track    <number>\n");
	LOG("   Year     <ISO-8601 date>\n");
	LOG("\n");
	LOG("Example:\n");
	LOG("   tag.exe APE2 \"C:\\My Folder\\File.mp3\" \"Artist=John Doe\"\n");
	LOG("\n");
}

///////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////

static int tag_main(int argc, char* argv[])
{
	LOG("\nSimple Tag Creator v%u.%02u [%s]\n", TAG_VERSION_MAJOR, TAG_VERSION_MINOR, __DATE__);
	LOG("Copyright (c) 2004-2013 LoRd_MuldeR <mulder2@gmx.de>. Some rights reserved.\n\n");
	
	LOG("This program is free software; you can redistribute it and/or modify\n");
	LOG("it under the terms of the GNU General Public License <http://www.gnu.org/>.\n");
	LOG("Note that this program is distributed with ABSOLUTELY NO WARRANTY.\n\n");

	if(argc < 3)
	{
		tag_help();
		return 1;
	}

	if(_stricmp(argv[1], "APE2") != 0)
	{
		LOG("Unknown tag type:\n%s\n\n", argv[1]);
		return 1;
	}

	FILE *file = fopen_utf8(argv[2], "ab");

	if(!file)
	{
		LOG("Failed to open file for appending:\n%s\n\nInvalid file specified or access denied!\n\n", argv[2]);
		return 1;
	}

	std::vector<TagItem*> tagItems;
	if(!TagParser::parse(argc, argv, tagItems))
	{
		LOG("Failed to parse tag specification, invalid input!\n\n");
		fclose(file);
		return 1;
	}

	if(tagItems.size() < 1)
	{
		LOG("No tags have been specified. Need to specify at least one tag!\n\n");
		fclose(file);
		return 1;
	}

	if(!ApeTagger::writeTags(file, tagItems))
	{
		LOG("An error occurred while trying to write tags to file!\n\n");
		fclose(file);
		return 1;
	}

	while(!tagItems.empty())
	{
		TagItem *tmp = tagItems.back();
		tagItems.pop_back();
		delete tmp; tmp = NULL;
	}

	fclose(file);
	LOG("Tags have been written successfully.\n\n");
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Error handlers
///////////////////////////////////////////////////////////////////////////////

static void invalid_param_handler(const wchar_t* exp, const wchar_t* fun, const wchar_t* fil, unsigned int, uintptr_t)
{
	LOG("\nGURU MEDITATION !!!\n\nInvalid parameter handler invoked, application will exit!\n");
	_exit(1);
}

static void signal_handler(int signal_num)
{
	signal(signal_num, signal_handler);
	LOG("\nGURU MEDITATION !!!\n\nSignal handler #%d invoked, application will exit!\n", signal_num);
	_exit(1);
}

static LONG WINAPI exception_handler(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	LOG("\nGURU MEDITATION !!!\n\nUnhandeled exception handler invoked, application will exit!\n");
	_exit(1);
	return LONG_MAX;
}

///////////////////////////////////////////////////////////////////////////////
// Applicaton entry point
///////////////////////////////////////////////////////////////////////////////

static int tag_zero(int argc, char* argv[])
{
		int iResult = -1;
		int argc_utf8;
		char **argv_utf8;

		try
		{
			init_console_utf8();
			init_commandline_arguments_utf8(&argc_utf8, &argv_utf8);

			iResult = tag_main(argc_utf8, argv_utf8);

			free_commandline_arguments_utf8(&argc_utf8, &argv_utf8);
			uninit_console_utf8();
		}
		catch(const std::exception &error)
		{
			fflush(stdout); fflush(stderr);
			LOG("\nGURU MEDITATION !!!\n\nException error:\n%s\n", error.what());
			_exit(-1);
		}
		catch(...)
		{
			fflush(stdout); fflush(stderr);
			LOG("\nGURU MEDITATION !!!\n\nUnknown exception error!\n");
			_exit(-1);
		}
		return iResult;
}

int main(int argc, char* argv[])
{
	__try
	{
		SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
		SetUnhandledExceptionFilter(exception_handler);
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
		_set_invalid_parameter_handler(invalid_param_handler);
	
		static const int signal_num[6] = { SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM };

		for(size_t i = 0; i < 6; i++)
		{
			signal(signal_num[i], signal_handler);
		}

		return tag_zero(argc, argv);
	}
	__except(1)
	{
		fflush(stdout);
		fflush(stderr);
		LOG("\nGURU MEDITATION !!!\n\nUnhandeled structured exception error!\n");
		_exit(-1);
	}
}
