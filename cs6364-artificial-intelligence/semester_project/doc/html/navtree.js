var NAVTREE =
[
  [ "UnoAI", "index.html", [
    [ "Class List", "annotated.html", [
      [ "CStopWatch", "class_c_stop_watch.html", null ],
      [ "stop_watch", "structstop__watch.html", null ],
      [ "Uno_Action", "class_uno___action.html", null ],
      [ "Uno_AI_Player", "class_uno___a_i___player.html", null ],
      [ "Uno_AI_Search_Node", "class_uno___a_i___search___node.html", null ],
      [ "Uno_GState", "class_uno___g_state.html", null ],
      [ "Uno_Human_Player", "class_uno___human___player.html", null ],
      [ "Uno_Player", "class_uno___player.html", null ],
      [ "Uno_PState", "class_uno___p_state.html", null ],
      [ "Uno_Runner", "class_uno___runner.html", null ],
      [ "Uno_State", "class_uno___state.html", null ]
    ] ],
    [ "Class Index", "classes.html", null ],
    [ "Class Hierarchy", "hierarchy.html", [
      [ "CStopWatch", "class_c_stop_watch.html", null ],
      [ "stop_watch", "structstop__watch.html", null ],
      [ "Uno_Action", "class_uno___action.html", null ],
      [ "Uno_AI_Search_Node", "class_uno___a_i___search___node.html", null ],
      [ "Uno_Player", "class_uno___player.html", [
        [ "Uno_AI_Player", "class_uno___a_i___player.html", null ],
        [ "Uno_Human_Player", "class_uno___human___player.html", null ]
      ] ],
      [ "Uno_Runner", "class_uno___runner.html", null ],
      [ "Uno_State", "class_uno___state.html", [
        [ "Uno_GState", "class_uno___g_state.html", null ],
        [ "Uno_PState", "class_uno___p_state.html", null ]
      ] ]
    ] ],
    [ "Class Members", "functions.html", null ],
    [ "File List", "files.html", [
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/driver.cpp", "driver_8cpp.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/hr_timer.cpp", "hr__timer_8cpp.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/hr_timer.h", "hr__timer_8h.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_action.h", "uno__action_8h.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_ai_player.cpp", "uno__ai__player_8cpp.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_ai_player.h", "uno__ai__player_8h.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_ai_search_node.h", "uno__ai__search__node_8h.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_card.cpp", "uno__card_8cpp.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_card.h", "uno__card_8h.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_deck.cpp", "uno__deck_8cpp.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_deck.h", "uno__deck_8h.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_gstate.h", "uno__gstate_8h.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_human_player.cpp", "uno__human__player_8cpp.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_human_player.h", "uno__human__player_8h.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_player.cpp", "uno__player_8cpp.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_player.h", "uno__player_8h.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_pstate.h", "uno__pstate_8h.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_runner.cpp", "uno__runner_8cpp.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_runner.h", "uno__runner_8h.html", null ],
      [ "C:/Users/Gary/Documents/School/my-random-cpp-libraries/trunk/projects/graduate/cs6364/semester_project/semester_project/uno_state.h", "uno__state_8h.html", null ]
    ] ],
    [ "Examples", "examples.html", [
      [ "/examples/uno_card_examples.cpp", "_2examples_2uno_card_examples_8cpp-example.html", null ]
    ] ],
    [ "File Members", "globals.html", null ]
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

