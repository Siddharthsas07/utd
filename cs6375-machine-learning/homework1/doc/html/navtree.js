var NAVTREE =
[
  [ "Decision Tree", "index.html", [
    [ "Class List", "annotated.html", [
      [ "CStopWatch", "class_c_stop_watch.html", null ],
      [ "DataSet< T >", "class_data_set.html", null ],
      [ "DecisionTree< T >", "class_decision_tree.html", null ],
      [ "DNode< T >", "class_d_node.html", null ],
      [ "DTreeHeuristic< T >", "class_d_tree_heuristic.html", null ],
      [ "h_1StepLookAhead< T >", "classh__1_step_look_ahead.html", null ],
      [ "h_InfoGain< T >", "classh___info_gain.html", null ],
      [ "h_VarianceImpurity< T >", "classh___variance_impurity.html", null ],
      [ "stop_watch", "structstop__watch.html", null ]
    ] ],
    [ "Class Index", "classes.html", null ],
    [ "Class Hierarchy", "hierarchy.html", [
      [ "CStopWatch", "class_c_stop_watch.html", null ],
      [ "DataSet< T >", "class_data_set.html", null ],
      [ "DecisionTree< T >", "class_decision_tree.html", null ],
      [ "DNode< T >", "class_d_node.html", null ],
      [ "DTreeHeuristic< T >", "class_d_tree_heuristic.html", [
        [ "h_1StepLookAhead< T >", "classh__1_step_look_ahead.html", null ],
        [ "h_InfoGain< T >", "classh___info_gain.html", null ],
        [ "h_VarianceImpurity< T >", "classh___variance_impurity.html", null ]
      ] ],
      [ "stop_watch", "structstop__watch.html", null ]
    ] ],
    [ "Class Members", "functions.html", null ],
    [ "File List", "files.html", [
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6375/homework1/homework1/d_tree_heuristic.h", null, null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6375/homework1/homework1/dataset.h", null, null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6375/homework1/homework1/dataset.hpp", null, null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6375/homework1/homework1/decision_tree.h", null, null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6375/homework1/homework1/decision_tree.hpp", null, null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6375/homework1/homework1/dnode.h", null, null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6375/homework1/homework1/h_1step_lookahead.h", null, null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6375/homework1/homework1/h_infogain.h", null, null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6375/homework1/homework1/h_variance_impurity.h", null, null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6375/homework1/homework1/hr_timer.cpp", "hr__timer_8cpp.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6375/homework1/homework1/hr_timer.h", "hr__timer_8h.html", null ]
    ] ]
  ] ]
];

function createIndent(o,domNode,node,level)
{
  if (node.parentNode && node.parentNode.parentNode)
  {
    createIndent(o,domNode,node.parentNode,level+1);
  }
  var imgNode = document.createElement("img");
  if (level==0 && node.childrenData)
  {
    node.plus_img = imgNode;
    node.expandToggle = document.createElement("a");
    node.expandToggle.href = "javascript:void(0)";
    node.expandToggle.onclick = function() 
    {
      if (node.expanded) 
      {
        $(node.getChildrenUL()).slideUp("fast");
        if (node.isLast)
        {
          node.plus_img.src = node.relpath+"ftv2plastnode.png";
        }
        else
        {
          node.plus_img.src = node.relpath+"ftv2pnode.png";
        }
        node.expanded = false;
      } 
      else 
      {
        expandNode(o, node, false);
      }
    }
    node.expandToggle.appendChild(imgNode);
    domNode.appendChild(node.expandToggle);
  }
  else
  {
    domNode.appendChild(imgNode);
  }
  if (level==0)
  {
    if (node.isLast)
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2plastnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2lastnode.png";
        domNode.appendChild(imgNode);
      }
    }
    else
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2pnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2node.png";
        domNode.appendChild(imgNode);
      }
    }
  }
  else
  {
    if (node.isLast)
    {
      imgNode.src = node.relpath+"ftv2blank.png";
    }
    else
    {
      imgNode.src = node.relpath+"ftv2vertline.png";
    }
  }
  imgNode.border = "0";
}

