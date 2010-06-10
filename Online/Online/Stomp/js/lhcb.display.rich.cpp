_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** RICH web widgets
 *
 *  @author  M.Frank
 */
var RichStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('RICH Operation State');

  table.options = {logo:     lhcb_logo('http://lbtwiki.cern.ch/pub/RICH/WebHome/LOGO.png'),
        	   logo_url: 'http://cern.ch/lhcb-public/en/Detector/RICH-en.html',
		   title:    'RICH Detector Status',
		   tips:     'RICH operations status:<br>-- High/Low voltage status<br>-- Infrastructure'
  };

  table.beforeComment = function() {
    this.rich1DAQ            = lhcb.widgets.SystemSummary({	   system: 'RICH1',
								   title: 'RICH1 Subsystem State',
								   split: false,
								   items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
								   legend:true,
								   rowing:true,
								   logger:this.logger});
    
    this.rich2DAQ            = lhcb.widgets.SystemSummary({	   system: 'RICH2',
								   title: 'RICH2 Subsystem State',
								   split: false,
								   items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
								   legend:true,
								   rowing:true,
								   logger:this.logger});
    this.richDAQ            = lhcb.widgets.SystemSummary({ 	   system: 'RICH',
								   title: 'RICH Subsystem State',
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

    tab.className = tb.className = 'MonitorPage';
    tr = document.createElement('tr');
    tr.appendChild(td=Cell('RICH',1,'MonitorDataHeader'));
    tr.appendChild(td=Cell('RICH 1',1,'MonitorDataHeader'));
    tr.appendChild(td=Cell('RICH 2',1,'MonitorDataHeader'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.richDAQ);
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.rich1DAQ);
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.rich2DAQ);
    tb.appendChild(tr);

    this.subscriptions.push(this.richDAQ);
    this.subscriptions.push(this.rich1DAQ);
    this.subscriptions.push(this.rich2DAQ);
  };
  table.attachWidgets = function() {
    var opts = {style:'Arial12pt',legend:true,rich:true,logger:this.logger};
    this.system = 'RICH1';
    this.left.addItem(this.hvSummary({hv:true,lv:true,nosplit:true}));
    this.system = 'RICH2';
    this.left.addItem(this.hvSummary({hv:true,lv:true,nosplit:true}));
    this.system = 'RICH';
    this.left.addSpacer(10);
    this.left.addItem(lhcb.widgets.SafetySummary(opts));

    this.right.addItem(lhcb.widgets.LHCStateSummary(opts));
    this.right.addItem(lhcb.widgets.rich.Pressures(opts));
    this.right.addItem(lhcb.widgets.DSS.CaVPlantStatus('Rich',opts));
  };
  return table;
};

var rich_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var rich_body = function() { return lhcb.widgets.SubdetectorPage.start(RichStatus); };
