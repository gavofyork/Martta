function currentChanged(_oldId)
{
	if (document.getElementById(_oldId).id == _oldId)
		CodeView.onCurrentChanged(_oldId);
	else
		CodeView.onCurrentChanged(0);
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
function entityParent(_e)
{
	var p = _e.parentNode;
	while (p != null && p.getAttribute('entity') != 'true')
		p = p.parentNode;
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
function goPrevious()
{
	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)
		return;
	var old = g_currentIterator.referenceNode;
	var oldId = old.parentNode.id;
	CodeView.onCurrentAboutToChange();
	g_currentIterator.previousNode();
	if (g_currentIterator.referenceNode == old)
		g_currentIterator.previousNode();
	ensureViewable(g_currentIterator.referenceNode.parentNode);
	currentChanged(oldId);
}
function goNext()
{
	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)
		return;
	var old = g_currentIterator.referenceNode;
	var oldId = old.parentNode.id;
	CodeView.onCurrentAboutToChange();
	g_currentIterator.nextNode();
	if (g_currentIterator.referenceNode == old)
		g_currentIterator.nextNode();
	ensureViewable(g_currentIterator.referenceNode.parentNode);
	currentChanged(oldId);
}
function goUp()
{
	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)
		return;
	var otop = g_currentIterator.referenceNode.parentNode.getBoundingClientRect().top;
	var oldId = g_currentIterator.referenceNode.parentNode.id;
	CodeView.onCurrentAboutToChange();
	while (g_currentIterator.referenceNode.parentNode.getBoundingClientRect().bottom > otop + 1)
		if (g_currentIterator.previousNode() == null)
			break;
	ensureViewable(g_currentIterator.referenceNode.parentNode);
	currentChanged(oldId);
}
function goDown()
{
	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)
		return;
	var obottom = g_currentIterator.referenceNode.parentNode.getBoundingClientRect().bottom;
	var oldId = g_currentIterator.referenceNode.parentNode.id;
	CodeView.onCurrentAboutToChange();
	while (g_currentIterator.referenceNode.parentNode.getBoundingClientRect().top < obottom - 1)
		if (g_currentIterator.nextNode() == null)
			break;
	ensureViewable(g_currentIterator.referenceNode.parentNode);
	currentChanged(oldId);
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
	var oldId = (g_currentIterator.referenceNode && g_currentIterator.referenceNode.parentNode && document.getElementById(g_currentIterator.referenceNode.parentNode.id)) ? g_currentIterator.referenceNode.parentNode.id : '';
	if (setReferenceNode(_e, true))
		currentChanged(oldId);
}
function setCurrentById(_id)
{
	var e = document.getElementById(_id);
	setCurrent(e);
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
function navigateInto(_centre)
{
	/// Selects _centre's leftmost, innermost focusable child. e.g. X on ()s: (++X + 4)
	var e = document.getElementById(_centre);
	if (!e)
		return;
	var oldId = g_currentIterator.referenceNode && g_currentIterator.referenceNode.parentNode ? g_currentIterator.referenceNode.parentNode.id : '';
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
			currentChanged(oldId);
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

	var oldId = g_currentIterator.referenceNode && g_currentIterator.referenceNode.parentNode ? g_currentIterator.referenceNode.parentNode.id : '';
	if (setReferenceNode(it.referenceNode.parentNode, true))
	{
		ensureViewable(g_currentIterator.referenceNode.parentNode);
		currentChanged(oldId);
	}
}
function navigateToNew(_from)
{
	/// Selects closest focusable sibling-owned entity visually forwards from _from, or parent if none.
	var e = document.getElementById(_from);

	var oldId = g_currentIterator.referenceNode && g_currentIterator.referenceNode.parentNode ? g_currentIterator.referenceNode.parentNode.id : '';
	if (setReferenceNode(e, true))
	{
		while (hasAncestor(g_currentIterator.referenceNode, e))
			g_currentIterator.nextNode();
		if (!hasAncestor(g_currentIterator.referenceNode, entityParent(e)))
			setReferenceNode(entityParent(e), false);
		ensureViewable(g_currentIterator.referenceNode.parentNode);
		currentChanged(oldId);
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
	var p = document.getElementById(_parent);
	var it = document.createNodeIterator(p, NodeFilter.SHOW_ELEMENT, onlyThese, false);
	var bestx;
	var besty;
	while (it.nextNode())
	{
		if (entityParent(it.referenceNode.parentNode) == p)
		{
			var r = it.referenceNode.parentNode.getBoundingClientRect();
			if (r.top <= _y && r.bottom >= _y)
			{
				besty = it.referenceNode.parentNode;
				// Y exact.
				if (r.left <= _x && r.right >= _x)
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
