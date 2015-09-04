_loadFile('lhcb.display.general','css');
var HTML_HEAD = new Object();

function setupHTML_HEAD() {
  HTML_HEAD.head = document.getElementsByTagName("head")[0];
  HTML_HEAD.url_base  = the_displayObject.url_base;
  HTML_HEAD.url_comet = lhcb.constants.lhcb_comet_url();
  HTML_HEAD.img_base  = lhcb.constants.lhcb_base_url();
  HTML_HEAD.img_base  = _fileBase;
};

function setupHTML_BASE(url) {
  var e = document.createElement("base");
  e.setAttribute("href",url);
  HTML_HEAD.head.appendChild(e);
};

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
  table.cellSpacing = 2;
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
  };

  table.get_doc = function() {
    var doc = document;
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
    return doc;
  };  
  table.open_abs_url = function(loc) {
    var doc = this.get_doc();
    doc.location = loc;
  };

  table.open_url = function(loc) {
    return this.open_abs_url(HTML_HEAD.url_comet+'/'+loc);
  };

  table.addSized = function(text, tooltip, url, style, small_pic, sx, sy, large_pic, lx, ly) {
    var item     = new Object();
    item.client  = this;
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
    //item.img.src      = item.large;
    //item.img.height   = item.large_height;
    //item.img.width    = item.large_width;
    this.items.push(item);
    return this;
  };

  table.add = function(text, tooltip, url, style, small_pic, large_pic) {
    return this.addSized(text, tooltip, url, style, small_pic, 16, 16, large_pic, 32, 32);
  };

  table.addURL = function(tooltip, url) {
    return this.addSized(url.title, tooltip, 
			 'JavaScript:navBar.open_abs_url("'+url.src+'")',
			 'NavigationBar',
			 url.img.icon, 16, 16, url.img.src, 32, 32);
  };

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
  };

  table.build = function() {
    var item, itm, a, br, tr, td;
    for (var i=0, len=this.items.length; i < len; ++i)  {
      item = this.items[i];
      tr = document.createElement('tr');
      td = document.createElement('td');
      td.className = 'NavigationBar';
      td.style.verticalAlign = 'top';
      td.style.textAlign='center';
      td.style.border = 'solid';
      td.style.borderWidth = '1px';
      td.style.borderColor = '#000000';
      
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
  };

  table.changeImages = function() {
    this.client.haveSmallIcons = !this.client.haveSmallIcons;
    if ( this.client.haveSmallIcons ) the_displayObject['icons'] = 'small';
    else the_displayObject['icons'] = 'large';
    return this.client.setImages(this.client.haveSmallIcons);
  };
  table.seeSubdetectors = function() {
    var icns = the_displayObject['icons'];
    var loc  = HTML_HEAD.url_base+'?type=navbar&system=subdetectors';
    if ( icns && icns == 'small' ) loc = loc + '&icons=small';
    document.location = loc;
  };
  table.seeGeneral = function()  {
    var icns = the_displayObject['icons'];
    var loc = HTML_HEAD.url_base+'?type=navbar';
    if ( icns && icns == 'small' ) loc = loc + '&icons=small';
    document.location = loc;
  };
  table.add_bookmark = function(title, url) {
    if( window.external && ('AddFavorite' in window.external) ) { // ie
      window.external.AddFavorite(url, title);
    }
    else if ( window.sidebar )  { // firefox
      alert('Dear Firefox user, \n\n'+
	    'Your browser does no longer allow to \n'+
	    'add bookmarks programatically.\n\n'+
	    'The browser will now switch to a url, \n'+
	    'which when bookmarked will reload this view.\n\n'+
	    'Please press then CTRL+D to Bookmark the page:\n'+
	    '  ** '+title+' **\n\n'+
	    'or use alternatively the following URL to restart \n'+
	    'the viewer in this configuration:\n\n\n'+
	    url+'\n\n\n'
	    );
      window.parent.document.location = url;
    }
    else if(window.opera && window.print) { // opera
        var elem = document.createElement('a');
        elem.setAttribute('href',url);
        elem.setAttribute('title',title);
        elem.setAttribute('rel','sidebar');
        elem.click();
    }
    else   {
      var key = (navigator.userAgent.toLowerCase().indexOf('mac') != - 1 
		 ? 'CMD' 
		 : 'CTRL');
      alert('Dear user, \n\n'+
	    'Your browser does no longer allow to \n'+
	    'add bookmarks programatically.\n\n'+
	    'The browser will now switch to a url, \n'+
	    'which when bookmarked will reload this view.\n\n'+
	    'Please press then '+key+'-D to Bookmark the page:\n'+
	    '  ** '+title+' **\n\n'+
	    'or use alternatively the following URL to restart \n'+
	    'the viewer in this configuration:\n\n\n'+
	    url+'\n\n\n'
	    ); 
      window.parent.document.location = url;
    }
  };
  table.bookmark = function()  {
    var par = this.client.get_doc().location.search;
    var bm = lhcb.constants.lhcb_base_url();
    var tit = top.document.title;
    if ( par.length>1 )  {
      bm = bm + par;
    }
    if ( tit.search('LHCb Online Status -- ')<0 )
      this.client.add_bookmark('LHCb Online Status -- '+top.document.title,bm);
    else
      this.client.add_bookmark(top.document.title,bm);
  };
  return table;
};


