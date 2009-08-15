
var _DisplayDebug = 0;

function detector_run_header(part)  
{ return '<DIV class="StatusText">'+lhcb_online_picture()+' '+part+' Run Status</DIV>';  }

function detector_header(part)  
{ return '<DIV class="StatusText">'+lhcb_online_picture()+' '+part+' FSM Status</DIV>';  }

function lhcb_online_picture() 
{ return '<IMG SRC="http://lhcb-online.web.cern.ch/lhcb-online/elog/images/lhcb-online-logo.PNG" HEIGHT="50"></IMG>';   }

/** @class DisplayItem
 *  
 *  Note:
 *  "element" data member must be set externally, once the HTML is built!
 *
 *  @author  M.Frank
 *  @version 1.0
 */
DisplayItem = function(item_name)  {
  this.name = item_name;
  this.element = null;

  /** Standard callback for data provider for updates
   *  @param data   Data value as string
   *
   *  @return On success reference to self, null otherwise
   */
  this.set = function(data)  {
    if ( _DisplayDebug > 2 ) alert('DisplayItem.set('+this.name+'): '+data);
    var v = data.split('#',3);
    if ( v[0]==1 ) return this.display(v[1]);       // integer
    else if ( v[0]==2 ) return this.display(v[1]);  // float
    else if ( v[0]==3 ) return this.display(v[1]);  // simple text
    return this.display(v[1]);
  }

  /** Display rendering function for received data
   *
   *  @param      data    Display data
   *
   *  @return On success reference to self, null otherwise
   */
  this.display = function(data)  {
    this.element.innerHTML = data;
    return this;
  }
  return this;
}

/** @class StyledItem
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
StyledItem = function(item_name, class_name)  {
  this.className = class_name;
  this.inheritsFrom = DisplayItem;
  this.inheritsFrom(item_name);

  /** Standard callback for data provider for updates
   *  @param data   Data value as string
   *
   *  @return On success reference to self, null otherwise
   */
  this.set = function(data)   {
    return this.display(data);
  }

  /** Display rendering function for received data
   *
   *  @param      data    Display data
   *
   *  @return On success reference to self, null otherwise
   */
  this.display = function(data)  {
    var item_data = data[1];
    this.element.innerHTML = item_data;
    this.element.setAttribute('className',this.className);
    if ( _DisplayDebug>1 ) alert('StyledItem.display:'+item_data);
    return this;
  }
  return this;
}

/** @class FSMItem
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
FSMItem = function(item_name, logger, is_child)  {
  this.inheritsFrom = DisplayItem;
  this.inheritsFrom(item_name+'.FSM.state');
  this.sysname = item_name;
  this.lock = null;
  this._label = null;
  this._logger = logger;
  this._is_child = is_child;

  this._logger.debug('FSMItem.init:'+item_name+' -> '+this.name);


  this.setState = function(name) {
    this.lock.innerHTML = '<IMG SRC="'+_fileBase+'/Icons/Modes/'+name+'.bmp">';
    return this;
  }

  /** Subscribe data access to FSM items
   *
   *  @return On success reference to self, null otherwise
   */
  this.subscribe = function(provider) {
    provider.subscribe('lbWeb.'+this.name,this);
    this.label.setAttribute('className','FSMLabel');
    this.lock.setAttribute('className','FwLock');
    return this;
  }

  /** Standard callback for data provider for updates
   *  @param data   Data value as string
   *
   *  @return On success reference to self, null otherwise
   */
  this.set = function(data)  {
    if ( data.length == 2 )   {
      // alert(this.name+':  '+data);
      return this.display(data[1]);
    }
    this._logger.error('FSMItem['+this.name+'].set: Invalid data:'+data);
    return null;
  }

  /** Display rendering function for received data
   *
   *  @param      data    Display data
   *
   *  @return On success reference to self, null otherwise
   */
  this.display = function(data)  {
    var v = data.split('/',10);
    // State name:  v[0]
    // Meta state:  v[1]  (DEAD,...)
    // In/Excluded & Style: v[2]
    //
    if ( !this._is_child ) this.setState(v[2]);
    this.element.setAttribute('innerHTML',v[0]);
    this.element.setAttribute('className',v[1]);
    this._logger.debug('FSMItem['+this.name+'].display: Udate Done.');
    return this;
  }

  /** Set callback function if the detector item is clicked
   *
   *  @param      caller callback handler
   *  @param      call callback function
   *
   *  @return On success reference to self, null otherwise
   */
  this.setCallback = function(caller, call)   {
    this.label.onclick = call;
    this.label.callback_item = this;
    this.label.callback_handler = caller;
    return this;
  }
  return this;
}

