/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version 1.0
 * (the "License"); you may not use this file except in compliance with the
 * License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations under
 * the License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#pragma once

#include <msString.h>
using namespace MarttaSupport;

#include "Meta.h"

namespace Martta
{

/*
#ifdef Q_WS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

void updateRootSupportPath()
{
#ifdef Q_WS_MAC
	CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
	root->setSupportPath(String(CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding())) + L"/Support/");
	CFRelease(appUrlRef);
	CFRelease(macPath);
#endif
#ifdef Q_WS_WIN
	root->setSupportPath(qs(QCoreApplication::applicationDirPath() + "/Support/"));
#endif
#ifdef Q_WS_X11
	root->setSupportPath(qs(QCoreApplication::applicationDirPath() + "/../support/"));
#endif
}
*/

class Solution
{
	MARTTA_INTERFACE

public:
	virtual ~Solution() {}
	String const& supportPath() const { return m_supportPath; }
	virtual void setSupportPath(String const& _p) { m_supportPath = _p; }
	virtual void initialiseNew() {}

protected:
	String			m_supportPath;
};

}
