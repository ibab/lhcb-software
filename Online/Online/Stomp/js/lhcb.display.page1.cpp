_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadScript('lhcb.display.zoom.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');

/*
----------------------------------------------------------------  8-SEP 18:41:05
 LEP Fill 8986 is SHUTDOWN               | ALEPH run 58472 is INACTIVE
--------------------------------------------------------------------------------
 Energy   6.971  nominal  -1.000 GeV     | RUN_196 , Started  2-NOV-2000 08:02
 Current (mA)   0.000 e+  *   0.000 e-   |           Last 4 mn     Run    Fill
 Luminosity : Now      0.00 10^30        | Events            0       0       0
        This fill      0.00 nb-1         | LCAL              0       0       0
 Collimators are Physics                 | QQbar             0       0       0
-------- LEP operator comments ----------| L1 rate Hz    0.000   0.000 Ineff.(%)
LEP in limbo, awaiting decision ...      | L2 rate Hz    0.000   0.000 Op   0.00
                                         | L3 rate Hz    0.000   0.000 DAQ  0.00
                                         | Deadtime %     0.00    0.00 Dt   0.00
                                         | Downtime %     0.00    0.00 Tot  0.00
                                         | Magnet          3.1    12.1     9.9
                                         | BKG    TPC    -0.80  SAMBA     0.00
----- Shift crew : None --------------------------------------------------------
----- Comment updated on 18-SEP-2000 16:28 -------------------------------------
*/