/** @class FSMDisplay
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
FSMDisplay = function(par,provider,logger)  {
  this.items     = new Object();
  this.names     = null;
  this.root      = null;
  this.title     = null;

  this._body     = null;
  this._parent   = par;
  this._logger   = logger;
  this._provider = provider;

  this._logger.debug('FSMDisplay.init: Creating FSM display...parent:'+this._parent);

  /** Function to add new subsystem to the display object
   *
   *  @param name          FSM item name
   *  @return              Reference to self
   */
  this.add = function(name,fsm_obj)   {
    this.items[name] = fsm_obj;
    return this;
  }

  /** Remove single item from FSM display
   *
   *  @param name          FSM item name
   *  @return              Reference to self
   */
  this.remove = function(name)   {
    if ( this._body != null )  {
      var n = this.items[name];
      if ( n != null )  {
        var p = n.parentNode.parentNode;
        this._logger.verbose('FSMDisplay.remove:'+name+' '+p);
        this._body.removeChild(p);
      }
    }
    delete this.items[name];
    return this;
  }

  /** Add all sub-systems to the display
   *
   *  @param sub_system       Array of string containing sub-system names
   */
  this.setup = function(sub_systems)  {
    this.names = sub_systems;
    this._logger.debug('FSMDisplay.set:'+this.names);
    for (var i=0; i < this.names.length; ++i)  {
      var n = this.names[i];
      this.add(n,new FSMItem(n,this._logger,true));
    }
    return this;
  }

  /** Add single text cell to the display
   *
   *  @param row       Row object the cell should be added to
   *  @param name      Name of the item with dynamic information
   *  @param txt       Data content
   *
   *  @return Reference to the created cell.
   */
  this.addTextCell = function(row,name,txt,nspan)  {
    var c = document.createElement('td');
    c.innerHTML = txt;
    if ( nspan != null ) c.setAttribute('colSpan',nspan);
    row.appendChild(c);
    return c;
  }

  /** Build widget and display it as an HTML table
   *
   *  @return Reference to self
   */
  this.build = function()  {
    this._logger.debug('FSMDisplay.build:'+this.names);
    this.root = document.createElement('table');
    this.root.setAttribute('className','FSMTable');
    this._body  = document.createElement('tbody');
    if ( this.title != null ) {
      var tr = document.createElement('tr');
      var td = document.createElement('td');
      td.setAttribute('colSpan',ncols);
      td.innerHTML = '<div class="StatusText">Subdetectors:</div>';
      row.appendChild(td);
      this._body.appendChild(tr);
    }

    for (var i=0; i < this.names.length; ++i)  {
      var n = this.names[i];
      var itm = this.items[n];
      var row = document.createElement('tr');
      this._logger.debug('FSMDisplay.build: detector:'+n+' id:'+n+'.Status');
      itm.label = this.addTextCell(row, n+'.Name',     n, 1);
      itm.element = this.addTextCell(row, n+'.FSM.state','Unknown', 1);
      itm.element.textAlign = 'center';
      itm.lock = this.addTextCell(row, n+'.Lock',     '', 1);
      this._body.appendChild(row);
    }
    this.root.appendChild(this._body);
    this.root.cellSpacing = 3;
    this._parent.appendChild(this.root);
    // Once the table is ready, we can subscribe to the data items
    for (var i=0; i < this.names.length; ++i)  {
      this.items[this.names[i]].subscribe(this._provider);
    }
    this._logger.debug('FSMDisplay.build: ... done ...');
    return this;
  }

  /** Build widget and display it as an HTML table
   *
   *  @return Reference to self
   */
  this.buildHorizontal = function(ncols)  {
    var item = new Object();
    var count = 0;
    var name_row = null;
    var state_row = null;
    var empty_row = null;

    this._logger.debug('FSMDisplay.build:'+this.names);
    this.root = document.createElement('table');
    this.root.setAttribute('className','FSMTable');
    this._body  = document.createElement('tbody');

    if ( this.title != null ) {
      var tr = document.createElement('tr');
      var td = document.createElement('td');
      td.setAttribute('colSpan',ncols);
      td.innerHTML = '<DIV class="StatusText">Subdetectors:</DIV>';
      row.appendChild(td);
      this._body.appendChild(tr);
    }

    for (var i=0; i < this.names.length; ++i)  {
      var td, c;
      var n = this.names[i];
      var itm = this.items[n];
      if ( name_row == null ) name_row = document.createElement('tr');
      if ( state_row == null ) state_row = document.createElement('tr');
      if ( empty_row == null ) empty_row = document.createElement('tr');
      this._logger.debug('FSMDisplay.build: detector:'+n+' id:'+n+'.Status');
      itm.lock = this.addTextCell(name_row, n+'.Lock',     '', 1);
      itm.label = this.addTextCell(name_row, n+'.Name',     n, 1);
      itm.label.setAttribute('className','FSMLabel');
      itm.element = this.addTextCell(state_row, n+'.FSM.state','Unknown', 2);
      itm.element.textAlign = 'center';
      c = document.createElement('td');
      c.innerHTML = '&nbsp;';
      name_row.appendChild(c);

      c = document.createElement('td');
      c.innerHTML = '&nbsp;';
      state_row.appendChild(c);

      c = document.createElement('td');
      c.innerHTML = '&nbsp;';
      empty_row.appendChild(c);

      count = count + 1;
      if ( (count%ncols) == 0 ) {
	this._body.appendChild(name_row);
	this._body.appendChild(state_row);
	if ( count < this.names.length ) {
	  var td = document.createElement('td');
	  td.setAttribute('colSpan',3*ncols);
	  td.innerHTML='&nbsp';
	  empty_row.appendChild(td);
	  this._body.appendChild(empty_row);
	}
	name_row  = null;
	state_row = null;
	empty_row = null;
      }
    }
    if ( name_row  != null ) this._body.appendChild(name_row);
    if ( state_row != null ) this._body.appendChild(state_row);

    this.root.appendChild(this._body);
    this.root.cellSpacing = 3;
    this._parent.appendChild(this.root);
    // Once the table is ready, we can subscribe to the data items
    for (var i=0; i < this.names.length; ++i)
      this.items[this.names[i]].subscribe(this._provider);
    this._logger.debug('FSMDisplay.build: ... done ...');
    return this;
  }

  /** Clear display and remove widget
   *
   *  @return Reference to self
   */
  this.clear = function()  {
    this._logger.debug('FSMDisplay.clear:Table:'+this.root+' Parent:'+this._parent);
    if ( this.root != null ) {
      this._parent.removeChild(this.root);
    }
    this.items = new Object();
    this.names = null;
    return this;
  }
}

