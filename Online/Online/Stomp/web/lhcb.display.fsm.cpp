_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.listener.cpp');
_loadScript('dom.print.cpp');
_loadFile('lhcb.display.status','css');


FSMTable = function(parent,partition) {
  this.parent = parent;
  this.system = partition;
  this.provider = null;
  this.listener = null;
  this.logger = null;
  
  this.add = function(object) {
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.appendChild(object);
    td.colSpan = 3;
    tr.appendChild(td);
    this._body.appendChild(tr);
  }

  this.close_display = function() {
    var self = this.invoker;
    //alert('close_display: '+self.listener+' self.system:'+self.system);
    if ( self.listener )  {
      self.listener.close_child(self.system);
    }
  }

  this.element  = document.createElement('table');
  this.element.className = 'FSMStatus';
  this._body   = document.createElement('tbody');

  this.close = document.createElement('button');
  this.close.value = 'Close Display';
  this.close.onclick = this.close_display;
  this.close.invoker = this;

  this.update = document.createElement('button');
  this.update.value = 'Update Display';
  this.update.onclick = function() { _dataProvider.update();  }

  this.display = document.createElement('div');
  this.logDisplay = document.createElement('div');

  var tr = document.createElement('tr');
  var td = document.createElement('td');
  td.innerHTML = 'FSM display of '+this.system;
  tr.appendChild(td);

  td = document.createElement('td');
  td.appendChild(this.update);
  tr.appendChild(td);

  td = document.createElement('td');
  td.appendChild(this.close);
  tr.appendChild(td);

  this._body.appendChild(tr);

  this.add(this.display);
  this.add(this.logDisplay);

  this.element.appendChild(this._body);
  this.parent.appendChild(this.element);

  this.createDisplay = function()   {
    if ( null != this.listener ) this.listener.close2();
    this.listener = new FSMListener(this.logger,this.provider,this.display);
    this.listener.start(this.system,'lbWeb.'+this.system+'.FSM.children');
  }
  return this;
}


fsm_unload = function()  {
  dataProviderReset();
  //alert('Connection reset.\n\nBye, Bye my friend....');
}

fsm_body2 = function(body)  {
  document.bgcolor = '#CCCCCC';
  var selector = new FSMTable(body,the_displayObject['system']);
  selector.logger   = new OutputLogger(selector.logDisplay, 200, LOG_INFO,'FSMStatusLogger');
  selector.provider = new DataProvider(selector.logger);
  selector.createDisplay();
  selector.provider.start();
}

fsm_body = function()  {
  fsm_body2(document.getElementsByTagName('body')[0]);
}


if ( _debugLoading ) alert('Script lhcb.display.fsm.cpp loaded successfully');
