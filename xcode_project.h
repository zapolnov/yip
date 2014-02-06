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
#ifndef __c2f59d4a58e379e2289a28376e51f150__
#define __c2f59d4a58e379e2289a28376e51f150__

#include "xcode_object.h"
#include "xcode_group.h"
#include "xcode_native_target.h"
#include <set>
#include <string>
#include <vector>

class XCodeProject : public XCodeObject
{
public:
	XCodeProject();
	~XCodeProject();

	std::string objectName() const { return "Project object"; }

	inline const std::string & organizationName() const { return m_OrganizationName; }
	inline void setOrganizationName(const std::string & name) { m_OrganizationName = name; }

	inline const std::string & developmentRegion() const { return m_DevelopmentRegion; }
	inline void setDevelopmentRegion(const std::string & reg) { m_DevelopmentRegion = reg; }

	inline void addKnownRegion(const std::string & reg) { m_KnownRegions.insert(reg); }
	inline void removeKnownRegion(const std::string & reg) { m_KnownRegions.erase(reg); }

	inline const std::string & projectDirPath() const { return m_ProjectDirPath; }
	inline void setProjectDirPath(const std::string & path) { m_ProjectDirPath = path; }

	inline const std::string & projectRoot() const { return m_ProjectRoot; }
	inline void setProjectRoot(const std::string & path) { m_ProjectRoot = path; }

	XCodeGroup * addGroup();

	inline XCodeGroup * mainGroup() const { return m_MainGroup; }
	inline void setMainGroup(XCodeGroup * group) { m_MainGroup = group; }

	inline XCodeGroup * productRefGroup() const { return m_ProductRefGroup; }
	inline void setProductRefGroup(XCodeGroup * group) { m_ProductRefGroup = group; }

	std::string toString() const;

private:
	std::string m_OrganizationName;
	std::string m_DevelopmentRegion;
	std::string m_ProjectDirPath;
	std::string m_ProjectRoot;
	std::set<std::string> m_KnownRegions;
	XCodeGroup * m_MainGroup;
	XCodeGroup * m_ProductRefGroup;
	std::vector<XCodeGroup *> m_Groups;
	std::vector<XCodeNativeTarget *> m_NativeTargets;
};

#endif
