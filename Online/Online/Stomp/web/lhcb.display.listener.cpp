
FSMFrame = function(root,system) {
  this.root = root;
  this.frame              = document.createElement('iframe');
  this.frame.frame        = this;
  this.frame.id           = "fsm_iframe_"+system;
  this.frame.name         = "fsm_iframe_"+system;
  this.frame.src          = "http://lxplus208:8000/static/demos/test/lhcb.display.htm?type=fsm&system="+system;
  this.frame.bgcolor      = '#CCCCCC';
  this.frame.width        = 570;
  this.frame.height       = 700;
  this.frame.marginWidth  = 3;
  this.frame.marginHeight = 3;
  this.frame.hspace       = 0;
  this.frame.vspace       = 0;
  this.frame.scrolling    = "auto";
  this.frame.style.left   = '350px';
  this.frame.style.top    = '50px';
  this.frame.style.position = 'absolute';
}


ItemListener = function(logger,provider) {
  this.partition  = '';
  this.logger     = logger;
  this.provider   = provider;
  this._data      = '';
  this._item      = '';
  this._trgListener = null;
  this._detListener = null;

  this.start = function(name,item) {
    this.partition = name;
    this._item     = item;
    this.provider.subscribe(this._item,this);
    return this;
  }

  this.stop = function() {
    this.provider.unsubscribe(this._item);
    return this;
  }

  this.set = function(data) {
    alert('Got data:'+data);
  }
  return this;
}


DetectorListener = function(logger,provider,parent) {
  this.inheritsFrom = ItemListener;
  this.inheritsFrom(logger,provider);
  this._trace    = true;
  this._trigger  = null;
  this._detector = null;
  this._display  = null;
  this._root     = parent;
  this._type     = RunStatusDisplay;

  this.remove_display= function() {
    if ( this._display != null ) {
      this._root.removeChild(this._display.root);
      delete this._display;
      this._display = null;
    }
  }

  this.handle_data = function(partition, sys_names, sys_states) {  
    this.remove_display();
    this._display = new this._type(this._root, this.partition, this.provider, this.logger);
    this._display.build().attach(sys_names);
    for(var i=0; i<sys_states.length; ++i) {
      var item = this._display.fsmDisplay.items[sys_names[i]];
      item.setState(sys_states[i]);
      item.setCallback(this,this.open_child);
    }
    this._display.statusDisplay.setCallback(this,this.close);
  }

  this.close = function() {
    this.callback_handler.close2();
  }

  this.close2 = function() {
    this.stop();
    this.provider.unsubscribeAll();
    this.remove_display();
  }

  this.open_child = function() {
    var self = this.callback_handler;
    var i = this.callback_item;
    var f = new FSMFrame(this._root,i.sysname);
    self._root.appendChild(f.frame);
  }

  this.close_child = function(sys) {
    this.provider.reset();
    var e = window.parent.document.getElementById("fsm_iframe_"+sys);
    e.parentNode.removeChild(e);
  }

  this.subscribeSubListeners = function() {
    if ( this._trigger == null ) {
      this._trigger = new DetectorListener(this.logger,this.provider,null);
      this._trigger._trace = false;
      this._trigger._root = this._root;
      this._trigger._display = this._display;
      this._trigger.master = this;
      this._trigger.partition = this.partition;
      this._trigger.subscribeSubListeners = function() {}
      this._trigger.handle_data = function(partition, sys_names, sys_states) {
	this._display.attachTriggers(sys_names);
	for(var i=0; i<sys_states.length; ++i)   {
	  var item = this._display.trgDisplay.items[sys_names[i]];
	  item.setState(sys_states[i]);
	  item.setCallback(this.master,this.master.open_child);
	}
      }
      this._trigger.start(this.partition,'lbWeb.'+this.partition+'.FSM.triggers');
    }
    if ( this._detector == null ) {
      this._detector = new DetectorListener(this.logger,this.provider);
      this._detector._trace = false; 
      this._detector._root = this._root;
      this._detector._display = this._display;
      this._detector.master = this;
      this._detector.partition = this.partition;
      this._detector.subscribeSubListeners = function() {}
      this._detector.handle_data = function(partition, sys_names, sys_states) {
	this._display.attachDetectors(sys_names);
	for(var i=0; i<sys_states.length; ++i) {
	  var item = this._display.detDisplay.items[sys_names[i]];
	  item.setState(sys_states[i]);
	  item.setCallback(this.master,this.master.open_child);
	}
      }
      this._detector.start(this.partition,'lbWeb.'+this.partition+'.FSM.detectors');
    }
  }


  this.set = function(data) {
    this.logger.info('DetectorListener: Got data:'+data);
    if ( data.length < 2 ) {
      if ( this._trace ) {
	alert('The service '+this._item+' looks not sane....\n\n'+
	      'These are the data I received:\n'+data);
      }
      return null;
    }
    if ( this._data == data[1] ) {
      //alert('No new data --- ignore '+data[1]);
      return;
    }
    this._data = data[1];
    var systems = data[1].split('/');
    if ( systems.length < 1 || systems[0] == "DEAD" ) {
      if ( this._trace ) {
	alert('The service '+this._item+' looks not sane....\n\n'+
	      'These are the data I received:\n'+data[0]+'\n'+data[1]);
      }
      return null;
    }

    var sys_names = new Array();
    var sys_states = new Array();

    for(var i=0; i<systems.length; ++i) {
      var sys = systems[i].split('|');
      sys_names[i] = sys[0];
      sys_states[i] = sys[1];
    }
    this.handle_data(this.partition, sys_names, sys_states);
    this.subscribeSubListeners();
    return this;
  }

  return this;
}

FSMListener = function(logger,provider,parent) {
  this.inheritsFrom = DetectorListener;
  this.inheritsFrom(logger,provider,parent);
  this.subscribeSubListeners = function() {}
  this._type = FSMStatusDisplay;
}

if ( _debugLoading ) alert('Script lhcb.display.listener.cpp loaded successfully');
