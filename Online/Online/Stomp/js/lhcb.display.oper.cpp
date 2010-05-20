_loadScript('lhcb.display.items.cpp');
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
  };

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
  };

  table.appendFSMItem = lhcb.widgets.mkFSMitem1;

  table.subscribeItem2 = function(item) {
    if ( item ) {
      this.provider.subscribe(item.name,item);
    }
  };
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
  };

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
  };

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
  };

  table.Voltage_Summary = function() {
    var tab = lhcb.widgets.HVSummary(this.logger).build('Arial12pt');
    tab.hvState.par = this;
    tab.hvState.conversion = function(obj,state,style) {
      var o = this.par.injState;
      o.className = this.className;
      o.style.borderStyle = 'solid';
      return state;
    };
    tab.hvRequest.par = this;
    tab.hvRequest.conversion = function(data) {
      this.par.injState.innerHTML = data;
      return data;
    };
    return tab;
  };


  table.Magnet_Summary = function() {
    var tb, tr, cell, tab = document.createElement('table');
    tab.className  = 'MonitorPage';
    
    tb = document.createElement('tbody');
    tooltips.set(tb,'Magnet summary<br>Click to move to magnet page.');
    tb.onclick = function() { document.location = lhcb.constants.lhcb_display_url("magnet");};
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
    this.magPolarity.conversion = function(data) {      return data>0 ? 'Down' : 'Up';    };
    tr = document.createElement('tr');
    tr.appendChild(this.magSet);
    tr.appendChild(this.magRead);
    tr.appendChild(this.magPolarity);
    tb.appendChild(tr);
    tab.appendChild(tb);
    return tab;
  };

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

    this.subscribeItem2(this.operState);

    this.subscribeItem2(this.lhcHandshake);
    this.subscribeItem2(this.lhcbHandshake);

    this.subscribeItem2(this.lhcState);
    this.subscribeItem2(this.bcmState);
    this.subscribeItem2(this.magState);
    this.subscribeItem2(this.clockState);

    this.subscribeItem2(this.magSet);
    this.subscribeItem2(this.magRead);
    this.subscribeItem2(this.magPolarity);

    this.subscribeItem2(this.dbRun);
    this.subscribeItem2(this.dbCond);
    this.subscribeItem2(this.dbArchive);

    this.bkg.subscribe(this.provider);
    this.lhc.subscribe(this.provider);
    this.safety.subscribe(this.provider);
    this.cooling.subscribe(this.provider);
    this.voltages.subscribe(this.provider);
    this.pressures.subscribe(this.provider);
    this.veloPosition.subscribe(this.provider);
  };

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

    this.timerHandler = function() {document.getElementById('current_time').innerHTML = (new Date()).toString(); };
    setInterval(this.timerHandler,2000);

    tr = document.createElement('tr');

    // Left half of the display
    tr.appendChild(td=document.createElement('td'));
    td.style.width = '50%';
    td.appendChild(t1=document.createElement('table'));
    t1.appendChild(tb1=document.createElement('tbody'));
    t1.className = tb1.className = 'MonitorPage';
    t1.style.border = tb1.style.border = 'none';

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
    td1.appendChild(this.voltages=this.Voltage_Summary());
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
    td1.appendChild(this.lhc=lhcb.widgets.LHCStateSummary({style:'Arial12pt',legend:true,logger:this.logger}));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.Magnet_Summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.veloPosition=lhcb.widgets.velo.positionSummary({style:'Arial12pt',legend:true,logger:this.logger}));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.pressures=lhcb.widgets.RICHPressures({style:'Arial12pt',logger:this.logger}));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.Database_Summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.cooling = lhcb.widgets.CoolingSummary({style:'Arial12pt',legend:true,logger:this.logger}));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.bkg = lhcb.widgets.BackgroundSummary({style:'Arial12pt',legend:true,logger:this.logger}));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(cell=Cell('',1,null));
    cell.height = '40px';

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.safety=lhcb.widgets.SafetySummary({style:'Arial12pt',logger:this.logger}));


    tb.appendChild(tr);

    // Finally add suggestions text
    tr = document.createElement('tr');
    tr.appendChild(Cell('',null,null));
    tr.appendChild(Cell('Comments and suggestions to M.Frank CERN/LHCb',2,'MonitorTinyHeader'));
    tb.appendChild(tr);

    tab.appendChild(tb);
    this.display.appendChild(tab);
    return this;
  };

  return table;
};

var oper_unload = function()  {
  dataProviderReset();
};

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
    selector.logger = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'StatusLogger');
  else
    selector.logger = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'StatusLogger');
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  //selector.start('PARTITIONS','lbWeb.PARTITIONS');
  selector.build();
  selector.subscribe();
  selector.provider.start();
  body.style.cursor = 'default';
};

if ( _debugLoading ) alert('Script lhcb.display.hv.cpp loaded successfully');