/** @class PropertyTable
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
PropertyTable = function(parent, data_provider, logger, num_cols, desc_class, value_class) {
  this._parent      = parent;
  this._body        = null;
  this._logger      = logger;

  this.cols        = num_cols;
  this.root        = null;
  this.items       = new Array();
  this.property    = new Object();
  this.desc_class  = desc_class;
  this.value_class = value_class;
  this.data_provider = data_provider;

  this._logger.debug('PropertyTable.init: Creating property table...');

  /** Add new data item to property table
   *
   *  @param name        Name of the item with dynamic information
   *  @param description Descriptive text of the item
   */
  this.add = function(name, description, span)   {
    var nam = name;
    var obj = new StyledItem(nam,this.value_class);
    obj['span'] = span;
    obj['description'] = description;
    this.property[name] = obj;
    this.items.push(obj);
    this._logger.debug('PropertyTable.add: added '+nam+' ['+description+'] '+this.items.length);
    return this;
  }

  /** Add single text cell to the display
   *
   *  @param row       Row object the cell should be added to
   *  @param name      Name of the item with dynamic information
   *  @param txt       Data content
   *  @param nspan     Spanning for multi column cells
   *  @param className Class name for cell item
   *
   *  @return Reference to the created cell.
   */
  this.addTextCell = function(row,name,txt,nspan,className)  {
    var c = document.createElement('td');
    var t = document.createTextNode(txt);
    if ( nspan       != null ) c.setAttribute('colSpan',nspan);
    if ( className   != null ) c.setAttribute('className',className);
    c.appendChild(t);
    row.appendChild(c);
    return c;
  }

  /** Build widget and display it as an HTML table
   *
   *  @return Reference to self
   */
  this.build = function() {
    var c, ncols = 0;
    var desc    = document.createElement('tr');
    var data    = document.createElement('tr');
    var fill    = document.createElement('tr');
    this.root  = document.createElement('table');
    this._body   = document.createElement('tbody');

    this._logger.debug('PropertyTable.build: start to build table with '+this.items.length+' items');
    for (var i=0; i < this.items.length; ++i)  {
      var n = this.items[i];
      if ( ncols>=this.cols ) {
        this._logger.debug('PropertyTable.build: new row');
        this._body.appendChild(desc);
        this._body.appendChild(data);
        this._body.appendChild(fill);
        desc = document.createElement('tr');
        data = document.createElement('tr');
        fill = document.createElement('tr');
        ncols = 0;
      }
      nspan = n['span'];
      ncols = ncols + nspan;
      this.addTextCell(desc, null,   n['description'],nspan, this.desc_class);
      n.element = 
	this.addTextCell(data, n.name, '---',           nspan, this.value_class);
      this.addTextCell(fill, n.name, '',              nspan, 'PropertyTableSpacer');
      this._logger.verbose('PropertyTable.build: '+n.name+'  '+n['description']+'  Span:'+nspan);
    }
    this._body.appendChild(desc);
    this._body.appendChild(data);
    this._body.appendChild(fill);
    this.root.appendChild(this._body);
    this.root.setAttribute('width','95%');
    this.root.setAttribute('className','PropertyTable');
    this._parent.appendChild(this.root);

    for (var i=0; i < this.items.length; ++i)  {
      var n = this.items[i];
      this.data_provider.subscribe(n.name,n);
    }

    this._logger.debug('PropertyTable.build: table finished');
    return this;
  }
  this._logger.debug('PropertyTable.init: Creating property table...Done.');
}

