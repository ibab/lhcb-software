var constants = lhcb.constants;
var desktop   = lhcb.desktop;

var Params = function()   {
  this.type = null;
  this.header = null;
  this.body = null;
  this.config = 'session';
  var url = document.location.toString();
  var pars = url.split('?');
  if ( pars.length > 1 )  {
    var disp_func = '';
    var p = pars[1].split('&');
    this.params = p;
    for (var i=0; i<p.length; ++i) {
      var v = p[i].split('=');
      this[v[0]] = v[1];
    }
    this.url_base = pars[0];
  }
};


// Sample desktop configuration
LbDesktop = new Ext.app.App({
  params: new Params(),

  init:   function(){
    Ext.QuickTips.init();
  },

  getStyles: function() {
    return   {
        backgroundcolor: '575757',
        fontcolor: 'FFFFFF',
        transparency: 100,
        theme: {
            id: 2,
            name: 'Vista Black',
            pathtofile: 'ExtJs/resources/themes/xtheme-vistablack/css/xtheme-vistablack.css'
        },
        wallpaper: {
            id: 4,
            name: 'Swirl',
            pathtothumbnail:	'ExtJs/resources/wallpapers/thumbnails/blue-swirl.jpg', 
	    pathtofile:		'ExtJs/resources/wallpapers/blue-swirl.jpg'
        },
        wallpaperposition: 'tile'
     };
  },
  getModules : function()     {
    return [ 
         new LbDesktop.News()
	 ,new LbDesktop.Page1()
         ,new LbDesktop.LHCbStatus()
         ,new LbDesktop.FESTStatus()
         ,new LbDesktop.SDETStatus()
         ,new LbDesktop.Operator()
         ,new LbDesktop.DetStatus()
         ,new LbDesktop.Collimators()
         ,new LbDesktop.BCM()
         ,new LbDesktop.Magnet()
         ,new LbDesktop.LHCStatus()
         ,new LbDesktop.LHC()
         ,new LbDesktop.ELog()
         ,new LbDesktop.LHCbMeetings()
         //,new LbDesktop.CERN()
         ,new LbDesktop.BeamMenuModule()
         ,new LbDesktop.LHCbMenuModule()
	 ,new LbDesktop.LHCbSDetModule()
         ,new LbDesktop.DiracMenuModule()
         ,new LbDesktop.ALICEMenuModule()
         ,new LbDesktop.ATLASMenuModule()
         ,new LbDesktop.CMSMenuModule()
         ,new LbDesktop.CERNMenuModule()
	 ,new Ext.app.Preferences()
	 ,new Ext.app.About()
    ];
  },

  handleLogout: function(btn){
     if ( btn == 'yes' ) window.history.back();
  },
  createLogoutWindow: function(src) {
    Ext.MessageBox.confirm('Confirm', 'Are you sure <br>you want to do logout?', this.handleLogout);
  },

  getStartItemsConfig: function() {
    var ms = this.modules;
    for(var i = 0, len = ms.length; i < len; i++) {
      var l = ms[i].launcher;
      var t = l.text;
      ( !(t == 'Preferences' || t=='About') ) ? this.launcher.add(l) : this.launcher.addTool(l);
    }
    this.launcher.addToolSeparator();
    this.launcher.addTool({
          text:     'Logout'
          ,iconCls: 'logout'
          ,scope:    this
          ,style:   {padding: '3px 1px 3px 28px'}
          ,handler:  this.createLogoutWindow
          ,scope:    this
		       });
  },

  // config for the start menu
  getStartConfig : function() {
    return {
      width:  350,
      height: 600,
      title: 'LHCb Desktop',
      iconCls: 'user',
      toolItems: []
    };
  },

  initPreferences: function() {
    return this.loadPreferencesWithCookie(this.params.config);
  },

  savePreferences : function(params) {
    return this.savePreferencesWithCookie(params,this.params.config);
  }
});

LbDesktop.News        = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.display_news);}});
LbDesktop.LHCbStatus  = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.lhcb_run_status);}});
LbDesktop.FESTStatus  = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.fest_run_status);}});
LbDesktop.SDETStatus  = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.sdet_run_status);}});
LbDesktop.Page1       = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.page1);}});
LbDesktop.DetStatus   = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.detstatus);}});
LbDesktop.Operator    = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.operstatus);}});
LbDesktop.Magnet      = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.magnet);}});
LbDesktop.Collimators = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.collimators);}});
LbDesktop.BCM         = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.bcm); }});
LbDesktop.ELog        = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.elog); }});
LbDesktop.LHCStatus   = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.lhc_status); }});
LbDesktop.LHC         = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhc.project); }});
LbDesktop.CERN        = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.cern);}});
LbDesktop.LHCbMeetings= Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.weekly_meetings); }});

