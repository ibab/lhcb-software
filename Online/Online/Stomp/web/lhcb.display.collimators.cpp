_loadScript('lhcb.display.tooltips.cpp');
_loadScript('lhcb.display.items.cpp');
_loadFile('lhcb.display.general','css');

var Collimators = function(msg)   {
  var tr, td, tb, tab;
  table           = document.createElement('table');
  table.body      = document.createElement('tbody');
  table.messages  = msg;
  table.provider  = null;
  table.listener  = null;
  table.logger    = null;

  table.add = function() {
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.setAttribute('colSpan',1);
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  }

  table.className = table.body.className = 'MonitorPage';
  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  table.LHC_header = function() {
    var tab = document.createElement('table');
    var tb  = document.createElement('tbody');
    var tr  = document.createElement('tr');

    this.lhcFillNumber = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.FillNumber',null,null);
    this.lhcBeamMode = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.BeamMode','Text-Left',null);

    tab.style.width = tb.style.width = '100%';
    tab.className = tb.className  = 'MonitorBigHeaderBLUE';

    tb.appendChild(tr);
    tr.appendChild(Cell('LHC Fill',null,null));    
    tr.appendChild(this.lhcFillNumber);
    tr.appendChild(Cell('Beam status:',null,'Text-Right'));    
    tr.appendChild(this.lhcBeamMode);
    tr.appendChild(Cell('',null,null));

    tab.appendChild(tb);
    return tab;
  }
  
  /**
     Build collimator and TED table
  */
  table.LHC_collimators = function() {
    var base, cell, td, tr, tab = document.createElement('table');
    var tb = document.createElement('tbody');

    tab.className   = tb.className = 'MonitorData';
    tab.style.width = tb.style.width = '100%';
    base = 'lbWeb.LHCCOM/LHC.LHC.Machine.CollimatorPositions.';
    this.TCLIB_6L8_B2 = new Object();
    this.TCLIB_6L8_B2.lvdt_gap_downstream   = StyledItem(base+'TCLIB_6L8_B2.lvdt_gap_downstream',  'MonitorTableDataLeft', '%8.2f mm');
    this.TCLIB_6L8_B2.lvdt_gap_upstream     = StyledItem(base+'TCLIB_6L8_B2.lvdt_gap_upstream',    'MonitorTableDataRight','%8.2f mm');
    this.TCLIB_6L8_B2.lvdt_left_downstream  = StyledItem(base+'TCLIB_6L8_B2.lvdt_left_downstream', 'MonitorTableDataLeft', '%8.2f mm');
    this.TCLIB_6L8_B2.lvdt_left_upstream    = StyledItem(base+'TCLIB_6L8_B2.lvdt_left_upstream',   'MonitorTableDataRight','%8.2f mm');
    this.TCLIB_6L8_B2.lvdt_right_downstream = StyledItem(base+'TCLIB_6L8_B2.lvdt_right_downstream','MonitorTableDataLeft', '%8.2f mm');
    this.TCLIB_6L8_B2.lvdt_right_upstream   = StyledItem(base+'TCLIB_6L8_B2.lvdt_right_upstream',  'MonitorTableDataRight','%8.2f mm');

    this.TCTH_4L8_B1 = new Object();
    this.TCTH_4L8_B1.lvdt_gap_downstream   = StyledItem(base+'TCTH_4L8_B1.lvdt_gap_downstream',    'MonitorTableDataLeft', '%8.2f mm');
    this.TCTH_4L8_B1.lvdt_gap_upstream     = StyledItem(base+'TCTH_4L8_B1.lvdt_gap_upstream',      'MonitorTableDataRight','%8.2f mm');
    this.TCTH_4L8_B1.lvdt_left_downstream  = StyledItem(base+'TCTH_4L8_B1.lvdt_left_downstream',   'MonitorTableDataLeft', '%8.2f mm');
    this.TCTH_4L8_B1.lvdt_left_upstream    = StyledItem(base+'TCTH_4L8_B1.lvdt_left_upstream',     'MonitorTableDataRight','%8.2f mm');
    this.TCTH_4L8_B1.lvdt_right_downstream = StyledItem(base+'TCTH_4L8_B1.lvdt_right_downstream',  'MonitorTableDataLeft', '%8.2f mm');
    this.TCTH_4L8_B1.lvdt_right_upstream   = StyledItem(base+'TCTH_4L8_B1.lvdt_right_upstream',    'MonitorTableDataRight','%8.2f mm');

    this.TCTH_4R8_B2 = new Object();
    this.TCTH_4R8_B2.lvdt_gap_downstream   = StyledItem(base+'TCTH_4R8_B2.lvdt_gap_downstream',    'MonitorTableDataLeft', '%8.2f mm');
    this.TCTH_4R8_B2.lvdt_gap_upstream     = StyledItem(base+'TCTH_4R8_B2.lvdt_gap_upstream',      'MonitorTableDataRight','%8.2f mm');
    this.TCTH_4R8_B2.lvdt_left_downstream  = StyledItem(base+'TCTH_4R8_B2.lvdt_left_downstream',   'MonitorTableDataLeft', '%8.2f mm');
    this.TCTH_4R8_B2.lvdt_left_upstream    = StyledItem(base+'TCTH_4R8_B2.lvdt_left_upstream',     'MonitorTableDataRight','%8.2f mm');
    this.TCTH_4R8_B2.lvdt_right_downstream = StyledItem(base+'TCTH_4R8_B2.lvdt_right_downstream',  'MonitorTableDataLeft', '%8.2f mm');
    this.TCTH_4R8_B2.lvdt_right_upstream   = StyledItem(base+'TCTH_4R8_B2.lvdt_right_upstream',    'MonitorTableDataRight','%8.2f mm');

    this.TCTVB_4L8 = new Object();
    this.TCTVB_4L8.lvdt_gap_downstream   = StyledItem(base+'TCTVB_4L8.lvdt_gap_downstream',    'MonitorTableDataLeft', '%8.2f mm');
    this.TCTVB_4L8.lvdt_gap_upstream     = StyledItem(base+'TCTVB_4L8.lvdt_gap_upstream',      'MonitorTableDataRight','%8.2f mm');
    this.TCTVB_4L8.lvdt_left_downstream  = StyledItem(base+'TCTVB_4L8.lvdt_left_downstream',   'MonitorTableDataLeft', '%8.2f mm');
    this.TCTVB_4L8.lvdt_left_upstream    = StyledItem(base+'TCTVB_4L8.lvdt_left_upstream',     'MonitorTableDataRight','%8.2f mm');
    this.TCTVB_4L8.lvdt_right_downstream = StyledItem(base+'TCTVB_4L8.lvdt_right_downstream',  'MonitorTableDataLeft', '%8.2f mm');
    this.TCTVB_4L8.lvdt_right_upstream   = StyledItem(base+'TCTVB_4L8.lvdt_right_upstream',    'MonitorTableDataRight','%8.2f mm');

    this.TCTVB_4R8 = new Object();
    this.TCTVB_4R8.lvdt_gap_downstream   = StyledItem(base+'TCTVB_4R8.lvdt_gap_downstream',    'MonitorTableDataLeft', '%8.2f mm');
    this.TCTVB_4R8.lvdt_gap_upstream     = StyledItem(base+'TCTVB_4R8.lvdt_gap_upstream',      'MonitorTableDataRight','%8.2f mm');
    this.TCTVB_4R8.lvdt_left_downstream  = StyledItem(base+'TCTVB_4R8.lvdt_left_downstream',   'MonitorTableDataLeft', '%8.2f mm');
    this.TCTVB_4R8.lvdt_left_upstream    = StyledItem(base+'TCTVB_4R8.lvdt_left_upstream',     'MonitorTableDataRight','%8.2f mm');
    this.TCTVB_4R8.lvdt_right_downstream = StyledItem(base+'TCTVB_4R8.lvdt_right_downstream',  'MonitorTableDataLeft', '%8.2f mm');
    this.TCTVB_4R8.lvdt_right_upstream   = StyledItem(base+'TCTVB_4R8.lvdt_right_upstream',    'MonitorTableDataRight','%8.2f mm');

    this.TDI_4R8 = new Object();
    this.TDI_4R8.lvdt_gap_downstream   = StyledItem(base+'TDI_4R8.lvdt_gap_downstream',            'MonitorTableDataLeft', '%8.2f mm');
    this.TDI_4R8.lvdt_gap_upstream     = StyledItem(base+'TDI_4R8.lvdt_gap_upstream',              'MonitorTableDataRight','%8.2f mm');
    this.TDI_4R8.lvdt_left_downstream  = StyledItem(base+'TDI_4R8.lvdt_left_downstream',           'MonitorTableDataLeft', '%8.2f mm');
    this.TDI_4R8.lvdt_left_upstream    = StyledItem(base+'TDI_4R8.lvdt_left_upstream',             'MonitorTableDataRight','%8.2f mm');
    this.TDI_4R8.lvdt_right_downstream = StyledItem(base+'TDI_4R8.lvdt_right_downstream',          'MonitorTableDataLeft', '%8.2f mm');
    this.TDI_4R8.lvdt_right_upstream   = StyledItem(base+'TDI_4R8.lvdt_right_upstream',            'MonitorTableDataRight','%8.2f mm');

    base = 'lbWeb.LHCCOM/LHC.LHC.Machine.TEDPosition.';
    this.TEDTI2  = StyledItem(base+'TEDTI2.position', 'MonitorTableDataCenter','%s');
    this.TEDTI8  = StyledItem(base+'TEDTI8.position', 'MonitorTableDataCenter','%s');
    this.TEDTT40 = StyledItem(base+'TEDTT40.position','MonitorTableDataCenter','%s');
    this.TEDTT60 = StyledItem(base+'TEDTT60.position','MonitorTableDataCenter','%s');

    this.ted_conversion = function(value) {
      var v = '';//' ('+value+')';
      if ( value == 0 ) return 'Moving'+v;
      else if ( value == 1 ) return 'Installation'+v;
      else if ( value == 2 ) return 'Open'+v;
      else if ( value == 3 ) return 'Closed'+v;
      else if ( value == 4 ) return 'Transport'+v;
      else if ( value == 5 ) return 'Undefined'+v;
      return 'Unknown ('+value+')';
    }
    this.TEDTI2.conversion = this.ted_conversion;
    this.TEDTI8.conversion = this.ted_conversion;
    this.TEDTT40.conversion = this.ted_conversion;
    this.TEDTT60.conversion = this.ted_conversion;
    

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('&nbsp;&nbsp;&#151;&#151;&#62; LHC collimators positions around LHCb at Point 8',2,'MonitorDataHeaderRED'));

    tr = document.createElement('tr');
    td = document.createElement('td');
    tr.appendChild(td);
    tb.appendChild(tr);
    this.colltab = document.createElement('table');
    this.collbody = document.createElement('tbody');
    this.colltab.className = this.collbody.className = 'MonitorPage';
    tr = document.createElement('tr');    
    this.collbody.appendChild(tr);
    
    tr.appendChild(cell=Cell('Collimator',1,'MonitorDataHeaderCenter'));
    cell.style.width='14%';

    tr.appendChild(cell=Cell('Down&nbsp;&#60;&#151;LEFT&#151;&#62;&nbsp;Up',2,'MonitorDataHeaderCenter'));
    cell.style.width='25%';
    tr.appendChild(cell=Cell('&nbsp',1,null));
    cell.style.width='3%';

    tr.appendChild(cell=Cell('Down&nbsp;&#60;&#151;GAP&#151;&#62;&nbsp;Up',2,'MonitorDataHeaderCenter'));
    cell.style.width='25%';
    tr.appendChild(cell=Cell('&nbsp',1,null));
    cell.style.width='3%';

    tr.appendChild(cell=Cell('Down&nbsp;&#60;&#151;RIGHT&#151;&#62;&nbsp;Up',2,'MonitorDataHeaderCenter'));
    cell.style.width='25%';

    tr = document.createElement('tr');
    this.collbody.appendChild(tr);
    tr.appendChild(Cell('TCLIB_6L8_B2',1,'MonitorDataHeader'));
    tr.appendChild(this.TCLIB_6L8_B2.lvdt_left_downstream);
    tr.appendChild(this.TCLIB_6L8_B2.lvdt_left_upstream);
    tr.appendChild(Cell('&nbsp',1,null));
    tr.appendChild(this.TCLIB_6L8_B2.lvdt_gap_downstream);
    tr.appendChild(this.TCLIB_6L8_B2.lvdt_gap_upstream);
    tr.appendChild(Cell('&nbsp',1,null));
    tr.appendChild(this.TCLIB_6L8_B2.lvdt_right_downstream);
    tr.appendChild(this.TCLIB_6L8_B2.lvdt_right_upstream);

    tr = document.createElement('tr');
    this.collbody.appendChild(tr);
    tr.appendChild(Cell('TCTH_4L8_B1',1,'MonitorDataHeader'));
    tr.appendChild(this.TCTH_4L8_B1.lvdt_left_downstream);
    tr.appendChild(this.TCTH_4L8_B1.lvdt_left_upstream);
    tr.appendChild(Cell('&nbsp',1,null));
    tr.appendChild(this.TCTH_4L8_B1.lvdt_gap_downstream);
    tr.appendChild(this.TCTH_4L8_B1.lvdt_gap_upstream);
    tr.appendChild(Cell('&nbsp',1,null));
    tr.appendChild(this.TCTH_4L8_B1.lvdt_right_downstream);
    tr.appendChild(this.TCTH_4L8_B1.lvdt_right_upstream);

    tr = document.createElement('tr');
    this.collbody.appendChild(tr);
    tr.appendChild(Cell('TCTVB_4L8',1,'MonitorDataHeader'));
    tr.appendChild(this.TCTVB_4L8.lvdt_left_downstream);
    tr.appendChild(this.TCTVB_4L8.lvdt_left_upstream);
    tr.appendChild(Cell('&nbsp',1,null));
    tr.appendChild(this.TCTVB_4L8.lvdt_gap_downstream);
    tr.appendChild(this.TCTVB_4L8.lvdt_gap_upstream);
    tr.appendChild(Cell('&nbsp',1,null));
    tr.appendChild(this.TCTVB_4L8.lvdt_right_downstream);
    tr.appendChild(this.TCTVB_4L8.lvdt_right_upstream);

    tr = document.createElement('tr');
    this.collbody.appendChild(tr);
    tr.appendChild(Cell('TCTH_4R8_B2',1,'MonitorDataHeader'));
    tr.appendChild(this.TCTH_4R8_B2.lvdt_left_downstream);
    tr.appendChild(this.TCTH_4R8_B2.lvdt_left_upstream);
    tr.appendChild(Cell('&nbsp',1,null));
    tr.appendChild(this.TCTH_4R8_B2.lvdt_gap_downstream);
    tr.appendChild(this.TCTH_4R8_B2.lvdt_gap_upstream);
    tr.appendChild(Cell('&nbsp',1,null));
    tr.appendChild(this.TCTH_4R8_B2.lvdt_right_downstream);
    tr.appendChild(this.TCTH_4R8_B2.lvdt_right_upstream);

    tr = document.createElement('tr');
    this.collbody.appendChild(tr);
    tr.appendChild(Cell('TDI_4R8',1,'MonitorDataHeader'));
    tr.appendChild(this.TDI_4R8.lvdt_left_downstream);
    tr.appendChild(this.TDI_4R8.lvdt_left_upstream);
    tr.appendChild(Cell('&nbsp',1,null));
    tr.appendChild(this.TDI_4R8.lvdt_gap_downstream);
    tr.appendChild(this.TDI_4R8.lvdt_gap_upstream);
    tr.appendChild(Cell('&nbsp',1,null));
    tr.appendChild(this.TDI_4R8.lvdt_right_downstream);
    tr.appendChild(this.TDI_4R8.lvdt_right_upstream);

    tr = document.createElement('tr');
    this.collbody.appendChild(tr);
    tr.appendChild(Cell('TCTVB_4R8',1,'MonitorDataHeader'));
    tr.appendChild(this.TCTVB_4R8.lvdt_left_downstream);
    tr.appendChild(this.TCTVB_4R8.lvdt_left_upstream);
    tr.appendChild(Cell('&nbsp',1,null));
    tr.appendChild(this.TCTVB_4R8.lvdt_gap_downstream);
    tr.appendChild(this.TCTVB_4R8.lvdt_gap_upstream);
    tr.appendChild(Cell('&nbsp',1,null));
    tr.appendChild(this.TCTVB_4R8.lvdt_right_downstream);
    tr.appendChild(this.TCTVB_4R8.lvdt_right_upstream);

    this.colltab.appendChild(this.collbody);
    td.appendChild(this.colltab);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('&nbsp;&nbsp;&#151;&#151;&#62; LHC TED positions around LHCb at Point 8',2,'MonitorDataHeaderRED'));

    tr = document.createElement('tr');
    td = document.createElement('td');
    tr.appendChild(td);
    tb.appendChild(tr);
    this.tedtab = document.createElement('table');
    this.tedbody = document.createElement('tbody');
    this.tedtab.width = this.tedbody.width = '100%';
    this.tedtab.className = this.tedbody.className = 'MonitorPage';

    tr = document.createElement('tr');    
    this.tedbody.appendChild(tr);
    tr.appendChild(cell=Cell('TED',1,'MonitorDataHeaderCenter'));
    cell.style.width='20%';
    tr.appendChild(cell=Cell('TEDTI2',1,'MonitorDataHeaderCenter'));
    cell.style.width='20%';
    tr.appendChild(cell=Cell('TEDTI8',1,'MonitorDataHeaderCenter'));
    cell.style.width='20%';
    tr.appendChild(cell=Cell('TEDTT40',1,'MonitorDataHeaderCenter'));
    cell.style.width='20%';
    tr.appendChild(cell=Cell('TEDTT60',1,'MonitorDataHeaderCenter'));
    cell.style.width='20%';

    tr = document.createElement('tr');    
    this.tedbody.appendChild(tr);
    tr.appendChild(cell=Cell('Position',1,'MonitorDataHeaderCenter'));
    tr.appendChild(this.TEDTI2);
    tr.appendChild(this.TEDTI8);
    tr.appendChild(this.TEDTT40);
    tr.appendChild(this.TEDTT60);

    this.tedtab.appendChild(this.tedbody);
    td.appendChild(this.tedtab);


    tab.appendChild(tb);
    return tab;
  }

  table.subscribeItem = function(item) {
    this.provider.subscribe(item.name,item);
  }
  table.subscribe = function() {
    //this.subscribeItem(this.);
    //this.subscribeItem(this.);

    this.subscribeItem(this.lhcBeamMode);
    this.subscribeItem(this.lhcFillNumber);

    this.subscribeItem(this.TCLIB_6L8_B2.lvdt_gap_downstream);
    this.subscribeItem(this.TCLIB_6L8_B2.lvdt_gap_upstream);
    this.subscribeItem(this.TCLIB_6L8_B2.lvdt_left_downstream);
    this.subscribeItem(this.TCLIB_6L8_B2.lvdt_left_upstream);
    this.subscribeItem(this.TCLIB_6L8_B2.lvdt_right_downstream);
    this.subscribeItem(this.TCLIB_6L8_B2.lvdt_right_upstream);

    this.subscribeItem(this.TCTH_4L8_B1.lvdt_gap_downstream);
    this.subscribeItem(this.TCTH_4L8_B1.lvdt_gap_upstream);
    this.subscribeItem(this.TCTH_4L8_B1.lvdt_left_downstream);
    this.subscribeItem(this.TCTH_4L8_B1.lvdt_left_upstream);
    this.subscribeItem(this.TCTH_4L8_B1.lvdt_right_downstream);
    this.subscribeItem(this.TCTH_4L8_B1.lvdt_right_upstream);

    this.subscribeItem(this.TCTH_4R8_B2.lvdt_gap_downstream);
    this.subscribeItem(this.TCTH_4R8_B2.lvdt_gap_upstream);
    this.subscribeItem(this.TCTH_4R8_B2.lvdt_left_downstream);
    this.subscribeItem(this.TCTH_4R8_B2.lvdt_left_upstream);
    this.subscribeItem(this.TCTH_4R8_B2.lvdt_right_downstream);
    this.subscribeItem(this.TCTH_4R8_B2.lvdt_right_upstream);

    this.subscribeItem(this.TDI_4R8.lvdt_gap_downstream);
    this.subscribeItem(this.TDI_4R8.lvdt_gap_upstream);
    this.subscribeItem(this.TDI_4R8.lvdt_left_downstream);
    this.subscribeItem(this.TDI_4R8.lvdt_left_upstream);
    this.subscribeItem(this.TDI_4R8.lvdt_right_downstream);
    this.subscribeItem(this.TDI_4R8.lvdt_right_upstream);

    this.subscribeItem(this.TCTVB_4L8.lvdt_gap_downstream);
    this.subscribeItem(this.TCTVB_4L8.lvdt_gap_upstream);
    this.subscribeItem(this.TCTVB_4L8.lvdt_left_downstream);
    this.subscribeItem(this.TCTVB_4L8.lvdt_left_upstream);
    this.subscribeItem(this.TCTVB_4L8.lvdt_right_downstream);
    this.subscribeItem(this.TCTVB_4L8.lvdt_right_upstream);

    this.subscribeItem(this.TCTVB_4R8.lvdt_gap_downstream);
    this.subscribeItem(this.TCTVB_4R8.lvdt_gap_upstream);
    this.subscribeItem(this.TCTVB_4R8.lvdt_left_downstream);
    this.subscribeItem(this.TCTVB_4R8.lvdt_left_upstream);
    this.subscribeItem(this.TCTVB_4R8.lvdt_right_downstream);
    this.subscribeItem(this.TCTVB_4R8.lvdt_right_upstream);

    this.subscribeItem(this.TEDTI2);
    this.subscribeItem(this.TEDTI8);
    this.subscribeItem(this.TEDTT40);
    this.subscribeItem(this.TEDTT60);

    return this;
  }

  table.build = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var d = new Date();

    tab.className = tb.className = 'MonitorPage';
    this.heading = document.createElement('tr');
    var cell = Cell(lhcb_online_picture()+'&nbsp;LHCb Collimator Monitor',1,'MonitorBigHeader');
    cell.style.textAlign = 'left';
    this.heading.appendChild(cell);
    this.head_date = Cell(d.toString(),1,'MonitorTinyHeader');
    this.head_date.style.textAlign = 'right';
    this.head_date.id = 'current_time';
    this.heading.appendChild(this.head_date);
    tb.appendChild(this.heading);

    this.timerHandler = function() {
      document.getElementById('current_time').innerHTML = Date().toString();
    }
    setInterval(this.timerHandler,2000);
    
    tb.appendChild(tr=document.createElement('tr'));
    
    td = document.createElement('td');
    td.colSpan = 2;
    td.appendChild(this.LHC_header());
    tr.appendChild(td);
    
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.colSpan = 2;
    td.appendChild(this.LHC_collimators());
    tr.appendChild(td);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(cell=Cell('Comments and suggestions to M.Frank CERN/LHCb',2,'MonitorTinyHeader'));
    cell.style.textAlign = 'right';
    tb.appendChild(tr);

    tab.appendChild(tb);
    this.display.appendChild(tab);
    return this;
  }

  return table;
}

var collimators_unload = function()  {
  dataProviderReset();
  //alert('Connection reset.\n\nBye, Bye my friend....');
}


var collimators_body = function()  {
  var prt  = the_displayObject['external_print'];
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector;

  selector = Collimators(msg);
  body.appendChild(selector);
  body.className = 'MainBody';
  setWindowTitle('LHC Collimator settings @ Pt.8');
  //msg = 1;
  if ( msg > 0 )
    selector.logger   = new OutputLogger(selector.logDisplay, 50, LOG_INFO, 'StatusLogger');
  else
    selector.logger   = new OutputLogger(selector.logDisplay, -1, LOG_INFO, 'StatusLogger');
  if ( prt ) selector.logger.print = prt;
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  selector.build();
  selector.subscribe();
  selector.provider.start();
}

if ( _debugLoading ) alert('Script lhcb.display.collimators.cpp loaded successfully');
