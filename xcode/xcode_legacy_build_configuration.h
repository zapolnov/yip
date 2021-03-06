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
#ifndef __32df4be0f91ed6b746f3562d224430f7__
#define __32df4be0f91ed6b746f3562d224430f7__

#include "xcode_build_configuration.h"
#include <vector>
#include <string>

class XCodeProject;

class XCodeLegacyBuildConfiguration : public XCodeBuildConfiguration
{
public:
	inline const std::string & productName() const { return m_ProductName; }
	inline void setProductName(const std::string & name_) { m_ProductName = name_; }

protected:
	void writeBuildSettings(std::stringstream & ss) const;

private:
	std::string m_ProductName;

	XCodeLegacyBuildConfiguration();
	~XCodeLegacyBuildConfiguration();

	friend class XCodeProject;
};

#endif
