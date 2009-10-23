/* 
 Stefano de Capua
 2009 October 22nd
*/

_loadScriptAbs('http://cern.ch/frankm/test/Online/Stomp/web', 'lhcb.display.tooltips.cpp');
_loadScriptAbs('http://cern.ch/frankm/test/Online/Stomp/web', 'lhcb.display.items.cpp');
_loadFileAbs('http://cern.ch/frankm/test/Online/Stomp/web', 'lhcb.display.general','css');

var VELOCoolingStatus = function(msg)
{
  var tr, td, tb, tab;
  table           = document.createElement('table');
  table.body      = document.createElement('tbody');
  table.messages  = msg;
  table.provider  = null;
  table.logger    = null;


  table.add = function()
  {
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

  /** Display rendering function for received data
   *  @param      data    Display data
   *  @return On success reference to self, null otherwise
   */

  table.temp_cnv = function(data)
  {
    var fmt = this.format;
    
    this.className = this.className;
    if ( data[1] == 'undefined' )
    {
      item_data = '&lt;undefined&gt;';
    }
    else
    {
      var fval = parseFloat(data[1]);
      var item_value, item_data = fval;
      item_data = sprintf(fmt,item_data);
      if ( fval > 15.0 )
      {
        this.style.backgroundColor = 'red';
      }
      else
      {
        this.style.backgroundColor = 'green';
      }
    }
    
    this.innerHTML = item_data;
    if ( _DisplayDebug>1 ) alert('StyledItem.display:'+item_data);
    return this;
  }
  /**
   */
  table.Cooling_summary = function(detType)
  {
    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');
    tab.className  = 'MonitorData';
    tab.width      = '100%';
    tb.className   = 'MonitorData';
    tb.cellSpacing = 0;
    tb.cellPadding = 0;
    tab.style.borderWidth = '2px';
    
    if (detType == 'VELO')
    {
      tr = document.createElement('tr');
      tb.appendChild(tr);
      td = document.createElement('td');
      td.innerHTML = 'VELO Cooling data';
      td.colSpan = 2;
      tr.appendChild(td);
      
      tr = document.createElement('tr');
      this.velo_cooling_setpointA = StyledItem('lbWeb.VTCS_TL_AC101.SETPOINT',null,'Setpoint A: %.1f &#186;C');
      tr.appendChild(this.velo_cooling_setpointA);
      this.velo_cooling_setpointC = StyledItem('lbWeb.VTCS_TL_AC101.SETPOINT',null,'Setpoint C: %7.2f &#186;C');
      tr.appendChild(this.velo_cooling_setpointC);
      tb.appendChild(tr);
      tab.appendChild(tb);
      return tab;
    }     
    return tab;
  }

  table.subscribeItem = function(item)
  {
    this.provider.subscribe(item.name,item);
  }
  table.subscribe = function()
  {
    this.subscribeItem(this.velo_cooling_setpointA);
    this.subscribeItem(this.velo_cooling_setpointC);
  }

  table.build = function()
  {
    var tab = document.createElement('table');
    var hd = document.createElement('thead');
    var tb = document.createElement('tbody');
    var t1, tb1, tr1, td1, d = new Date();

    tab.width ='100%';
    tb.width  = '100%';

    hd.appendChild(pic=document.createElement('img'));
    pic.setAttribute('width', '100');
    pic.setAttribute('height', '60');
    pic.setAttribute('alt', 'VELO Logo');
    pic.setAttribute('src', 'http://lhcb-vd.web.cern.ch/lhcb-vd/assets/images/Velo.jpg');

    tr = document.createElement('tr');

    // Right hand of the display
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(t1=document.createElement('table'));
    td.style.width = '60%';
    td.colSpan = 2;
    t1.appendChild(tb1=document.createElement('tbody'));
    
    
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Cooling_summary('VELO'));

    tb.appendChild(tr);

    // Finally add suggestions text
    tr = document.createElement('tr');
    tr.appendChild(Cell('',null,null));
    tr.appendChild(Cell('Comments and suggestions to Stefano de Capua CERN/LHCb',2,'MonitorTinyHeader'));
    tb.appendChild(tr);

    tab.appendChild(hd);
    tab.appendChild(tb);
    this.display.appendChild(tab);
    return this;
  }

  return table;
}

var velocooling_unload = function()
{
  dataProviderReset();
  //alert('Connection reset.\n\nBye, Bye my friend....');
}


var velocooling_body = function()
{
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector;
  
  selector = VELOCoolingStatus(msg);
  body.appendChild(selector);
  setWindowTitle('VELO temperatures');
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
