_loadScriptAbs(_lhcbScriptBase,'lhcb.tools.cpp');

var _dataProvider   = null;
var _dataLogger     = null;

var LOG_ERROR       = 0;
var LOG_WARNING     = 1;
var LOG_INFO        = 2;
var LOG_DEBUG       = 3;
var LOG_VERBOSE     = 4;

/**@class OutputLogger
 *
 *
 *  @author  M.frank
 *  @version 1.0
 */
var OutputLogger = function(parent, len, level, style)  {
  this.className    = style;
  this.lines        = null;
  if ( len>0 ) {
    this.lines = new Array();
    this.lines.length = 0;
  }
  this.parent       = parent;
  this.length       = len;
  this.level        = level;
  this.curr         = 0;
  this.table        = document.createElement('table');
  this.body         = document.createElement('tbody');
  this.messages     = document.createElement('tr');
  this.output_td    = document.createElement('td');
  this.output       = document.createElement('div');
  this.b_hide       = document.createElement('td');
  this.b_show       = document.createElement('td');
  this.b_clear      = document.createElement('td');

  _dataLogger = this;

  this.output.className = this.className;
  this.output.innerHTML = '';
  this.output_td.colSpan = 3;
  this.output_td.appendChild(this.output);
  this.messages.appendChild(this.output_td);
  this.body.appendChild(this.messages);

  var row = document.createElement('tr');
  row.appendChild(this.b_hide);
  row.appendChild(this.b_show);
  row.appendChild(this.b_clear);

  this.body.appendChild(row);
  this.table.appendChild(this.body);
  this.parent.appendChild(this.table);


  this.showMessages = function() {
    if ( this.lines ) {
      if ( this.output != null && this.lines.length > 0 ) {
	var message = '';
	for(var i=this.curr+1; i<this.lines.length; ++i)
          message += '&rarr; ' + this.lines[i] + '<br></br>';
	for(var i=0; i<=this.curr; ++i)
          message += '&rarr; ' + this.lines[i] + '<br></br>';
        this.output.innerHTML = message;
        this.output.scrollTop = this.output.scrollHeight;
      }
    }
    return this;
  };

  this.print = function(level, msg) {
    /*
    if ( this.lines != null && level <= this.level && this.length>0 ) {
     if ( this.lines.length < this.length ) {
	this.lines.length = this.lines.length+1;
      }
      if ( this.curr>this.lines.length-1 ) this.curr = 0;
      if ( level == LOG_VERBOSE ) {
	this.lines[this.curr] = this.format(Date().toString()+' [VERBOSE]:  '+msg);
      }
      else if ( level == LOG_DEBUG ) {
	this.lines[this.curr] = this.format(Date().toString()+' [DEBUG]:    '+msg);
      }
      else if ( level == LOG_INFO ) {
	this.lines[this.curr] = this.format(Date().toString()+' [INFO]:     '+msg);
      }
      else if ( level == LOG_ERROR ) {
	this.lines[this.curr] = this.format(Date().toString()+' [ERROR]:    '+msg);
      }
      else  {
	this.lines[this.curr] = this.format(Date().toString()+' [UNKNOWN]:  '+msg);
      }
      this.showMessages();
      this.curr = this.curr + 1;
    }
    */
    return this;
  };

  this.hide = function() {
    this.b_hide.innerHTML = '';
    this.b_clear.innerHTML = '';
    if ( this.length > 0 ) {
      this.b_show.innerHTML = '<BUTTON class="DisplayButton" onclick="_dataLogger.show()">Show Messages</BUTTON>';
    }
    else {
      this.b_show.innerHTML = '';
    }
    this.output_td.removeChild(this.output);
  };
  
  this.show = function() {
    this.b_show.innerHTML = '';
    this.b_clear.innerHTML = '<BUTTON class="DisplayButton" onclick="_dataLogger.clear()">Clear</BUTTON>';
    this.b_hide.innerHTML  = '<BUTTON class="DisplayButton" onclick="_dataLogger.hide()">Hide Messages</BUTTON>';
    this.output_td.appendChild(this.output);
    this.showMessages();
  };
  
  this.error = function(msg) {
    return this.print(LOG_ERROR,msg);
  };
  
  this.info = function(msg) {
    return this.print(LOG_INFO,msg);
  };
  
  this.debug = function(msg) {
    return this.print(LOG_DEBUG,msg);
  };

  this.verbose = function(msg) {
    return this.print(LOG_VERBOSE,msg);
  };

  this.clear = function() {
    if ( this.lines ) {
      this.curr         = 0;
      this.lines.length = 0;
      this.output.innerHTML = '';
      this.output.scrollTop = this.output.scrollHeight;
    }
  };

  this.format = function(expr) {
    var s = prettyprint(expr);
    s = htmlescape(s);
    return s;
  };
  
  var htmlescape = function(expr) {
    var s = expr.replace("&", "&amp;", "g");
    s = s.replace("<", "&lt;", "g");
    s = s.replace(">", "&gt;", "g");
    s = s.replace(" ", "&nbsp;", "g");
    s = s.replace("\n", "<br></br>", "g");
    return s;
  };

  var prettyprint = function(s) {
    var q = "(";
    if(typeof(s) == "string") return s;
    for (var i=0; i<s.length; i++) {
      if (typeof(s[i]) != "object")
	q += s[i];
      else
	q += prettyprint(s[i]);
      if (i < s.length -1)
	q += " ";
    }
    q += ")";
    return q;
  };
  
  this.hide();
  return this;
};


