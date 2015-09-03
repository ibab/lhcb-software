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
         ,new LbDesktop.HLT2Status()
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
         //,new LbDesktop.DiracMenuModule()
         ,new LbDesktop.ALICEMenuModule()
         ,new LbDesktop.ATLASMenuModule()
         //,new LbDesktop.CMSMenuModule()
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
LbDesktop.HLT2Status  = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.lhcb2_run_status);}});
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
      var sps_icon = 'http://images.iop.org/objects/ccr/cern/41/6/17/cernsps3_6-01.jpg';
      var clic_icon = 'http://clic-study.web.cern.ch/sites/clic-study.web.cern.ch/themes/cliccern/img/logos/CLIClogo.png';
      var items = 
	[ this.menuItem('LHC logbook',       'https://op-webtools.web.cern.ch/op-webtools/eLogbook/?lgbkID=60',1000,700).makeIcon(constants.images.lhc)
	  ,this.menuItem('LHC Operation',     constants.operations_url('LHC3'),          1100,870).makeIcon(constants.images.lhc)
	  ,this.menuItem('LHC Page 1',        constants.operations_url('LHC1'),          1100,870).makeIcon(constants.images.lhc)
	  ,this.menuItem('LHC Page 2',        constants.operations_url('LHC2'),          1100,870).makeIcon(constants.images.lhc)
	  ,this.menuItem('LHC Configuration', constants.operations_url('LHCCONFIG'),     1100,870).makeIcon(constants.images.lhc)
	  ,this.menuItem('LHC Magnets',       constants.operations_url('LHCEXPMAG'),     1100,920).makeIcon(constants.images.lhc)
	  ,this.menuItem('LHC Beam Dump',     constants.operations_url('LHCBDS'),        1100,700).makeIcon(constants.images.lhc)
	  ,this.menuItem('LHC Dashboard',     constants.operations_url('LHCDASHBOARD'),  1150,1000).makeIcon(constants.images.lhc) 
	  ,this.menuItem('Leir',              constants.operations_url('LEIR'),          900,700)
	  ,this.menuItem('Linac',             constants.operations_url('LIN'),           900,700)
	  ,this.menuItem('PSB',               constants.operations_url('PSB'),           1150,870)
	  ,this.menuItem('HRS',               constants.operations_url('HRS'),           1150,870)
	  ,this.menuItem('GPS',               constants.operations_url('GPS'),           1150,870)
	  ,this.menuItem('LHCf',              constants.operations_url('LHCfExperiment'),1150,870)

	  ,this.menuItem('SPS Page 1',        constants.operations_url('SPS1'),          900,700).makeIcon({icon:sps_icon})
	  ,this.menuItem('SPS Home page',     'https://home.web.cern.ch/about/accelerators/super-proton-synchrotron', 1150,700).makeIcon({icon:sps_icon})
	  ,this.menuItem('SPS Operation',     'https://ab-dep-op-sps.web.cern.ch/ab-dep-op-sps', 900,700).makeIcon({icon:sps_icon})
	  ,this.menuItem('SPS logbook',       'https://op-webtools.web.cern.ch/op-webtools/eLogbook/?lgbkID=50',1000,700).makeIcon({icon:sps_icon})
	  ,this.menuItem('Beams department',  'https://espace.cern.ch/be-dep',900,700)
	  ,this.menuItem('Linac4 Project',    'https://linac4-project.web.cern.ch/linac4-project',1100,870).makeIcon({icon:'https://cern.ch/linac4/images/Logo-Linacresized.jpg'})
	  ,this.menuItem('CLIC Project',      'https://clic-study.web.cern.ch',1100,870).makeIcon({icon:clic_icon})
	  ,this.menuItem('CLICdp',            'https://clicdp.web.cern.ch',1200,870).makeIcon({icon:clic_icon})
	  ];
    this._launch('Beam&amp;Operations',constants.images.beams_department,items);
  }
});

LbDesktop.LHCbMenuModule = Ext.extend(desktop.URLMenuModule, {
  init: function(){
    var items = 
      [ this.menuURL(constants.urls.lhcb.home_page).makeIcon(constants.images.lhcb,1300,700)
      ,this.menuItem('Run News (requires login)', 'https://groups.cern.ch/group/lhcb-run-news/default.aspx',1000,700)
      ,this.menuItem('Run DB',                    'https://lbrundb.cern.ch/rundb?destination=rundb',1000,700)
      ,this.menuItem('Problem DB',                'https://lbproblems.cern.ch',1000,700)
      ,this.menuItem('Shifts',                    'https://lbshiftdb.cern.ch',1000,700)
      ,this.menuItem('Pit Webcam',                'https://lhcb.web.cern.ch/lhcb/web-cam.htm',1000,700)
      ,this.menuURL(constants.urls.lhcb.twiki).makeIcon(constants.images.twiki).makeIcon(constants.images.lhcb,1300,700)
      ,this.menuURL(constants.urls.lhcb.online_home_page).makeIcon(constants.images.lhcb,1300,700)
      ,this.menuURL(constants.urls.lhcb.computing_home_page).makeIcon(constants.images.lhcb,1300,700)
      ,this.menuURL(constants.urls.lhcb.weekly_meetings).makeIcon(constants.images.lhcb,1300,700)
      ,this.menuURL(constants.urls.lhcb.offline_databases_status).makeIcon(constants.images.database)
      ,this.menuURL(constants.urls.lhcb.online_databases_status).makeIcon(constants.images.database)
     ];
    this._launch('LHCb Experiment',constants.images.lhcb,items);
  }
});

