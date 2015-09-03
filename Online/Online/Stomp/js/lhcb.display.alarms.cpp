_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadScript('lhcb.display.zoom.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');

var Alarms = function(msg)  {
  //alarms
  var tr, td, tb, tab;
  var table       = document.createElement('table');
  table.body      = document.createElement('tbody');
  table.messages  = msg;
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
 

  table.build = function()  {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr, d = new Date();

    tab.className = tb.className = 'MonitorInnerFrame';
    this.heading = document.createElement('tr');
    var cell = Cell(lhcb_online_picture()+'&nbsp;LHCb PVSS Alarms',1,'MonitorBigHeader');
    cell.style.textAlign = 'left';
    cell.style.width = '800px';
    cell.setAttribute('colSpan',2);
    cell.onmouseout  = function() { document.body.style.cursor="default";     };
    cell.onmouseover = function() { document.body.style.cursor="pointer";     };
    tooltips.set(cell,'LHCb PVSS Alarms');
    this.heading.appendChild(cell);
    
    tab.className = tb.className = 'MonitorSubHeader';
  

    var sub_tab  = document.createElement('table');
    var sub_body = document.createElement('tbody');
    tr = document.createElement('tr');
    sub_tab.width=sub_body.width="100%";
    this.head_date = Cell(d.toString(),1,'MonitorTinyHeader');
    this.head_date.textAlign = 'right';
    this.head_date.id = 'current_time';
    this.head_date.style.width = '100%';
    tr.appendChild(this.head_date);
/*
    cell = Cell('<B>A<sup>+</sup></B>',1,'MonitorDataHeader');
    cell.onclick     = function() { zoom_increaseFontSize();};
    tooltips.set(cell,'Increase font size');
    tr.appendChild(cell);
    cell = Cell('<B>A<sup>-</sup></B>',1,'MonitorDataHeader');
    cell.onclick     = function() { zoom_decreaseFontSize();};
    tooltips.set(cell,'Decrease font size');
    tr.appendChild(cell);
*/
    sub_body.appendChild(tr);
    sub_tab.appendChild(sub_body);
    cell = Cell('',1,'MonitorTinyHeader');
    cell.appendChild(sub_tab);

    this.heading.appendChild(cell);
    tb.appendChild(this.heading);

    this.timerHandler = function() {document.getElementById('current_time').innerHTML = (new Date()).toString(); };
    setInterval(this.timerHandler,5000);
    tab.appendChild(tb);
    
    //alarms part
    this.alarms = StyledItem('lbWeb.Alarms_Ctrl.webAlarms',null,null);
    var tbd = document.createElement('tbody');
    this.alarms.parent = tbd;
    
    tab.appendChild(tbd);
    tab.style.backgroundColor='white';
    this.display.appendChild(tab);
    
    this.alarms.display = function(data) {
    
      while ( this.parent.childNodes.length >= 1 )
      {
        this.parent.removeChild( this.parent.firstChild );       
      } 
      var th = document.createElement('th');

      tr = document.createElement('tr');
      tr.style.backgroundColor='gray';
      tr.style.color='black';
      tr.style.font='Arial';
      th.appendChild(document.createTextNode('Description'));
      tr.appendChild(th);
      th = document.createElement('th');
      th.appendChild(document.createTextNode('Alarm Text'));
      tr.appendChild(th);
      th = document.createElement('th');
      th.appendChild(document.createTextNode('Time'));
      tr.appendChild(th);
      this.parent.appendChild(tr);
      
      var items = (''+data).split('{');
      for(var i=1; i<items.length; ++i) {
        var alarmTokens = items[i].split(' $ ');
	var fatalPatt = /fatal/i;
	var errorPatt = /error/i;
	var warnPatt = /warn/i;
	var color = 'green', td = null;
        var blink = false;
	var val;
	if (alarmTokens[4].match(fatalPatt)) {
	  color = '#FF0000';
	  blink = true;
	}
	else if (alarmTokens[4].match(errorPatt)) {
	  color = '#FFA500';
	  blink = true;
	}
	else if (alarmTokens[4].match(warnPatt)) {
	  color = '#FFFF00';
	}
	else {
	  color='green';
	}
	
	tr = document.createElement('tr');
	tr.style.textAlign='left';
	tr.style.backgroundColor=color;
	tr.style.color    = 'black';
	tr.style.height   = '30px';
	tr.style.font     = 'Arial';
	tr.style.fontSize = '15px';

	td = document.createElement('td');
	td.appendChild(document.createTextNode(alarmTokens[1]));
	td.style.width='30%';
        tr.appendChild(td);

	val = alarmTokens[2];
	td = document.createElement('td');
        td.appendChild(document.createTextNode(val));
	td.style.width='55%';
        tr.appendChild(td);

	val = alarmTokens[3];
	td = document.createElement('td');
        td.appendChild(document.createTextNode(val.substring(0,val.length-4)));
	td.style.width='15%';
	td.style.textAlign='center';
        tr.appendChild(td);
        this.parent.appendChild(tr);
      }
      //console.log(this.parent.innerHTML);
    };
    //=====
    return this;
  };
  
  table.subscribe = function()  {
     this.alarms.subscribe(this.provider);
     return this;
  };
  return table;
};
  
  
var alarms_unload = function()  {
  dataProviderReset();
  return 1;
};

var alarms_body = function()  {
  var prt  = the_displayObject['external_print'];
  var msg  = the_displayObject['messages'];
  var siz  = the_displayObject['size'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector;

  selector = Alarms(msg);
  body.appendChild(selector);
  body.className = 'MainBody';
  setWindowTitle('LHCb PVSS Alarms');
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

if ( _debugLoading ) alert('Script lhcb.display.alarms.cpp loaded successfully');