var dataLoggerTest = function(id) {
  var logger  = new OutputLogger(document.getElementById(id),10,LOG_DEBUG);
  logger.print(LOG_DEBUG,   'LOG_DEBUG: hello 1');
  logger.print(LOG_INFO,    'LOG_INFO: hello 1');
  logger.print(LOG_WARNING, 'LOG_WARNING: hello 1');
  logger.print(LOG_ERROR,   'LOG_ERROR: hello 1');
  for(var k=0; k<20; ++k)  {
    logger.info(k+' : hello 1  --> '+k);
  }
};

/** @class DataItem
 *
 * Simple scripting class to place streaming data items into an html file.
 *
 *  @author  M.frank
 *  @version 1.0
 */
var DataItem = function(provider, name)   {
  this._name  = name;
  this._elem = null;
  provider.subscribe(this._name,this);
  
  /// Late access of the document's element
  this.element = function() {
    if ( this._elem == null )   {
      this._elem = document.getElementById(this._name);
      if ( this._elem == null ) {
	alert('Invalid document:'+document.location+'\n\nNo element found with ID:'+this._name+'\n');
      }
    }
    return this._elem;
  };
  
  /// Default callback for dataprovider on feeeding data
  this.set = function(data) {
    this.element().innerHTML = data;
  };
};

/** @class ElementItem
 *
 * Simple scripting class to place streaming data items into an html file.
 *
 *  @author  M.frank
 *  @version 1.0
 */
var ElementItem = function(provider, name, fmt, element)   {
  e = element;
  e._name  = name;
  e._format = fmt;
  provider.subscribe(e._name,e);
  
  /// Default callback for dataprovider on feeeding data
  e.set = function(data) {
    var item_data = 'Unknown';
    if ( this._format != null ) {
      if ( data[0] == 21 )        // Integer
	item_data = sprintf(this._format,parseInt(data[1]));
      else if ( data[0] == 22 )   // Float
	item_data = sprintf(this._format,parseFloat(data[1]));
      else if ( data[0] == 25 )   // String
	item_data = sprintf(this._format,data[1]);
      else
	item_data = data[1];
    }
    else {
      item_data = data[1];
    }
    this.innerHTML = item_data;
  };
  return e;
};

var RawItem = function(provider, name, fmt, element)   {
  e = element;
  e._name  = name;
  provider.subscribe(e._name,e);
  
  /// Default callback for dataprovider on feeeding data
  e.set = function(data) {
    var s = ''+data;
    this.innerHTML = s.replace("<", "&lt;", "g").replace(">", "&gt;", "g").replace(" ", "&nbsp;", "g").replace("\n", "<br></br>", "g");
  };
  return e;
};

