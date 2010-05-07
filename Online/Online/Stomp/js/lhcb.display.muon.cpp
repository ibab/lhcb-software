_loadScript('lhcb.display.tooltips.cpp');
_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.constants.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** MUON web widgets
 *
 *  @author  M.Frank
 */
var MuonStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('MUON');

  table.options = {logo:    lhcb_logo('http://cern.ch/lhcb-muon/images/lhcbmulogo.gif'),
        	   logo_url:'http://cern.ch/lhcb-public/en/Detector/Muon-en.html',
		   title:   'MUON Detector Status',
		   tips:    'MUON operations status:<br>-- High/Low voltage status<br>-- Infrastructure'
  };


  lhcb.widgets.muon = new Object();

  /** MUON web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.muon.HVsystems = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr, cell;

    tab.logger = options.logger;
    tab.className = tb.className   = 'MonitorPage';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('MUON HV for Quadrants',6,options.style));
      tb.appendChild(tr);
    }
    tab.subscriptions=[];
    tab.addBoard = function(n1,n2,q) {
      var name1 = 'MUONA_'+q+'_'+n1, name2='MUONC_'+q+'_'+n2;
      var tr = document.createElement('tr');
      var cell=Cell(n1,1,'MonitorDataHeader');
      this[name1] = FSMItem('lbWeb.'+name1,this.logger,true);
      tr.appendChild(cell);
      tr.appendChild(this[name1]);
      tr.appendChild(this[name1].lock);
      this.subscriptions.push(this[name1]);
      cell=Cell(n2,1,'MonitorDataHeader');
      this[name2] = FSMItem('lbWeb.'+name2,this.logger,true);
      tr.appendChild(cell);
      tr.appendChild(this[name2]);
      tr.appendChild(this[name2].lock);
      this.subscriptions.push(this[name2]);
      return tr;
    };

    //tb.appendChild(tab.addBoard('Q1',   'Q3', 'HV'));
    //tb.appendChild(tab.addBoard('Q2',   'Q4', 'HV'));
    //tb.appendChild(tr = document.createElement('tr'));
    //tr.appendChild(Cell('',6,null));
    tb.appendChild(tab.addBoard('Q1M1A','Q3M1C', 'HV'));
    tb.appendChild(tab.addBoard('Q1M2A','Q3M2C', 'HV'));
    tb.appendChild(tab.addBoard('Q1M3A','Q3M3C', 'HV'));
    tb.appendChild(tab.addBoard('Q1M4A','Q3M4C', 'HV'));
    tb.appendChild(tab.addBoard('Q1M5A','Q3M5C', 'HV'));
    tb.appendChild(tab.addBoard('Q2M1A','Q4M1C', 'HV'));
    tb.appendChild(tab.addBoard('Q2M2A','Q4M2C', 'HV'));
    tb.appendChild(tab.addBoard('Q2M3A','Q4M3C', 'HV'));
    tb.appendChild(tab.addBoard('Q2M4A','Q4M4C', 'HV'));
    tb.appendChild(tab.addBoard('Q2M5A','Q4M5C', 'HV'));

    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('MUON LV for Quadrants',6,options.style));
      tb.appendChild(tr);
    }
    //tb.appendChild(tab.addBoard('Q1',   'Q3', 'DCS_LV'));
    //tb.appendChild(tab.addBoard('Q2',   'Q4', 'DCS_LV'));
    //tb.appendChild(tr = document.createElement('tr'));
    //tr.appendChild(Cell('',6,null));
    tb.appendChild(tab.addBoard('Q1M1A', 'Q3M1C',  'DCS_LV'));
    tb.appendChild(tab.addBoard('Q1M23A','Q3M23C', 'DCS_LV'));
    tb.appendChild(tab.addBoard('Q1M45A','Q3M45C', 'DCS_LV'));
    tb.appendChild(tab.addBoard('Q2M1A', 'Q4M1C',  'DCS_LV'));
    tb.appendChild(tab.addBoard('Q2M23A','Q4M23C', 'DCS_LV'));
    tb.appendChild(tab.addBoard('Q2M45A','Q4M45C', 'DCS_LV'));

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      for(var i=0; i<this.subscriptions.length;++i) {
	provider.subscribeItem(this.subscriptions[i]);
      }
    };
    return tab;
  };

  /** MUON web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.muon.Safety = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr;

    tab.logger = options.logger;
    tab.className = tb.className   = 'MonitorPage';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('MUON Safety',6,options.style));
      tb.appendChild(tr);
    }
    tab.muonSafety = FSMItem('lbWeb.MUON_Safety',options.logger,true);
    tab.gemSafety  = FSMItem('lbWeb.GEM_Safety', options.logger,true);
    tab.mwpcSafety = FSMItem('lbWeb.MWPC_Safety',options.logger,true);
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('MUON Safety',2,'MonitorDataHeader'));
      tr.appendChild(Cell('GEM',2,'MonitorDataHeader'));
      tr.appendChild(Cell('MWPC',2,'MonitorDataHeader'));
      tb.appendChild(tr);
    }
    tr = document.createElement('tr');
    tr.appendChild(tab.muonSafety);
    tr.appendChild(tab.muonSafety.lock);
    tr.appendChild(tab.gemSafety);
    tr.appendChild(tab.gemSafety.lock);
    tr.appendChild(tab.mwpcSafety);
    tr.appendChild(tab.mwpcSafety.lock);
    tb.appendChild(tr);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.muonSafety);
      provider.subscribeItem(this.gemSafety);
      provider.subscribeItem(this.mwpcSafety);
    };

    return tab;
  };

  table.attachWidgets = function() {
    var opts = {style:'Arial12pt',legend:true,logger:this.logger};
    this.muonVoltages       = this.hvSummary({hv:true,lv:false});
    this.lhc_state          = lhcb.widgets.LHCStateSummary(opts);
    this.muonDAQ            = lhcb.widgets.SystemSummary({style:'Arial12pt',
								  system: 'MUON',
								  title: 'MUON Subsystem State',
								  split: true,
								  items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
								  legend:true,
								  rowing:true,
								  logger:this.logger});

    this.muonHVquadrants = lhcb.widgets.muon.HVsystems(opts);
    this.muonSafety      = lhcb.widgets.muon.Safety({legend:true,logger:this.logger});

    this.left.addItem(this.muonVoltages);
    this.left.addItem(this.muonHVquadrants);
    this.right.addItem(this.lhc_state);
    this.right.addItem(this.muonSafety);
    this.right.addSpacer('150px');
    this.right.addItem(this.muonDAQ);
  };
  return table;
};

var muon_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var muon_body = function()   { return lhcb.widgets.SubdetectorPage.start(MuonStatus); };
