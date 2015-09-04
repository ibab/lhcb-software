_loadScript('lhcb.display.items.cpp');
_loadFile('lhcb.display.general','css');

var evdisp = function() {
  return document.getElementById('panoramix_display');
};

var EventDisplay = function(msg, id)   {
  var tr, td, tb, tab;
  var table       = document.createElement('table');
  table.id        = id;
  table.body      = document.createElement('tbody');
  table.messages  = msg;
  table.provider  = null;
  table.logger    = null;
  table.cycle     = 0;
  table.className = table.body.className = 'MonitorOuterFrame';
  table.source    = 'OFFLINE';
  table.BeamMode  = '';
  table.MachMode  = '';
  table.timeout    = 15000;
  table.forceOnline = false;

  table.add = function()   {
    var tr = document.createElement('tr');
    var td = document.createElement('td');
    td.setAttribute('colSpan',2);
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  };

  table.display    = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  /// Build table with summary of the LHC
  table.LHC_header = function(par) {
    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');
    tab.className = tb.className = 'MonitorSubHeader';
  
    tab.body = tb;
    tb.appendChild(tr=document.createElement('tr'));
  
    tab.lhcFillNumber = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.FillNumber',null,'Fill %d');
    tr.appendChild(tab.lhcFillNumber);
  
    tab.lhcBeamMode = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.BeamMode','Text-Right',null);
    tab.lhcBeamMode.parent = par;
    tab.lhcBeamMode.conversion = function(data) {
      this.parent.update('BeamMode',data);
      return data;
    };
    tr.appendChild(tab.lhcBeamMode);

    tab.lhcMachMode = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.MachineMode','Text-Left','  --  %s');
    tab.lhcMachMode.parent = par;
    tab.lhcMachMode.conversion = function(data) {
      this.parent.update('MachMode',data);
      return data;
    };
    tr.appendChild(tab.lhcMachMode);
  
    tab.appendChild(tb);
    tooltips.set(tab,'LHC status information');
   
    /** Subscribe all required data item to receive data from data provider object
     *
     *  @param      provider  Data provider object
     *
     *  @return On success reference to self, null otherwise
     */
    tab.subscribe = function(provider) {
      this.lhcBeamMode.subscribe(provider);
      this.lhcFillNumber.subscribe(provider);
      this.lhcMachMode.subscribe(provider);
      return this;
    };
    return tab;
  };
 
  table.button = function(value,onclick) {
    var but, cell = document.createElement('td');
    if ( _isInternetExplorer() ) {
      cell.appendChild(but=document.createElement('button'));
    }
    else {
      cell.appendChild(but=document.createElement('input'));
      but.type = 'button';
    }
    but.value = value;
    but.onclick = onclick;
    return [cell,but];
  };

  table.subscribe = function() {
    this.LHC_header.subscribe(this.provider);
    return this;
  };

  table.update = function(tag,value)   {
    if ( value ) this[tag] = value;
    if ( this['BeamMode'].toUpperCase() == 'STABLE BEAMS' && 
	 this['MachMode'].toUpperCase() == 'PROTON PHYSICS' )
      this.setInput('ONLINE');
    else
      this.setInput('ARCHIVE');
  };

  table.setInput = function(what) {
    var src = this.source;
    this.source = what;
    if ( this.forceOnline ) this.source = 'ONLINE';
    this.timeout = (this.source=='ONLINE') ? 25000 : 15000;
    this.online_status.innerHTML='Source: '+this.source;
  };

  table.setState = function(state) {
    var s = this.state;
    this.status.innerHTML='Display: '+state;
    this.state = state;
    if ( state == 'RUNNING' && s != 'RUNNING' )  {
      this.run();
    }
  };

  table.build = function() {
    var td, tr, tb, tab = document.createElement('table');

    tab.appendChild(tb=document.createElement('tbody'));
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(td=document.createElement('td'));
    td.colSpan       = 4;
    td.style.bgColor = 'black';
    tab.className = tb.className = 'MonitorInnerFrame';

    td.appendChild(this.picture=document.createElement('img'));
    this.picture.id = 'panoramix_img';
    this.picture.style.width  = '1050px';
    this.picture.style.height = '600px';
    this.picture.style.width  = '100%';
    this.picture.style.height = '600px';
    this.picture.src = '';
    this.picture.setOpacity = function(opacity) {
      this.opacity = (opacity == 100) ? 99.999 : opacity;  
      // IE/Win
      if ( this.style.filter ) this.style.filter = "alpha(opacity:"+opacity+")";
      // Safari<1.2, Konqueror
      this.style.KHTMLOpacity = opacity/100;
      // Older Mozilla and Firefox
      this.style.MozOpacity = opacity/100;      
      // Safari 1.2, newer Firefox and Mozilla, CSS3
      this.style.opacity = opacity/100;
    };
    this.picture.fadeIn = function(opacity) {
      if (opacity <= 100) {
	this.setOpacity(opacity);
	this.opacity += 3;
	window.setTimeout(function(){var i=evdisp().picture;i.fadeIn(i.opacity);}, 25);
      }
    };
    this.picture.setOpacity(0);
    tooltips.set(this.picture,'LHCb event display panel.');

    tb.appendChild(tr=document.createElement('tr'));
    td = this.button('Pause',
		     function(){evdisp().setState('STOPPED')})[0];
    tooltips.set(td,'Click to pause<br>the display refresh.');
    td.style.width = '5%';
    tr.appendChild(td);

    tr.appendChild(this.status = document.createElement('td'));
    this.status.style.textAlign = 'Left';
    this.status.style.width = '20%';
    tooltips.set(this.status,'Display state:<br>RUNNING: Automatic refresh ON<br>STOPPED: Automatice refresh OFF.');

    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.LHC_header=this.LHC_header(this));
    td.rowSpan = 2;
    td.style.width = '75%';

    tb.appendChild(tr=document.createElement('tr'));
    td = this.button('Play',function(){evdisp().setState('RUNNING')})[0];
    tooltips.set(td,'Click to start<br>the automatic display refresh.');
    td.style.width = '4%';
    tr.appendChild(td);
    this.online_status = document.createElement('td');
    tooltips.set(this.online_status,'Data source:<br>--Events are displayed from the LHCb ONLINE system or<br>--from the media archive if the DAQ is inactive');
    tr.appendChild(this.online_status);

    // Finally add suggestions text
    tb.appendChild(tr=document.createElement('tr'));
    td = this.button('Force Online',function(){var d=evdisp();d.forceOnline=true;d.setInput('ONLINE');d.reload();})[0];
    tooltips.set(td,'Force online input.');
    tr.appendChild(td);
    td = this.button('Reset',function(){var d=evdisp();d.forceOnline=false;d.update(null,null);d.reload();})[0];
    tooltips.set(td,'Reload page.');
    tr.appendChild(td);
    tr.appendChild(td=Cell('Comments and suggestions to M.Frank CERN/LHCb',2,'MonitorTinyHeader'));
    td.style.textAlign = 'right';
    td.colSpan = 2;

    this.display.appendChild(tab);
    return this;
  };

  table.reload = function()  {
    var arch_img = ["http://lhcb-public.web.cern.ch/lhcb-public/Images2015/jpsipk5p.png",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2015/jpsipk1p.png",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2015/FirstPhysicsLHCb.png",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2015/13TeVLHCb.png",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2015/CMSLHCb_EDfig4.png",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2014/PhotonPol.jpg",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2013/Bs2MuMu2013.png",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2013/Bs2MuMu2013v.png",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2013/BsKpi.jpg",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2013/BsKpiVertex.jpg",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2013/BDpp.jpg",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2014/Images_2012/BsMuMuMay2012.jpg",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2014/Images_2012/BsMuMuMay2012zoomzx_force.png",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2014/Images_2012/pASep_13_12.jpg",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2014/Images_2012/Bd_Kpi.jpg",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2014/Images_2011/BsMuMUPt_b.jpg",
		    "http://lhcb-public.web.cern.ch/lhcb-public/Images2014/Images_2011/BsMuMu_xz_b.jpg",
		    "http://marwww.in2p3.fr/~oleroy/pro/firstb/firstBplus2JpsiKplus_LargeOverview.jpg"
		    ];
    this.cycle = this.cycle + 1;
    this.picture.setOpacity(20);
    if ( this.forceOnline || this.source == 'ONLINE' ) this.picture.src = '/Online/Images/evdisp.jpg?'+Math.random();
    else this.picture.src = arch_img[this.cycle%arch_img.length];
    // this.picture.src = 'http://lbevent.cern.ch/Panoramix/PanoramixEventDisplay_0.jpg?'+Math.random();
    this.picture.fadeIn(20);
  };

  table.run = function() {
    var display  = evdisp();
    if ( display.state == 'RUNNING' ) {
      display.reload();
      setTimeout(display.run,display.timeout);
    }
  };

  return table;
};

var evdisp_unload = function()  {
  dataProviderReset();
  return 1;
};

var evdisp_body = function()  {
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector = EventDisplay(msg,'panoramix_display');

  body.appendChild(selector);
  body.className = 'MainBody';
  setWindowTitle('LHCb Event Display');
  if ( msg > 0 )
    selector.logger = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'StatusLogger');
  else
    selector.logger = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'StatusLogger');
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  selector.build();

  selector.subscribe();
  selector.setState('RUNNING');
  selector.provider.start();
  body.style.cursor = 'default';
  return selector;
};

if ( _debugLoading ) alert('Script lhcb.display.evdisp.cpp loaded successfully');