/** @class DataProvider
 *
 *
 *  @author  M.Frank
 *  @version 1.0
 */
var DataProvider = function(logger)  {
  this.calls = new Array();
  this.items = new Object();
  this.calls.length = 0;
  this.logger = logger;
  this.isConnected = false;
  this.needConnection = false;
  this.topic = '/topic/home';
  this.sessionKey = null;
  this.command = '/topic/clientCommands';
  this.pageName = 'dataPage';
  _dataProvider = this;

  // set up stomp client.
  var stomp = new STOMPClient();
  this.logger.info("Created STOMP client....");
  stomp.parent = this;
  this.service = stomp;

  // Setup subscription options for ActiveMQ
  this.connectOpts = {};
  this.subscribeOpts = {};
  this.subscribeOpts['exchange'] = '';
  this.subscribeOpts['activemq.maximumPendingMessageLimit']=5;
  this.subscribeOpts['activemq.dispatchAsync']=true;
  this.subscribeOpts['activemq.prefetchSize']=2;

  if ( the_displayObject ) this.pageName=the_displayObject.type;

  var clid = '';
  try {
    var b, l, s = ''+document.location, d = new Date();
    s = s.replace(/type=/g,'');
    if ( (b=s.lastIndexOf('/')+1) < 1 ) b = 0;
    if ( (l=s.indexOf('&'))       < 0 ) l = s.length;
    clid = s.substring(b,l)+'  '+d.getFullYear()+'.'+d.getMonth()+'.'+d.getDate()+'--'+
                                 d.getHours()+':'+ d.getMinutes()+':'+d.getSeconds()+'.'+
                                 d.getMilliseconds()+'--'+Math.floor(Math.random()*100);
    clid = clid.replace(/\?/g,'-');
    clid = get_ip_address()+'  '+get_ip_name()+'  '+clid;
  }
  catch (e) {
  }
  this.connectOpts['client-id'] = 'ID:'+clid;

  this.reconnectHandler = function() {
    _dataProvider.logger.info('Starting reconnect timer');
    if ( !_dataProvider.isConnected && this._dataProvider.needConnection ) {
      _dataProvider.reconnect();
      setTimeout(_dataProvider.reconnectHandler,10000);
    }
  };

  stomp.onopen = function() {
    _dataProvider.logger.info("Transport opened");
    try {
      this.parent.sessionKey = this.transport.session.sessionKey;
    }
    catch (e) {
    }
  };

  stomp.onclose = function(code) {
    _dataProvider.isConnected = false;
    if ( code == 18 ) {
      _dataProvider.logger.info("Transport closed (code: " + code + ", SECURITY_ERR)");
    }
    else if ( code == 101 ) {
      _dataProvider.logger.info("Transport closed (code: " + code + ", NETWORK_ERR)");
      _dataProvider.logger.info("Transport closed "+window.location);
    }
    else if ( code == 102 ) {
      _dataProvider.logger.info("Transport closed (code: " + code + ", ABORT_ERR)");
    }
    else {
      _dataProvider.logger.info("Transport closed (code: " + code + ")"); 
    }
    _dataProvider.logger.info("Reconnect flag:"+_dataProvider.needConnection);

    if ( _dataProvider.needConnection ) {
      _dataProvider.logger.info('Starting reconnection timer');
      setTimeout(_dataProvider.reconnectHandler,10000);
    }
  };

  stomp.onerror = function(error) {
    _dataProvider.logger.error("onerror: " + error);
  };

  stomp.onerrorframe = function(frame) {
    _dataProvider.logger.error("onerrorframe: " + frame.body);
  };

  stomp.onconnectedframe = function() {
    _dataProvider.logger.info("Connected ..");
    _dataProvider.needConnection = true;
    _dataProvider.isConnected = true;
    _dataProvider.connect();
  };

  /// Data dispatch callback once stomp receieves data
  stomp.onmessageframe = function(frame) {
    //alert('stomp.onmessageframe:'+frame.body);
    var i, d, o, len, v = frame.body.split('#');
    if ( v.length >= 2 ) {
      var itm  = v[1];
      var data = v.slice(2);
      if ( data != 'DEAD' ) {
	o = _dataProvider.items[itm];
	len = o.length;
	if ( !o.prev_data ) {
	  o.prev_data = data;
	}
	else if ( o.prev_data+'' == data+'' ) {
	  _dataProvider.logger.info('Ignore: [' +frame.body.length+' bytes] '+itm+'='+o.prev_data);
	  return;
	}
	o.prev_data = data;
	for(i=0; i<len; ++i) {
	  if ( o[i] ) {
	    o[i].set(data);
	  }
	  else {
	    alert('Debug: Dead element: '+itm+'['+i+'] out of '+len);
	  }
	}
	d = new String(data);
	if (d.length > 10) d = d.substr(0,10);
	_dataProvider.logger.info("Update: [" +frame.body.length+' bytes] '+itm+'='+d+' prev:'+o.prev_data);
      }
      return;
    }
    else if ( frame.body == this.parent.pageName+':reload' ) {
      this.parent.reset();
      window.location.replace(document.location);
    }
    else if ( frame.body == 'allPages:reload' ) {
      this.parent.reset();
      window.location.replace(document.location);
    }
    else if ( frame.body.substring(0,this.parent.pageName.length+5) == this.parent.pageName+':url:' ) {
      this.parent.reset();
      window.location.assign(frame.body.substring(this.parent.pageName.length+5));
    }
    _dataProvider.logger.error('onmessage: retrieved data with invalid item number');    
  };

  /** Connect to stomp channel
   *
   *  @return  Reference to self
   */
  this.start = function() {
    this.logger.info("Connecting STOMP client....");
    this.service.connect('localhost', 61613, 'guest', 'guest', this.connectOpts);
    this.needConnection = true;
    this.logger.info("Connecting STOMP client....Done");
  };

  /** Disconnect from stomp channel
   *
   *  @return  Reference to self
   */
  this.reset = function()  {
    if ( this.isConnected ) {
      this.service.disconnect();
      this.service.reset();
      this.isConnected = false;
    }
    return this;
  };

  this.reconnect = function() {
    this.reset();
    this.start();
    return this;
  };

  /** Pre-Subscribe to data items
   *  @param item      stomp topic to subscribe to. Must be an object with a "name" property.
   *
   *  @return  Reference to self
   */
  this.subscribeItem = function(item)  {
    return this.subscribe(item.name,item);
  };


  /** Pre-Subscribe to data items
   *  @param item      Name of stomp topic to subscribe to
   *  @param callback  Object implementing "set" method when new data is received.
   *
   *  @return  Reference to self
   */
  this.subscribe = function(item,callback)  {
    var len = this.calls.length;

    this.calls.length = this.calls.length+1;
    this.calls[len] = item;

    if ( !this.items.hasOwnProperty(item) )
      this.items[item] = new Array(callback);
    else
      this.items[item].push(callback);
    this.logger.debug('DataProvider: Subscribed to data item:'+item+'   '+stomp);
    if ( _dataProvider.isConnected )   {
      var svc = this.calls[len];
      var msg = 'SUBSCRIBE:'+svc;
      if ( svc.substring(0,7)!='/topic/' ) svc = '/topic/'+svc;
      this.logger.info('Subscribe STOMP service:'+svc);
      this.service.subscribe(svc,this.subscribeOpts);
      this.service.send(msg,this.topic,{exchange:''});
    }
    return this;
  };

  /** Unubscribe to data items
   *  @param item      Name of stomp topic to subscribe to
   *
   *  @return  Reference to self
   */
  this.unsubscribe = function(item)  {
    if ( this.isConnected ) {
      this.service.unsubscribe(this.command,{exchange:''});
      for(var i=0; i<this.calls.length;++i) {
	if ( this.calls[i] == item ) {
	  this.service.unsubscribe(item,{exchange:''});
	  delete this.calls[i];
	  this.calls.length = this.calls.length-1;
	  return this;
	}
      }
    }
    else {
      this.unsubscribeAll();
    }
    return null;
  };

  /** Disconnect from all item topics
   *
   *  @return  Reference to self
   */
  this.unsubscribeAll = function()  {
    this.logger.info("Disconnect all pending data services ..");
    this.service.unsubscribe(this.command,{exchange:''});
    if ( this.isConnected ) {
      for(var i=0; i<this.calls.length;++i) {
	var item = this.calls[i];
	this.service.unsubscribe(item,{exchange:''});
      }
    }
    this.calls = new Array();
    this.calls.length = 0;
    this.items = new Object();
    return this;
  };
  /** Update all data items by requesting a "SUBSCRIBE:<item> call to the server
   *
   *  @return  Reference to self
   */
  this.update = function() {
    for (var i=0; i < this.calls.length; ++i)  {
      var msg = 'SUBSCRIBE:'+this.calls[i];
      this.service.send(msg,this.topic,{exchange:''});
      this.logger.verbose('DataProvider: Connect data item:'+msg);
    }
    return this;
  };

  /// Connect to item topics and force first update
  this.connect = function()  {
    this.logger.info("Connecting all pending data leaves to services ..");
    this.service.subscribe(this.command,{exchange:''});
    for (var i=0; i < this.calls.length; ++i) {
      var svc = this.calls[i];
      if ( svc.substring(0,7)!='/topic/' ) svc = '/topic/'+svc;
      this.logger.info('Subscribe STOMP service:'+svc);
      this.service.subscribe(svc,this.subscribeOpts);
    }
    this.update();
  };

  /// Disconnect to item topics and force first update
  this.disconnect = function()  {
    this.unsubscribeAll();
    this.service.disconnect();
    this.service.reset();
  };

  //this.start();
  /// Since long running documents have memory leaks, we reload the page
  /// regularly to avoid them.
  this.reloadHandler = function() {
    window.location.reload();
  };
  setTimeout(_dataProvider.reloadHandler,3600000);
  return this;
};

