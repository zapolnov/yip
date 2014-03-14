/* vim: set ai noet ts=4 sw=4 tw=115: */
//
// Copyright (c) 2014 Nikolay Zapolnov (zapolnov@gmail.com).
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#include "project/project_file_parser.h"
#include "project/generate_xcode.h"
#include "3rdparty/libgit2/include/git2/threads.h"
#include "xcode/xcode_unique_id.h"
#include "util/fmt.h"
#include "util/shell.h"
#include "util/path.h"
#include "config.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cassert>

static Platform::Type defaultTargetPlatform();

static void usage()
{
	const char * apple_default = "";

	switch (defaultTargetPlatform())
	{
	case Platform::None: break;
	case Platform::OSX: apple_default = " (default)"; break;
	default: assert(false);
	}

	/*   1234567890123456789012345678901234567890123456789012345678901234567890123456789| */
	std::cout <<                                                                      /*|*/
	    "Usage: yip [command] [options]\n"                                            /*|*/
	    "\n"                                                                          /*|*/
	    "The following commands are available (short form is given in parentheses, where\n"
	    "available):\n"                                                               /*|*/
	    "\n"                                                                          /*|*/
	    " * build: Build the project. This is the default command.\n"                 /*|*/
	    "\n"                                                                          /*|*/
	    "     By default, yip builds for the current platform. This can be overriden by\n"
	    "     specifying one or more of the following options:\n"                     /*|*/
//	    "       -a, --android  Build for Android.\n"                                  /*|*/
	  #ifdef __APPLE__
	    "       -i, --ios      Build for iOS.\n"                                      /*|*/
	    "       -j, --ios-sym  Build for iOS simulator.\n"                            /*|*/
	  #endif
//	    "       -m, --mingw    Build for Windows using MinGW.\n"                      /*|*/
	  #ifdef __APPLE__
	    "       -o, --osx      Build for OSX" << apple_default << ".\n"               /*|*/
	  #endif
//	    "       -p, --pnacl    Build for PNaCl.\n"                                    /*|*/
//	    "       -q, --qt       Build for Qt.\n"                                       /*|*/
//	    "       -t, --tizen    Build for Tizen.\n"                                    /*|*/
//	  #if defined(_WIN64) || defined(_WIN32)
//	    "       -w, --winrt    Build for Windows.\n"                                  /*|*/
//	  #endif
	    "\n"                                                                          /*|*/
	    "     By default, yip builds the debug build. To override this, one or more\n"/*|*/
	    "     of the following options could be used:\n"                              /*|*/
	    "       -d, --debug    Build the debug build.\n"                              /*|*/
	    "       -r, --release  Build the release build.\n"                            /*|*/
	    "\n"                                                                          /*|*/
	    " * generate (gen): Generate project file but do not build.\n"                /*|*/
	    "\n"                                                                          /*|*/
	    "     By default, yip generates project file for the current platform only."  /*|*/
	    "     This can be overriden by specifying one or more of the following options:\n"
//	    "       -a, --android  Generate project for Android.\n"                       /*|*/
//	    "       -c, --cmake    Generate project files for CMake.\n"                   /*|*/
	    "       -i, --ios      Generate XCode project for iOS.\n"                     /*|*/
	    "       -o, --osx      Generate XCode project for OSX" << apple_default << ".\n"
//	    "       -p, --pnacl    Generate project files for PNaCl.\n"                   /*|*/
//	    "       -t, --tizen    Generate project files for Tizen.\n"                   /*|*/
//	    "       -v, --vs2012   Generate project files for Visual Studio 2012.\n"      /*|*/
	    "\n"                                                                          /*|*/
	    "     Yip will open the generated project file in the IDE after generating\n" /*|*/
	    "     (except when generating multiple projects were requested). If this is not\n"
	    "     intended, then the following option could be specified:\n"              /*|*/
	    "       -n, --no-open  Do not open project file in the IDE.\n"                /*|*/
	    "\n"                                                                          /*|*/
	    "     All generated project files are stored into the .yip subdirectory in your\n"
	    "     source directory.\n"                                                    /*|*/
	    "\n"                                                                          /*|*/
	    " * update (up): Download latest versions of dependencies.\n"                 /*|*/
	    "\n"                                                                          /*|*/
	    " * help: Display this help message.\n"                                       /*|*/
	    << std::endl;
}

static ProjectFilePtr loadProjectFile()
{
	std::string projectPath = pathGetDirectory(pathMakeAbsolute(g_Config->projectFileName));
	XCodeUniqueID::setSeed(projectPath);

	ProjectFilePtr projectFile = std::make_shared<ProjectFile>(projectPath);

	ProjectFileParser parser(g_Config->projectFileName);
	parser.parse(projectFile);

	return projectFile;
}