LbDesktop.LHCbSDetModule = Ext.extend(desktop.URLMenuModule, {
  init: function(){
    var items = 
    [ 
     this.menuURL(constants.urls.lhcb.subdetectors.daq_evdisp).makeIcon(constants.images.lhcb,1300,700)
     ,this.menuURL(constants.urls.lhcb.subdetectors.daq_lumi).makeIcon(constants.images.lhcb,1300,700)
     ,this.menuURL(constants.urls.lhcb.subdetectors.daq_trigger).makeIcon(constants.images.lhcb,1300,700)
     ,this.menuURL(constants.urls.lhcb.alarmScreen).makeIcon(constants.images.lhcb,1300,700)
     ,this.menuURL(constants.urls.lhcb.subdetectors.trg_l0status).makeIcon(constants.images.lhcb,1300,700)
     ,this.menuURL(constants.urls.lhcb.subdetectors.velo).makeIcon(constants.images.lhcb,1300,700)
     ,this.menuURL(constants.urls.lhcb.subdetectors.velo.motion).makeIcon(constants.images.lhcb,1300,700)
     ,this.menuURL(constants.urls.lhcb.subdetectors.st).makeIcon(constants.images.lhcb,1300,700)
     ,this.menuURL(constants.urls.lhcb.subdetectors.ot).makeIcon(constants.images.lhcb,1300,700)
     ,this.menuURL(constants.urls.lhcb.subdetectors.rich).makeIcon(constants.images.lhcb,1300,700)
     ,this.menuURL(constants.urls.lhcb.subdetectors.calo).makeIcon(constants.images.lhcb,1300,700)
     ,this.menuURL(constants.urls.lhcb.subdetectors.muon).makeIcon(constants.images.lhcb,1300,700)
     ,this.menuURL(constants.urls.lhcb.subdetectors.cooling).makeIcon(constants.images.lhcb,1300,700)
     ];
    this._launch('LHCb Subdetectors',constants.images.lhcb,items);
  }
});

LbDesktop.DiracMenuModule = Ext.extend(desktop.URLMenuModule, {
  init: function(){
    var items = 
    [
     this.menuURL(constants.urls.lhcb.dirac.home_page).makeIcon(constants.images.dirac)
     //,this.menuURL(constants.urls.lhcb.dirac.launch_pad)
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
               ,this.menuItem('Service Portal',    constants.service_now_url('sls.do'), 1100,700).makeIcon(constants.images.database)
               ,this.menuItem('Phonebook',         'https://phonebook.cern.ch/phonebook#',1100,700)
               ,this.menuItem('Maps',              'https://maps.cern.ch',1100,700)
               ,this.menuItem('Library',           'https://library.web.cern.ch', 1100,700)
               ,this.menuItem('Shuttle service',   'https://gs-dep.web.cern.ch/en/ShuttleService', 1100,700)
    ];
    this._launch('CERN',constants.images.cern,items);
  }
});

LbDesktop.ALICEMenuModule = Ext.extend(desktop.URLMenuModule, {
  init: function() {
      var items=[//this.menuItem('ALICE Home','http://alice.cern.ch',900,700).makeIcon(constants.images.alice)
		 this.menuItem('ALICE DCS','https://cern.ch/alicedcs',900,700).makeIcon(constants.images.alice)
		 ,this.menuItem('ALICE DCS Status',    'https://alicedcs.web.cern.ch/AliceDCS/Status',1100,700).makeIcon(constants.images.alice)
		 ,this.menuItem('ALICE DCS Operation', 'https://alicedcs.web.cern.ch/AliceDCS/DCSoperation',1100,700).makeIcon(constants.images.alice)
		 ,this.menuItem('ALICE DCS Monitoring','https://alicedcs.web.cern.ch/AliceDCS/monitoring',1100,700).makeIcon(constants.images.alice)
    ];
    this._launch('ALICE Experiment',constants.images.alice,items);
  }
});


LbDesktop.ATLASMenuModule = Ext.extend(desktop.URLMenuModule, {
  init: function(){
    var items=[this.menuItem('ATLAS Home','https://atlas.web.cern.ch/Atlas/Collaboration',900,700).makeIcon(constants.images.atlas),
               this.menuItem('ATLAS Computing','https://twiki.cern.ch/twiki/bin/view/Atlas/AtlasComputing',900,700),
               this.menuItem('ATLAS Event display','https://atlantis.web.cern.ch/atlantis',900,700)
    ];
    this._launch('ATLAS Experiment',constants.images.atlas,items);
  }
});

LbDesktop.CMSMenuModule = Ext.extend(desktop.URLMenuModule, {
  cmstv: function(channel) { return 'http://cmsdoc.cern.ch/cmscc/cmstv/cmstv.jsp?channel='+channel; },
  init: function()  {
    var items=[this.menuItem('CMS Home','http://cms.web.cern.ch',900,700).makeIcon(constants.images.cms)
              ,this.menuItem('Public Data',  'http://cms.web.cern.ch/content/cms-public-data',1000,700)
	       //,this.menuItem('Event Display (2008)', this.cmstv(5),1000,700)
	       //,this.menuItem('Event Display (2009)', this.cmstv(6),1000,700)
	       //,this.menuItem('Event Display (2010)', this.cmstv(7),1000,700)
	       //,this.menuItem('Event Display (Sim)',  this.cmstv(8),1000,700)
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
