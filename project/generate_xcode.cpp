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
#include "generate_xcode.h"
#include "../xcode/xcode_project.h"
#include "../util/path.h"
#include "../util/file_type.h"
#include <map>
#include <sstream>
#include <cassert>
#include <memory>

namespace
{
	struct Gen
	{
		// Input
		ProjectFilePtr projectFile;
		bool iOS;

		// Output
		std::string projectPath;

		// Private
		std::string projectName;
		std::shared_ptr<XCodeProject> project;
		XCodeBuildPhase * frameworksBuildPhase = nullptr;
		XCodeBuildPhase * sourcesBuildPhase = nullptr;
		XCodeBuildPhase * resourcesBuildPhase = nullptr;
		XCodeGroup * mainGroup = nullptr;
		XCodeGroup * sourcesGroup = nullptr;
		XCodeGroup * generatedGroup = nullptr;
		XCodeGroup * productsGroup = nullptr;
		XCodeGroup * resourcesGroup = nullptr;
		std::map<std::pair<XCodeGroup *, std::string>, XCodeGroup *> dirGroups;
		XCodeTargetBuildConfiguration * cfgTargetDebug = nullptr;
		XCodeTargetBuildConfiguration * cfgTargetRelease = nullptr;
		XCodeProjectBuildConfiguration * cfgProjectDebug = nullptr;
		XCodeProjectBuildConfiguration * cfgProjectRelease = nullptr;
		XCodeConfigurationList * targetCfgList = nullptr;
		XCodeConfigurationList * projectCfgList = nullptr;

		/* Methods */

		// Build phases
		void createBuildPhases();

		// Groups
		void createGroups();
		XCodeGroup * groupForPath(XCodeGroup * rootGroup, const std::string & path);

		// Source files
		void addSourceFile(XCodeGroup * group, XCodeBuildPhase * phase, const SourceFilePtr & file);
		void addSourceFiles();

		// Configurations
		void initDebugConfiguration();
		void initReleaseConfiguration();
		void createConfigurationLists();

		// Preprocessor definitions
		void addDefines();

		// Native target
		void createNativeTarget();

		// Auxiliary files
		void writeInfoPList();
		void writeImageAssets();