static Platform::Type defaultTargetPlatform()
{
	Platform::Type platform = Platform::None;
  #ifdef __APPLE__
	platform = Platform::OSX;
  #endif
	return platform;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build

static int build(int argc, char ** argv)
{
	BuildType::Value buildType = BuildType::Unspecified;
	bool buildIOS = false, buildIOSSimulator = false;
	Platform::Type platform = Platform::None;

	for (int i = 0; i < argc; i++)
	{
		if (argv[i][0] != '-')
			throw std::runtime_error(fmt() << "invalid parameter '" << argv[i] << "'.");
		else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug"))
			buildType |= BuildType::Debug;
		else if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--release"))
			buildType |= BuildType::Release;
		else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--osx"))
			platform |= Platform::OSX;
		else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--ios"))
		{
			platform |= Platform::iOS;
			buildIOS = true;
		}
		else if (!strcmp(argv[i], "-j") || !strcmp(argv[i], "--ios-sym"))
		{
			platform |= Platform::iOS;
			buildIOSSimulator = true;
		}
	}

	if (buildType == BuildType::Unspecified)
		buildType = BuildType::Debug;
	if (platform == Platform::None)
		platform = defaultTargetPlatform();

	if (platform == Platform::None)
	{
		std::cerr << "unable to determine target platform - please specify one using the command line option "
			"(try 'yip help' for more information)." << std::endl;
		return 1;
	}

	ProjectFilePtr projectFile = loadProjectFile();
	if (!projectFile->isValid())
		return 1;

	if (platform & Platform::OSX)
	{
		generateXCode(projectFile, false);
		platform &= ~Platform::OSX;
	}

	if (platform & Platform::iOS)
	{
		generateXCode(projectFile, true);
		platform &= ~Platform::iOS;
	}

	if (platform != 0)
	{
		std::cerr << "Not all platforms were built (0x" << std::hex << std::setw(4) << std::setfill('0')
			<< platform << ")." << std::endl;
		return 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generate

static int generate(int argc, char ** argv)
{
	Platform::Type platform = Platform::None;
	bool noOpen = false;

	for (int i = 0; i < argc; i++)
	{
		if (argv[i][0] != '-')
			throw std::runtime_error(fmt() << "invalid parameter '" << argv[i] << "'.");
		else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--osx"))
			platform |= Platform::OSX;
		else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--ios"))
			platform |= Platform::iOS;
		else if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--no-open"))
			noOpen = true;
	}

	if (platform == Platform::None)
		platform = defaultTargetPlatform();

	if (platform == Platform::None)
	{
		std::cerr << "unable to determine target platform - please specify one using the command line option "
			"(try 'yip help' for more information)." << std::endl;
		return 1;
	}

	ProjectFilePtr projectFile = loadProjectFile();
	if (!projectFile->isValid())
		return 1;

	size_t numPlatforms = 0;
	for (size_t i = 1; i <= 0x8000; i <<= 1)
	{
		if (platform & i)
			++numPlatforms;
	}

	if (numPlatforms > 1)
		noOpen = true;

	if (platform & Platform::OSX)
	{
		std::string generatedFile = generateXCode(projectFile, false);
	  #ifdef __APPLE__
		if (!noOpen)
			shellExec("open " + shellEscapeArgument(generatedFile));
	  #endif
		platform &= ~Platform::OSX;
	}

	if (platform & Platform::iOS)
	{
		std::string generatedFile = generateXCode(projectFile, true);
	  #ifdef __APPLE__
		if (!noOpen)
			shellExec("open " + shellEscapeArgument(generatedFile));
	  #endif
		platform &= ~Platform::iOS;
	}

	if (platform != 0)
	{
		std::cerr << "Not all platforms were generated (0x" << std::hex << std::setw(4) << std::setfill('0')
			<< platform << ")." << std::endl;
		return 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Update

static int update(int, char **)
{
	ProjectFilePtr projectFile = loadProjectFile();

	if (projectFile->repositories().size() == 0)
	{
		std::cout << "nothing to update." << std::endl;
		return 0;
	}

	GitProgressPrinter printer;
	for (const GitRepositoryPtr & repo : projectFile->repositories())
		repo->updateHeadToRemote("origin", &printer);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Help

static int help(int, char **)
{
	usage();
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char ** argv)
{
	try
	{
		git_threads_init();
		loadConfig();

		for (int i = 1; i < argc; i++)
		{
			if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") || !strcmp(argv[i], "/?"))
			{
				usage();
				return 1;
			}
		}

		if (argc > 1 && argv[1][0] != '-')
		{
			std::unordered_map<std::string, int (*)(int, char **)> commands;
			commands.insert(std::make_pair("build", &build));
			commands.insert(std::make_pair("generate", &generate));
			commands.insert(std::make_pair("gen", &generate));
			commands.insert(std::make_pair("help", &help));
			commands.insert(std::make_pair("up", &update));
			commands.insert(std::make_pair("update", &update));

			auto it = commands.find(argv[1]);
			if (it == commands.end())
				throw std::runtime_error(fmt() << "unknown command '" << argv[1] << "'. try 'yip help'.");

			return it->second(argc - 2, argv + 2);
		}

		build(argc - 1, argv + 1);
	}
	catch (const std::exception & e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
