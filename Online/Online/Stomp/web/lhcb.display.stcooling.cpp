_loadScript('lhcb.display.tooltips.cpp');
_loadScript('lhcb.display.items.cpp');
_loadFile('lhcb.display.general','css');


var STCoolingStatus = function(msg)   {
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

  table.className = 'MonitorPage';
  table.body.className = 'MonitorPage';
  table.body.cellpadding = 0;
  table.body.cellspacing = 0;
  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  /**
  */
  table.Cooling_summary = function() {
    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');
    tab.className  = 'MonitorData';
    tab.width      = '100%';
    tb.className   = 'MonitorData';
    tb.cellSpacing = 0;
    tb.cellPadding = 0;
    //tab.style.borderWidth = '10px';

    tr = document.createElement('tr');
    tb.appendChild(tr);
    td = document.createElement('td');
    td.innerHTML = 'ST Cooling data';
    td.colSpan = 2;
    tr.appendChild(td);

    tr = document.createElement('tr');
    this.manifold_temp01 = StyledItem('lbWeb.CaV/ItPlant.Actual.Measurements.param01',null,'Temperature 1: %7.2f &#186;C');
    tr.appendChild(this.manifold_temp01);
    this.manifold_temp02 = StyledItem('lbWeb.CaV/ItPlant.Actual.Measurements.param02',null,'Temperature 2: %7.2f &#186;C');
    tr.appendChild(this.manifold_temp02);
    tb.appendChild(tr);
    tab.appendChild(tb);
    return tab;
  }

  table.subscribeItem = function(item) {
    this.provider.subscribe(item.name,item);
  }
  table.subscribe = function() {
    this.subscribeItem(this.manifold_temp01);
    this.subscribeItem(this.manifold_temp02);
  }

  table.build = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var t1, tb1, tr1, td1, d = new Date();

    tab.width ='100%';
    tb.width  = '100%';

    tr = document.createElement('tr');

    // Right hand of the display
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(t1=document.createElement('table'));
    td.style.width = '60%';
    td.colSpan = 2;
    t1.appendChild(tb1=document.createElement('tbody'));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Cooling_summary());

    tb.appendChild(tr);

    // Finally add suggestions text
    tr = document.createElement('tr');
    tr.appendChild(Cell('',null,null));
    tr.appendChild(Cell('Comments and suggestions to J.Luisier CERN/LHCb',2,'MonitorTinyHeader'));
    tb.appendChild(tr);

    tab.appendChild(tb);
    this.display.appendChild(tab);
    return this;
  }

  return table;
}

var stcooling_unload = function()  {
  dataProviderReset();
  //alert('Connection reset.\n\nBye, Bye my friend....');
}


var stcooling_body = function()  {
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector;

  selector = STCoolingStatus(msg);
  body.appendChild(selector);
  setWindowTitle('LHCb Detector High Voltage Status');
  if ( msg > 0 )
    selector.logger   = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'StatusLogger');
  else
    selector.logger   = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'StatusLogger');
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  selector.build();
  selector.subscribe();
  selector.provider.start();
}

if ( _debugLoading ) alert('Script lhcb.display.detstatus.cpp loaded successfully');
