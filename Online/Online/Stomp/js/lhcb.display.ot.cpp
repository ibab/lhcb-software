_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** OT web widgets
 *
 *  @author  M.Frank
 */
var OtStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('OT Operation State');

  table.options = {logo:    lhcb_logo('http://cern.ch/lhcb-public/Objects/Detector/Tracking-system-diagram-2.jpg'),
        	   logo_url:'http://cern.ch/lhcb-public/en/Detector/Trackers-en.html',
		   title:   'OT Detector Status',
		   tips:    'OT operations status:<br>-- High/Low voltage status<br>-- Tell1 Board Status<br>-- Infrastructure'
  };


  lhcb.widgets.ot = new Object();

  /** OT web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.ot.HVsystems = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr, cell;

    tab.logger = options.logger;
    tab.subscriptions=[];
    tab.className = tb.className   = 'MonitorPage';

    tab.addBoard = function(n1,n2,q) {
      var name1 = 'OTA_'+q+n1, name2='OTC_'+q+n2;
      var tr = document.createElement('tr');
      var cell=Cell('OTA/'+n1,1,'MonitorDataHeader');
      this[name1] = FSMItem('lbWeb.'+name1,this.logger,true);
      tr.appendChild(cell);
      tr.appendChild(this[name1]);
      tr.appendChild(this[name1].lock);
      this.subscriptions.push(this[name1]);
      cell=Cell('OTC/'+n2,1,'MonitorDataHeader');
      this[name2] = FSMItem('lbWeb.'+name2,this.logger,true);
      tr.appendChild(cell);
      tr.appendChild(this[name2]);
      tr.appendChild(this[name2].lock);
      this.subscriptions.push(this[name2]);
      return tr;
    };

    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('OT High Voltage per Station',6,options.style));
      tb.appendChild(tr);
    }

    tb.appendChild(tab.addBoard('T1', 'T1', 'HV_'));
    tb.appendChild(tab.addBoard('T2', 'T2', 'HV_'));
    tb.appendChild(tab.addBoard('T3', 'T3', 'HV_'));

    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('OT Low Voltage per Station',6,options.style));
      tb.appendChild(tr);
    }
    tb.appendChild(tab.addBoard('T1', 'T1', 'DCS_LV_'));
    tb.appendChild(tab.addBoard('T2', 'T2', 'DCS_LV_'));
    tb.appendChild(tab.addBoard('T3', 'T3', 'DCS_LV_'));

    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('OT TELL1 Board Status per Station',6,options.style));
      tb.appendChild(tr);
    }
    tb.appendChild(tab.addBoard('T1', 'T1', 'DAQ_TELL1_'));
    tb.appendChild(tab.addBoard('T2', 'T2', 'DAQ_TELL1_'));
    tb.appendChild(tab.addBoard('T3', 'T3', 'DAQ_TELL1_'));

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      for(var i=0; i<this.subscriptions.length;++i) {
	provider.subscribeItem(this.subscriptions[i]);
      }
    };
    return tab;
  };

  table.attachWidgets = function() {
    var opts = {style:'Arial12pt',legend:true,logger:this.logger};
    this.otVoltages       = this.hvSummary({hv:true,lv:false});
    this.lhc_state        = lhcb.widgets.LHCStateSummary(opts);
    this.otDAQ            = lhcb.widgets.SystemSummary({style:'Arial12pt',
								system: 'OT',
								title: 'OT Subsystem State',
								split: true,
								items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
								legend:true,
								rowing:true,
								logger:this.logger});

    this.otHV     = lhcb.widgets.ot.HVsystems(opts);

    this.left.addItem(this.otVoltages);
    this.left.addItem(this.otHV);
    this.right.addItem(this.lhc_state);
    this.right.addSpacer('50px');
    this.right.addItem(this.otDAQ);
  };
  return table;
};

var ot_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var ot_body = function()   { return lhcb.widgets.SubdetectorPage.start(OtStatus); };
