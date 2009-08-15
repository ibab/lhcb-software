_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.listener.cpp');
_loadScript('dom.print.cpp');
_loadFile('lhcb.display.status','css');

SelectionBox = function() {
  this.root = document.createElement('select');

  this.add = function(tag, text, selected) {
    var o = document.createElement('option');
    var i = document.createTextNode(text);
    o.setAttribute('selected',selected);
    o.setAttribute('value',tag);
    o.appendChild(i);
    this.root.appendChild(o);
    return this;
  }

  this.value = function() {
    return this.root.options[this.root.selectedIndex].value;
  }

  return this;
}


PartitionSelector = function() {
  this.inheritsFrom = ItemListener;
  this.inheritsFrom(null,null);
  this.select = new SelectionBox();
  this.provider = null;
  this.listener = null;
  this.logger = null;

  this.add = function() {
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.setAttribute('colSpan',4);
    tr.appendChild(td);
    this._body.appendChild(tr);
    return td;
  }

  this.element  = document.createElement('table');
  this.element.className = 'RunStatus';
  this._body   = document.createElement('tbody');

  this.change = document.createElement('button');
  this.change.value = 'Show';
  this.change.invoker = this;
  this.change.onclick = function() { this.invoker.createDisplay();       }

  this.update = document.createElement('button');
  this.update.value = 'Update Display';
  this.update.onclick = function() { _dataProvider.update();  }

  var tr = document.createElement('tr');
  var td = document.createElement('td');
  td.innerHTML = 'Select partition:';
  tr.appendChild(td);

  td = document.createElement('td');
  td.appendChild(this.select.root);
  tr.appendChild(td);

  td = document.createElement('td');
  td.appendChild(this.change);
  tr.appendChild(td);

  td = document.createElement('td');
  td.appendChild(this.update);
  tr.appendChild(td);

  this._body.appendChild(tr);

  this.display = this.add();
  this.logDisplay = this.add();

  this.element.appendChild(this._body);

  this.createDisplay = function()   {
    var partition = this.select.value();
    if ( null != this.listener ) this.listener.close2();
    this.listener = new DetectorListener(this.logger,this.provider,this.display);
    this.listener.start(partition,'lbWeb.'+partition+'.FSM.children');
  }

  this.set = function(data) {
    if ( data.length < 2 ) {
      alert('The service '+this._item+' looks not sane....\n\n'+
	    'These are the data I received:\n'+data);
      return null;
    }
    if ( this._data == data[1] ) {
      //alert('No new data --- ignore '+data[1]);
      return;
    }
    this._data = data[1];
    var systems = data[1].split('/');
    if ( systems.length < 1 || systems[0] == "DEAD" ) {
      alert('The service '+this._item+' looks not sane....\n\n'+
	    'These are the data I received:\n'+data[0]+'\n'+data[1]);
      return null;
    }
    for(var i=0; i<systems.length; ++i) {
      var s = systems[i];
      if ( s == 'CALO' ) this.select.add(s,s,true);
      else this.select.add(s,s,false);
    }
    return this;
  }
  return this;
}


status_unload = function()  {
  dataProviderReset();
  //alert('Connection reset.\n\nBye, Bye my friend....');
}

status_body = function()  {
  var body = document.getElementsByTagName('body')[0];
  var selector = new PartitionSelector();
  body.appendChild(selector.element);
  selector.logger   = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'RunStatusLogger');
  selector.provider = new DataProvider(selector.logger);
  selector.start('PARTITIONS','lbWeb.PARTITIONS');
  selector.provider.start();
}


if ( _debugLoading ) alert('Script lhcb.display.status.cpp loaded successfully');
