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

  table.attachWidgets = function() {
    var opts = {style:'Arial12pt',legend:true,logger:this.logger};
    this.lhc_state           = lhcb.widgets.LHCStateSummary(opts);
    this.richPressures       = lhcb.widgets.rich.Pressures(opts);
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
    this.rich2DAQ.height = '100%';
    this.richDAQ            = lhcb.widgets.SystemSummary({ 	   system: 'RICH',
								   title: 'RICH Subsystem State',
								   split: false,
								   items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
								   legend:true,
								   rowing:true,
								   logger:this.logger});
    
    this.system = 'RICH1';
    this.rich1Voltages       = this.hvSummary({hv:true,lv:true,nosplit:true});
    this.system = 'RICH2';
    this.rich2Voltages       = this.hvSummary({hv:true,lv:true,legend:false,nosplit:true});

    this.left.addItem(this.rich1Voltages);
    this.left.addItem(this.rich2Voltages);
    this.right.addItem(this.lhc_state);
    this.right.addSpacer(90);
    this.right.addItem(this.richPressures);

    var tab = document.createElement('table');
    var tb = document.createElement('tb');
    var td, tr;

    tb.style.width  = '100%';
    tab.style.width = '100%';
    tr = document.createElement('tr');
    tr.appendChild(td=Cell('RICH',1,'MonitorDataHeader'));
    td.style.width = '100%';
    tr.appendChild(td=Cell('RICH 1',1,'MonitorDataHeader'));
    td.style.width = '100%';
    tr.appendChild(td=Cell('RICH 2',1,'MonitorDataHeader'));
    td.style.width = '100%';
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.richDAQ);
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.rich1DAQ);
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.rich2DAQ);

    this.subscriptions.push(this.richDAQ);
    this.subscriptions.push(this.rich1DAQ);
    this.subscriptions.push(this.rich2DAQ);
    
    tb.appendChild(tr);
    tab.appendChild(tb);
    this.beforeComment = tab;

  };
  return table;
};

var rich_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var rich_body = function() { return lhcb.widgets.SubdetectorPage.start(RichStatus); };
