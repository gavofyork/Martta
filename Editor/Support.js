function bounds(_id)
{
	var bb = document.getElementById(_id).getBoundingClientRect();
	return bb.left + ' ' + bb.top + ' ' + bb.width + ' ' + bb.height;
}
function currentChanged()
{
	CodeView.onCurrentChanged();
}
function isInvisible(node)
{
	var n = node;
	while (n)
		if (n.style && n.style.display == 'none')
			return true;
		else
			n = n.parentNode;
	return false;
}
function ensureViewable(_e)
{
	if (_e.getBoundingClientRect().top - 10 < 0)
		window.scroll(window.scrollX, window.scrollY + _e.getBoundingClientRect().top - 10);
	if (_e.getBoundingClientRect().bottom + 20 > window.innerHeight)
		window.scroll(window.scrollX, _e.getBoundingClientRect().bottom + window.scrollY - window.innerHeight + 20);
}
function onlyThese(node)
{
	if (isInvisible(node))
		return NodeFilter.FILTER_REJECT;
	if (node.id == 'this')
		return NodeFilter.FILTER_ACCEPT;
	return NodeFilter.FILTER_SKIP;
}
var g_currentIterator = document.createNodeIterator(document, NodeFilter.SHOW_ELEMENT, onlyThese, false);
var g_inOutCache = new Array();
String.prototype.escapeHTML =
	function ()
	{
		return this.replace(/&/g,'&amp;').replace(/>/g,'&gt;').replace(/</g,'&lt;').replace(/\"/g,'&quot;');
	};
function thisNode(_e)
{
	var children = _e.childNodes;
	for (var i = 0; i < children.length; i++)
		if (children[i].id == 'this')
			return children[i];
	return null;
}
function hasAncestor(_c, _a)
{
	var e = _c;
	while (e != null)
		if (e == _a)
			return true;
		else
			e = e.parentNode;
	return false;
}
function entityParent(_e)
{
	var p = _e.parentNode;
	while (p != null && p.getAttribute('entity') != 'true')
		p = p.parentNode;
	return p;
}
function focusableEntityParent(_e)
{
	var p = _e.parentNode;
	for (; p != null; p = p.parentNode)
		if (thisNode(p) && p.getAttribute('entity') == 'true')
			break;
	return p;
}
function routeKeyPress(_e, _k)
{
	if (_e.onkeypress != null && _e.onkeypress(_k))
		return true;
	var nodes = _e.childNodes;
	for (var i = 0; i < nodes.length; i++)
		if (!nodes[i].getAttribute('entity') && routeKeyPress(nodes[i], _k))
			return true;
	return false;
}
function goIn()
{
	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)
		return;
	var e = g_currentIterator.referenceNode.parentNode;
	var c;
	if (g_inOutCache[e.id] && document.getElementById(g_inOutCache[e.id]) && focusableEntityParent(document.getElementById(g_inOutCache[e.id])) == e)
	{
		c = document.getElementById(g_inOutCache[e.id]);
	}
	else
	{
		var it = document.createNodeIterator(e, NodeFilter.SHOW_ELEMENT, function(node) { if (isInvisible(node)) return NodeFilter.FILTER_REJECT; if (node.id == 'this' && entityParent(node.parentNode) == e) return NodeFilter.FILTER_ACCEPT; return NodeFilter.FILTER_SKIP; }, false);
		if (it.nextNode() == null)
			return;
		c = it.referenceNode.parentNode;
	}
	if (c && setReferenceNode(c))
		currentChanged();
}
function goOut()
{
	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)
		return;
	var e = g_currentIterator.referenceNode.parentNode;
	var p = focusableEntityParent(g_currentIterator.referenceNode.parentNode);
	if (p && setReferenceNode(p))
	{
		g_inOutCache[p.id] = e.id;
		currentChanged();
	}
}
function goPrevious()
{
	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)
		return;
	var old = g_currentIterator.referenceNode;
	CodeView.onCurrentAboutToChange();
	g_currentIterator.previousNode();
	if (g_currentIterator.referenceNode == old)
		g_currentIterator.previousNode();
	ensureViewable(g_currentIterator.referenceNode.parentNode);
	currentChanged();
}
function goNext()
{
	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)
		return;
	var old = g_currentIterator.referenceNode;
	CodeView.onCurrentAboutToChange();
	g_currentIterator.nextNode();
	if (g_currentIterator.referenceNode == old)
		g_currentIterator.nextNode();
	ensureViewable(g_currentIterator.referenceNode.parentNode);
	currentChanged();
}
function goVert(_d)
{
	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)
		return;

	var orig = g_currentIterator.referenceNode.parentNode.getBoundingClientRect();

	function top(_r) { return _d ? _r.top : -_r.bottom; }
	function bottom(_r) { return _d ? _r.bottom : -_r.top; }
	function overlapBonus(_r) { return (_r.left < orig.right || orig.left < _r.right) ? 0.1 : 0; }
	function dx(_r) { return Math.abs(_r.left + _r.right - orig.left - orig.right) * 2 + Math.abs(_r.right - _r.left - orig.right + orig.left) - overlapBonus(_r); }
	function goOn() { return _d ? g_currentIterator.nextNode() : g_currentIterator.previousNode(); }
	function backup() { return _d ? g_currentIterator.previousNode() : g_currentIterator.nextNode(); }
	function curTop() { return top(g_currentIterator.referenceNode.parentNode.getBoundingClientRect()); }
	function curBottom() { return bottom(g_currentIterator.referenceNode.parentNode.getBoundingClientRect()); }
	function curDx() { return dx(g_currentIterator.referenceNode.parentNode.getBoundingClientRect()); }

	CodeView.onCurrentAboutToChange();

	while (curTop() < bottom(orig) && goOn()) {}

	if (g_currentIterator.referenceNode)
	{
		var bestBottom = curBottom();
		var bestDx = curDx();
		var extras = 1;
		while (goOn())
		{
			extras++;
			if (curTop() >= bestBottom)
				break;
			else if (curBottom() < bestBottom)
			{
				bestBottom = curBottom();
				bestDx = curDx();
				extras = 1;
			}
			else if (curDx() < bestDx && curBottom() <= bestBottom)
			{
				bestDx = curDx();
				extras = 1;
			}
		}
		for (var i = 0; i < extras; i++)
			backup();
		ensureViewable(g_currentIterator.referenceNode.parentNode);
	}

	currentChanged();
}
function goDown()
{
	goVert(1);
}
function goUp()
{
	goVert(0);
}
function thisParent(_e)
{
	var t = _e.parentNode;
	while (t)
	{
		if (thisNode(t) != null)
			break;
		t = t.parentNode;
	}
	return t;
}
function setReferenceNode(_e, _n)
{
	var t = _e;
	while (t)
	{
		if (thisNode(t) != null && !isInvisible(t))
			break;
		t = t.parentNode;
	}
	if (!t && document.getElementById(g_currentIterator.referenceNode.parentNode.id))
		return false;
	ensureViewable(t);
	if (_n)
		CodeView.onCurrentAboutToChange();
	g_currentIterator.detach();
	g_currentIterator = document.createNodeIterator(document, NodeFilter.SHOW_ELEMENT, onlyThese, false);
	while (g_currentIterator.nextNode() != null)
		if (g_currentIterator.referenceNode.parentNode == t || !t)
			return true;
	alert('Couldn\'t navigate to ' + _e.id + ' (' + t.id + ') though exists in document: ' + document.getElementById(t.id) + ' and has this child ' + thisNode(t) + ' and is visible: ' + (!isInvisible(t)));
	return false;
}
function setCurrent(_e)
{
	if (setReferenceNode(_e, true))
		currentChanged();
}
function setCurrentById(_id)
{
	var e = document.getElementById(_id);
	setCurrent(e);
}
function navigateInto(_centre)
{
	/// Selects _centre's leftmost, innermost focusable child. e.g. X on ()s: (++X + 4)
	var e = document.getElementById(_centre);
	if (!e)
		return;
	CodeView.onCurrentAboutToChange();
	g_currentIterator.detach();
	g_currentIterator = document.createNodeIterator(document, NodeFilter.SHOW_ELEMENT, onlyThese, false);
	while (g_currentIterator.nextNode() != null)
		if (hasAncestor(g_currentIterator.referenceNode.parentNode, e))
			break;
	while (true)
	{
		var lastNodeParent = g_currentIterator.referenceNode.parentNode;
		if (g_currentIterator.nextNode() == null || !hasAncestor(g_currentIterator.referenceNode.parentNode, lastNodeParent))
		{
			while (g_currentIterator.referenceNode.parentNode != lastNodeParent)
				g_currentIterator.previousNode();
			ensureViewable(g_currentIterator.referenceNode.parentNode);
			currentChanged();
			return;
		}
	}
}
function navigateOnto(_shell)
{
	/// Selects _shell's leftmost focusable child. e.g. ++X on ()s: (++X + 4)
	var e = document.getElementById(_shell);
	if (!e)
		return;
	var it = document.createNodeIterator(e, NodeFilter.SHOW_ELEMENT, function(node) { if (isInvisible(node)) return NodeFilter.FILTER_REJECT; if (node.id == 'this' && (entityParent(node.parentNode) == e || thisNode(e) == node)) return NodeFilter.FILTER_ACCEPT; return NodeFilter.FILTER_SKIP; }, false);
	if (it.nextNode() == null)
		return;

	if (setReferenceNode(it.referenceNode.parentNode, true))
	{
		ensureViewable(g_currentIterator.referenceNode.parentNode);
		currentChanged();
	}
}
function navigateToNew(_from)
{
	/// Selects closest focusable sibling-or-self-owned entity visually forwards from _from, or parent if none.
	var e = document.getElementById(_from);

	if (setReferenceNode(e, true))
	{
//		while (hasAncestor(g_currentIterator.referenceNode, e))
			g_currentIterator.nextNode();
		if (!hasAncestor(g_currentIterator.referenceNode, entityParent(e)))
			setReferenceNode(entityParent(e), false);
		ensureViewable(g_currentIterator.referenceNode.parentNode);
		currentChanged();
	}
}
function getAttribs(_e, _a)
{
	var ns = _e.childNodes;
	for (var i = 0; i < ns.length; i++)
	{
		if (ns[i].getAttribute && ns[i].getAttribute('entity'))
			continue;
		if (ns[i].id && ns[i].attributes && ns[i].id != 'this')
			_a[ns[i].id] = ns[i].attributes;
		if (ns[i].attributes)
			getAttribs(ns[i], _a);
	}
}
function changeContent(_id, _html)
{
	var e = document.getElementById(_id);
	var s = new Array();
	getAttribs(e, s);
	e.innerHTML = _html;
	for (var i in s)
		if (document.getElementById(i))
			for (var j = 0; j < s[i].length; j++)
				document.getElementById(i).setAttribute(s[i][j].name, s[i][j].value);
}
function toggle(_id1, _id2)
{
	var a = document.getElementById(_id1);
	var b = document.getElementById(_id2);
	a.style.display = a.style.display == 'none' ? null : 'none';
	b.style.display = b.style.display == 'none' ? null : 'none';
	setCurrentWithin(a.style.display == 'none' ? b : a);
}
function setCurrentWithin(_e)
{
	var iter = document.createNodeIterator(_e, NodeFilter.SHOW_ELEMENT, onlyThese, false);
	if (iter.nextNode() != null)
	{
		setCurrent(iter.referenceNode.parentNode);
		return true;
	}
	setCurrent(_e);
	return false;
}
function set1(_id1, _id2)
{
	var a = document.getElementById(_id1);
	var b = document.getElementById(_id2);
	if (b.style.display == 'none')
		return false;
	a.style.display = null;
	b.style.display = 'none';
	setCurrentWithin(a);
	return true;
}
function set2(_id1, _id2)
{
	var a = document.getElementById(_id1);
	var b = document.getElementById(_id2);
	if (a.style.display == 'none')
		return false;
	a.style.display = 'none';
	b.style.display = null;
	setCurrentWithin(b);
	return true;
}
function procMouseDown(event)
{
	setCurrent(event.target);
}
function changeEditContent(_e, _c)
{
	var e = document.getElementById(_e);
	thisNode(e).outerHTML = _c;
	setReferenceNode(e, false);
}
function restoreCurrent(_x, _y, _parent)
{
	var p = _parent ? document.getElementById(_parent) : document;
	var it = document.createNodeIterator(p, NodeFilter.SHOW_ELEMENT, onlyThese, false);
	var bestx;
	var besty;
	while (it.nextNode())
	{
		if (entityParent(it.referenceNode.parentNode) == p || !_parent)
		{
			var r = it.referenceNode.parentNode.getBoundingClientRect();
			if (r.top <= _y && r.bottom >= _y)
			{
				besty = it.referenceNode.parentNode;
				// Y exact.
				if (r.left <= _x && r.right >= _x && _parent)
				{
					// X exact.
					bestx = it.referenceNode.parentNode;
					break;
				}
				else if (r.left >= _x)
				{
					// Overshot on X
					if (!bestx)
						bestx = it.referenceNode.parentNode;
					break;
				}
				else // Not far enough on X
					bestx = it.referenceNode.parentNode;
			}
			else if (r.top >= _y)
			{
				// Overshot on Y
				if (!besty)
					besty = it.referenceNode.parentNode;
				break;
			}
			else // Not far enough on Y
				besty = it.referenceNode.parentNode;
		}
	}
	if (bestx)
		setCurrent(bestx);
	else if (besty)
		setCurrent(besty);
	else
		setCurrent(p);
}
g_currentIterator.nextNode();