		// Generating
		void writePBXProj();
		void generate();
	};
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private functions

static std::string fileTypeForXCode(FileType type)
{
	switch (type)
	{
	case FILE_UNKNOWN: return XCODE_FILETYPE_TEXT;				// FIXME
	case FILE_TEXT_PLAIN: return XCODE_FILETYPE_TEXT;
	case FILE_TEXT_HTML: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_TEXT_CSS: return XCODE_FILETYPE_TEXT;				// FIXME
	case FILE_TEXT_MARKDOWN: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_TEXT_XML: return XCODE_FILETYPE_TEXT;				// FIXME
	case FILE_ARCHIVE_ARJ: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_ARCHIVE_ZIP: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_ARCHIVE_RAR: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_ARCHIVE_TAR: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_ARCHIVE_TAR_GZ: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_ARCHIVE_TAR_BZ2: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_ARCHIVE_GZIP: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_ARCHIVE_BZIP2: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BUILD_MAKEFILE: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BUILD_CMAKE_SCRIPT: return XCODE_FILETYPE_TEXT;	// FIXME
	case FILE_BUILD_CMAKELISTS: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_WIN32_EXE: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_WIN32_DLL: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_WIN32_RES: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_WIN32_LIB: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_OBJECT: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_SHARED_OBJECT: return XCODE_FILETYPE_TEXT;	// FIXME
	case FILE_BINARY_ARCHIVE: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_APPLE_FRAMEWORK: return XCODE_FILETYPE_WRAPPER_FRAMEWORK;
	case FILE_SOURCE_C: return XCODE_FILETYPE_SOURCECODE_C_C;
	case FILE_SOURCE_CXX: return XCODE_FILETYPE_SOURCECODE_CPP_CPP;
	case FILE_SOURCE_C_HEADER: return XCODE_FILETYPE_SOURCECODE_C_H;
	case FILE_SOURCE_CXX_HEADER: return XCODE_FILETYPE_SOURCECODE_CPP_H;
	case FILE_SOURCE_OBJC: return XCODE_FILETYPE_SOURCECODE_C_OBJC;
	case FILE_SOURCE_OBJCXX: return XCODE_FILETYPE_SOURCECODE_CPP_OBJCPP;
	case FILE_SOURCE_JAVA: return XCODE_FILETYPE_SOURCECODE_JAVA;
	case FILE_SOURCE_JAVASCRIPT: return XCODE_FILETYPE_TEXT;	// FIXME
	case FILE_SOURCE_LUA: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_SOURCE_SHELL: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_SOURCE_PHP: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_SOURCE_PYTHON: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_SOURCE_WIN32_RC: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_SOURCE_WIN32_DEF: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_SOURCE_PLIST: return XCODE_FILETYPE_TEXT_PLIST_XML;
	case FILE_IMAGE_XCASSETS: return XCODE_FILETYPE_FOLDER_ASSETCATALOG;
	case FILE_IMAGE_PNG: return XCODE_FILETYPE_IMAGE_PNG;
	case FILE_IMAGE_JPEG: return XCODE_FILETYPE_IMAGE_JPEG;
	case FILE_IMAGE_GIF: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_IMAGE_BMP: return XCODE_FILETYPE_TEXT;			// FIXME
	}

	assert(false);
	throw std::runtime_error("internal error: invalid file type.");
}

static bool isCompilableFileType(FileType type)
{
	switch (type)
	{
	case FILE_SOURCE_C:
	case FILE_SOURCE_CXX:
	case FILE_SOURCE_OBJC:
	case FILE_SOURCE_OBJCXX:
		return true;
	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build phases

void Gen::createBuildPhases()
{
	frameworksBuildPhase = project->addFrameworksBuildPhase();
	sourcesBuildPhase = project->addSourcesBuildPhase();
	resourcesBuildPhase = project->addResourcesBuildPhase();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Groups

void Gen::createGroups()
{
	mainGroup = project->addGroup();
	project->setMainGroup(mainGroup);

	generatedGroup = project->addGroup();
	generatedGroup->setName("Generated");
	mainGroup->addChild(generatedGroup);

	productsGroup = project->addGroup();
	productsGroup->setName("Products");
	project->setProductRefGroup(productsGroup);
	mainGroup->addChild(productsGroup);

	resourcesGroup = project->addGroup();
	resourcesGroup->setName("Resources");
	mainGroup->addChild(resourcesGroup);

	sourcesGroup = project->addGroup();
	sourcesGroup->setName("Sources");
	mainGroup->addChild(sourcesGroup);
}

XCodeGroup * Gen::groupForPath(XCodeGroup * rootGroup, const std::string & path)
{
	std::pair<XCodeGroup *, std::string> key = std::make_pair(rootGroup, path);
	auto it = dirGroups.find(key);
	if (it != dirGroups.end())
		return it->second;

	std::string dir = pathGetDirectory(path);
	if (dir.length() > 0)
		rootGroup = groupForPath(rootGroup, dir);

	XCodeGroup * childGroup = project->addGroup();
	childGroup->setName(pathGetFileName(path));
	childGroup->setSourceTree("<group>");
	rootGroup->addChild(childGroup);

	dirGroups.insert(std::make_pair(key, childGroup));

	return childGroup;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Source files

void Gen::addSourceFile(XCodeGroup * group, XCodeBuildPhase * phase, const SourceFilePtr & file)
{
	XCodeFileReference * ref = project->addFileReference();
	ref->setPath(file->path());
	ref->setSourceTree("<absolute>");

	// Set file name
	std::string filename = pathGetFileName(file->name());
	if (filename != file->path())
		ref->setName(filename);

	// Set file type
	std::string explicitType = fileTypeForXCode(file->type());
	std::string lastKnownType = fileTypeForXCode(determineFileType(file->path()));
	if (lastKnownType == explicitType)
		ref->setLastKnownFileType(lastKnownType);
	else
		ref->setExplicitFileType(explicitType);

	// Add file into the group
	std::string path = pathGetDirectory(file->name());
	if (path.length() > 0)
		group = groupForPath(group, path);
	group->addChild(ref);

	// Add file to the build phase
	if (isCompilableFileType(file->type()))
	{
		XCodeBuildFile * buildFile = phase->addFile();
		buildFile->setFileRef(ref);
	}
}

void Gen::addSourceFiles()
{
	for (auto it : projectFile->sourceFiles())
	{
		const SourceFilePtr & file = it.second;
		if (!(file->platforms() & (iOS ? Platform::iOS : Platform::OSX)))
			continue;
		addSourceFile(sourcesGroup, sourcesBuildPhase, file);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Configurations

void Gen::initDebugConfiguration()
{
	cfgTargetDebug = project->addTargetBuildConfiguration();
	cfgTargetDebug->setName("Debug");
	cfgTargetDebug->setInfoPListFile(projectName + "/Info.plist");
	cfgTargetDebug->setAssetCatalogAppIconName("AppIcon");
	cfgTargetDebug->setAssetCatalogLaunchImageName("LaunchImage");

	cfgProjectDebug = project->addProjectBuildConfiguration();
	cfgProjectDebug->setName("Debug");
	cfgProjectDebug->setGccOptimizationLevel("0");
	cfgProjectDebug->setIPhoneOSDeploymentTarget("");						// FIXME: make configurable
	cfgProjectDebug->setSDKRoot("iphoneos");
	cfgProjectDebug->setTargetedDeviceFamily("");							// FIXME: make configurable
	cfgProjectDebug->addPreprocessorDefinition("DEBUG=1");
	cfgProjectDebug->setCodeSignIdentity("iphoneos*", "iPhone Developer");	// FIXME: make configurable
}

void Gen::initReleaseConfiguration()
{
	cfgTargetRelease = project->addTargetBuildConfiguration();
	cfgTargetRelease->setName("Release");
	cfgTargetRelease->setInfoPListFile(projectName + "/Info.plist");
	cfgTargetRelease->setAssetCatalogAppIconName("AppIcon");
	cfgTargetRelease->setAssetCatalogLaunchImageName("LaunchImage");

	cfgProjectRelease = project->addProjectBuildConfiguration();
	cfgProjectRelease->setName("Release");
	cfgProjectRelease->setCopyPhaseStrip(true);
	cfgProjectRelease->setEnableNSAssertions(false);
	cfgProjectRelease->setIPhoneOSDeploymentTarget("");						// FIXME: make configurable
	cfgProjectRelease->setOnlyActiveArch(false);
	cfgProjectRelease->setSDKRoot("iphoneos");
	cfgProjectRelease->setTargetedDeviceFamily("");							// FIXME: make configurable
	cfgProjectRelease->setValidateProduct(true);
	cfgProjectRelease->addPreprocessorDefinition("NDEBUG=1");
	cfgProjectRelease->addPreprocessorDefinition("DISABLE_ASSERTIONS=1");
	cfgProjectRelease->setCodeSignIdentity("iphoneos*", "iPhone Developer");// FIXME: make configurable
}

void Gen::createConfigurationLists()
{
	targetCfgList = project->addConfigurationList();
	targetCfgList->setDefaultConfigurationName("Release");
	targetCfgList->addConfiguration(cfgTargetDebug);
	targetCfgList->addConfiguration(cfgTargetRelease);

	projectCfgList = project->addConfigurationList();
	projectCfgList->setDefaultConfigurationName("Release");
	projectCfgList->addConfiguration(cfgProjectDebug);
	projectCfgList->addConfiguration(cfgProjectRelease);
	project->setBuildConfigurationList(projectCfgList);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Preprocessor definitions

void Gen::addDefines()
{
	for (auto it : projectFile->defines())
	{
		const DefinePtr & define = it.second;
		if (!(define->platforms() & (iOS ? Platform::iOS : Platform::OSX)))
			continue;

		std::stringstream ss;
		for (char ch : define->name())
		{
			if (ch != '"')
				ss << ch;
			else
				ss << "\\\"";
		}
		std::string defineName = ss.str();

		if (define->buildTypes() & BuildType::Debug)
			cfgTargetDebug->addPreprocessorDefinition(defineName);
		if (define->buildTypes() & BuildType::Release)
			cfgTargetRelease->addPreprocessorDefinition(defineName);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Native target

void Gen::createNativeTarget()
{
	XCodeFileReference * productRef = project->addFileReference();
	productRef->setExplicitFileType(XCODE_FILETYPE_WRAPPER_APPLICATION);
	productRef->setIncludeInIndex(false);
	productRef->setPath(projectName + ".app");
	productRef->setSourceTree("BUILT_PRODUCTS_DIR");
	productsGroup->addChild(productRef);

	XCodeNativeTarget * target = project->addNativeTarget();
	target->setName(projectName);
	target->setBuildConfigurationList(targetCfgList);
	target->setProductName(projectName);
	target->setProductReference(productRef);
	target->addBuildPhase(sourcesBuildPhase);
	target->addBuildPhase(frameworksBuildPhase);
	target->addBuildPhase(resourcesBuildPhase);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Auxiliary files

void Gen::writeInfoPList()
{
	XCodeFileReference * plistFileRef = project->addFileReference();
	plistFileRef->setLastKnownFileType(XCODE_FILETYPE_TEXT_PLIST_XML);
	plistFileRef->setName("Info.plist");
	plistFileRef->setPath(projectName + "/Info.plist");
	plistFileRef->setSourceTree("SOURCE_ROOT");

	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" "
		"\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
	ss << "<plist version=\"1.0\">\n";
	ss << "<dict>\n";
	ss << "\t<key>CFBundleDevelopmentRegion</key>\n";
	ss << "\t<string>en</string>\n";								// FIXME: make configurable
	ss << "\t<key>CFBundleDisplayName</key>\n";
	ss << "\t<string>${PRODUCT_NAME}</string>\n";					// FIXME: make configurable
	ss << "\t<key>CFBundleExecutable</key>\n";
	ss << "\t<string>${EXECUTABLE_NAME}</string>\n";				// FIXME: make configurable
	ss << "\t<key>CFBundleIdentifier</key>\n";
	ss << "\t<string>com.zapolnov.${PRODUCT_NAME:rfc1034identifier}</string>\n";	// FIXME: make configurable
	ss << "\t<key>CFBundleInfoDictionaryVersion</key>\n";
	ss << "\t<string>6.0</string>\n";								// FIXME: make configurable
	ss << "\t<key>CFBundleName</key>\n";
	ss << "\t<string>${PRODUCT_NAME}</string>\n";					// FIXME: make configurable
	ss << "\t<key>CFBundlePackageType</key>\n";
	ss << "\t<string>APPL</string>\n";
	ss << "\t<key>CFBundleShortVersionString</key>\n";
	ss << "\t<string>1.0</string>\n";								// FIXME: make configurable
	ss << "\t<key>CFBundleSignature</key>\n";
	ss << "\t<string>\?\?\?\?</string>\n";
	ss << "\t<key>CFBundleVersion</key>\n";
	ss << "\t<string>1.0</string>\n";								// FIXME: make configurable
	ss << "\t<key>LSRequiresIPhoneOS</key>\n";
	ss << "\t<true/>\n";
	ss << "\t<key>UIRequiredDeviceCapabilities</key>\n";
	ss << "\t<array>\n";
	ss << "\t	<string>armv7</string>\n";							// FIXME: make configurable
	ss << "\t</array>\n";
	ss << "\t<key>UISupportedInterfaceOrientations</key>\n";		// FIXME: make configurable
	ss << "\t<array>\n";
	ss << "\t\t<string>UIInterfaceOrientationPortrait</string>\n";
	ss << "\t\t<string>UIInterfaceOrientationLandscapeLeft</string>\n";
	ss << "\t\t<string>UIInterfaceOrientationLandscapeRight</string>\n";
	ss << "\t</array>\n";
	ss << "\t<key>UISupportedInterfaceOrientations~ipad</key>\n";	// FIXME: make configurable
	ss << "\t<array>\n";
	ss << "\t\t<string>UIInterfaceOrientationPortrait</string>\n";
	ss << "\t\t<string>UIInterfaceOrientationPortraitUpsideDown</string>\n";
	ss << "\t\t<string>UIInterfaceOrientationLandscapeLeft</string>\n";
	ss << "\t\t<string>UIInterfaceOrientationLandscapeRight</string>\n";
	ss << "\t</array>\n";
	ss << "</dict>\n";
	ss << "</plist>\n";
	projectFile->config()->writeFile(projectName + "/Info.plist", ss.str());
}

void Gen::writeImageAssets()
{
	XCodeFileReference * assetsFileRef = project->addFileReference();
	assetsFileRef->setLastKnownFileType(XCODE_FILETYPE_FOLDER_ASSETCATALOG);
	assetsFileRef->setName("Images.xcassets");
	assetsFileRef->setPath(projectName + "/Images.xcassets");
	assetsFileRef->setSourceTree("SOURCE_ROOT");
	resourcesGroup->addChild(assetsFileRef);

	XCodeBuildFile * buildFile = resourcesBuildPhase->addFile();
	buildFile->setFileRef(assetsFileRef);

	// FIXME: generate actual files
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generating

void Gen::writePBXProj()
{
	projectFile->config()->writeFile(projectName + ".xcodeproj/project.pbxproj", project->toString());
}

void Gen::generate()
{
	projectName = (iOS ? "ios" : "osx");
	projectPath = pathConcat(projectFile->config()->path(), projectName) + ".xcodeproj";

	project = std::make_shared<XCodeProject>();
	project->setOrganizationName("");										// FIXME: make configurable

	createBuildPhases();
	createGroups();
	addSourceFiles();
	initDebugConfiguration();
	initReleaseConfiguration();
	addDefines();
	createConfigurationLists();
	createNativeTarget();

	writeInfoPList();
	writeImageAssets();
	writePBXProj();
}

std::string generateXCode(const ProjectFilePtr & projectFile, bool iOS)
{
	Gen gen;
	gen.projectFile = projectFile;
	gen.iOS = iOS;
	gen.generate();
	return gen.projectPath;
}
