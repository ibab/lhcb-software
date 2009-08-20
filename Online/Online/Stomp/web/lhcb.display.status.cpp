_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.listener.cpp');
_loadScript('dom.print.cpp');
_loadFile('lhcb.display.status','css');

var SelectionBox = function() {
  select = document.createElement('select');
  select.add = function(tag, text, selected) {
    var o = document.createElement('option');
    var i = document.createTextNode(text);
    if ( selected ) o.setAttribute('selected',selected);
    o.setAttribute('value',tag);
    o.appendChild(i);
    this.appendChild(o);
    return this;
  }

  select.get_value = function() {
    return this.options[this.selectedIndex].value;
  }
  return select;
}


var PartitionSelector = function(msg) {
  table = document.createElement('table');
  table.body = document.createElement('tbody');
  table.inheritsFrom = ItemListener;
  table.inheritsFrom(null,null);
  table.select = SelectionBox();
  table.messages = msg;
  table.provider = null;
  table.listener = null;
  table.logger = null;

  table.add = function() {
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.setAttribute('colSpan',4);
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  }

  table.className = 'RunStatus';
  table.body.className = 'RunStatus';
  table.body.cellpadding = 0;

  var tr = document.createElement('tr');
  tr.appendChild(StatusText('Select partition:',1));

  var td = document.createElement('td');
  td.width = 135;
  td.appendChild(table.select);
  tr.appendChild(td);

  table.change = document.createElement('td');
  table.change.handler   = table;
  table.change.innerHTML = 'Show';
  table.change.className = 'DisplayButton';
  table.change.onclick   = function() { this.handler.createDisplay();  }
  tr.appendChild(table.change);

  table.update = document.createElement('td');
  table.update.handler   = table;
  table.update.innerHTML = 'Update';
  table.update.className = 'DisplayButton';
  table.update.onclick   = function() { _dataProvider.update();  }
  tr.appendChild(table.update);

  table.body.appendChild(tr);

  table.display = table.add();
  table.logDisplay = table.add();

  table.appendChild(table.body);

  table.createDisplay = function()   {
    var partition = this.select.get_value();
    if ( null != this.listener )   {
      this.listener.close();
      this.provider.start();
    }
    this.listener = new DetectorListener(this.logger,this.provider,this.display,this.messages);
    this.listener.trace = false;
    this.listener.start(partition,'lbWeb.'+partition+'.FSM.children');
  }

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
    this.select.length = 0;
    for(var i=0; i<systems.length; ++i) {
      var s = systems[i];
      if ( s == 'LHCb' ) {
	this.select.add(s,s,true);
	has_selected = true;
      }
      else if ( s == 'CALO' && !has_selected ) {
	this.select.add(s,s,true);
      }
      else {
	this.select.add(s,s,false);
      }
    }
    return this;
  }
  return table;
}


var status_unload = function()  {
  dataProviderReset();
  //alert('Connection reset.\n\nBye, Bye my friend....');
}

var status_body = function()  {
  var msg = the_displayObject['messages'];
  var sys = the_displayObject['system'];
  var body = document.getElementsByTagName('body')[0];
  var selector = new PartitionSelector(msg);
  body.appendChild(selector);

  //alert('status_body: system:'+sys+' msg:'+msg);

  if ( msg > 0 )
    selector.logger   = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'RunStatusLogger');
  else
    selector.logger   = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'RunStatusLogger');
  selector.provider = new DataProvider(selector.logger);
  if ( sys == null ) {
    selector.start('PARTITIONS','lbWeb.PARTITIONS');
  }
  else {
    selector.select.add(sys,sys,true);
    selector.createDisplay(); 
  }
  selector.provider.start();
}


if ( _debugLoading ) alert('Script lhcb.display.status.cpp loaded successfully');
