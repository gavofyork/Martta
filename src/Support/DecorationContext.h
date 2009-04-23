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

#include <QList>
#include <QRectF>

class QPainter;

namespace Martta
{

class DecorationContext
{
public:
	inline DecorationContext(QPainter* _p, QRectF const& _s, QList<QRectF> const& _c): m_p(_p), m_size(_s), m_captured(_c) {}
	
	inline QPainter*		operator->() const { return m_p; }
	inline QRectF			operator()(int _i = 0) const { return cap(_i); }
	inline QRectF			cap(int _i) const { return (_i && _i <= m_captured.size()) ? m_captured[_i - 1] : m_size; }
	
private:
	QPainter*				m_p;
	QRectF					m_size;
	QList<QRectF>			m_captured;
};

}