LbDesktop.BeamMenuModule = Ext.extend(desktop.URLMenuModule, {
  init : function()  { 
    var items = 
    [ this.menuItem('LHC logbook',       'http://eLogbook.cern.ch/eLogbook/eLogbook.jsp',1000,700)
     ,this.menuItem('SPS Page 1',        constants.operations_url('SPS1'),       900,700)
     ,this.menuItem('LHC Operation',     constants.operations_url('LHC3'),      1150,870).makeIcon(constants.images.lhc)
     ,this.menuItem('LHC Page 1',        constants.operations_url('LHC1'),      1150,870)
     ,this.menuItem('LHC Page 2',        constants.operations_url('LHC2'),      1150,870)
     ,this.menuItem('LHC Configuration', constants.operations_url('LHC_CONFIG'),1150,870)
     ,this.menuItem('LHC Magnets',       constants.operations_url('LHCEXPMAG'),  900,700)
     ,this.menuItem('Leir',              constants.operations_url('LEIR'),       900,700)
     ,this.menuItem('Linac',             constants.operations_url('LIN'),        900,700)
     ,this.menuItem('LHCf',              constants.operations_url('LHCF'),      1150,870)
     ,this.menuItem('SPS Operation',     'http://ab-dep-op-sps.web.cern.ch/ab-dep-op-sps', 900,700)
     ,this.menuItem('SPS Operation',     'http://ab-dep-op-sps.web.cern.ch/ab-dep-op-sps', 900,700)
     ,this.menuItem('SPS logbook',       'http://eLogbook.cern.ch/eLogbook/eLogbook.jsp?lgbk=50',900,700)
     ,this.menuItem('SPS wiki',          'http://wikis/display/SPSOP/SPS+Start+page',900,700)
     ,this.menuItem('Beams department',  'https://espace.cern.ch/be-dep',900,700)
     ,this.menuItem('Linac4 Project',    'http://cern.ch/linac4',900,700).makeIcon({icon:'http://cern.ch/linac4/images/Logo-Linacresized.jpg'})
     ,this.menuItem('CLIC Project',     'http://cern.ch/CLIC-Study',900,700)
     ];
    this._launch('Beam&amp;Operations',constants.images.beams_department,items);
  }
});

LbDesktop.LHCbMenuModule = Ext.extend(desktop.URLMenuModule, {
  init: function(){
    var items = 
    [ this.menuURL(constants.urls.lhcb.home_page).makeIcon(constants.images.lhcb)
      ,this.menuItem('Run News (requires login)', 'https://groups.cern.ch/group/lhcb-run-news/default.aspx',900,700)
      ,this.menuItem('Run DB',                    'https://lbrundb.cern.ch/rundb?destination=rundb',900,700)
      ,this.menuItem('Problem DB',                'http://lbproblems.cern.ch',1000,700)
      ,this.menuItem('Pit Webcam',                'http://lhcb.web.cern.ch/lhcb/web-cam.htm',1000,700)
      ,this.menuItem('Shifts',                    'http://lbshiftdb.cern.ch/index.py/login',1000,700)
      ,this.menuURL(constants.urls.lhcb.twiki).makeIcon(constants.images.twiki)
      ,this.menuURL(constants.urls.lhcb.online_home_page)
      ,this.menuURL(constants.urls.lhcb.computing_home_page)
      ,this.menuURL(constants.urls.lhcb.weekly_meetings)
      ,this.menuURL(constants.urls.lhcb.offline_databases_status).makeIcon(constants.images.database)
      ,this.menuURL(constants.urls.lhcb.online_databases_status).makeIcon(constants.images.database)
     ];
    this._launch('LHCb Experiment',constants.images.lhcb,items);
  }
});

LbDesktop.LHCbSDetModule = Ext.extend(desktop.URLMenuModule, {
  init: function(){
    var items = 
    [ this.menuURL(constants.urls.lhcb.subdetectors.daq_lumi)
      ,this.menuURL(constants.urls.lhcb.subdetectors.daq_trigger)
      ,this.menuURL(constants.urls.lhcb.subdetectors.velo)
      ,this.menuURL(constants.urls.lhcb.subdetectors.ot)
      ,this.menuURL(constants.urls.lhcb.subdetectors.muon)
     ];
    this._launch('LHCb Subdetectors',constants.images.lhcb,items);
  }
});

