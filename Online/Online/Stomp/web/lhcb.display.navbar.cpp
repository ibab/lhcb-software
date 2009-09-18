_loadFile('lhcb.display.general','css');
var HTML_HEAD = new Object();

function setupHTML_HEAD() {
  HTML_HEAD.head = document.getElementsByTagName("head")[0];
  HTML_HEAD.url_base = the_displayObject.url_base;
  HTML_HEAD.url_comet = 'http://lbcomet.cern.ch/static/RunStatus';
  HTML_HEAD.img_base  = 'http://frankm.web.cern.ch/frankm/test/Online/Stomp/web';
}

function setupHTML_BASE(url) {
  var e = document.createElement("base");
  e.setAttribute("href",url);
  HTML_HEAD.head.appendChild(e);
}

var NavigationBar = function() {
  var table = document.createElement('table');
  table.body = document.createElement('tbody');
  table.haveSmallIcons = false;
  table.items = new Array();
  table.icons = null;
  table.className = 'NavigationBar';
  table.body.className = 'NavigationBar';
  table.style.width = '100%';
  table.body.style.width = '100%';
 
  table.addButton = function(text,tooltip,style, action) {
    var tr = document.createElement('tr');
    var td = document.createElement('td');
    td.style.verticalAlign = 'top';
    td.style.textAlign='center';
    td.className = style;
    td.innerHTML = text;
    td.onclick = action;
    td.client = this;
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  }
  
  table.open_abs_url = function(loc) {
    //alert(navigator.appName+'\n\n'+loc);
    doc = document;
    if ( parent.frames["viewer"] != null )  {
      if ( navigator.appName.indexOf("Netscape") != -1 )  {
	doc = parent.frames.viewer;
      }
      else if ( navigator.appName.indexOf("Microsoft") != -1 )  {
	doc = parent.viewer;
      }
      else {
	doc = parent.frames.viewer;
      }
    }
    doc.location = loc;
  }

  table.open_url = function(loc) {
    return this.open_abs_url(HTML_HEAD.url_comet+'/'+loc);
  }

  table.addSized = function(text, tooltip, url, style, small_pic, sx, sy, large_pic, lx, ly) {
    var item     = new Object();
    item.text    = text;
    item.url     = url;
    item.small   = small_pic;
    item.small_height = sy;
    item.small_width  = sx;

    item.large   = large_pic;
    item.large_height = ly;
    item.large_width  = lx;

    item.style        = style;
    item.img          = document.createElement('img');
    item.img.border   = 1;
    item.img.alt      = tooltip;

    this.items.push(item);
    return this;
  }

  table.add = function(text, tooltip, url, style, small_pic, large_pic) {
    return this.addSized(text, tooltip, url, style, small_pic, 16, 16, large_pic, 32, 32);
  }

  table.setImages = function(small) {
    if ( small ) {
      if ( this.icons ) this.icons.innerHTML = 'Large Icons';
      for (var i=0, len=this.items.length; i < len; ++i)  {
	var item = this.items[i];
	item.img.src = item.small;
	item.img.height = item.small_height;
	item.img.width  = item.small_width;
      }
    }
    else {
      if ( this.icons ) this.icons.innerHTML = 'Small Icons';
      for (var i=0, len=this.items.length; i < len; ++i)  {
	var item = this.items[i];
	item.img.src = item.large;
	item.img.height = item.large_height;
	item.img.width  = item.large_width;
      }
    }
    return this;
  }

  table.build = function() {
    var item, itm, a, br, tr, td;
    for (var i=0, len=this.items.length; i < len; ++i)  {
      item = this.items[i];
      tr = document.createElement('tr');
      td = document.createElement('td');
      td.style.verticalAlign = 'top';
      td.style.textAlign='center';
      a = document.createElement('a');
      a.href = item.url;
      if ( item.style ) a.className = item.style;
      a.appendChild(item.img);
      a.appendChild(document.createElement('br'));
      a.appendChild(document.createTextNode(item.text));      
      td.appendChild(a);
      tr.appendChild(td);
      this.body.appendChild(tr);
      item.cell = tr;
    }
    this.appendChild(this.body);
    return this.setImages(this.haveSmallIcons);
  }

  table.changeImages = function() {
    this.client.haveSmallIcons = !this.client.haveSmallIcons;
    return this.client.setImages(this.client.haveSmallIcons);
  }

  return table;
}


