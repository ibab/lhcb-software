//------------------------------------------------------------------------------------------------------------------------------------------------
if ( !_lhcb().constants ) {
  lhcb.constants = function() {
    this.lhcb_comet_url = function() { return 'http://lbcomet.cern.ch/static/RunStatus'; }

    this.lhcb_display_url = function(name) {    return this.lhcb_comet_url()+'/lhcb.display.htm?type='+name; }
    this.lhcb_static      = function(name) {    return 'http://cern.ch/frankm/Online/'+name; }
    this.operations_url   = function(name) {    return 'http://op-webtools.web.cern.ch/op-webtools/vistar/vistars.php?usr='+name; }
    this.sls_service_url  = function(name) {    return 'http://sls.cern.ch/sls/service.php?id='+name; }
    this.mmm_url          = function(name) {    var n = 'https://mmm.cern.ch'; if ( name ) n = n + name; return n; }

    this.mkAbsImage       = function(name) {    return {src: name, icon: name}; }
    this.mkStaticImage    = function(name) {    return this.mkAbsImage(this.lhcb_static('Images/'+name)); }
    this.mkImage=function(name,ext) {
      var obj  = new Object();
      var n    = 'Images/'+name;
      obj.src  = this.lhcb_static(n+'_32x32.'+ext);
      obj.icon = this.lhcb_static(n+'_16x16.'+ext);
      return obj;
    }

    this.images = {
      bogus:            this.mkAbsImage(this.lhcb_static('Desktop/images/bogus.png')),
      window:           this.mkAbsImage(this.lhcb_static('Desktop/images/bogus.png')),
      gears:            this.mkAbsImage(this.lhcb_static('Desktop/images/gears.png')),
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
      mail:             this.mkAbsImage('https://mmmservices.web.cern.ch/mmmservices/Images/Icons/outlook.gif'),
      webstats:         this.mkAbsImage('https://web-statistics.web.cern.ch/web-statistics/icon/other/awstats_logo6.png'),

      twiki:            this.mkStaticImage('twiki.gif'),
      lhcb:             this.mkAbsImage('http://lhcb.web.cern.ch/lhcb/lhcblogo.gif'),
      lhcb_online:      this.mkAbsImage('http://lhcb-online.web.cern.ch/lhcb-online/elog/images/lhcb-online-logo.png'),
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
      beams_department: this.mkImage('BeamsDep','jpg'),
      cern:             this.mkAbsImage('http://lhc.web.cern.ch/lhc/images/logoCERN.GIF')
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
	src:    'http://cern.ch',
	img:    this.images.cern,
	title:  'CERN',
	nick:   'cern-win',
	cls:    'bogus',
	size:   {width:  1000,	height: 850}
      },
      lhc: {
	project: {
	  src:    'http://cern.ch/lhc',
	  img:    this.images.lhc,
	  title:  'LHC Project',
	  nick:   'lhc-win',
	  cls:    'LHC16',
	  iconCls:'LHC32',
	  size:   {width:  1000,	height: 850}
	}
      },
      lhcb: {
	home_page: {
	  src:    'http://lhcb.cern.ch',
	  img:    this.images.window,
	  title:  'Home Page',
	  nick:   'lhcb-home-page-win',
	  cls:    'bogus',
	  size:   {width:  900,	height: 800}
	},
	dirac: {
	  home_page: {
	    src:    'http://lhcbweb.pic.es',
	    img:    this.images.dirac,
	    title:  'Dirac Home Page',
	    nick:   'lhcb-dirac-home-page-win',
	    cls:    'bogus',
	    size:   {width:  900,	height: 800}
	  },
	  launch_pad: {
	    src:    'https://launchpad.net/dirac',
	    img:    this.images.window,
	    title:  'Launch pad',
	    nick:   'lhcb-dirac_launch_pad-win',
	    cls:    'bogus',
	    size:   {width:  900,	height: 800}
	  }
	},
	online_home_page: {
	  src:    'http://lhcb-online.web.cern.ch/lhcb-online',
	  img:    this.images.home,
	  title:  'Online Home Page',
	  nick:   'lhcb-online-home-page-win',
	  cls:    'Home16',
	  iconCls:'Home32',
	  size:   {width:  900,	height: 800}
	},
	computing_home_page: {
	  src:    'https://lhcb-comp.web.cern.ch/lhcb-comp',
	  img:    this.images.window,
	  title:  'Computing Home Page',
	  nick:   'lhcb-computing-home-page-win',
	  cls:    'bogus',
	  size:   {width:  900,	height: 800}
	},
	twiki: {
	  src:    'https://twiki.cern.ch/twiki/bin/view/LHCb',
	  img:    this.images.window,
	  title:  'TWiki',
	  nick:   'lhcb-twiki-page-win',
	  cls:    'bogus',
	  iconCls:'bogus',
	  size:   {width:  900,	height: 800}
	},
	lhcb_run_status: {
	  src:    this.lhcb_display_url('status&system=LHCb'),
	  img:    this.images.lookfile,
	  title:  'LHCb Run Status',
	  nick:   'lhcb-daq-status-win',
	  cls:    'LHCbDAQ16',
	  iconCls:'LHCbDAQ32',
	  size:   {width:  950,	height: 820}
	},
	fest_run_status: {
	  src:    this.lhcb_display_url('status&system=FEST'),
	  img:    this.images.options,
	  title:  'FEST Run Status',
	  nick:   'fest-daq-status-win',
	  cls:    'FESTDAQ16',
	  iconCls:'FESTDAQ32',
	  size:   {width:  950,	height: 600}
	},
	sdet_run_status: {
	  src:    this.lhcb_display_url('status'),
	  img:    this.images.lookjob,
	  title:  'Subdetector Run Status',
	  nick:   'lhcb-sdet-daq-status-win',
	  cls:    'SdetDAQ16',
	  iconCls:'SdetDAQ32',
	  size:   {width:  950,	height: 820}
	},
	display_news: {
	  src:    this.lhcb_static('html/News.htm'),
	  img:    this.images.help,
	  title:  'LHCb Online Displays',
	  nick:   'lhcb-display-news-win',
	  cls:    'Help16',
	  iconCls:'Help32',
	  size:   {width:  590,	height: 700}
	},
	page1: {
	  src:    this.lhcb_display_url('page1'),
	  img:    this.images.search,
	  title:  'LHCb Page 1',
	  nick:   'lhcb-page1-win',
	  cls:    'Page116',
	  iconCls:'Page132',
	  size:   {width:  770,	height: 820}
	},
	detstatus: {
	  src:    this.lhcb_display_url('detstatus'),
	  img:    this.images.wizard,
	  title:  'HV Status',
	  nick:   'lhcb-detstatus-win',
	  cls:    'DetStatus16',
	  iconCls:'DetStatus32',
	  size:   {width:  950,	height: 500}
	},
	operstatus: {
	  src:    this.lhcb_display_url('oper'),
	  img:    this.images.gears,
	  title:  'Operations Status',
	  nick:   'lhcb-oper-win',
	  cls:    'OperStatus16',
	  iconCls:'OperStatus32',
	  size:   {width:  950,	height: 850}
	},
	magnet: {
	  src:    this.lhcb_display_url('magnet'),
	  img:    this.images.magnet,
	  title:  'LHCb Magnet Summary',
	  nick:   'lhcb-magnet-win',
	  cls:    'Magnet16',
	  iconCls:'Magnet32',
	  size:   {width:  950,	height: 820}
	},
	collimators: {
	  src:    this.lhcb_display_url('collimators'),
	  img:    this.images.lookprod,
	  title:  'Collimators',
	  nick:   'lhcb-collimators-win',
	  cls:    'Collimators16',
	  iconCls:'Collimators32',
	  size:   {width:  950,	height: 400}
	},
	bcm: {
	  src:    this.lhcb_display_url('bcm'),
	  img:    this.images.bcm,
	  title:  'BCM',
	  nick:   'lhcb-bcm-win',
	  cls:    'BCM16',
	  iconCls:'BCM32',
	  size:   {width:  900,	height: 400}
	},
	lhc_status: {
	  src:    this.lhcb_display_url('lhc'),
	  img:    this.images.beams,
	  title:  'LHC Status',
	  nick:   'lhcb-lhc-status-win',
	  cls:    'Beams16',
	  iconCls:'Beams32',
	  size:   {width:  900,	height: 820}
	},
	elog: {
	  src:    'http://lblogbook.cern.ch/Shift/',
	  img:    this.images.elog,
	  title:  'ELog LHCb',
	  nick:   'lhcb-elog-win',
	  cls:    'ELOG16',
	  iconCls:'ELOG32',
	  size:   {width:  1000,	height: 850}
	},
	weekly_meetings: {
	  src:    'http://indico.cern.ch/categOverview.py?selCateg=1l22&period=week&detail=conference',
	  img:    this.images.indico,
	  title:  'Meetings',
	  nick:   'lhcb-meetings-win',
	  cls:    'INDICO16',
	  iconCls:'INDICO32',
	  size:   {width:  1000,	height: 850}
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
	}
      }
    };
    return this;
  }();

  if ( _debugLoading ) alert('Script lhcb.display.constants.cpp loaded successfully');
}