/** @class ControlsStatusDisplay
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
ControlsStatusDisplay = function(parent,partition,provider,logger) {
  // private variables
  this._provider = provider;
  this._parent = parent;
  this._logger = logger;
  this._body = null;

  // public stuff
  this.partition = partition;
  this.root = null;
  this.fsm = new FSMItem(this.partition,this._logger,false);

  /** Build widget and display it as an HTML table
   *
   *  @return Reference to self
   */
  this.build = function() {
    this.root   = document.createElement('table');
    this._body  = document.createElement('tbody');
    var row     = document.createElement('tr');;

    this.root.width = '95%';
    this.root.className = 'FSMTable';

    this._logger.debug('ControlsStatusDisplay: Building.');
    this.fsm.label = document.createElement('td');
    this.fsm.label.innerHTML = this.partition;
    row.appendChild(this.fsm.label);

    this.fsm.element = document.createElement('td');
    this.fsm.element.innerHTML = 'Unknown';
    row.appendChild(this.fsm.element);

    this.fsm.lock = document.createElement('td');
    this.fsm.lock.innerHTML = '';
    row.appendChild(this.fsm.lock);

    this._body.appendChild(row);
    this.root.appendChild(this._body);
    this._parent.appendChild(this.root);
    return this;
  }

  /// Function to subscribe to the required data items
  this.subscribe = function() {
    this.fsm.subscribe(this._provider);
  }

  /** Set callback function if the detector item is clicked
   *
   *  @param      caller callback handler
   *  @param      call callback function
   *
   *  @return On success reference to self, null otherwise
   */
  this.setCallback = function(caller, call)   {
    this.fsm.setCallback(caller,call);
  }

  this._logger.debug('ControlsStatusDisplay: Constructor OK.');
  return this;
}

