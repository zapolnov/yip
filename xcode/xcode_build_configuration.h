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
#ifndef __34005306ac7d3391afe4aae2d0a45951__
#define __34005306ac7d3391afe4aae2d0a45951__

#include "xcode_object.h"
#include <sstream>

class XCodeProject;

class XCodeBuildConfiguration : public XCodeObject
{
public:
	std::string objectName() const { return m_Name; }

	inline const std::string & name() const { return m_Name; }
	inline void setName(const std::string & name_) { m_Name = name_; }

	std::string toString() const;

protected:
	XCodeBuildConfiguration();
	~XCodeBuildConfiguration();

	virtual void writeBuildSettings(std::stringstream & ss) const = 0;

private:
	std::string m_Name;

	friend class XCodeProject;
};

#endif