var navBar = null;


var navbar_unload = function()  {
}

var navbar_body = function()  {
  setupHTML_HEAD();
  setupHTML_BASE(HTML_HEAD.url_base+'/..');
  navBar = NavigationBar();
  navBar.icons = navBar.addButton('Small Icons','Change icon layout', 'DisplayButton', navBar.changeImages);
  navBar.add('Online Home',
	     'Got to the LHCb Online home page',
	     'JavaScript:navBar.open_abs_url("http://lhcb-online.web.cern.ch/lhcb-online")',
	     'NavigationBar',
	     '../Images/Home_16x16.gif',
	     '../Images/Home_32x32.gif');
  navBar.add('Help',
	     'Help',
	     'JavaScript:navBar.open_abs_url("News.htm")',
	     'NavigationBar',
	     '../Images/Help_16x16.gif',
	     '../Images/Help_32x32.gif');

  navBar.add('Page 1',
	     'Show LHCb Page 1',
	     'JavaScript:navBar.open_url("lhcb.display.htm?type=page1")',
	     'NavigationBar',
	     '../Images/Search_16x16.gif',
	     '../Images/Search_32x32.gif');

  navBar.add('LHCb Run Status',
	     'Show LHCb run status display',
	     'JavaScript:navBar.open_url("lhcb.display.htm?type=status&system=LHCb")',
	     'NavigationBar',
	     '../Images/LookFile_16x16.gif',
	     '../Images/LookFile_32x32.gif');

  navBar.add('Subdetector Run Status',
	     'Show run status display with coice of detector',
	     'JavaScript:navBar.open_url("lhcb.display.htm?type=status")',
	     'NavigationBar',
	     '../Images/LookJob_16x16.gif',
	     '../Images/LookJob_32x32.gif');

  navBar.add('Collimators',
	     'Show collimator settings around Point 8',
	     'JavaScript:navBar.open_url("lhcb.display.htm?type=collimators")',
	     'NavigationBar',
	     '../Images/LookProd_16x16.gif',
	     '../Images/LookProd_32x32.gif');
  /*
  navBar.add('',
	     '',
	     'JavaScript:navBar.open_url("")',
	     'NavigationBar',
	     '../Images/Wizard_16x16.gif',
	     '../Images/Wizard_32x32.gif');

  navBar.add('',
	     '',
	     'JavaScript:navBar.open_url("")',
	     'NavigationBar',
	     '../Images/Options_16x16.gif',
	     '../Images/Options_32x32.gif');
  */
  navBar.addSized('LHCb',
		  'LHCb home page',
		  'JavaScript:navBar.open_abs_url("http://lhcb.cern.ch")',
		  'NavigationBar',
		  'http://lhcb.cern.ch/lhcblogo.gif',64,32,
		  'http://lhcb.cern.ch/lhcblogo.gif',64,32);
  
  navBar.addSized('LHC',
		  'LHC collider home page',
		  'JavaScript:navBar.open_abs_url("http://cern.ch/lhc")',
		  'NavigationBar',
		  'http://user.web.cern.ch/user/Welcome/LHCDipole.gif',32,32,
		  'http://user.web.cern.ch/user/Welcome/LHCDipole.gif',48,48);
  
  navBar.addSized('CERN',
		  'CERN home page',
		  'JavaScript:navBar.open_abs_url("http://cern.ch")',
		  'NavigationBar',
		  'http://user.web.cern.ch/User/zCommonImages/CERNLogo.png',32,32,
		  'http://user.web.cern.ch/User/zCommonImages/CERNLogo.png',48,48);
  
  navBar.build();
  HTML_HEAD.body = document.getElementsByTagName('body')[0];
  HTML_HEAD.body.appendChild(navBar);
  HTML_HEAD.body.className = 'NavigationBar';
  HTML_HEAD.body.style.width = '100%';
  HTML_HEAD.body.style.backgroundColor = '#666666';
  HTML_HEAD.body.style.color = '#FFFFFF';
  return navBar;
}

if ( _debugLoading ) alert('Script lhcb.display.navbar.cpp loaded successfully');