/** @class RunStatusDisplay
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
RunStatusDisplay = function(parent,partition,provider,logger) {
  this.experiment_run_header = null;
  this.ctrl_status = null;
  this.run_properties = null;
  this.fsm_items = null;
  this.det_items = null;
  this.trg_items = null;
  this.root = null;
  this.detDisplay = null;
  this.trgDisplay = null;
  this.det_row = null;
  this.det_label = null;
  this.trg_row = null;
  this.trg_label = null;
  this.statusDisplay = null;


  this._partition = partition;
  this._provider = provider;
  this._parent = parent;
  this._logger = logger;
  this._body = null;

  this._logger.debug('RunStatusDisplay.init: Creating display...parent:'+this._parent+' Partition:'+this._partition);

  /** Build widget and display it as an HTML table
   *
   *  @return Reference to self
   */
  this.build = function() {
    this.root   = document.createElement('TABLE');
    this._body  = document.createElement('TBODY');
    var row     = null;
    var td      = null;

    this._logger.debug('RunStatusDisplay.build: Creating table...');
    this.root.className = 'RunStatusPanel';

    row = document.createElement('tr');
    this.experiment_run_header = document.createElement('td');
    row.appendChild(this.experiment_run_header);
    row.appendChild(document.createElement('td'));
    this._body.appendChild(row);

    row = document.createElement('tr');
    this.ctrl_status = document.createElement('td');
    row.appendChild(this.ctrl_status);
    row.appendChild(document.createElement('td'));
    this._body.appendChild(row);

    row = document.createElement('tr');
    this.fsm_items = document.createElement('td');
    row.appendChild(this.fsm_items);

    this.run_properties = document.createElement('td');
    row.appendChild(this.run_properties);
    this._body.appendChild(row);
    
    this.det_row = document.createElement('tr');
    this.det_items = document.createElement('td');
    this.det_items.colSpan = 2;
    this.det_row.appendChild(this.det_items);
    this._body.appendChild(this.det_row);

    this.trg_row = document.createElement('tr');
    this.trg_items = document.createElement('td');
    this.trg_items.colSpan = 2;
    this.trg_row.appendChild(this.trg_items);
    this._body.appendChild(this.trg_row);

    this.root.appendChild(this._body);
    this._parent.appendChild(this.root);

    this._logger.info('RunStatusDisplay.build: Created table...Done.');
    return this;
  }

  /** Attach display items. 
   * Note: Display must be "built" before. Otherwise the service updates do not find the
   * necessary html identifiers in in the documents,
   *
   *  @return Reference to self
   */
  this.attach = function(systems)   {
    this.experiment_run_header.innerHTML = detector_run_header(this._partition);
    this.statusDisplay = new ControlsStatusDisplay(this.ctrl_status,this._partition,this._provider,this._logger);
    this.statusDisplay.build();

    this.runPropertyDisplay = new PropertyTable(this.run_properties, 
						this._provider, 
						this._logger, 
						3,
						'PropertyTableItem',
						'PropertyTableValue');
    var prop = this.runPropertyDisplay;
    var prefix = 'lbWeb.'+this._partition+'_RunInfo.';
    prop.add(prefix+'general.runNumber',    'Run Number',1);
    prop.add(prefix+'general.runType',      'Run Type',2);
    prop.add(prefix+'TrgConf',              'Trigger Configuration',1);
    prop.add(prefix+'L0Gap',                'L0 Gap',1);
    prop.add(prefix+'TAE',                  'TAE Half window',1);
    prop.add(prefix+'general.runStartTime', 'Run Start Time',1);
    prop.add(prefix+'general.runStopTime',  'Run Duration',2);
    prop.add(prefix+'TFC.nTriggers',        'Number of Events',1);
    prop.add(prefix+'TFC.deadTime',         'Dead Time',2);
    prop.add(prefix+'TFC.runTriggerRate',   'Trigger Rate',1);
    prop.add(prefix+'TFC.nTriggersU',       'nTriggersU',2);
    prop.build();

    this.statusDisplay.subscribe();

    this.fsmDisplay = new FSMDisplay(this.fsm_items,this._provider,this._logger);
    this.fsmDisplay.setup(systems).build();

    return this;
  }

  this.attachDetectors = function(systems) {
    if ( this.det_label == null ) {
      var row = document.createElement('tr');
      this.det_label = document.createElement('td');
      this.det_items.setAttribute('colSpan','2');
      this.det_label.className = 'StatusText';
      row.appendChild(this.det_label);
      this._body.insertBefore(row,this.det_row);
    }
    if ( this.detDisplay != null ) {      
      this.det_items.removeChild(this.detDisplay.root);
    }
    this.detDisplay = new FSMDisplay(this.det_items,this._provider,this._logger);
    this.detDisplay.setup(systems);
    this.detDisplay.buildHorizontal(6);
    return this;
  } 

  this.attachTriggers = function(systems) {
    if ( this.trg_label == null ) {
      var row = document.createElement('tr');
      this.trg_label = document.createElement('td');
      this.trg_items.setAttribute('colSpan','2');
      this.trg_label.innerHTML = '<div class="StatusText">Triggers:</div>';
      row.appendChild(this.trg_label);
      this._body.insertBefore(row,this.trg_row);
    }
    if ( this.trgDisplay != null ) {
      this.trg_items.removeChild(this.trgDisplay.root);
    }
    this.trgDisplay = new FSMDisplay(this.trg_items,this._provider,this._logger);
    this.trgDisplay.setup(systems);
    this.trgDisplay.buildHorizontal(6);
    return this;
  }

  this._logger.info('RunStatusDisplay.init: Creating display...Done.');
}

