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
  lhcb.widgets.ot.OTsystems = function(options) {
    var tab = document.createElement('table');
    tab.subscriptions=[];
    tab.logger = options.logger;
    tab.className = 'MonitorPage';

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
    var otDAQ = lhcb.widgets.SystemSummary({style:'Arial12pt',
						    system: 'OT',
						    title: 'OT Subsystem State',
						    split: true,
						    items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
						    legend:true,
						    rowing:true,
						    logger:this.logger});
    this.left.addItem(this.hvSummary({hv:true,lv:false}));
    this.left.addItem(lhcb.widgets.ot.OTsystems(opts).addHV());
    this.left.addItem(lhcb.widgets.ot.OTsystems(opts).addLV());
    this.left.addItem(lhcb.widgets.ot.OTsystems(opts).addTell1());
    this.left.addItem(lhcb.widgets.ot.OTsystems(opts).addFE());
    this.left.addSpacer('30px');

    this.right.addItem(lhcb.widgets.LHCStateSummary(opts));
    this.right.addItem(lhcb.widgets.DSS.MixedWaterStatus(opts));
    this.right.addItem(lhcb.widgets.DSS.OTCaVPlantStatus(opts));
    this.right.addItem(otDAQ);
  };
  return table;
};

var ot_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var ot_body = function()   { return lhcb.widgets.SubdetectorPage.start(OtStatus); };
