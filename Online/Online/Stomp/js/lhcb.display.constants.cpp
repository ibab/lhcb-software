//------------------------------------------------------------------------------------------------------------------------------------------------
if ( !_lhcb().constants ) {
  lhcb.constants = function() {
    this._url = (''+document.location).substr(0,(''+document.location).indexOf('/Online/')+7);//'http://www.cern.ch/lhcbproject/online/comet/Online';
    this._loc = ''+document.location;
    this.lhcb_comet_url2  = function()     {    return '//lbcomet.cern.ch/static/RunStatus';  };
    this.lhcb_comet_url   = function()     {    return this._url;                                  };
    this.lhcb_base_url    = function()     {    return this._url;                                  };
    //alert('\nDomain:'+document.domain+'\nURL:'+this._url+'\nLoc:'+this._loc+'\nComet:'+this.lhcb_comet_url()+'\nBase:'+this.lhcb_base_url()+'\n');

    if ( this._loc.indexOf('/frankm/')>0 ) {
      this.lhcb_display_url = function(name) {    return this.lhcb_comet_url()+'/lhcb.display.htm?type='+name;     };
    }
    else {
      this.lhcb_display_url = function(name) {    return this.lhcb_comet_url()+'/lhcb.comet.htm?type='+name;       };
    }
    this.lhcb_static      = function(name) {    return this.lhcb_base_url()+'/'+name;                              };
    this.operations_url   = function(name) {    return '//op-webtools.web.cern.ch/op-webtools/vistar/vistars.php?usr='+name; };
    this.sls_service_url  = function(name) {    return '//sls.cern.ch/sls/service.php?id='+name;              };
    this.service_now_url  = function(name) {    return '//cern.service-now.com/service-portal/'+name;        };
    this.mmm_url          = function(name) {    var n = '//mmm.cern.ch'; if ( name ) n = n + name; return n; };

    this.mkAbsImage       = function(name) {    return {src: name, icon: name};                                    };
    this.mkStaticImage    = function(name) {    return this.mkAbsImage(this.lhcb_static('Images/'+name));          };
    this.mkImage=function(name,ext) {
      var obj  = new Object();
      var n    = 'Images/'+name;
      obj.src  = this.lhcb_static(n+'_32x32.'+ext);
      obj.icon = this.lhcb_static(n+'_16x16.'+ext);
      return obj;
    };

    this.images = {
      bogus:            this.mkAbsImage(this.lhcb_static('Desktop/images/bogus.png')),
      window:           this.mkAbsImage(this.lhcb_static('Desktop/images/bogus.png')),
      gears:            this.mkImage('Collision','png'),
      hlt2:             this.mkImage('Hlt2','png'),
      wizard:           this.mkImage('Wizard','gif'),
      options:          this.mkImage('Options','gif'),
      lookfile:         this.mkImage('LookFile','gif'),
      lookjob:          this.mkImage('LookJob','gif'),
      lookprod:         this.mkImage('LookProd','gif'),
      search:           this.mkImage('Search','gif'),
      wizard:           this.mkImage('Wizard','gif'),
      home:             this.mkImage('Home','gif'),
      help:             this.mkImage('Help','gif'),
      database:         this.mkStaticImage('database.jpg'),
      mail:             this.mkAbsImage('//mmmservices.web.cern.ch/mmmservices/Images/Icons/outlook.gif'),
      webstats:         this.mkAbsImage('//web-statistics.web.cern.ch/web-statistics/icon/other/awstats_logo6.png'),

      twiki:            this.mkStaticImage('twiki.gif'),
      lhcb:             this.mkAbsImage('//lhcb.web.cern.ch/lhcb/lhcblogo.gif'),
      lhcb_online:      this.mkAbsImage('Images/lhcb-online-logo.png'),
      dirac:            this.mkStaticImage('DIRAC.png'),
      magnet:           this.mkImage('LHCb/Magnet_Icon','png'),
      magnet_big:       this.mkStaticImage('LHCb/Magnet.jpg'),
      bcm:              this.mkImage('BCM','png'),
      lhc:              this.mkImage('LHC','gif'),
      elog:             this.mkImage('ELOG','png'),
      indico:           this.mkImage('indico','gif'),

      alice:            this.mkImage('ALICE','gif'),
      atlas:            this.mkImage('ATLAS','png'),
      cms:              this.mkImage('CMS','gif'),

      ramses:           this.mkStaticImage('LHCb/Ramses_icon.png'),
      beams:            this.mkImage('Beams','jpg'),
      beams_department: this.mkImage('BE-BeamsDep','jpg'),
      cern:             this.mkImage('cern-logo','jpg'),
      alarmScreen:	this.mkImage('AlarmScreen', 'png')
    };
    this.images.dirac.iconSize = { 
      small: { width: 16, height: 16},
      large: { width: 32, height: 32}
    };
    this.images.webstats.iconSize = { 
      small: { width: 32, height: 16},
      large: { width: 64, height: 32}
    };
    this.images.lhcb.iconSize = { 
      small: { width: 24, height: 16},
      large: { width: 48, height: 32}
    };
    this.images.beams_department.iconSize = { 
      small: { width: 80,  height: 16},
      large: { width: 160, height: 32}
    };

    this.urls = {
      empty: {
	src:       '',
	title:     'Unknown',
	img:       this.images.bogus,
	nick:      '',
	size:      {width:100,height:100}
      },
      cern: {
        src:    '//home.web.cern.ch', //'http://cern.ch',
	img:    this.images.cern,
	title:  'CERN',
	nick:   'cern-win',
	cls:    'bogus',
	size:   {width:  1100,	height: 850}
      },
      lhc: {
	project: {
	  src:    '//home.web.cern.ch/topics/large-hadron-collider',
	  img:    this.images.lhc,
	  title:  'LHC Project',
	  nick:   'lhc-win',
	  cls:    'LHC16',
	  iconCls:'LHC32',
	  size:   {width:  1100,	height: 850}
	}
      },
      lhcb: {
	subdetectors: {
	  daq_lumi: {
	    src:    this.lhcb_display_url('lumi'),
	    img:    this.images.lookprod,
	    title:  'DAQ/Lumi',
	    nick:   'lhcb-lumi-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  1150,	height: 850}
	  },
	  daq_trigger: {
	    src:    this.lhcb_display_url('trigger'),
	    img:    this.images.lookprod,
	    title:  'DAQ/Trigger',
	    nick:   'lhcb-trigger-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  1150,	height: 950}
	  },
	  daq_bigbrother: {
	    src:    this.lhcb_display_url('bigbrother'),
	    img:    this.images.lookprod,
	    title:  'DAQ/Big Brother',
	    nick:   'lhcb-bigbrother-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  1150,	height: 850}
	  },
	  daq_evdisp: {
	    src:    this.lhcb_display_url('evdisp'),
	    img:    this.images.lookprod,
	    title:  'DAQ/Event Display',
	    nick:   'lhcb-evdisp-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  1150,	height: 850}
	  },
	  trg_l0status: {
	    src:    this.lhcb_display_url('trgstatus'),
	    img:    this.images.lookprod,
	    title:  'TRG/L0 Status',
	    nick:   'lhcb-trigger-l0status-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  1150,	height: 850}
	  },
	  velo: {
	    src:    this.lhcb_display_url('velo'),
	    img:    this.images.lookprod,
	    title:  'VELO/LV/HV',
	    nick:   'lhcb-velo-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  1150,	height: 900},
	    motion: {
	      src:    this.lhcb_display_url('velo_move'),
	      img:    this.images.lookprod,
	      title:  'VELO/Motion',
	      nick:   'lhcb-velo-motion-win',
	      cls:    'Collimators16',
	      iconCls:'Collimators32',
	      size:   {width:  1250,	height: 900}
	    }
	  },
	  st: {
	    src:    this.lhcb_display_url('st'),
	    img:    this.images.lookprod,
	    title:  'ST',
	    nick:   'lhcb-st-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  1150,	height: 950}
	  },
	  ot: {
	    src:    this.lhcb_display_url('ot'),
	    img:    this.images.lookprod,
	    title:  'OT',
	    nick:   'lhcb-ot-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  1150,	height: 900}
	  },
	  rich: {
	    src:    this.lhcb_display_url('rich'),
	    img:    this.images.lookprod,
	    title:  'RICH',
	    nick:   'lhcb-rich-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  1150,	height: 900}
	  },
	  calo: {
	    src:    this.lhcb_display_url('calo'),
	    img:    this.images.lookprod,
	    title:  'CALO',
	    nick:   'lhcb-calo-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  1150,	height: 950}
	  },
	  muon: {
	    src:    this.lhcb_display_url('muon'),
	    img:    this.images.lookprod,
	    title:  'MUON',
	    nick:   'lhcb-muon-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  1150,	height: 850}
	  },
	  cooling: {
	    src:    this.lhcb_display_url('cooling'),
	    img:    this.images.lookprod,
	    title:  'Cooling',
	    nick:   'lhcb-cooling-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  1150,	height: 700}
	  },
	  dq: {
	    src:    this.lhcb_display_url('dataquality'),
	    img:    this.images.lookprod,
	    title:  'Online DataQuality',
	    nick:   'lhcb-dataquality-win',
	    cls:    'Collimators16',
	    iconCls:'Collimators32',
	    size:   {width:  800,	height: 600}
	  }
	},
	dirac: {
	  home_page: {
	    src:    'http://diracgrid.org',
	    img:    this.images.dirac,
	    title:  'Dirac Home Page',
	    nick:   'lhcb-dirac-home-page-win',
	    cls:    'bogus',
	    size:   {width:  900,	height: 800}
	  }
	},
	home_page: {
	  src:    '//lhcb.web.cern.ch/lhcb',
	  img:    this.images.window,
	  title:  'Home Page',
	  nick:   'lhcb-home-page-win',
	  cls:    'bogus',
	  size:   {width:  1150,	height: 800}
	},
	online_home_page: {
	  src:    '//lhcb-online.web.cern.ch/lhcb-online',
	  img:    this.images.home,
	  title:  'Online Home Page',
	  nick:   'lhcb-online-home-page-win',
	  cls:    'Home16',
	  iconCls:'Home32',
	  size:   {width:  1150,	height: 800}
	},
	computing_home_page: {
	  src:    '//lhcb-comp.web.cern.ch/lhcb-comp',
	  img:    this.images.window,
	  title:  'Computing Home Page',
	  nick:   'lhcb-computing-home-page-win',
	  cls:    'bogus',
	  size:   {width:  1150,	height: 800}
	},
	twiki: {
	  src:    '//twiki.cern.ch/twiki/bin/view/LHCb',
	  img:    this.images.window,
	  title:  'TWiki',
	  nick:   'lhcb-twiki-page-win',
	  cls:    'bogus',
	  iconCls:'bogus',
	  size:   {width:  1150,	height: 800}
	},
	lhcb_run_status: {
	  src:    this.lhcb_display_url('status&system=LHCb'),
	  img:    this.images.lookfile,
	  title:  'LHCb Run Status',
	  nick:   'lhcb-daq-status-win',
	  cls:    'LHCbDAQ16',
	  iconCls:'LHCbDAQ32',
	  size:   {width:  1150,	height: 820}
	},
	lhcb2_run_status: {
	  src:    this.lhcb_display_url('hlt2'),
	  img:    this.images.hlt2,
	  title:  'HLT2 Status',
	  nick:   'hlt2-status-win',
	  cls:    'LHCbHLT16',
	  iconCls:'LHCbHLT32',
	  size:   {width:  1150,	height: 820}
	},
	fest_run_status: {
	  src:    this.lhcb_display_url('status&system=FEST'),
	  img:    this.images.options,
	  title:  'FEST Run Status',
	  nick:   'fest-daq-status-win',
	  cls:    'FESTDAQ16',
	  iconCls:'FESTDAQ32',
	  size:   {width:  1150,	height: 500}
	},
	sdet_run_status: {
	  src:    this.lhcb_display_url('status'),
	  img:    this.images.lookjob,
	  title:  'Subdetector Run Status',
	  nick:   'lhcb-sdet-daq-status-win',
	  cls:    'SdetDAQ16',
	  iconCls:'SdetDAQ32',
	  size:   {width:  1150,	height: 820}
	},
	display_news: {
	  src:    this.lhcb_static('html/News.htm'),
	  img:    this.images.help,
	  title:  'LHCb Online Displays',
	  nick:   'lhcb-display-news-win',
	  cls:    'Help16',
	  iconCls:'Help32',
	  size:   {width:  650,	       height: 700}
	},
	page1: {
	  src:    this.lhcb_display_url('page1'),
	  img:    this.images.search,
	  title:  'LHCb Page 1',
	  nick:   'lhcb-page1-win',
	  cls:    'Page116',
	  iconCls:'Page132',
	  size:   {width:  1100,	height: 820}
	},
	detstatus: {
	  src:    this.lhcb_display_url('detstatus'),
	  img:    this.images.wizard,
	  title:  'HV Status',
	  nick:   'lhcb-detstatus-win',
	  cls:    'DetStatus16',
	  iconCls:'DetStatus32',
	  size:   {width:  1150,	height: 850}
	},
	operstatus: {
	  src:    this.lhcb_display_url('oper'),
	  img:    this.images.gears,
	  title:  'Operations Status',
	  nick:   'lhcb-oper-win',
	  cls:    'OperStatus16',
	  iconCls:'OperStatus32',
	  size:   {width:  1150,	height: 950}
	},
	magnet: {
	  src:    this.lhcb_display_url('magnet'),
	  img:    this.images.magnet,
	  title:  'LHCb Magnet Summary',
	  nick:   'lhcb-magnet-win',
	  cls:    'Magnet16',
	  iconCls:'Magnet32',
	  size:   {width:  1150,	height: 820}
	},
	collimators: {
	  src:    this.lhcb_display_url('collimators'),
	  img:    this.images.lookprod,
	  title:  'Collimators',
	  nick:   'lhcb-collimators-win',
	  cls:    'Collimators16',
	  iconCls:'Collimators32',
	  size:   {width:  1150,	height: 550}
	},
	bcm: {
	  src:    this.lhcb_display_url('bcm'),
	  img:    this.images.bcm,
	  title:  'BCM',
	  nick:   'lhcb-bcm-win',
	  cls:    'BCM16',
	  iconCls:'BCM32',
	  size:   {width:  900,	height: 270}
	},
	lhc_status: {
	  src:    this.lhcb_display_url('lhc'),
	  img:    this.images.beams,
	  title:  'LHC Status',
	  nick:   'lhcb-lhc-status-win',
	  cls:    'Beams16',
	  iconCls:'Beams32',
	  size:   {width:  1150,	height: 820}
	},
	elog: {
	  src:    '//lblogbook.cern.ch/Shift/',
	  img:    this.images.elog,
	  title:  'ELog LHCb',
	  nick:   'lhcb-elog-win',
	  cls:    'ELOG16',
	  iconCls:'ELOG32',
	  size:   {width:  1150,	height: 850}
	},
	rundb: {
	  src:    '//lbrundb.cern.ch/',
	  img:    this.images.lookprod,
	  title:  'LHCb run database',
	  nick:   'lhcb-rundb-win',
	  cls:    'bogus',
	  iconCls:'bogus',
	  size:   {width:  1100,	height: 850}
	},
	weekly_meetings: {
	  src:    '//indico.cern.ch/categOverview.py?selCateg=1l22&period=week&detail=conference',
	  img:    this.images.indico,
	  title:  'Meetings',
	  nick:   'lhcb-meetings-win',
	  cls:    'INDICO16',
	  iconCls:'INDICO32',
	  size:   {width:  1200,	height: 850}
	},
	offline_databases_status: {
	  src:    this.sls_service_url('phydb_LHCBR'),
	  img:    this.images.window,
	  title:  'Offline databases',
	  nick:   'lhcb-offline_databases_status-win',
	  cls:    'bogus',
	  iconCls:'bogus',
	  size:   {width:  1100,	height: 800}
	},
	online_databases_status: {
	  src:    this.sls_service_url('phydb_LHCBONR'),
	  img:    this.images.window,
	  title:  'Online databases',
	  nick:   'lhcb-online_databases_status-win',
	  cls:    'bogus',
	  iconCls:'bogus',
	  size:   {width:  1100,	height: 800}
        },
	alarmScreen: {
	  src:    this.lhcb_display_url('alarms'),
	  img:    this.images.alarmScreen,
	  title:  'LHCb PVSS Alarms',
	  nick:   'lhcb-pvss-alarms',
	  cls:    'PvssAlarms16',
	  iconCls:'bogus',
	  size:   {width: 1000,	height: 820}
	},
	eventDisplay: {
	  src:    'https://lbevent.cern.ch/EventDisplay/index.html',
	  img:    this.images.alarmScreen,
	  title:  'LHCb Event Display',
	  nick:   'lhcb-event-display',
	  cls:    'bogus',
	  iconCls:'bogus',
	  size:   {width: 1100,	height: 820}
	}
      }
    };
    return this;
  }();

  if ( _debugLoading ) alert('Script lhcb.display.constants.cpp loaded successfully');
}
