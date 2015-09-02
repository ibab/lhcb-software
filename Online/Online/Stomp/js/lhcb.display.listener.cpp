
var FSMFrame = function(root) {
  this.root               = root;
  this.frame              = document.createElement('iframe');
  this.frame.frame        = this;
  this.frame.bgcolor      = '#CCCCCC';
  this.frame.width        = 570;
  this.frame.height       = 700;
  this.frame.marginWidth  = 3;
  this.frame.marginHeight = 3;
  this.frame.hspace       = 0;
  this.frame.vspace       = 0;
  this.frame.scrolling    = "auto";
  this.frame.style.left   = '100px';
  this.frame.style.top    = '50px';
  this.frame.style.position = 'absolute';
  this.frame.className    = 'FSMFrame';
};

var ItemListener = function(logger,provider) {
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
  };

  this.stop = function() {
    this.provider.unsubscribe(this._item);
    return this;
  };

  this.set = function(data) {
    alert('Got data:'+data);
  };
  return this;
};

var DetectorListener = function(logger,provider,parent,msg,opts) {
  this.inheritsFrom = ItemListener;
  this.inheritsFrom(logger,provider);
  this.createProperties = null;
  this._messages = msg;
  this.trace    = false;
  this._trigger  = null;
  this._detector = null;
  this._display  = null;
  this._root     = parent;
  this._type     = RunStatusDisplay;
  this._opts     = opts;
  this.url_base  = the_displayObject.url_base;
  this.bind_children = function(items, sys_names, sys_states, set_data) {
    for(var i=0; i<sys_states.length; ++i) {
      var item = items[sys_names[i]];
      item.setChildState(sys_states[i]);
      item.setCallback(this,this.open_child);
      if ( set_data ) item.set(item.data);
    }
  };
  
  this.handle_data = function(partition, sys_names, sys_states) {
    var disp = this._display;
    if ( !disp ) {
      this.remove_display();
      this._display = this._type(this.partition, this.provider, this.logger);
      if ( this.createProperties )  {
	this._display.createProperties = this.createProperties;
      }
      this._display.attach(sys_names);
      this._root.appendChild(this._display);
    }
    this.bind_children(this._display.fsmDisplay.items, sys_names, sys_states, disp);
    this._display.statusDisplay.setCallback(this,this.close_from_button);
    return this;
  };

  this.handle_no_data = function(partition) {
    return null;
  };

  this.remove_display= function() {
    this.logger.debug('this.remove_display:'+this.partition);
    if ( this._display != null ) {
      if ( this._display.parentNode == this._root ) {
	this._root.removeChild(this._display);
      }
      this._display = null;
      this.close_child(this.partition);
    }
  };

  this.close_from_button = function() {
    this.handler.close();
  };

  this.close = function() {
    this.stop();
    this.provider.unsubscribeAll();
    this.remove_display();
  };

  this.close_child = function(sys) {
    this.provider.reset();
    try {
      var e = window.parent.document.getElementById("fsm_iframe_"+sys);
      this.logger.debug('this.close_child() system:'+sys+'  element:'+e);
      if ( e != null ) {
	e.parentNode.removeChild(e);
      }
    } catch(err) {
    }
  };

  this.open_child = function() {
    var self = this.handler;
    var i = this.item;
    var url = self.url_base+'?type=fsm&system='+i.sysname;
    var is_IE = _isInternetExplorer();
    if ( is_IE )  {
      alert('\nSorry, you are running\n\n'+navigator.appName+
	    '.\n\nOpening child panels is not supported\n'+
	    'since unexpected Javascript side-effects appear.');
      return;
    }

    if ( self._messages ) url += '&messages=1';
    url += '&state='+i.childState;
    self.logger.info('Open subsystem URL:'+url+' '+self._messages);
    var f = new FSMFrame(this._root);
    f.frame.id   = "fsm_iframe_"+i.sysname;
    f.frame.name = "fsm_iframe_"+i.sysname;
    f.frame.src  = url;
    self._root.appendChild(f.frame);
  };

  this.subscribeSubListeners = function() {
    if ( this._trigger == null ) {
      this._trigger = new DetectorListener(this.logger,this.provider,null,this._messages,this._opts);
      this._trigger.master = this;
      this._trigger.subscribeSubListeners = function() {};
      this._trigger.handle_data = function(partition, sys_names, sys_states) {
	this.master._display.attachTriggers(sys_names);
	this.master.bind_children(this.master._display.trgDisplay.items, sys_names, sys_states, false);
      };
      this._trigger.start(this.partition,'lbWeb.'+this.partition+'.FSM.triggers');
    }
    if ( this._detector == null ) {
      this._detector = new DetectorListener(this.logger,this.provider,null,this._messages,this._opts);
      this._detector.master = this;
      this._detector.subscribeSubListeners = function() {};
      this._detector.handle_data = function(partition, sys_names, sys_states) {
	this.master._display.attachDetectors(sys_names);
	this.master.bind_children(this.master._display.detDisplay.items, sys_names, sys_states, false);
      };
      this._detector.start(this.partition,'lbWeb.'+this.partition+'.FSM.detectors');
    }
  };

  this.set = function(data) {
    this.logger.info('DetectorListener: Got data:'+data);
    if ( data.length < 2 ) {
      if ( this.handle_no_data(this.partition) != null ) {
	return this;
      }
      else if ( this.trace ) {
	alert('(1) The service '+this._item+' looks not sane....\n\n'+
	      'These are the data I received:\n'+data);
      }
      return null;
    }
    if ( this._data == data[1] ) {
      //alert('No new data --- ignore '+data[1]);
      return null;
    }

    this._data = data[1];
    var systems = data[1].split('/');
    if ( systems.length < 1 || systems[0] == "DEAD" ) {
      if ( this.trace ) {
	alert('(2) The service '+this._item+' looks not sane....\n\n'+
	      'These are the data I received:\n'+data[0]+'\n'+data[1]);
      }
      return null;
    }

    var sys_names = new Array();
    var sys_states = new Array();

    for(var i=0; i<systems.length; ++i) {
      var sys = systems[i].split('|');
      // Remove HV, INF and injector from LHCb.
      // These are not controlled by the run-control itself.
      if ( sys[0]=='LHCb_HV' ) continue;
      if ( sys[0]=='LHCb_INF' ) continue;
      if ( sys[0]=='LHCb_Injector' ) continue;
      sys_names.push(sys[0]);
      sys_states.push(sys[1]);
    }
    this.handle_data(this.partition, sys_names, sys_states);
    this.subscribeSubListeners();
    if ( this._opts && this._opts.size_call ) this._opts.size_call();
    return this;
  };

  return this;
};

var FSMListener = function(logger,provider,parent,msg) {
  this.inheritsFrom = DetectorListener;
  this.inheritsFrom(logger,provider,parent,msg,null);
  this._type = FSMStatusDisplay;

  this.subscribeSubListeners = function() {  };
  this.handle_no_data = function(partition) {
    this.remove_display();
    this._display = this._type(this.partition, this.provider, this.logger);
    this._display.attach(new Array());
    this._display.statusDisplay.setCallback(this,this.close_from_button);
    this._root.appendChild(this._display);
    return this;
  };
};

if ( _debugLoading ) alert('Script lhcb.display.listener.cpp loaded successfully');
