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

#include <QRect>
#include <QRectF>
#include <QPainter>
#include <QPicture>
#include <QFontMetrics>

#include "Common.h"

namespace Martta
{
	
static const QRectF BigRect(0.f, 0.f, 1.e99, 1.e99);

inline QColor qRgba(QRgb _v, int _alpha)
{
	return QColor(qRed(_v), qGreen(_v), qBlue(_v), _alpha);
}

inline QRect QRF2QR(QRectF const& _r)
{
	return QRect((int)_r.x(), (int)_r.y(), (int)_r.width(), (int)_r.height());
}

inline QRectF alignedForUnitPen(QRectF const& _r)
{
	return QRectF(floor(_r.x()) + 0.5f, floor(_r.y()) + 0.5f, ceil(_r.width() - 1.f), ceil(_r.height() - 1.f));
}

inline QPointF round(QPointF const& _p)
{
	return QPointF(::round(_p.x()), ::round(_p.y()));
}

inline QRectF operator++(QRectF& _r, int)
{
	QRectF ret = _r;
	_r = QRectF(_r.x() - 1.f, _r.y() - 1.f, _r.width() + 2.f, _r.height() + 2.f);
	return ret;
}

inline QRectF operator--(QRectF& _r, int)
{
	QRectF ret = _r;
	_r = QRectF(_r.x() + 1.f, _r.y() + 1.f, _r.width() - 2.f, _r.height() - 2.f);
	return ret;
}

inline QRectF& operator++(QRectF& _r)
{
	return (_r = QRectF(_r.x() - 1.f, _r.y() - 1.f, _r.width() + 2.f, _r.height() + 2.f));
}

inline QRectF& operator--(QRectF& _r)
{
	return (_r = QRectF(_r.x() + 1.f, _r.y() + 1.f, _r.width() - 2.f, _r.height() - 2.f));
}

inline QRectF operator+(QRectF const& _a, float _x)
{
	return QRectF(_a.x() - _x, _a.y() - _x, _a.width() + 2.f * _x, _a.height() + 2.f * _x);
}

inline QRectF operator-(QRectF const& _a, float _x)
{
	return operator+(_a, -_x);
}

inline QRectF& operator +=(QRectF& _r, float _x)
{
	return _r = _r + _x;
}

inline QRectF& operator -=(QRectF& _r, float _x)
{
	return operator+=(_r, -_x);
}

inline QRectF operator>>(QRectF const& _a, float _x)
{
	return QRectF(_a.x() + _x, _a.y() + _x, _a.width(), _a.height());
}

inline QRectF operator<<(QRectF const& _a, float _x)
{
	return operator>>(_a, -_x);
}

inline QRectF operator>>=(QRectF& _a, float _x)
{
	return _a = _a + _x;
}

inline QRectF operator<<=(QRectF& _a, float _x)
{
	return operator>>=(_a, -_x);
}

inline void drawRoundRect(QPainter* _p, QRectF _r, float _o = 50.f)
{
	_p->drawRoundRect(_r, int(_r.height() * _o / qMax(_r.width(), _r.height())), int(_r.width() * _o / qMax(_r.width(), _r.height())));
}

inline QSizeF textSize(QString const& _t, QFont _f = QFont())
{
	QPicture pic;
	QPainter p(&pic);
	p.setFont(_f);
	return p.boundingRect(BigRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextSingleLine, _t).size();
//	return QFontMetrics(_f).boundingRect(_t).size();
}

inline void drawToken(QPainter* _p, QRectF _r, float _o = 50.f)
{
	_p->drawRoundRect(alignedForUnitPen(_r), int(_o * _r.height() / _r.width()), int(_o));
}

inline QSizeF bringToToken(QSizeF const& _s, float _o = 50.f)
{
	return QSizeF(_s.width() < _s.height() / 100.f * _o ? _s.height() / 100.f * _o : _s.width(), _s.height());
}

inline QSizeF tokenSize(QString const& _t, float _o = 50.f)
{
	return bringToToken(textSize(_t) + QSizeF(8.f, 0.f), _o);
}

}
