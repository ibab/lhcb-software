_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** MUON web widgets
 *
 *  @author  M.Frank
 */
var MuonStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('MUON Operation State');

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
    var tab = lhcb.widgets.SubdetectorSystem(options);
    var tb = tab.body('MUON HV for Quadrants');
    tb.appendChild(tab.addFSMRow(['MUONA_HV_Q1M1A','MUONC_HV_Q3M1C']));
    tb.appendChild(tab.addFSMRow(['MUONA_HV_Q1M2A','MUONC_HV_Q3M2C']));
    tb.appendChild(tab.addFSMRow(['MUONA_HV_Q1M3A','MUONC_HV_Q3M3C']));
    tb.appendChild(tab.addFSMRow(['MUONA_HV_Q1M4A','MUONC_HV_Q3M4C']));
    tb.appendChild(tab.addFSMRow(['MUONA_HV_Q1M5A','MUONC_HV_Q3M5C']));
    tb.appendChild(tab.addFSMRow(['MUONA_HV_Q2M1A','MUONC_HV_Q4M1C']));
    tb.appendChild(tab.addFSMRow(['MUONA_HV_Q2M2A','MUONC_HV_Q4M2C']));
    tb.appendChild(tab.addFSMRow(['MUONA_HV_Q2M3A','MUONC_HV_Q4M3C']));
    tb.appendChild(tab.addFSMRow(['MUONA_HV_Q2M4A','MUONC_HV_Q4M4C']));
    tb.appendChild(tab.addFSMRow(['MUONA_HV_Q2M5A','MUONC_HV_Q4M5C']));
    tb = tab.body('MUON LV for Quadrants');
    tb.appendChild(tab.addFSMRow(['MUONA_DCS_LV_Q1M1A', 'MUONC_DCS_LV_Q3M1C']));
    tb.appendChild(tab.addFSMRow(['MUONA_DCS_LV_Q1M23A','MUONC_DCS_LV_Q3M23C']));
    tb.appendChild(tab.addFSMRow(['MUONA_DCS_LV_Q1M45A','MUONC_DCS_LV_Q3M45C']));
    tb.appendChild(tab.addFSMRow(['MUONA_DCS_LV_Q2M1A', 'MUONC_DCS_LV_Q4M1C']));
    tb.appendChild(tab.addFSMRow(['MUONA_DCS_LV_Q2M23A','MUONC_DCS_LV_Q4M23C']));
    tb.appendChild(tab.addFSMRow(['MUONA_DCS_LV_Q2M45A','MUONC_DCS_LV_Q4M45C']));
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
