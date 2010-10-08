_loadScript('lhcb.display.data.cpp');
_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.listener.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadScript('lhcb.display.zoom.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.status','css');
_loadFile('lhcb.display.fsm','css');

var s_display_font_size = null;
var s_org_display_font_size = null;

var SelectionBox = function() {
  select = document.createElement('select');
  select.curr_value = null;
  select.add = function(tag, text, selected) {
    var o = document.createElement('option');
    var i = document.createTextNode(text);
    if ( selected ) {
      o.setAttribute('selected',selected);
      this.selectedIndex = this.options.length-1;
      this.curr_value = tag;
    }
    o.setAttribute('value',tag);
    o.appendChild(i);
    this.appendChild(o);
    return this;
  };

  select.get_value = function() {
    var o = this.options[this.selectedIndex];
    if ( o )  return o.value;
    return this.curr_value;
  };
  return select;
};

var PartitionSelector = function(msg) {
  var td;
  table = document.createElement('table');
  table.body = document.createElement('tbody');
  table.inheritsFrom = ItemListener;
  table.inheritsFrom(null,null);
  table.selectBox = SelectionBox();
  table.messages = msg;
  table.provider = null;
  table.listener = null;
  table.logger = null;

  table.add = function() {
    var tr = document.createElement('tr');
    var td = document.createElement('td');
    td.setAttribute('colSpan',5);
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  };

  table.className = table.body.className = 'RunStatus';
  table.body.cellpadding = 0;

  table.heading = table.add();
  table.heading.innerHTML = 'LHCb Run Status Display';
  table.heading.colSpan = 2;
  table.heading.className = 'RunStatusHeader';
  table.heading.style.width = '90%';
  table.heading.style.textAlign='left';

  td = Cell(lhcb_online_picture(),1,null);
  table.heading.parentNode.appendChild(td);
  td = Cell('A<sup>+</sup>',1,'MonitorDataHeader');
  td.onclick     = function() { zoom_increaseFontSize();};
  tooltips.set(td,'Increase font size');
  table.heading.parentNode.appendChild(td);
  td = Cell('A<sup>-</sup>',1,'MonitorDataHeader');
  td.onclick     = function() { zoom_decreaseFontSize();};
  tooltips.set(td,'Decrease font size');
  table.heading.parentNode.appendChild(td);

  table.row = document.createElement('tr');
  table.label = StatusText('Select partition:',1);
  table.row.appendChild(table.label);

  table.select = document.createElement('td');
  table.select.width = 135;
  table.select.appendChild(table.selectBox);
  table.row.appendChild(table.select);
  tooltips.set(table.select,'Select the correct partition to initiate the display');
  table.row.appendChild(Cell('',null,null));

  table.change = document.createElement('td');
  table.change.handler     = table;
  table.change.innerHTML   = 'Show';
  table.change.className   = 'DisplayButton';
  table.change.onclick     = function() { 
    if (s_org_display_font_size) s_display_font_size = s_org_display_font_size; 
    this.handler.createDisplay();
  };
  tooltips.set(table.change,'Click to invoke display for selected partition');
  table.row.appendChild(table.change);

  table.update = document.createElement('td');
  table.update.handler     = table;
  table.update.innerHTML   = 'Update';
  table.update.className   = 'DisplayButton';
  table.update.onclick     = function() { _dataProvider.update();  };
  tooltips.set(table.update,'Click to update display');
  table.row.appendChild(table.update);

  table.body.appendChild(table.row);

  table.display = table.add();
  table.logDisplay = table.add();

  table.suggestions = document.createElement('tr');
  table.suggestions.appendChild(td=Cell('Comments and suggestions to M.Frank CERN/LHCb',4,'MonitorTinyHeader'));
  td.style.textAlign = 'right';
  table.body.appendChild(table.suggestions);

  table.appendChild(table.body);

  table.hideInput = function() {
    /*
    this.row.removeChild(this.update);
    this.change.innerHTML = 'Reload';
    this.change.width = '10%';
    this.row.removeChild(this.select);
    this.row.removeChild(this.label);
    this.row.removeChild(this.change);
    tooltips.set(this.change,'Click to reload display');
    this.row.appendChild(this.update=document.createElement('td'));
    while(this.row.childNodes.length>0) {
      this.row.removeChild(this.row.childNodes[0]);
    }
    */
    this.body.removeChild(this.row);
    this.row = null;
  };

  table.showLHCstate = function() {
    var td=document.createElement('td');
    this.row = document.createElement('tr');
    this.row.appendChild(td);
    td.colSpan = this.heading.parentNode.childNodes.length+1;
    this.lhcState = lhcb.widgets.LHC_header2();
    td.appendChild(this.lhcState);
    this.body.insertBefore(this.row,this.display.parentNode);
    this.lhcState.subscribe(this.provider);
  };

  table.callbackNo = 3;
  table.resize_font = function() {
    if ( s_display_font_size != null ) {
      if ( _isInternetExplorer() ) zoom_changeFontSizeEx(2);
      else  zoom_changeFontSizeEx(0);
      //alert(s_display_font_size); 
      zoom_changeFontSizeEx(s_display_font_size);
      s_org_display_font_size = s_display_font_size;
      s_display_font_size = null;
    }
  };
  table.size_call = function()  {
    ++this.callbackNo;
    //alert(this.callbackNo);
    if ( (this.callbackNo%3)==0 && s_display_font_size != null ) {
      setTimeout(this.resize_font,3000);
    }
  };
  table.createDisplay = function()   {
    var partition = this.selectBox.get_value();
    var opts = {'size_call': this.size_call};
    if ( null != this.listener )   {
      this.listener.close();
      this.provider.start();
    }
    this.callbackNo = 3;
    this.listener = new DetectorListener(this.logger,this.provider,this.display,this.messages,this);
    this.listener.trace = false;
    this.listener.start(partition,'lbWeb.'+partition+'.FSM.children');
    this.heading.innerHTML = partition+' Run Status Display';
    setWindowTitle(partition+' Run Status');
  };

  table.set = function(data) {
    if ( data.length < 2 ) {
      alert('The service '+this._item+' looks not sane....\n\n'+
	    'These are the data I received:\n'+data);
      return null;
    }
    if ( this._data == data[1] ) {
      //alert('No new data --- ignore '+data[1]);
      return null;
    }
    this._data = data[1];
    var systems = data[1].split('/');
    if ( systems.length < 1 || systems[0] == "DEAD" ) {
      alert('The service '+this._item+' looks not sane....\n\n'+
	    'These are the data I received:\n'+data[0]+'\n'+data[1]);
      return null;
    }
    var has_selected = false;
    this.selectBox.length = 0;
    for(var i=0; i<systems.length; ++i) {
      var s = systems[i];
      if ( s == 'LHCb' ) {
	this.selectBox.add(s,s,true);
	has_selected = true;
      }
      else if ( s == 'CALO' && !has_selected ) {
	this.selectBox.add(s,s,true);
      }
      else {
	this.selectBox.add(s,s,false);
      }
    }
    return this;
  };
  return table;
};

var status_unload = function()  {
  dataProviderReset();
};

var status_body = function()  {
  var prt  = the_displayObject['external_print'];
  var msg  = the_displayObject['messages'];
  var sys  = the_displayObject['system'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector = new PartitionSelector(msg);

  s_display_font_size = the_displayObject['size'];
  if ( null == s_display_font_size && screen.width>1500 ) s_display_font_size = 2;

  body.appendChild(selector);
  body.className = 'MainBody';

  if ( msg > 0 )
    selector.logger = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'RunStatusLogger');
  else
    selector.logger = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'RunStatusLogger');
  if ( prt ) selector.logger.print = prt;
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  if ( sys == null ) {
    setWindowTitle('Run Status Display');
    selector.start('PARTITIONS','lbWeb.PARTITIONS');
  }
  else {
    setWindowTitle(sys+' Run Status');
    selector.selectBox.add(sys,sys,true);
    selector.createDisplay(); 
    selector.hideInput();
    if ( sys == 'LHCb' ) {
      selector.showLHCstate();
    }
  }
  selector.provider.start();
};


if ( _debugLoading ) alert('Script lhcb.display.status.cpp loaded successfully');