/** @class FSMStatusDisplay
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
FSMStatusDisplay = function(parent,partition,provider,logger) {
  this.experiment_run_header = null;
  this.ctrl_status = null;
  this.fsm_items = null;
  this.root = null;
  this.detData = null;
  this.statusDisplay = null;


  this._partition = partition;
  this._provider = provider;
  this._parent = parent;
  this._logger = logger;
  this._body = null;

  this._logger.debug('FSMStatusDisplay.init: Creating display...parent:'+this._parent+' Partition:'+this._partition);

  /** Build widget and display it as an HTML table
   *
   *  @return Reference to self
   */
  this.build = function() {
    this.root   = document.createElement('TABLE');
    this._body   = document.createElement('TBODY');
    var row     = null;
    var td      = null;

    this._logger.debug('FSMStatusDisplay.build: Creating table...');
    this.root.width = '100%';
    this.root.className = 'FSMStatusPanel';

    row = document.createElement('tr');
    this.experiment_run_header = document.createElement('td');
    row.appendChild(this.experiment_run_header);
    row.appendChild(document.createElement('td'));
    this._body.appendChild(row);

    row = document.createElement('tr');
    this.ctrl_status = document.createElement('td');
    row.appendChild(this.ctrl_status);
    row.appendChild(document.createElement('td'));
    this._body.appendChild(row);

    row = document.createElement('tr');
    this.fsm_items = document.createElement('td');
    //this.fsm_items.setAttribute('width','100%');
    row.appendChild(this.fsm_items);
    this._body.appendChild(row);

    this.root.appendChild(this._body);
    this._parent.appendChild(this.root);

    this._logger.info('FSMStatusDisplay.build: Created table...Done.');
    return this;
  }

  /** Attach display items. 
   * Note: Display must be "built" before. Otherwise the service updates do not find the
   * necessary html identifiers in in the documents,
   *
   *  @return Reference to self
   */
  this.attach = function(systems)   {
    this.experiment_run_header.innerHTML = detector_header(this._partition);
    this.statusDisplay = new ControlsStatusDisplay(this.ctrl_status,this._partition,this._provider,this._logger);
    this.statusDisplay.build();
    this.statusDisplay.subscribe();
    this.fsmDisplay = new FSMDisplay(this.fsm_items,this._provider,this._logger);
    this.fsmDisplay.setup(systems);
    this.fsmDisplay.build();
    return this;
  }

  this._logger.info('FSMStatusDisplay.init: Creating display...Done.');
}

if ( _debugLoading ) alert('Script lhcb.display.items.cpp loaded successfully');