var dataProviderReset = function() {
  if ( null != _dataProvider ) {
    _dataProvider.needConnection = false;
    _dataProvider.unsubscribeAll();
    _dataProvider.isConnected = true;
    _dataProvider.reset();
  }
};

_lhcb().setup = function(show_log) {
  var body = document.getElementsByTagName('body')[0];
  lhcb.data = new Object();
  lhcb.logWindow = document.createElement('div');
  lhcb.logWindow.id = 'LHCb_LogWindow_std';
  body.appendChild(lhcb.logWindow);
  body.onunload = function() { dataProviderReset(); };

  lhcb.data.logger   = new OutputLogger(lhcb.logWindow, -1, LOG_INFO, 'RunStatusLogger');
  lhcb.data.provider = new DataProvider(lhcb.data.logger);
  lhcb.data.provider.topic = '/topic/farm';
  lhcb.data.stomp    = new Object();
  lhcb.data.stomp.scanDocument = function() {
    var elts = document.getElementsByTagName('STOMP');
    var items = new Array();
    var provider = lhcb.data.provider;
    items.length = elts.length;
    for (var i=0; i<elts.length;++i) {
      var e = elts[i];
      var item = e.getAttribute('data');
      var fmt  = e.getAttribute('format');
      items[i] = ElementItem(provider,item,fmt,e.parentNode);
    }
    lhcb.data.items = items;
    elts = document.getElementsByTagName('DIM');
    var raw_items = new Array();
    raw_items.length = elts.length;
    for (var i=0; i<elts.length;++i) {
      var e = elts[i];
      var item = e.getAttribute('data');
      var fmt  = e.getAttribute('format');
      raw_items[i] = RawItem(provider,item,fmt,e.parentNode);
    }
    lhcb.data.raw_items = raw_items;
    provider.start();
  };
  return lhcb;
};

if ( _debugLoading ) alert('Script lhcb.display.data.cpp loaded successfully');