var navBar = null;

var navbar_general = function(navBar) {
  navBar.sdets = navBar.addButton('Subdetectors','See subdetector pages', 'DisplayButton', navBar.seeSubdetectors);
  //navBar.addURL('Got to the LHCb Online home page',lhcb.constants.lhcb.online_home_page);
  navBar.add('Help',
	     'Help',
	     'JavaScript:navBar.open_abs_url("News.htm")',
	     'NavigationBar',
	     '../Images/Help_16x16.gif',
	     '../Images/Help_32x32.gif');
  navBar.addURL('Show LHCb Page 1',lhcb.constants.urls.lhcb.page1);
  navBar.addURL('Show LHCb run status display',lhcb.constants.urls.lhcb.lhcb_run_status);
  navBar.addURL('Show LHCb2 run status display',lhcb.constants.urls.lhcb.lhcb2_run_status);
  navBar.addURL('Show FEST run status display',lhcb.constants.urls.lhcb.fest_run_status);
  navBar.addURL('Show subdetector run status display',lhcb.constants.urls.lhcb.sdet_run_status);
  navBar.addURL('Show collimator settings around Point 8',lhcb.constants.urls.lhcb.collimators);
  navBar.addURL('Show LHCb HV status',lhcb.constants.urls.lhcb.detstatus);
  navBar.addURL('Show Operations status',lhcb.constants.urls.lhcb.operstatus);
  lhcb.constants.urls.lhcb.bcm.src += '&charts=1';
  navBar.addURL('Show BCM status',lhcb.constants.urls.lhcb.bcm);
  navBar.addURL('Show LHCb PVSS Alarms',lhcb.constants.urls.lhcb.alarmScreen);
  /*
  navBar.addURL('Show Magnet status',lhcb.constants.urls.lhcb.magnet);
  */
  navBar.addURL('LHC status around LHCb',lhcb.constants.urls.lhcb.lhc_status);
  navBar.addURL('LHCb electronic logbook',lhcb.constants.urls.lhcb.elog);
  /*
  navBar.addSized('Ramses',
		  'Radiation Monitoring in LHCb',
		  'JavaScript:navBar.open_url(lhcb.constants.lhcb_display_url("ramses"))',
		  'NavigationBar',
		  '../Images/LHCb/Ramses_icon.jpg',20,20,
		  '../Images/LHCb/Ramses_icon.jpg',32,32);
  */
  navBar.bm = navBar.addButton('Bookmark','Bookmark this frame configuration','DisplayButton', navBar.bookmark);
};

