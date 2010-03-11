var constants = lhcb.constants;
var desktop   = lhcb.desktop;

// Sample desktop configuration
LbDesktop = new Ext.app.App({
        init :function(){
                Ext.QuickTips.init();
        },
        getModules : function(){
	  //return [new LbDesktop.Magnet()];
	  return [ new LbDesktop.Page1()
		   ,new LbDesktop.LHCbStatus()
		   ,new LbDesktop.FESTStatus()
		   ,new LbDesktop.SDETStatus()
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
		   ,new LbDesktop.DiracMenuModule()
		   ,new LbDesktop.ALICEMenuModule()
		   ,new LbDesktop.ATLASMenuModule()
		   ,new LbDesktop.CMSMenuModule()
		   ,new LbDesktop.CERNMenuModule()
	  ];
        },

    // config for the start menu
    getStartConfig : function(){
      return {  width:  350,
		height: 600,
		title: 'LHCb Desktop',
		iconCls: 'user',
		toolItems: []
		/*
		  toolItems: [{
		  text:'Settings',
		  iconCls:'settings',
		  scope:this
		  },'-',{
		  text:'Logout',
		  iconCls:'logout',
		  scope:this
		  }]
		*/
		};
    }
});

LbDesktop.LHCbStatus  = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.lhcb_run_status);}});
LbDesktop.FESTStatus  = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.fest_run_status);}});
LbDesktop.SDETStatus  = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.sdet_run_status);}});
LbDesktop.Page1       = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.page1);}});
LbDesktop.DetStatus   = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.detstatus);}});
LbDesktop.Magnet      = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.magnet);}});
LbDesktop.Collimators = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.collimators);}});
LbDesktop.BCM         = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.bcm); }});
LbDesktop.ELog        = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.elog); }});
LbDesktop.LHCStatus   = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.lhc_status); }});
LbDesktop.LHC         = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.lhc_project); }});
LbDesktop.CERN        = Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.cern);}});
LbDesktop.LHCbMeetings= Ext.extend(desktop.URLModule, {init: function(){this._launch(constants.urls.lhcb.weekly_meetings); }});

LbDesktop.BeamMenuModule = Ext.extend(desktop.URLMenuModule, {
  init : function()  { 
    var items = 
    [ this.menuItem('SPS Page 1',        constants.operations_url('SPS1'),       900,700)
     ,this.menuItem('LHC Operation',     constants.operations_url('LHC3'),      1150,870)
     ,this.menuItem('LHC Page 1',        constants.operations_url('LHC1'),      1150,870)
     ,this.menuItem('LHC Page 2',        constants.operations_url('LHC2'),      1150,870)
     ,this.menuItem('LHC Configuration', constants.operations_url('LHC_CONFIG'),1150,870)
     ,this.menuItem('LHC Magnets',       constants.operations_url('LHCEXPMAG'),  900,700)
     ,this.menuItem('Leir',              constants.operations_url('LEIR'),       900,700)
     ,this.menuItem('Linac',             constants.operations_url('LIN'),        900,700)
     ,this.menuItem('LHCf',              constants.operations_url('LHCF'),      1150,870)
     ];
    this._launch('Beam&amp;Operations',constants.images.beams_department,items);
  }
});

LbDesktop.LHCbMenuModule = Ext.extend(desktop.URLMenuModule, {
  init: function(){
    var items = 
    [ this.menuURL(constants.urls.lhcb.home_page).makeIcon(constants.images.lhcb)
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
  init : function(){
    var items=[//this.menuItem('Home Page',          constants.mmm_url(null),1000,800).makeIcon(constants.images.cern)
	        this.menuItem('MMM',               constants.mmm_url(null),1000,800)
	       ,this.menuItem('Mail',              constants.mmm_url('/exchange'),1000,800).makeIcon(constants.images.mail)
	       ,this.menuItem('Web site stats',    'https://web-statistics.web.cern.ch/web-statistics/cgi-bin/awstats.pl?month=all&year=2010&output=main&config=frankm&framename=index',1000,800).makeIcon(constants.images.webstats)
	       ,this.menuItem('LHCOPN and Tier2s', 'http://network-statistics.web.cern.ch/network-statistics/ext/LHCOPN-Total/?p=sc&q=LHCOPN Total Traffic',1100,800)
	       ,this.menuItem('Databases',         constants.sls_service_url('IT-DM-Group'), 1100,800).makeIcon(constants.images.database)
	       ,this.menuItem('FIO services',      constants.sls_service_url('IT-FIO-Group'),1100,800)
	       ,this.menuItem('DES services',      constants.sls_service_url('IT-DES-Group'),1100,800)
	       ,this.menuItem('IS services',       constants.sls_service_url('IT-IS-Group'), 1100,800)
    ];
    this._launch('CERN',constants.images.cern,items);
  }
});

LbDesktop.ALICEMenuModule = Ext.extend(desktop.URLMenuModule, {
  init : function() {
    var items=[this.menuItem('ALICE Home','http://aliceinfo.cern.ch',900,800).makeIcon(constants.images.alice)
	       ,this.menuItem('ALICE DCS','http://cern.ch/alicedcs',900,800)
	       ,this.menuItem('ALICE DCS Monitoring','http://cern.ch/alicedcs/monitoring',1100,850)
    ];
    this._launch('ALICE Experiment',constants.images.alice,items);
  }
});


LbDesktop.ATLASMenuModule = Ext.extend(desktop.URLMenuModule, {
  init : function(){
    var items=[this.menuItem('ATLAS Home','http://atlas.ch',900,800).makeIcon(constants.images.atlas),
	       this.menuItem('ATLAS Computing','https://twiki.cern.ch/twiki/bin/view/Atlas/AtlasComputing',900,800)
    ];
    this._launch('ATLAS Experiment',constants.images.atlas,items);
  }
});

LbDesktop.CMSMenuModule = Ext.extend(desktop.URLMenuModule, {
  init : function(){
    var items=[this.menuItem('CMS Home','http://cms.cern.ch',900,800).makeIcon(constants.images.cms)
	      ,this.menuItem('Event Display (1)',           'http://cmsdoc.cern.ch/cmscc/cmstv/cmstv.jsp?channel=4',1000,1000)
	      ,this.menuItem('Event Display (2)',           'http://cmsdoc.cern.ch/cmscc/cmstv/cmstv.jsp?channel=5',1000,1000)
	      ,this.menuItem('Event Display (3)',           'http://cmsdoc.cern.ch/cmscc/cmstv/cmstv.jsp?channel=8',1000,1000)
	      ,this.menuItem('Tier 0 Status',                'https://cmsweb.cern.ch/T0Mon',1000,900)
    ];
    this._launch('CMS Experiment',constants.images.cms,items);
  }
});

var defineShortcuts = function() {
  var p = document.getElementById('x-shortcuts');
  p.appendChild(desktop.shortcut(constants.urls.lhcb.lhcb_run_status));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.fest_run_status));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.page1));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.detstatus));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.magnet));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.lhc_status));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.elog));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.lhc_project));
  p.appendChild(desktop.shortcut(constants.urls.lhcb.weekly_meetings));
  return p;
}

if ( _debugLoading ) alert('Script lhcb.display.desktop.cpp loaded successfully');
