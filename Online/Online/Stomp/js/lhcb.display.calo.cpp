_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** CALO web widgets
 *
 *  @author  M.Frank
 */
var CaloStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('CALO Operation State');

  table.options = {logo:     null, //lhcb_logo('http://lbtwiki.cern.ch/pub/CALO/WebHome/LOGO.png'),
        	   logo_url: 'http://cern.ch/lhcb-public/en/Detector/Calorimeters-en.html',
		   title:    'CALO Detector Status',
		   tips:     'CALO operations status:<br>-- High/Low voltage status<br>-- Infrastructure'
  };

  table.beforeComment = function() {
    this.ecalDAQ            = lhcb.widgets.SystemSummary({	   system: 'ECAL',
								   title: 'ECAL Subsystem State',
								   split: false,
								   items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
								   legend:true,
								   rowing:true,
								   logger:this.logger});
    
    this.hcalDAQ            = lhcb.widgets.SystemSummary({	   system: 'HCAL',
								   title: 'HCAL Subsystem State',
								   split: false,
								   items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
								   legend:true,
								   rowing:true,
								   logger:this.logger});
    this.prsDAQ            = lhcb.widgets.SystemSummary({ 	   system: 'PRS',
								   title: 'PRS Subsystem State',
								   split: false,
								   items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
								   legend:true,
								   rowing:true,
								   logger:this.logger});
    
    var td, tr, tab, tb, tr0;
    tab = document.createElement('table');
    tb = document.createElement('tbody');
    tr0 = document.createElement('tr');
    td0 = document.createElement('td');
    this.tbody.appendChild(tr0);
    td0.appendChild(tab);
    td0.colSpan = 2;
    tr0.appendChild(td0);
    tab.appendChild(tb);

    tr = document.createElement('tr');
    tr.appendChild(td=Cell('ECAL',1,'MonitorDataHeader'));
    td.style.width = '38%';
    tr.appendChild(td=Cell('HCAL',1,'MonitorDataHeader'));
    td.style.width = '38%';
    tr.appendChild(td=Cell('PRS',1,'MonitorDataHeader'));
    td.style.width = '50%';
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.ecalDAQ);
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.hcalDAQ);
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.prsDAQ);
    tb.appendChild(tr);

    this.subscriptions.push(this.prsDAQ);
    this.subscriptions.push(this.ecalDAQ);
    this.subscriptions.push(this.hcalDAQ);
  };
  table.attachWidgets = function() {
    var opts = {style:'Arial12pt',legend:true,calo:true,logger:this.logger};
    var lhc_state           = lhcb.widgets.LHCStateSummary(opts);
    this.system = 'ECAL';
    var ecalVoltages       = this.hvSummary({hv:true,lv:false,nosplit:true});
    this.system = 'HCAL';
    var hcalVoltages       = this.hvSummary({hv:true,lv:false,nosplit:true});
    var safety              = lhcb.widgets.SafetySummary(opts);
    this.left.addItem(ecalVoltages);
    this.left.addItem(hcalVoltages);
    this.right.addItem(lhc_state);
  };
  return table;
};

var calo_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var calo_body = function() { return lhcb.widgets.SubdetectorPage.start(CaloStatus); };