var Page1 = function(msg, sys)   {
  var tr, td, tb, tab;
  var table       = document.createElement('table');
  table.body      = document.createElement('tbody');
  table.messages  = msg;
  table.system    = sys;
  table.provider  = null;
  table.logger    = null;

  table.add = function() {
    var tr = document.createElement('tr');
    var td = document.createElement('td');
    td.setAttribute('colSpan',2);
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  };

  table.className = table.body.className = 'MonitorOuterFrame';
  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  table.subscribe = function() {

    this.LHC_header.subscribe(this.provider);
    this.LHC_summary.subscribe(this.provider);
    this.LHC_operator.subscribe(this.provider);
    this.LHCb_shift.subscribe(this.provider);
    this.LHCb_header.subscribe(this.provider);
    this.LHCb_summary.subscribe(this.provider);
    return this;
  };

  table.build = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr, d = new Date();

    tab.className = tb.className = 'MonitorInnerFrame';
    this.heading = document.createElement('tr');
    var cell = Cell(lhcb_online_picture()+'&nbsp;'+this.system+' Page1',1,'MonitorBigHeader');
    cell.style.textAlign = 'left';
    cell.style.width = '360px';
    cell.onmouseout  = function() { document.body.style.cursor="default";     };
    cell.onmouseover = function() { document.body.style.cursor="pointer";     };
    cell.onclick     = function() { document.location = lhcb.constants.urls.lhcb.home_page.src;};
    tooltips.set(cell,'LHCb page1:<br>General status information of<br>-- the LHC collider and<br>-- the LHCb experiment<br>Click to go to home page');
    this.heading.appendChild(cell);

    var sub_tab  = document.createElement('table');
    var sub_body = document.createElement('tbody');
    tr = document.createElement('tr');
    sub_tab.width=sub_body.width="100%";
    this.head_date = Cell(d.toString(),1,'MonitorTinyHeader');
    this.head_date.textAlign = 'right';
    this.head_date.id = 'current_time';
    this.head_date.style.width = '100%';
    tr.appendChild(this.head_date);
    cell = Cell('<B>A<sup>+</sup></B>',1,'MonitorDataHeader');
    cell.onclick     = function() { zoom_increaseFontSize();};
    tooltips.set(cell,'Increase font size');
    tr.appendChild(cell);
    cell = Cell('<B>A<sup>-</sup></B>',1,'MonitorDataHeader');
    cell.onclick     = function() { zoom_decreaseFontSize();};
    tooltips.set(cell,'Decrease font size');
    tr.appendChild(cell);

    sub_body.appendChild(tr);
    sub_tab.appendChild(sub_body);
    cell = Cell('',1,'MonitorTinyHeader');
    cell.appendChild(sub_tab);
    this.heading.appendChild(cell);
    tb.appendChild(this.heading);

    this.timerHandler = function() {document.getElementById('current_time').innerHTML = (new Date()).toString(); };
    setInterval(this.timerHandler,5000);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    
    td = document.createElement('td');
    td.appendChild(this.LHC_header=lhcb.widgets.LHC_header());
    td.style.width = '45%';
    tooltips.set(td,'LHC status summary.<br>Click to see LHC operations page');
    td.onclick     = lhcb.widgets.goto_lhc_operations_page;
    tr.appendChild(td);
    
    td = document.createElement('td');
    td.style.height = '55%';
    td.appendChild(this.LHCb_header=lhcb.widgets.Det_Run_header(this.system,this.logger));
    tooltips.set(td,'LHCb run status<br>Click to see full run status information.');
    td.onclick = lhcb.widgets.goto_lhcb_daq_page;
    tr.appendChild(td);
    
    
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.appendChild(this.LHC_summary=lhcb.widgets.LHC_summary());
    tooltips.set(td,'LHC collider status information<br>Click to move to LHC info page.');
    td.onclick     = function() { document.location = lhcb.constants.urls.lhcb.lhc_status.src;};
    tr.appendChild(td);

    td = document.createElement('td');
    this.LHCb_summary = lhcb.widgets.Det_Run_Summary({system:this.system,logger:this.logger});
    this.LHCb_summary.height = '230px';
    td.appendChild(this.LHCb_summary);
    td.onclick     = function() { document.location = lhcb.constants.urls.lhcb.detstatus.src;};
    tooltips.set(td,'LHCb detector information<br>Click to see LHCb detector hardware status.');
    td.rowSpan = 2;
    tr.appendChild(td);
    tb.appendChild(tr);
    // LHC operator comments
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.appendChild(this.LHC_operator=lhcb.widgets.LHC_operator_comments());    
    td.onclick     = function() { document.location = lhcb.constants.operations_url("LHCCOORD"); };
    tooltips.set(td,'Comment from the LHC operator<br>Click to move to LHC coodination planning page.');
    tr.appendChild(td);
    tb.appendChild(tr);
    // Shift crew comments
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.appendChild(this.LHCb_shift=lhcb.widgets.LHCb_shift_comments());
    td.onclick     = function() { document.location = lhcb.constants.urls.lhcb.elog.src;};
    tooltips.set(td,'The last entries from elog (LHCb)<br>Click to see full elog.');
    td.colSpan = 2;
    tr.appendChild(td);
    tb.appendChild(tr);

    // Finally add suggestions text
    tr = document.createElement('tr');
    tr.appendChild(cell=Cell('Comments and suggestions to M.Frank CERN/LHCb',2,'MonitorTinyHeader'));
    cell.style.textAlign = 'right';
    tb.appendChild(tr);

    tab.appendChild(tb);
    this.display.appendChild(tab);
    return this;
  };

  return table;
};

var page1_unload = function()  {
  dataProviderReset();
  return 1;
};

var page1_body = function()  {
  var prt  = the_displayObject['external_print'];
  var msg  = the_displayObject['messages'];
  var sys  = the_displayObject['system'];
  var siz  = the_displayObject['size'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector;

  if ( sys == null ) sys = 'LHCb';
  selector = Page1(msg, sys);
  body.appendChild(selector);
  body.className = 'MainBody';
  setWindowTitle(sys+' Page 1');
  if ( msg > 0 )
    selector.logger   = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'StatusLogger');
  else
    selector.logger   = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'StatusLogger');
  if ( prt ) selector.logger.print = prt;
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  selector.build();

  if ( null == siz && screen.width>1600 ) siz = 3;
  if ( _isInternetExplorer() ) zoom_changeFontSizeEx(2);
  else  zoom_changeFontSizeEx(0);
  if ( siz != null ) zoom_changeFontSizeEx(siz);
  selector.subscribe();
  selector.provider.start();
  body.style.cursor = 'default';
  return selector;
};

if ( _debugLoading ) alert('Script lhcb.display.page1.cpp loaded successfully');