var navbar_subdetectors = function(navBar) {
  navBar.sdets = navBar.addButton('LHCb general','Go back', 'DisplayButton', navBar.seeGeneral);
  navBar.add('Help',
	     'Help',
	     'JavaScript:navBar.open_abs_url("News.htm")',
	     'NavigationBar',
	     '../Images/Help_16x16.gif',
	     '../Images/Help_32x32.gif');
  navBar.addURL('Lumi/DAQ page',lhcb.constants.urls.lhcb.subdetectors.daq_lumi);
  navBar.addURL('Trigger/DAQ page',lhcb.constants.urls.lhcb.subdetectors.daq_trigger);
  navBar.addURL('Big Brother/DAQ page',lhcb.constants.urls.lhcb.subdetectors.daq_bigbrother);
  navBar.addURL('Event Display/DAQ page',lhcb.constants.urls.lhcb.subdetectors.daq_evdisp);
  navBar.addURL('TRG/L0 status page',lhcb.constants.urls.lhcb.subdetectors.trg_l0status);
  navBar.addURL('Show VELO page',lhcb.constants.urls.lhcb.subdetectors.velo);
  navBar.addURL('Show VELO Motion page',lhcb.constants.urls.lhcb.subdetectors.velo.motion);
  navBar.addURL('Show ST page',lhcb.constants.urls.lhcb.subdetectors.st);
  navBar.addURL('Show OT page',lhcb.constants.urls.lhcb.subdetectors.ot);
  navBar.addURL('Show RICH page',lhcb.constants.urls.lhcb.subdetectors.rich);
  navBar.addURL('Show CALO page',lhcb.constants.urls.lhcb.subdetectors.calo);
  navBar.addURL('Show MUON page',lhcb.constants.urls.lhcb.subdetectors.muon);
  navBar.addURL('Show Cooling status page',lhcb.constants.urls.lhcb.subdetectors.cooling);
  navBar.addURL('Show DataQuality page',lhcb.constants.urls.lhcb.subdetectors.dq);
  navBar.bm = navBar.addButton('Bookmark','Bookmark this frame configuration','DisplayButton', navBar.bookmark);
};


var navbar_unload = function()  {};

var navbar_body = function()  {
  setupHTML_HEAD();
  setupHTML_BASE(HTML_HEAD.url_base+'/..');
  var sys  = the_displayObject['system'];
  var icns = the_displayObject['icons'];

  navBar = NavigationBar();
  navBar.icons = navBar.addButton('Small Icons','Change icon layout', 'DisplayButton', navBar.changeImages);

  if ( !sys ) {
    navbar_general(navBar);
  }
  else {
    navbar_subdetectors(navBar);
  }

  navBar.addSized('LHCb',
		  'LHCb home page',
		  'JavaScript:navBar.open_abs_url("'+lhcb.constants.urls.lhcb.home_page.src+'")',
		  'NavigationBar',
		  lhcb.constants.images.lhcb.icon,64,32,
		  lhcb.constants.images.lhcb.src,64,32);
  
  navBar.addURL(  'LHC collider home page',lhcb.constants.urls.lhc.project);
  navBar.addSized('Operations',
		  'CERN collider operations home page',
		  'JavaScript:navBar.open_abs_url("'+lhcb.constants.operations_url('LHC1')+'")',
		  'NavigationBar',
		  lhcb.constants.images.beams_department.icon,80,16,
		  lhcb.constants.images.beams_department.src,120,32);
  navBar.addURL('CERN home page',lhcb.constants.urls.cern);
  
  navBar.haveSmallIcons = !(icns && icns == 'large');
  navBar.build();
  HTML_HEAD.body = document.getElementsByTagName('body')[0];
  HTML_HEAD.body.appendChild(navBar);
  HTML_HEAD.body.className = 'NavigationBar';
  HTML_HEAD.body.style.width = '100%';
  HTML_HEAD.body.style.backgroundColor = '#666666';
  HTML_HEAD.body.style.color = '#FFFFFF';
  return navBar;
};

if ( _debugLoading ) alert('Script lhcb.display.navbar.cpp loaded successfully');