function newNode(o, po, text, link, childrenData, lastNode)
{
  var node = new Object();
  node.children = Array();
  node.childrenData = childrenData;
  node.depth = po.depth + 1;
  node.relpath = po.relpath;
  node.isLast = lastNode;

  node.li = document.createElement("li");
  po.getChildrenUL().appendChild(node.li);
  node.parentNode = po;

  node.itemDiv = document.createElement("div");
  node.itemDiv.className = "item";

  node.labelSpan = document.createElement("span");
  node.labelSpan.className = "label";

  createIndent(o,node.itemDiv,node,0);
  node.itemDiv.appendChild(node.labelSpan);
  node.li.appendChild(node.itemDiv);

  var a = document.createElement("a");
  node.labelSpan.appendChild(a);
  node.label = document.createTextNode(text);
  a.appendChild(node.label);
  if (link) 
  {
    a.href = node.relpath+link;
  } 
  else 
  {
    if (childrenData != null) 
    {
      a.className = "nolink";
      a.href = "javascript:void(0)";
      a.onclick = node.expandToggle.onclick;
      node.expanded = false;
    }
  }

  node.childrenUL = null;
  node.getChildrenUL = function() 
  {
    if (!node.childrenUL) 
    {
      node.childrenUL = document.createElement("ul");
      node.childrenUL.className = "children_ul";
      node.childrenUL.style.display = "none";
      node.li.appendChild(node.childrenUL);
    }
    return node.childrenUL;
  };

  return node;
}

function showRoot()
{
  var headerHeight = $("#top").height();
  var footerHeight = $("#nav-path").height();
  var windowHeight = $(window).height() - headerHeight - footerHeight;
  navtree.scrollTo('#selected',0,{offset:-windowHeight/2});
}

function expandNode(o, node, imm)
{
  if (node.childrenData && !node.expanded) 
  {
    if (!node.childrenVisited) 
    {
      getNode(o, node);
    }
    if (imm)
    {
      $(node.getChildrenUL()).show();
    } 
    else 
    {
      $(node.getChildrenUL()).slideDown("fast",showRoot);
    }
    if (node.isLast)
    {
      node.plus_img.src = node.relpath+"ftv2mlastnode.png";
    }
    else
    {
      node.plus_img.src = node.relpath+"ftv2mnode.png";
    }
    node.expanded = true;
  }
}

function getNode(o, po)
{
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) 
  {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
        i==l);
  }
}

function findNavTreePage(url, data)
{
  var nodes = data;
  var result = null;
  for (var i in nodes) 
  {
    var d = nodes[i];
    if (d[1] == url) 
    {
      return new Array(i);
    }
    else if (d[2] != null) // array of children
    {
      result = findNavTreePage(url, d[2]);
      if (result != null) 
      {
        return (new Array(i).concat(result));
      }
    }
  }
  return null;
}

function initNavTree(toroot,relpath)
{
  var o = new Object();
  o.toroot = toroot;
  o.node = new Object();
  o.node.li = document.getElementById("nav-tree-contents");
  o.node.childrenData = NAVTREE;
  o.node.children = new Array();
  o.node.childrenUL = document.createElement("ul");
  o.node.getChildrenUL = function() { return o.node.childrenUL; };
  o.node.li.appendChild(o.node.childrenUL);
  o.node.depth = 0;
  o.node.relpath = relpath;

  getNode(o, o.node);

  o.breadcrumbs = findNavTreePage(toroot, NAVTREE);
  if (o.breadcrumbs == null)
  {
    o.breadcrumbs = findNavTreePage("index.html",NAVTREE);
  }
  if (o.breadcrumbs != null && o.breadcrumbs.length>0)
  {
    var p = o.node;
    for (var i in o.breadcrumbs) 
    {
      var j = o.breadcrumbs[i];
      p = p.children[j];
      expandNode(o,p,true);
    }
    p.itemDiv.className = p.itemDiv.className + " selected";
    p.itemDiv.id = "selected";
    $(window).load(showRoot);
  }
}

