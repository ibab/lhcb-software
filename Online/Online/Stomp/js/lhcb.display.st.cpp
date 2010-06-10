_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** ST web widgets
 *
 *  @author  M.Frank
 */
var StStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('ST Operation State');

  table.options = {logo:    lhcb_logo('http://cern.ch/lhcb-public/Objects/Detector/Tracking-system-diagram-2.jpg'),
        	   logo_url:'http://cern.ch/lhcb-public/en/Detector/Trackers-en.html',
		   title:   'ST Detector Status',
		   tips:    'ST operations status:<br>-- High/Low voltage status<br>-- Tell1 Board Status<br>-- Infrastructure'
  };


  lhcb.widgets.st = new Object();

  /** ST web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.st.STsystems = function(options) {
    var tab = document.createElement('table');
    tab.subscriptions=[];
    tab.logger = options.logger;
    tab.className = 'MonitorPage';

    tab.addBoard = function(n1,n2,q) {
      var name1 = 'STA_'+q+n1, name2='STC_'+q+n2;
      var tr = document.createElement('tr');
      var cell=Cell('STA/'+n1,1,'MonitorDataHeader');
      this[name1] = FSMItem('lbWeb.'+name1,this.logger,true);
      tr.appendChild(cell);
      tr.appendChild(this[name1]);
      tr.appendChild(this[name1].lock);
      this.subscriptions.push(this[name1]);
      cell=Cell('STC/'+n2,1,'MonitorDataHeader');
      this[name2] = FSMItem('lbWeb.'+name2,this.logger,true);
      tr.appendChild(cell);
      tr.appendChild(this[name2]);
      tr.appendChild(this[name2].lock);
      this.subscriptions.push(this[name2]);
      return tr;
    };

    tab.addStations = function(title,id) {
      var tb = document.createElement('tbody');
      tb.className   = 'MonitorPage';
      if ( options.style ) {
	var tr = document.createElement('tr');
	tr.appendChild(Cell(title,6,options.style));
	tb.appendChild(tr);
      }
      tb.appendChild(tab.addBoard('T1', 'T1', id));
      tb.appendChild(tab.addBoard('T2', 'T2', id));
      tb.appendChild(tab.addBoard('T3', 'T3', id));
      this.appendChild(tb);
      return this;
    };
    tab.addHV = function() { return this.addStations('High Voltage per Station','HV_'); };
    tab.addLV = function() { return this.addStations('Low Voltage per Station', 'DCS_LV_'); };
    tab.addFE = function() { return this.addStations('Front end (FE) Status per Station','DAQ_FEE_'); };
    tab.addTell1 = function() { return this.addStations('TELL1 Board Status per Station','DAQ_TELL1_'); };

    tab.subscribe = function(provider) {
      for(var i=0; i<this.subscriptions.length;++i) {
	provider.subscribeItem(this.subscriptions[i]);
      }
    };
    return tab;
  };

  table.attachWidgets = function() {
    var opts = {style:'Arial12pt',legend:true,logger:this.logger};
    var ttDAQ = lhcb.widgets.SystemSummary({style:'Arial12pt',
						    system: 'TT',
						    title: 'TT Subsystem State',
						    split: false,
						    items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
						    legend:true,
						    rowing:true,
						    logger:this.logger});
    var itDAQ = lhcb.widgets.SystemSummary({style:'Arial12pt',
						    system: 'IT',
						    title: 'IT Subsystem State',
						    split: false,
						    items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
						    legend:true,
						    rowing:true,
						    logger:this.logger});
    this.system = 'IT';
    opts.it = true;
    this.left.addItem(this.hvSummary({hv:true,lv:true,nosplit:true}));
    this.left.addItem(lhcb.widgets.SafetySummary(opts));
    this.left.addItem(lhcb.widgets.DSS.CaVPlantStatus('It',opts));
    this.left.addSpacer('20px');
    this.left.addItem(itDAQ);

    this.system = 'TT';
    opts.it = false;
    opts.tt = true;
    this.right.addItem(this.hvSummary({hv:true,lv:true,nosplit:true}));
    this.right.addItem(lhcb.widgets.SafetySummary(opts));
    this.right.addItem(lhcb.widgets.DSS.CaVPlantStatus('Tt',opts));
    this.right.addItem(ttDAQ);

    //this.right.addItem(lhcb.widgets.LHCStateSummary(opts));
  };
  return table;
};

var st_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var st_body = function()   { return lhcb.widgets.SubdetectorPage.start(StStatus); };