LbDesktop.DiracMenuModule = Ext.extend(desktop.URLMenuModule, {
  init: function(){
    var items = 
    [
     this.menuURL(constants.urls.lhcb.dirac.home_page).makeIcon(constants.images.dirac)
    ,this.menuURL(constants.urls.lhcb.dirac.launch_pad)
     ];
    this._launch('LHCb Dirac',constants.images.dirac,items);
  }
});

LbDesktop.CERNMenuModule = Ext.extend(desktop.URLMenuModule, {
  init: function(){
    var items=[//this.menuItem('Home Page',        constants.mmm_url(null),1000,700).makeIcon(constants.images.cern)
                this.menuItem('MMM',               constants.mmm_url(null),1000,700)
               ,this.menuItem('Mail',              constants.mmm_url('/exchange'),1000,700).makeIcon(constants.images.mail)
               ,this.menuItem('Web site stats',    'https://web-statistics.web.cern.ch/web-statistics/cgi-bin/awstats.pl?month=all&year=2010&output=main&config=frankm&framename=index',1000,700).makeIcon(constants.images.webstats)
               ,this.menuItem('LHCOPN and Tier2s', 'http://network-statistics.web.cern.ch/network-statistics/ext/LHCOPN-Total/?p=sc&q=LHCOPN Total Traffic',1100,700)
               ,this.menuItem('Databases',         constants.sls_service_url('IT-DM-Group'), 1100,700).makeIcon(constants.images.database)
               ,this.menuItem('FIO services',      constants.sls_service_url('IT-FIO-Group'),1100,700)
               ,this.menuItem('DES services',      constants.sls_service_url('IT-DES-Group'),1100,700)
               ,this.menuItem('IS services',       constants.sls_service_url('IT-IS-Group'), 1100,700)
    ];
    this._launch('CERN',constants.images.cern,items);
  }
});

LbDesktop.ALICEMenuModule = Ext.extend(desktop.URLMenuModule, {
  init: function() {
    var items=[this.menuItem('ALICE Home','http://aliceinfo.cern.ch',900,700).makeIcon(constants.images.alice)
               ,this.menuItem('ALICE DCS','http://cern.ch/alicedcs',900,700)
               ,this.menuItem('ALICE DCS Monitoring','http://cern.ch/alicedcs/monitoring',1100,700)
    ];
    this._launch('ALICE Experiment',constants.images.alice,items);
  }
});


LbDesktop.ATLASMenuModule = Ext.extend(desktop.URLMenuModule, {
  init: function(){
    var items=[this.menuItem('ATLAS Home','http://atlas.ch',900,700).makeIcon(constants.images.atlas),
               this.menuItem('ATLAS Computing','https://twiki.cern.ch/twiki/bin/view/Atlas/AtlasComputing',900,700),
               this.menuItem('ATLAS Live events','http://atlantis.cern.ch/live',900,700)
    ];
    this._launch('ATLAS Experiment',constants.images.atlas,items);
  }
});

LbDesktop.CMSMenuModule = Ext.extend(desktop.URLMenuModule, {
  cmstv: function(channel) { return 'http://cmsdoc.cern.ch/cmscc/cmstv/cmstv.jsp?channel='+channel; },
  init: function()  {
    var items=[this.menuItem('CMS Home','http://cms.cern.ch',900,700).makeIcon(constants.images.cms)
              ,this.menuItem('Online Status',     this.cmstv(4),1000,700)
              ,this.menuItem('Event Display (2008)', this.cmstv(5),1000,700)
              ,this.menuItem('Event Display (2009)', this.cmstv(6),1000,700)
              ,this.menuItem('Event Display (2010)', this.cmstv(7),1000,700)
              ,this.menuItem('Event Display (Sim)',  this.cmstv(8),1000,700)
              ,this.menuItem('Tier 0 Status',    'https://cmsweb.cern.ch/T0Mon',1000,700)
    ];
    this._launch('CMS Experiment',constants.images.cms,items);
  }
});

var defineShortcuts = function() {
  var p = document.getElementById('x-shortcuts');
  /*
  p.appendChild(desktop.shortcut(constants.urls.lhcb.lhcb_run_status));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.fest_run_status));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.page1));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.detstatus));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.magnet));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.lhc_status));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.elog));
  p.appendChild(desktop.shortcut(constants.urls.lhc.project));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.weekly_meetings));
  */
  return p;
};

if ( _debugLoading ) alert('Script lhcb.display.lbdesk.cpp loaded successfully');
