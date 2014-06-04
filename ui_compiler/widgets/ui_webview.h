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
#ifndef __8e3344a5e83271c06f118b16e6f90ca2__
#define __8e3344a5e83271c06f118b16e6f90ca2__

#include "ui_widget.h"
#include <vector>

class UIWebView : public UIWidget
{
public:
	UIWebView(UILayout * layout, UIGroup * parentGroup);
	~UIWebView();

	// In ios_generator.cpp
	const char * iosClassName() const override { return "UIWebView"; }
	void iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss) override;
	void iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss) override;

	// In android_generator.cpp
	const char * androidClassName() const override { return "WebView"; }

protected:
	bool parseAttribute(const TiXmlAttribute * attr) override;

private:
	UIWebView(const UIWebView &) = delete;
	UIWebView & operator=(const UIWebView &) = delete;
};

#endif