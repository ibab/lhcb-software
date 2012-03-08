_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** TRIGGER web widgets
 *
 *  @author  M.Frank
 */
var TriggerStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('LHCb Triggers Overview');

  table.options = {logo:    '',
        	   logo_url:null,
		   title:   'Trigger Overview',
		   tips:    'LHCb Trigger overview:<br>-- L0DU counters<br>-- ODIN counters<br>-- HLT counters<br>-- Calibration farm'
  };


  lhcb.widgets.ODINtriggers = function(options) {
    var tb, tr, cell, tab = document.createElement('table');
    
    tab.className  = 'MonitorPage';
    tb = document.createElement('tbody');
    tooltips.set(tb,'Trigger counters from ODIN');
    tb.className  = 'MonitorPage';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('ODIN trigger counters',4,options.style));
      tb.appendChild(tr);
    }
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Event type',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Triggers',1,'MonitorDataHeader'));
      //tr.appendChild(Cell('Triggers',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Inst. rate [Hz]',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Run rate [Hz]',1,'MonitorDataHeader'));

      tb.appendChild(tr);
    }
    tab.nTriggersL = StyledItem('lbWeb.LHCb_RunInfo.TFC.triggerRates.nTriggersL',null,'%10.0f');
    //tab.nTriggersU = StyledItem('lbWeb.LHCb_RunInfo.TFC.triggerRates.nTriggersU',null,'%10.0f');
    tab.names = StyledItem('lbWeb.LHCb_RunInfo.TFC.triggerRates.names',null,null);
    tab.rates = StyledItem('lbWeb.LHCb_RunInfo.TFC.triggerRates.rates',null,'%10.2f');
    tab.runRates = StyledItem('lbWeb.LHCb_RunInfo.TFC.triggerRates.runRates',null,'%10.2f');

    tab.appendChild(tb);

    tab.trg_row = new Object();

    tab.names.tab = tab;
    tab.names.display = function(data) {
      var td, tr, v = data[1].split(' | ',20);
      var upda = false;
      var tab = this.tab;
      tab.trg_rows = v;
      for(var i=0; i<v.length; ++i) {
	if ( !tab.trg_row[v[i]] )  {
	  var st = '';
	  if ( v[i].indexOf('AllCalib')>=0 ) st = 'MonitorDataHeader';
	  else if ( v[i].indexOf('Physics_Total')>=0 ) st = 'MonitorDataHeader';
	  else if ( v[i].indexOf('Physics_Accepted')>=0 ) st = 'MonitorDataHeader';
	  else if ( v[i].indexOf('AllHLT_Accepted')>=0 ) st = 'MonitorDataHeader';

	  tab.trg_row[v[i]] = document.createElement('tr');
	  tr = tab.trg_row[v[i]];
	  tr.col_name    = Cell(v[i],1,'MonitorDataHeader');
	  tr.col_trL     = document.createElement('td');
	  //tr.col_trU     = document.createElement('td');
	  tr.col_runRate = document.createElement('td');
	  tr.col_rate    = document.createElement('td');
	  tr.col_trL.className = st;
	  tr.col_rate.className = st;
	  tr.col_runRate.className = st;
	  tr.col_trL.style.textAlign = 'right';
	  tr.col_rate.style.textAlign = 'right';
	  tr.col_runRate.style.textAlign = 'right';
	  
	  tr.appendChild(tr.col_name);
	  tr.appendChild(tr.col_trL);
	  //tr.appendChild(tr.col_trU);
	  tr.appendChild(tr.col_rate);
	  tr.appendChild(tr.col_runRate);
	  upda = true;
	}
      }
      if ( upda ) {
	var n;
	var tb_tot = document.createElement('tbody');
	var tb_acc = document.createElement('tbody');
	var tb_cal = document.createElement('tbody');
	if ( tab.odin_body_total  ) tab.deleteElementById('odin_body_total');
	if ( tab.odin_body_accept ) tab.deleteElementById('odin_body_accept');
	if ( tab.odin_body_calib  ) tab.deleteElementById('odin_body_calib');
	tab.odin_body_total = tb_tot;
	tab.odin_body_accept = tb_acc;
	tab.odin_body_total = tb_cal;
	for(var i=0; i<tab.trg_rows.length; ++i) {
	  n = tab.trg_rows[i];
	  if ( n.indexOf('_Total')>0 ) {
	    tr = tab.trg_row[n];
	    tb_tot.appendChild(tr);
	  }
	}
	tb_acc.appendChild(Cell('ODIN accepted triggers',4,'Arial12pt'));
	tb_cal.appendChild(Cell('Calibration triggers',4,'Arial12pt'));
	for(var i=0; i<tab.trg_rows.length; ++i) {
	  n = tab.trg_rows[i];
	  var id = n.indexOf('Calib');
	  if ( n.indexOf('_Accepted')>0 && id<0 ) {
	    tr = tab.trg_row[n];
	    tb_acc.appendChild(tr);
	  }
	  else if ( id>= 0 ) {
	    tr = tab.trg_row[n];
	    tb_cal.appendChild(tr);
	  }
	}
	tab.appendChild(tb_tot);
	tab.appendChild(tb_acc);
	tab.appendChild(tb_cal);
      }
      return this;
    };

    tab.nTriggersL.tab = tab;
    tab.nTriggersL.display = function(data) {
      var v = data[1].split(' | ',20);
      var t = this.tab;
      if ( t.trg_rows.length == v.length )  {
	var r = t.trg_row;
	var k = t.trg_rows;
	for(var i=0; i<v.length; ++i) {
	  r[k[i]]['col_trL'].innerHTML = v[i];
	}
      }
    };
    /*
    tab.nTriggersU.tab = tab;
    tab.nTriggersU.display = function(data) {
      var v = data[1].split(' | ',20);
      var t = this.tab;
      if ( t.trg_rows.length == v.length )  {
	var r = t.trg_row;
	var k = t.trg_rows;
	for(var i=0; i<v.length; ++i) {
	  r[k[i]]['col_trU'].innerHTML = v[i];
	}
      }
    };
    */
    tab.rates.tab = tab;
    tab.rates.display = function(data) {
      var v = data[1].split(' | ',20);
      var t = this.tab;
      if ( t.trg_rows.length == v.length )  {
	var r = t.trg_row;
	var k = t.trg_rows;
	var fmt = t.rates.format;
	for(var i=0; i<v.length; ++i) {
	  var f = parseFloat(v[i]);
	  r[k[i]].col_rate.innerHTML = sprintf(fmt,f);
	}
      }
    };

    tab.runRates.tab = tab;
    tab.runRates.display = function(data) {
      var v = data[1].split(' | ',20);
      var t = this.tab;
      var fmt = t.runRates.format;
      if ( t.trg_rows.length == v.length )  {
	var r = t.trg_row;
	var k = t.trg_rows;
	for(var i=0; i<v.length; ++i) {
	  var f = parseFloat(v[i]);
	  r[k[i]].col_runRate.innerHTML = sprintf(fmt,f);
	}
      }
    };

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.names);
      provider.subscribeItem(this.nTriggersL);
      //provider.subscribeItem(this.nTriggersU);
      provider.subscribeItem(this.rates);
      provider.subscribeItem(this.runRates);
    };

    return tab;
  };

  lhcb.widgets.L0DUmonitor = function(options) {
    var tb, tr, cell, tab = document.createElement('table');
    
    tab.className  = 'MonitorPage';
    tb = document.createElement('tbody');
    tooltips.set(tb,'Trigegr counters from the L0 Decision Unit (L0DU)');
    tb.className  = 'MonitorPage';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('L0 Decision Unit trigger counters',4,options.style));
      tb.appendChild(tr);
    }
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Trigger channel',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Number of Triggers',1,'MonitorDataHeader'));
      //tr.appendChild(Cell('Avg.rate [Hz]',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Inst.Rate [Hz]',1,'MonitorDataHeader'));
      tb.appendChild(tr);
    }
    tab.nTrigChan = StyledItem('lbWeb.Monitor_L0DU_1.Rates.nTrigChannels',null,null);
    tab.nTrigTot  = StyledItem('lbWeb.Monitor_L0DU_1.Rates.nTrigTot',null,null);
    tab.nBC       = StyledItem('lbWeb.Monitor_L0DU_1.nBC',null,null);
    
    tab.addRow = function(name) {
      var name_col;
      var tr = document.createElement('tr');
      tr.appendChild(name_col=Cell(name,1,'MonitorDataHeader'));
      name_col.style.width = '40%';

      tr.nTriggers = document.createElement('td');
      tr.instRate  = document.createElement('td');
      tr.nTriggers.style.textAlign = 'right';
      tr.instRate.style.textAlign = 'right';
      tr.instRate.innerHTML = '--';

      tr.nTriggers.style.width = '30%';
      tr.instRate.style.width = '30%';

      tr.appendChild(tr.nTriggers);
      tr.appendChild(tr.instRate);
      return tr;
    };
    tab.trg_rows = new Array();
    for(var i=0; i<32; ++i) {
      tab.trg_rows.push(tab.addRow('Channel '+(i+1)));
      tb.appendChild(tab.trg_rows[i]);
    }
    tab.appendChild(tb);

    tb = document.createElement('tbody');
    tb.className  = 'MonitorPage';
    tb.appendChild(tab.trg_total=tab.addRow('Total'));
    tab.trg_total.nTriggers.className = 'MonitorDataHeader';
    tab.trg_total.instRate.className = 'MonitorDataHeader';
    //tab.trg_total.avgRate.className = 'MonitorDataHeader';
    tab.appendChild(tb);

    tab.nTrigChan.tab = tab;
    tab.last_readings = null;
    tab.curr_readings = null;
    tab.last_time = 0;
    tab.curr_time = null;

    tab.nTrigChan.display = function(data) {
      var v = data[1].split(' | ',20);
      var val, diff, freq;
      var tot = 0, last_tot;
      var tab = this.tab;
      var len = v.length>tab.trg_rows.length ? tab.trg_rows.length : v.length;
      if ( tab.curr_readings ) {
	tab.last_readings = tab.curr_readings;
	tab.last_time = tab.curr_time;
      }
      tab.curr_time = (new Date()).getTime();
      tab.curr_readings = new Array();
      diff = (tab.curr_time-tab.last_time)/1000;
      for(var i=0; i<len;++i) {
	val = parseInt(v[i]);
	tab.trg_rows[i].nTriggers.innerHTML = v[i];
	tab.curr_readings[i] = val;
	if ( tab.last_readings ) {
	  freq = (val-tab.last_readings[i])/diff;
	  tab.trg_rows[i].instRate.innerHTML = sprintf('%10.2f Hz',freq);
	}
	tot = tot + val;
      }
      if ( tab.last_readings ) {
	last_tot = parseInt(tab.trg_total.nTriggers.innerHTML);
	freq = (tot-last_tot)/diff;
	tab.trg_total.instRate.innerHTML = sprintf('%10.2f Hz',freq);
      }
      tab.trg_total.nTriggers.innerHTML = tot;
    };

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.nTrigChan);
      provider.subscribeItem(this.nTrigTot);
      provider.subscribeItem(this.nBC);
    };

    return tab;
  };

  lhcb.widgets.HLTmonitor = function(options) {
    var tb, tr, cell, tab = document.createElement('table');
    
    tab.className  = 'MonitorPage';
    tb = document.createElement('tbody');
    tooltips.set(tb,'HLT trigger counters');
    tb.className  = 'MonitorPage';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('HLT Trigger counters',3,options.style));
      tb.appendChild(tr);
    }
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(cell=Cell('Event Type',    1,'MonitorDataHeader'));
      cell.style.width = '40%';
      tr.appendChild(cell=Cell('Triggers Incl.',1,'MonitorDataHeader'));
      cell.style.width = '30%';
      tr.appendChild(cell=Cell('Triggers Excl.',1,'MonitorDataHeader'));
      cell.style.width = '30%';
      tb.appendChild(tr);
    }
    tab.update_totals = function(data) {
      this.data = parseFloat(data);
      tab.totalHandler();
      return data;
    };
    tab.BeamGasEx = StyledItem('lbWeb.LHCB_STATS.BeamGasEx',null,'%10.2f Hz');
    tab.BeamGasIn = StyledItem('lbWeb.LHCB_STATS.BeamGasIn',null,'%10.2f Hz');
    tab.LumiEx    = StyledItem('lbWeb.LHCB_STATS.LumiEx',null,'%10.2f Hz');
    tab.LumiIn    = StyledItem('lbWeb.LHCB_STATS.LumiIn',null,'%10.2f Hz');
    tab.MBiasEx   = StyledItem('lbWeb.LHCB_STATS.MBiasEx',null,'%10.2f Hz');
    tab.MBiasIn   = StyledItem('lbWeb.LHCB_STATS.MBiasIn',null,'%10.2f Hz');
    tab.PhysEx    = StyledItem('lbWeb.LHCB_STATS.PhysEx',null,'%10.2f Hz');
    tab.PhysIn    = StyledItem('lbWeb.LHCB_STATS.PhysIn',null,'%10.2f Hz');
    tab.RandEx    = StyledItem('lbWeb.LHCB_STATS.RandEx',null,'%10.2f Hz');
    tab.RandIn    = StyledItem('lbWeb.LHCB_STATS.RandIn',null,'%10.2f Hz');

    tab.BeamGasEx.conversion = tab.update_totals;
    tab.BeamGasIn.conversion = tab.update_totals;
    tab.LumiEx.conversion    = tab.update_totals;
    tab.LumiIn.conversion    = tab.update_totals;
    tab.MBiasEx.conversion   = tab.update_totals;
    tab.MBiasIn.conversion   = tab.update_totals;
    tab.PhysEx.conversion    = tab.update_totals;
    tab.PhysIn.conversion    = tab.update_totals;
    tab.RandEx.conversion    = tab.update_totals;
    tab.RandIn.conversion    = tab.update_totals;

    tab.BeamGasEx.style.textAlign = 'right';
    tab.BeamGasIn.style.textAlign = 'right';
    tab.LumiEx.style.textAlign    = 'right';
    tab.LumiIn.style.textAlign    = 'right';
    tab.MBiasEx.style.textAlign   = 'right';
    tab.MBiasIn.style.textAlign   = 'right';
    tab.PhysEx.style.textAlign    = 'right';
    tab.PhysIn.style.textAlign    = 'right';
    tab.RandEx.style.textAlign    = 'right';
    tab.RandIn.style.textAlign    = 'right';

    tr = document.createElement('tr');
    tr.appendChild(Cell('Physics',1,'MonitorDataHeader'));
    tr.appendChild(tab.PhysIn);
    tr.appendChild(tab.PhysEx);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Min.Bias',1,'MonitorDataHeader'));
    tr.appendChild(tab.MBiasIn);
    tr.appendChild(tab.MBiasEx);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Lumi',1,'MonitorDataHeader'));
    tr.appendChild(tab.LumiIn);
    tr.appendChild(tab.LumiEx);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Beam Gas',1,'MonitorDataHeader'));
    tr.appendChild(tab.BeamGasIn);
    tr.appendChild(tab.BeamGasEx);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Random',1,'MonitorDataHeader'));
    tr.appendChild(tab.RandIn);
    tr.appendChild(tab.RandEx);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Total',1,'MonitorDataHeader'));
    tr.appendChild(tab.totalIn=Cell('0',1,'MonitorDataHeader'));
    tr.appendChild(tab.totalEx=Cell('0',1,'MonitorDataHeader'));
    tab.totalIn.style.textAlign = 'right';
    tab.totalEx.style.textAlign = 'right';
    tb.appendChild(tr);

    tab.totalHandler = function() {
      var fEx = 0.0, fIn = 0.0;
      if ( tab.BeamGasEx.data ) fEx = fEx + tab.BeamGasEx.data;
      if ( tab.LumiEx.data    ) fEx = fEx + tab.LumiEx.data;
      if ( tab.MBiasEx.data   ) fEx = fEx + tab.MBiasEx.data;
      if ( tab.PhysEx.data    ) fEx = fEx + tab.PhysEx.data;
      if ( tab.RandEx.data    ) fEx = fEx + tab.RandEx.data;
      if ( tab.BeamGasIn.data ) fIn = fIn + tab.BeamGasIn.data;
      if ( tab.LumiIn.data    ) fIn = fIn + tab.LumiIn.data;
      if ( tab.MBiasIn.data   ) fIn = fIn + tab.MBiasIn.data;
      if ( tab.PhysIn.data    ) fIn = fIn + tab.PhysIn.data;
      if ( tab.RandIn.data    ) fIn = fIn + tab.RandIn.data;
      tab.totalIn.innerHTML = sprintf(tab.RandIn.format,fIn);
      tab.totalEx.innerHTML = sprintf(tab.RandEx.format,fEx);
    };

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.BeamGasEx);
      provider.subscribeItem(this.BeamGasIn);
      provider.subscribeItem(this.LumiEx);
      provider.subscribeItem(this.LumiIn);
      provider.subscribeItem(this.MBiasEx);
      provider.subscribeItem(this.MBiasIn);
      provider.subscribeItem(this.PhysEx);
      provider.subscribeItem(this.PhysIn);
      provider.subscribeItem(this.RandEx);
      provider.subscribeItem(this.RandIn);
    };

    return tab;
  };
    
  lhcb.widgets.TriggerSetup = function(options) {
    var td, tr, tab = document.createElement('table');
    var tb = document.createElement('tbody');
    tab.className  = tb.className = 'MonitorPage';
    tb.cellSpacing = tb.cellPadding = 0;

    tab.l0Type        = StyledItem('lbWeb.LHCb_RunInfo.Trigger.L0Type',null, null);
    tab.HLTType       = StyledItem('lbWeb.LHCb_RunInfo.Trigger.HLTType',null, null);
    tab.TCK           = StyledItem('lbWeb.LHCb_RunInfo.Trigger.TCKLabel',null, null);

    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Trigger setup',options.legend ? 2 : 1,options.style));
      tb.appendChild(tr);
    }

    tr = document.createElement('tr');
    if ( options.legend ) tr.appendChild(Cell('L0 Configuration:',null,'MonitorDataHeader'));
    tr.appendChild(tab.l0Type);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    if ( options.legend ) tr.appendChild(Cell('HLT Configuration:',null,'MonitorDataHeader'));
    tr.appendChild(tab.HLTType);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    if ( options.legend ) tr.appendChild(Cell('TCK:',1,'MonitorDataHeader'));
    tr.appendChild(tab.TCK);
    tb.appendChild(tr);

    tab.appendChild(tb);
    tooltips.set(tab,'Trigger setup and status<br>Click to go to DAQ page');
    tab.onclick = lhcb.widgets.goto_lhcb_daq_page;

    tab.subscribe = function(provider) {
      provider.subscribeItem(tab.TCK);
      provider.subscribeItem(tab.l0Type);
      provider.subscribeItem(tab.HLTType);
    };
    return tab;
  };


  table.attachWidgets = function() {
    var opts = {system:'LHCb',style:'Arial12pt',legend:true,logger:this.logger};
    this.odin        = lhcb.widgets.ODINtriggers(opts);
    this.l0du        = lhcb.widgets.L0DUmonitor(opts);
    this.hlt         = lhcb.widgets.HLTmonitor(opts);
    this.trg         = lhcb.widgets.TriggerSetup(opts);

    this.left.addItem(this.trg);
    //this.left.addSpacer('50px');
    this.left.addItem(this.odin);
    this.left.addItem(this.hlt);

    this.right.addItem(this.l0du);
    //this.right.addSpacer('50px');
  };
  return table;
};

var trigger_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var trigger_body = function()   { return lhcb.widgets.SubdetectorPage.start(TriggerStatus); };
