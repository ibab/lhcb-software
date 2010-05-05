_loadScript('lhcb.display.tooltips.cpp');
_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.constants.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


var OperationsStatus = function(msg)   {
  var tr, td, tb, tab;
  table           = document.createElement('table');
  table.body      = document.createElement('tbody');
  table.messages  = msg;
  table.provider  = null;
  table.logger    = null;

  table.add = function() {
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.setAttribute('colSpan',2);
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  }

  table.className = table.body.className = 'MonitorPage';
  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  table.LHCb_HT_header = lhcb.widgets.LHCb_HT_header;
  tooltips.set(table.LHCb_HT_header,'HT summary state of the LHCb subdetector');

  table.hv_conf = function(val) {
    var v = val.split('/',4);
    if ( v.length>1 ) return v[1];
    return val;
  }

  table.addState = function(nam,split,n,m,q) {
    var cell,tr = document.createElement('tr');
    tr.appendChild(cell=Cell(nam,1,null));
    cell.style.width='120px';
    this[n+'LHC'+nam] = FSMItem('lbWeb.'+nam+'_LHC_'+m,this.logger,true);
    tr.appendChild(this[n+'LHC'+nam]);

    this[n+'LHCRQ'+nam] = StyledItem('lbWeb.'+nam+'_LHC_'+m+'_Comm.PrevLhcState',null,null);
    tr.appendChild(this[n+'LHCRQ'+nam]);

    //this[n+'Comm'+nam]  = FSMItem('lbWeb.'+nam+'_LHC_'+m'_Comm',this.logger,true);
    //tr.appendChild(this[n+'Comm'+nam]);
    //tr.appendChild(this[n+'Comm'+nam].lock);
    if ( split ) {
      this[n+'A'+nam] = FSMItem('lbWeb.'+nam+'A_'+q,this.logger,true);
      this[n+'C'+nam] = FSMItem('lbWeb.'+nam+'C_'+q,this.logger,true);
      tr.appendChild(this[n+'A'+nam]);
      tr.appendChild(this[n+'C'+nam]);
    }
    else {
      this[n+''+nam] = FSMItem('lbWeb.'+nam+'_'+q,this.logger,true);
      tr.appendChild(this[n+''+nam]);
    }
    tr.appendChild(this[n+'LHC'+nam].lock);
    this[n+'LHC'+nam].lock.className = null;
    tooltips.set(tr,'HT state of the '+nam+' subdetector');
    return tr;
  }
  table.addHTState = function(nam,split) {
    return this.addState(nam,split,'hv','HV','HV');
  }
  table.addLTState = function(nam,split) {
    return this.addState(nam,split,'lv','LV','DCS_Local');
  }
  table.appendFSMItem = lhcb.widgets.mkFSMitem1;

  table.subscribeItem2 = function(item) {
    if ( item ) {
      this.provider.subscribe(item.name,item);
    }
  }
  table.subscribeHV = function(nam) {
    this.subscribeItem2(this['hvLHC'+nam]);
    this.subscribeItem2(this['hvLHCRQ'+nam]);
    this.subscribeItem2(this['hv'+nam]);
    this.subscribeItem2(this['hvA'+nam]);
    this.subscribeItem2(this['hvC'+nam]);
    this.subscribeItem2(this['hvComm'+nam]);
  }
  table.subscribeLV = function(nam) {
    this.subscribeItem2(this['lvLHC'+nam]);
    this.subscribeItem2(this['lvLHCRQ'+nam]);
    this.subscribeItem2(this['lv'+nam]);
    this.subscribeItem2(this['lvA'+nam]);
    this.subscribeItem2(this['lvC'+nam]);
    this.subscribeItem2(this['lvComm'+nam]);
  }

  table.Operations_Summary = function() {
    var tb, tr, td, tab = document.createElement('table');
    tab.className  = 'MonitorPage';
    tab.width = '100%';
    tb = document.createElement('tbody');
    tb.className  = 'MonitorPage';
    tooltips.set(tb,'Operation state of LHCb');
    tb.appendChild(lhcb.widgets.mkFSMitem1(this.operState = FSMItem('lbWeb.LHCb_LHC',this.logger,true),'LHCb state'));
    tab.appendChild(tb);
    return tab;
  }

  table.System_Summary = function() {
    var tb, tr, td, tab = document.createElement('table');
    tab.className  = 'MonitorPage';
    tab.width = '50%';

    tb = document.createElement('tbody');
    tb.className  = 'MonitorPage';
    tooltips.set(tb,'Main system summary<br>Click to move to LHC info page.');
    tb.onclick = function() { document.location = lhcb.constants.urls.lhcb.lhc_status.src;};

    tr = document.createElement('tr');
    tr.appendChild(Cell('Subsystem',1,null));
    tr.appendChild(td=Cell('State',1,null));
    td.width = '50%';
    tr.appendChild(Cell('',1,null));
    tb.appendChild(tr);
    tb.appendChild(this.appendFSMItem(this.lhcState = FSMItem('lbWeb.LHC_STATE/LHC',this.logger,true),'LHC'));
    tb.appendChild(this.appendFSMItem(this.bcmState = FSMItem('lbWeb.BCM',this.logger,true),'BCM'));
    tb.appendChild(this.appendFSMItem(this.magState = FSMItem('lbWeb.INF_MAG',this.logger,true),'Magnet'));
    tb.appendChild(this.appendFSMItem(this.clockState = FSMItem('lbWeb.LHCb_CLOCK/LHCb',this.logger,true),'Clock'));
    tab.appendChild(tb);
    return tab;
  }

  table.Handshake_Summary = function() {
    var tb, tr, td, tab = document.createElement('table');
    tab.className  = 'MonitorPage';
    tb = document.createElement('tbody');
    tb.className  = 'MonitorPage';
    tb.height    = '120px';
    tooltips.set(tb,'Handshakes');
    tr = document.createElement('tr');
    tr.appendChild(td=Cell('Handshakes',1,'Arial12pt'));
    tr.appendChild(Cell('LHC',1,'MonitorDataHeader'));
    tr.appendChild(Cell('LHCb',1,'MonitorDataHeader'));
    td.width='15%';
    tb.appendChild(tr);
    tr = document.createElement('tr');
    this.lhcHandshake  = FSMItem('lbWeb.LHC_HANDSHAKE/LHC',this.logger,true);
    this.lhcbHandshake = FSMItem('lbWeb.LHCb_HANDSHAKE/LHCb',this.logger,true);
    tr.appendChild(Cell('Com',1,'MonitorDataHeader'));
    tr.appendChild(this.lhcHandshake);
    tr.appendChild(this.lhcbHandshake);
    tb.appendChild(tr);
    tab.appendChild(tb);
    return tab;
  }

  table.Voltage_Summary = function() {
    var tb, tr, cell, tab = document.createElement('table');
    tab.className  = 'MonitorPage';

    tb = document.createElement('tbody');
    tb.className  = 'MonitorPage';
    tb.height    = '120px';
    tooltips.set(tb,'HT summary of LHCb');
    tr = document.createElement('tr');
    tr.appendChild(Cell('Voltages',6,'Arial12pt'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('System',1,'MonitorDataHeader'));
    tr.appendChild(Cell('State',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Requested',1,'MonitorDataHeader'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    this.hvState  = FSMItem('lbWeb.LHCb_LHC_HV',this.logger,true);
    this.hvState.par = this;
    this.hvState.conversion = function(obj,state,style) {
      var o = this.par.injState;
      o.className = this.className;
      o.style.borderStyle = 'solid';
      return state;
    }

    this.hvRequest = StyledItem('lbWeb.LHCb_LHC_HV_Info.requestedState',null,null);
    this.hvRequest.par = this;
    this.hvRequest.conversion = function(data) {
      this.par.injState.innerHTML = data;
      return data;
    }

    tr.appendChild(Cell('LHCb HV &amp; LV',1,'MonitorDataHeader'));
    tr.appendChild(this.hvState);
    tr.appendChild(this.hvRequest);
    tr.appendChild(this.hvState.lock);
    this.hvState.lock.className = null;
    tb.appendChild(tr);
    tab.appendChild(tb);

    tb = document.createElement('tbody');
    tooltips.set(tab,'High voltage state of the subdetectors');
    tb.className  = 'MonitorPage';
    tb.height    = '120px';
    tr = document.createElement('tr');
    tr.appendChild(Cell('High Voltage',6,'Arial12pt'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(cell=Cell('Subdetector',1,'MonitorDataHeader'));
    tr.appendChild(cell=Cell('State',1,'MonitorDataHeader'));
    tr.appendChild(cell=Cell('Request',1,'MonitorDataHeader'));
    cell.style.width = '20%';
    tr.appendChild(cell=Cell('HV State Side A / Side C',2,'MonitorDataHeader'));
    tb.appendChild(tr);
    tb.appendChild(this.addHTState('VELO',true));
    tb.appendChild(this.addHTState('TT',  false));
    tb.appendChild(this.addHTState('IT',  false));
    tb.appendChild(this.addHTState('OT',  true));
    tb.appendChild(this.addHTState('RICH1',false));
    tb.appendChild(this.addHTState('RICH2',false));
    tb.appendChild(this.addHTState('PRS',  false));
    tb.appendChild(this.addHTState('ECAL', false));
    tb.appendChild(this.addHTState('HCAL', false));
    tb.appendChild(this.addHTState('MUON', true));
    tab.appendChild(tb);

    tb = document.createElement('tbody');
    tooltips.set(tb,'Low voltage state of the subdetectors');
    tb.className  = 'MonitorPage';
    tb.height    = '120px';
    tr = document.createElement('tr');
    tr.appendChild(Cell('Low Voltage',6,'Arial12pt'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Subdetector',1,'MonitorDataHeader'));
    tr.appendChild(Cell('State',1,'MonitorDataHeader'));
    tr.appendChild(cell=Cell('Request',1,'MonitorDataHeader'));
    tr.appendChild(Cell('LV State Side A / Side C',2,'MonitorDataHeader'));
    tb.appendChild(tr);
    tb.appendChild(this.addState('VELO',true,'lv','LV','DCS_Local'));
    tb.appendChild(this.addState('TT',false, 'lv','LV','DCS_Local'));
    tb.appendChild(this.addState('IT',false, 'lv','LV','DCS_Local'));
    tb.appendChild(this.addState('RICH1',false, 'lv','LV','DCS_Local'));
    tb.appendChild(this.addState('RICH2',false, 'lv','LV','DCS_Local'));
    tab.appendChild(tb);
    return tab;
  }

  table.LHC_Summary = function() {
    var tb, tr, cell, tab = document.createElement('table');
    tab.className  = 'MonitorPage';

    tb = document.createElement('tbody');
    tooltips.set(tb,'LHC state summary');
    tb.className  = 'MonitorPage';
    tb.height    = '120px';
    tr = document.createElement('tr');
    tr.appendChild(Cell('LHC',3,'Arial12pt'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Mode',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Fill Number',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Energy',1,'MonitorDataHeader'));
    tb.appendChild(tr);
    this.lhcMode = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.MachineMode',null,null);
    this.lhcFillNumber    = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.FillNumber',null,null);
    this.lhcEnergy        = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Energy',null,'%7.1f GeV');
    this.lhcEnergy.conversion = function(data) {      return (data*120.0)/1000.0;    }
    tr = document.createElement('tr');
    tr.appendChild(this.lhcMode);
    tr.appendChild(this.lhcFillNumber);
    tr.appendChild(this.lhcEnergy);
    tb.appendChild(tr);
    tab.appendChild(tb);
    return tab;
  }

  table.Magnet_Summary = function() {
    var tb, tr, cell, tab = document.createElement('table');
    tab.className  = 'MonitorPage';
    
    tb = document.createElement('tbody');
    tooltips.set(tb,'Magnet summary<br>Click to move to magnet page');
    tb.onclick = function() { document.location = "lhcb.display.htm?type=magnet";};
    tb.className  = 'MonitorPage';
    tb.height    = '120px';
    tr = document.createElement('tr');
    tr.appendChild(Cell('Magnet Status',3,'Arial12pt'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Set Current',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Measured Current',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Polarity',1,'MonitorDataHeader'));
    tb.appendChild(tr);
    this.magSet      = StyledItem('lbWeb.LbMagnet.SetCurrent',null,'%7.0f A');
    this.magRead     = StyledItem('lbWeb.LbMagnet.Current',null,'%7.0f A');
    this.magPolarity = StyledItem('lbWeb.LbMagnet.Polarity',null,null);
    this.magPolarity.conversion = function(data) {      return data>0 ? 'Down' : 'Up';    }
    tr = document.createElement('tr');
    tr.appendChild(this.magSet);
    tr.appendChild(this.magRead);
    tr.appendChild(this.magPolarity);
    tb.appendChild(tr);
    tab.appendChild(tb);
    return tab;
  }

  table.Velo_Summary = function() {
    var tb, tr, cell, tab = document.createElement('table');
    tab.className  = 'MonitorPage';
    tb = document.createElement('tbody');
    tooltips.set(tb,'Velo Position');
    tb.className  = 'MonitorPage';
    tb.height    = '120px';
    tr = document.createElement('tr');
    tr.appendChild(Cell('Velo Position',3,'Arial12pt'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Opening',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Center',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Y:',1,'MonitorDataHeader'));
    tb.appendChild(tr);
    this.veloOpening = StyledItem('lbWeb.LHCb_RunInfoCond.VELO.CurrentPosition.Opening',null,'%7.2f mm');
    this.veloCenter  = StyledItem('lbWeb.LHCb_RunInfoCond.VELO.CurrentPosition.Center',null,'%7.2f mm');
    this.veloY       = StyledItem('lbWeb.LHCb_RunInfoCond.VELO.CurrentPosition.Y',null,'%7.2f mm');
    tr = document.createElement('tr');
    tr.appendChild(this.veloOpening);
    tr.appendChild(this.veloCenter);
    tr.appendChild(this.veloY);
    tb.appendChild(tr);
    tab.appendChild(tb);
    return tab;
  }


  table.Database_Summary = function() {
    var tb, tr, cell, tab = document.createElement('table');
    tab.className  = 'MonitorPage';
    tb = document.createElement('tbody');
    tooltips.set(tb,'Database interface summary');
    tb.className   = 'MonitorPage';
    tb.height      = '120px';
    this.dbRun     = StyledItem('lbWeb.LHCb_RunInfoCond.RunDBServer',null,null);
    this.dbCond    = StyledItem('lbWeb.LHCb_RunInfoCond.CondDBServer',null,null);
    this.dbArchive = StyledItem('lbWeb.LHCb_RunInfoCond.ArchiveServer',null,null);
    this.db_conversion = function(data) {
      if ( data>0 ) {
	this.style.color = 'green';
	return 'OK';
      }
      this.style.color = 'red';
      return 'Not OK';
    };
    this.dbRun.conversion    = this.db_conversion;
    this.dbCond.conversion    = this.db_conversion;
    this.dbArchive.conversion = this.db_conversion;
    tr = document.createElement('tr');
    tr.appendChild(Cell('Database Status',6,'Arial12pt'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Run DB:',1,null));
    tr.appendChild(this.dbRun);
    tr.appendChild(Cell('Cond DB:',1,null));
    tr.appendChild(this.dbCond);
    tr.appendChild(Cell('PVSS Archive:',1,null));
    tr.appendChild(this.dbArchive);
    tb.appendChild(tr);
    tab.appendChild(tb);
    return tab;
  };

  /**
  */
  table.Panel_Summary = function() {
    var cell, tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');
    tooltips.set(tb,'State summary and\noperations status');
    tr.appendChild(cell=Cell('',1,null));
    tab.width = '100%';
    cell.id = 'LHCb_InjectionState';
    cell.height = '160px';
    cell.style.textAlign = 'center';
    cell.style.borderWidth = '5';
    cell.style.borderColor = 'black';
    cell.style.fontSize='400%';
    this.injState = cell;
    tb.appendChild(tr);
    tab.appendChild(tb);
    return tab;
  };


  table.subscribe = function() {
    this.subscribeHV('VELO');
    this.subscribeHV('TT');
    this.subscribeHV('IT');
    this.subscribeHV('OT');
    this.subscribeHV('RICH1');
    this.subscribeHV('RICH2');
    this.subscribeHV('PRS');
    this.subscribeHV('ECAL');
    this.subscribeHV('HCAL');
    this.subscribeHV('MUON');

    this.subscribeLV('VELO');
    this.subscribeLV('TT');
    this.subscribeLV('IT');
    this.subscribeLV('RICH1');
    this.subscribeLV('RICH2');

    this.subscribeItem2(this.operState);
    this.subscribeItem2(this.hvState);
    this.subscribeItem2(this.hvRequest);

    this.subscribeItem2(this.lhcHandshake);
    this.subscribeItem2(this.lhcbHandshake);

    this.subscribeItem2(this.lhcState);
    this.subscribeItem2(this.bcmState);
    this.subscribeItem2(this.magState);
    this.subscribeItem2(this.clockState);

    this.subscribeItem2(this.lhcMode);
    this.subscribeItem2(this.lhcFillNumber);
    this.subscribeItem2(this.lhcEnergy);

    this.subscribeItem2(this.magSet);
    this.subscribeItem2(this.magRead);
    this.subscribeItem2(this.magPolarity);

    this.subscribeItem2(this.veloOpening);
    this.subscribeItem2(this.veloCenter);
    this.subscribeItem2(this.veloY);

    this.subscribeItem2(this.dbRun);
    this.subscribeItem2(this.dbCond);
    this.subscribeItem2(this.dbArchive);

    this.bkg.subscribe(this.provider);
    this.safety.subscribe(this.provider);
    this.cooling.subscribe(this.provider);
    this.pressures.subscribe(this.provider);
  }

  table.build = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var t1, tb1, tr1, td1, cell, d = new Date();

    tab.className = tb.className = 'MonitorPage';
    tab.width = tb.width  = '100%';
    tab.style.fontSize = '90%';

    this.heading = document.createElement('tr');
    this.heading.appendChild(td=document.createElement('td'));
    td.style.width = '100%';
    td.colSpan = 3;
    td.appendChild(t1=document.createElement('table'));
    t1.appendChild(tb1=document.createElement('tbody'));
    t1.width  = '100%';

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.Operations_Summary());
    tr1.appendChild(this.head_date=Cell(d.toString(),1,'MonitorTinyHeader'));
    this.head_date.id = 'current_time';
    this.head_date.textAlign = 'right';

    cell = Cell('LHCb Operation State&nbsp;'+lhcb_online_picture(),1,'MonitorBigHeader');
    cell.style.textAlign = 'right';
    cell.onclick = function() { document.location = "http://lhcb.cern.ch";};
    tr1.appendChild(cell);


    tooltips.set(this.heading,'LHCb operations status:<br>-- High/Low voltage status<br>-- Magnet, BCM and TTC status<br>-- TTC status<br>-- Velo, Rich measurements<br>-- Cooling plant status<br>-- Safety status');


    tb.appendChild(this.heading);

    this.timerHandler = function() {document.getElementById('current_time').innerHTML = (new Date()).toString(); }
    setInterval(this.timerHandler,2000);

    tr = document.createElement('tr');

    // Left half of the display
    tr.appendChild(td=document.createElement('td'));
    td.style.width = '50%';
    td.appendChild(t1=document.createElement('table'));
    t1.appendChild(tb1=document.createElement('tbody'));
    t1.className = tb1.className = 'MonitorPage';
    t1.style.border = tb1.style.border = 'none';

    //tb1.appendChild(tr1=document.createElement('tr'));
    //tr1.appendChild(td1=document.createElement('td'));
    //td1.appendChild(cell=this.Operations_Summary());
    //td1.appendChild(cell=Cell('',1,null));


    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.System_Summary());
    td1.width = '50%';
    td1.colSpan = 1;

    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.Panel_Summary());
    td1.rowSpan = 2;


    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.Handshake_Summary());
    td1.width = '50%';
    td1.colSpan = 1;

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.Voltage_Summary());
    td1.width = '100%';
    td1.colSpan = 2;

    // Right hand of the display
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(t1=document.createElement('table'));
    td.width = '35%';
    t1.appendChild(tb1=document.createElement('tbody'));
    t1.className = tb1.className = 'MonitorPage';
    t1.style.border = tb1.style.border = 'none';
    td.colSpan = 2;

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.LHC_Summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.Magnet_Summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.Velo_Summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.pressures=lhcb.widgets.RICHPressures('Arial12pt',this.logger));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.Database_Summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.cooling = lhcb.widgets.CoolingSummary('Arial12pt',this.logger));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.bkg = lhcb.widgets.BackgroundSummary('Arial12pt',this.logger));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(cell=Cell('',1,null));
    cell.height = '40px';

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.safety=lhcb.widgets.SafetySummary('Arial12pt',this.logger));


    tb.appendChild(tr);

    // Finally add suggestions text
    tr = document.createElement('tr');
    tr.appendChild(Cell('',null,null));
    tr.appendChild(Cell('Comments and suggestions to M.Frank CERN/LHCb',2,'MonitorTinyHeader'));
    tb.appendChild(tr);

    tab.appendChild(tb);
    this.display.appendChild(tab);
    return this;
  }

  return table;
}

var oper_unload = function()  {
  dataProviderReset();
  //alert('Connection reset.\n\nBye, Bye my friend....');
}


var oper_body = function()  {
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector;

  selector = OperationsStatus(msg);
  body.appendChild(selector);
  body.className = 'MainBody';
  setWindowTitle('LHCb Detector High Voltage Status');
  if ( msg > 0 )
    selector.logger   = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'StatusLogger');
  else
    selector.logger   = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'StatusLogger');
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  //selector.start('PARTITIONS','lbWeb.PARTITIONS');
  selector.build();
  selector.subscribe();
  selector.provider.start();
  body.style.cursor = 'default';
}

if ( _debugLoading ) alert('Script lhcb.display.hv.cpp loaded successfully');
