
var _DisplayDebug = 0;

function detector_run_header(part)  
{ return '<DIV class="StatusText">'+part+' Run Status</DIV>';  };
//{ return '<DIV class="StatusText">'+lhcb_online_picture()+' '+part+' Run Status</DIV>';  };

function detector_header(part)  
{ return '<DIV class="StatusText">'+lhcb_online_picture()+' '+part+' FSM Status</DIV>';  };

function lhcb_logo(url)
{ return '<IMG SRC="'+url+'" HEIGHT="50"></IMG>';   };
function lhcb_online_picture() 
{ return lhcb_logo("/Online/Images/lhcb-online-logo.png");   };

function setWindowTitle(title) {
  try {
    top.document.title = title;
  }
  catch(error) {
  }
};

var Spacer = function() {
  var td = document.createElement('td');
  td.innerHTML = '&nbsp;';
  td.className = 'FSMSpacer';
  return td;
};

/** Add single text cell to the display
 *
 *  @param text      Data content
 *  @param nspan     Spanning for multi column cells
 *  @param className Class name for cell item
 *
 *  @return Reference to the created cell.
 */
var Cell = function(text,nspan,className) {
  var c = document.createElement('td');
  //var t = document.createTextNode(text);
  if ( nspan       != null ) c.colSpan = nspan;
  if ( className   != null ) c.className = className;
  c.innerHTML = text;
  //c.appendChild(t);
  return c;
};

var StatusText = function(text,ncols) {
  var td = document.createElement('td');
  td.colSpan = ncols;
  td.innerHTML = '<div class="StatusText">'+text+'</div>';
  return td;
};

var StatusRow = function(text,ncols,body) {
  if ( text ) {
    var tr = document.createElement('tr');
    tr.appendChild(StatusText(text,ncols));
    body.appendChild(tr);
    return tr;
  }
  return null;
};

/** @class DisplayItem
 *  
 *  Note:
 *  "element" data member must be set externally, once the HTML is built!
 *
 *  @author  M.Frank
 *  @version 1.0
 */
var DisplayItem = function(item_name)    {
  this.name    = item_name;
  this.element = null;

  /** Standard callback for data provider for updates
   *  @param data   Data value as string
   *
   *  @return On success reference to self, null otherwise
   */
  this.set = function(data)  {
    if ( _DisplayDebug > 2 ) alert('DisplayItem.set('+this.name+'): '+data);
    var v = data.split('#',3);
    return this.display(v);
    if ( v[0]==1 ) return this.display(v[1]);       // integer
    else if ( v[0]==2 ) return this.display(v[1]);  // float
    else if ( v[0]==3 ) return this.display(v[1]);  // simple text
    return this.display(v[1]);
  };

  /** Display rendering function for received data
   *
   *  @param      data    Display data
   *
   *  @return On success reference to self, null otherwise
   */
  this.display = function(data)  {
    this.element.innerHTML = data;
    return this;
  };
  return this;
};

/** @class StyledItem
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
var StyledItem = function(item_name, class_name, fmt)  {
  element = document.createElement('td');
  element.name   = item_name;
  if ( fmt )  {
    element.format = fmt;
  }
  if ( class_name ) {
    element.className = class_name;
  }

  /** Standard callback for data provider for updates
   *  @param data   Data value as string
   *
   *  @return On success reference to self, null otherwise
   */
  element.set = function(data) { return this.display(data);  };

  /** Display rendering function for received data
   *
   *  @param      data    Display data
   *
   *  @return On success reference to self, null otherwise
   */
  element.display = function(data)  {
    var fmt = this.format;
    var item_value, item_data;
    if ( fmt != null ) {
      if ( data[0] == 21 )        // Integer
	item_data = parseInt(data[1]);
      else if ( data[0] == 22 )   // Float
	item_data = parseFloat(data[1]);
      else if ( data[0] == 23 )   // Boolean: handle like string
	item_data = data[1];
      else if ( data[0] == 25 )   // String
	item_data = data[1];
      else
	item_data = data[1];
    }
    else {
      item_data = data[1];
    }
    if ( this.conversion != null )   {
      this.conversion.target = this;
      item_data = this.conversion(item_data);
    }
    if ( fmt != null ) {
      item_data = sprintf(fmt,item_data);
    }
    if ( ''+item_data == 'undefined' ) {
      item_data = '&lt;undefined&gt;';
    }

    this.innerHTML = item_data;
    this.className = this.className;
    if ( _DisplayDebug>1 ) alert('StyledItem.display:'+item_data);
    return this;
  };
  /** Subscribe item to receive data from data provider object
   *
   *  @param      provider  Data provider object
   *
   *  @return On success reference to self, null otherwise
   */
  element.subscribe = function(provider) {
    provider.subscribe(this.name,this);
    return this;
  };
  return element;
};

/** @class FSMItem
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
var FSMItem = function(item_name, logger, is_child)  {
  element = document.createElement('td');
  element.lock = document.createElement('td');
  element.label = document.createElement('td');

  element.name       = item_name+'.FSM.state';
  element.sysname    = item_name;
  element.childState = 'FwDEAD';
  element.data       = null;

  element._logger    = logger;
  element._is_child  = is_child;

  element.innerHTML = 'Unknown';
  element.textAlign = 'center';
  tooltips.set(element,'FSM state');

  element.lock.colSpan = 1;
  element.lock.innerHTML = '';
  element.lock.className = 'FwLock';

  element.label.colSpan = 1;
  element.label.innerHTML = element.sysname;
  element.label.className = 'FSMLabel';
  tooltips.set(element.label,'Click here to further explore the FSM object '+element.sysname);

  element._logger.debug('FSMItem.init:'+item_name+' -> '+element.name);

  element.setChildState = function(name) {
    if ( !(name == 'INCLUDED' || name == 'ENABLED') ) {
      this._is_child = false;
      this.childState = name;
    }
    else {
      this._is_child = true;
      this.childState = name;
    }
    this.setState(name);
  };

  element.setState = function(name) {
    this.lock.innerHTML = '<IMG SRC="'+_fileBase+'/Images/Modes/'+name+'.gif" ALT=""></IMG>';
    return this;
  };

  /** Subscribe data access to FSM items
   *
   *  @return On success reference to self, null otherwise
   */
  element.subscribe = function(provider) {
    this._logger.debug('FSMItem.init:'+this.name+' -> subscribe');
    provider.subscribe('lbWeb.'+this.name,this);
    return this;
  };

  /** Standard callback for data provider for updates
   *  @param data   Data value as string
   *
   *  @return On success reference to self, null otherwise
   */
  element.set = function(data)  {
    this._logger.debug('FSMItem.set:'+this.name+' -> data:'+data);
    if ( data != null ) {
      if ( data.length == 2 )   {
        if ( this.data && this.data[1] == data[1] ) return this;
        this.data = data;
	return this.display(data[1]);
      }
    }
    this._logger.error('FSMItem['+this.name+'].set: Invalid data:'+data);
    return null;
  };

  /** Display rendering function for received data
   *
   *  @param      data    Display data
   *
   *  @return On success reference to self, null otherwise
   */
  element.display = function(data)  {
    var v = data.split('/',10);
    // State name:  v[0]
    // Meta state:  v[1]  (DEAD,...)
    // In/Excluded & Style: v[2]
    //
    if ( !this._is_child )   {
      this.setState(this.childState);
      this.className = 'FwDEAD';
    }
    else {
      this.setState(v[3]);
      this.className = v[2];
      if ( this.conversion != null )   {
	this.conversion.target = this;
	this.conversion(v[0],v[1],v[2]);
      }
    }
    this.label.innerHTML = v[0];
    if ( this.format != null )
      this.innerHTML = sprintf(this.format,v[1]);
    else
      this.innerHTML = v[1];
    this._logger.debug('FSMItem['+this.name+'].display: '+data+' '+this.childState+' update Done.');
    return this;
  };

  /** Set callback function if the detector item is clicked
   *
   *  @param      caller callback handler
   *  @param      call callback function
   *
   *  @return On success reference to self, null otherwise
   */
  element.setCallback = function(caller, call)   {
    this.label.onclick = call;
    this.label.item = this;
    this.label.handler = caller;
    return this;
  };
  return element;
};

/** @class FSMDisplay
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
var FSMDisplay = function(provider,logger,title)  {
  table           = document.createElement('table');
  table.body      = document.createElement('tbody');

  table._logger   = logger;
  table._provider = provider;

  table.items     = new Object();
  table.names     = null;

  table.className = 'FSMTable';
  table.body.className = 'FSMTable';
  table.body.cellSpacing = 3;
  table.appendChild(table.body);

  if ( title ) table.title = title;
  table._logger.debug('FSMDisplay.init: Creating FSM display...');

  /** Function to add new subsystem to the display object
   *
   *  @param name          FSM item name
   *  @return              Reference to self
   */
  table.add = function(name,fsm_obj)   {
    this.items[name] = fsm_obj;
    return this;
  };

  /** Remove single item from FSM display
   *
   *  @param name          FSM item name
   *  @return              Reference to self
   */
  table.remove = function(name)   {
    var n = this.items[name];
    if ( n )  {
      var p = n.parentNode;
      this._logger.verbose('FSMDisplay.remove:'+name+' '+p);
      this.body.removeChild(p);
    }
    delete this.items[name];
    return this;
  };

  /** Add all sub-systems to the display
   *
   *  @param sub_system       Array of string containing sub-system names
   */
  table.setup = function(sub_systems)  {
    this.names = sub_systems;
    this._logger.info('FSMDisplay.set:'+this.names+' ['+this.names.length+']');
    for (var i=0; i < this.names.length; ++i)  {
      var n = this.names[i];
      this.items[n] = FSMItem(n,this._logger,true);
    }
    return this;
  };


  table.subscribe = function() {
    // Once the table is ready, we can subscribe to the data items
    for (var i=0; i < this.names.length; ++i)
      this.items[this.names[i]].subscribe(this._provider);
    return this;
  };

  /** Build widget and display it as an HTML table
   *
   *  @return Reference to self
   */
  table.build = function()  {
    if ( this.title ) StatusRow(this.names.length+' '+this.title,3,this.body);
    for (var i=0; i < this.names.length; ++i)  {
      var n = this.names[i];
      var itm = this.items[n];
      var row = document.createElement('tr');
      row.appendChild(itm.label);
      row.appendChild(itm);
      row.appendChild(itm.lock);
      this.body.appendChild(row);
    }
    return this.subscribe();
  };

  /** Build widget and display it as an HTML table
   *
   *  @return Reference to self
   */
  table.buildHorizontal = function(ncols)  {
    var count = 0;
    var name_row = null;
    var state_row = null;
    var empty_row = null;
    if ( this.title ) StatusRow(this.names.length+' '+this.title,ncols,this.body);
    for (var i=0; i < this.names.length; ++i)  {
      var td, c;
      var n = this.names[i];
      var itm = this.items[n];
      if ( name_row == null  ) name_row = document.createElement('tr');
      if ( state_row == null ) state_row = document.createElement('tr');
      if ( empty_row == null ) empty_row = document.createElement('tr');
      this._logger.debug('FSMDisplay.build: detector:'+n+' id:'+n+'.Status');

      itm.textAlign = 'center';
      itm.colSpan   = 2;
      itm.label.className = 'FSMLabelHorizontal';

      name_row.appendChild(itm.label);
      name_row.appendChild(itm.lock);
      name_row.appendChild(Spacer());
      state_row.appendChild(itm);
      state_row.appendChild(Spacer());
      empty_row.appendChild(Spacer());

      count = count + 1;
      if ( (count%ncols) == 0 ) {
	this.body.appendChild(name_row);
	this.body.appendChild(state_row);
	if ( count < this.names.length ) {
	  var td = Spacer();
	  td.setAttribute('colSpan',3*ncols);
	  empty_row.appendChild(td);
	  this.body.appendChild(empty_row);
	}
	name_row  = null;
	state_row = null;
	empty_row = null;
      }
    }
    if ( name_row  != null ) this.body.appendChild(name_row);
    if ( state_row != null ) this.body.appendChild(state_row);
    return this.subscribe();
  };

  /** Clear display and remove widget
   *
   *  @return Reference to self
   */
  table.clear = function()  {
    this._logger.debug('FSMDisplay.clear:Table:'+this+' Parent:'+this.parentNode);
    this.parentNode.removeChild(this);
    this.items = new Object();
    this.names = null;
    return this;
  };

  return table;
};

/** @class PropertyTable
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
var PropertyTable = function(data_provider, logger, num_cols, desc_class, value_class) {
  table = document.createElement('table');
  table.body = document.createElement('tbody');
  table._logger      = logger;

  table.cols        = num_cols;
  table.items       = new Array();
  table.desc_class  = desc_class;
  table.value_class = value_class;
  table.provider    = data_provider;

  table.className      = 'PropertyTable';
  table.body.className = 'PropertyTable';
  table.width = '100%';

  table._logger.debug('PropertyTable.init: Creating property table...');

  /** Add new data item to property table
   *
   *  @param name        Name of the item with dynamic information
   *  @param description Descriptive text of the item
   *  @param span        For multi-column entries: number of spaning columns
   *  @param fmt         Formatting string
   */
  table.addFormat = function(name, description, span, fmt)   {
    var obj         = StyledItem(name,this.value_class,fmt);
    obj.colSpan     = span;
    obj.spacer      = Cell('',span,'PropertyTableSpacer');
    obj.description = Cell(description,span,this.desc_class);
    tooltips.set(obj,'Current value of the property:"'+description+'"');
    tooltips.set(obj.description,'Property description');
    this.items.push(obj);
    this._logger.debug('PropertyTable.add: added '+name+' ['+description+'] '+
		       this.items.length);
    return obj;
  };

  table.add = function(name, description, span)   {
    return this.addFormat(name, description, span, null);
  };

  /** Subscribe data items to provider
   *
   *  @return Reference to self
   */
  table.subscribe = function() {
    for (var i=0; i < this.items.length; ++i)  {
      var n = this.items[i];
      this.provider.subscribe(n.name,n);
    }
    return this;
  };

  /** Build widget and display it as an HTML table
   *
   *  @return Reference to self
   */
  table.build = function() {
    var ncols = 0;
    var desc    = document.createElement('tr');
    var data    = document.createElement('tr');
    var fill    = document.createElement('tr');

    this._logger.debug('PropertyTable.build: start to build table with '+this.items.length+' items');
    for (var i=0, len=this.items.length; i < len; ++i)  {
      var n = this.items[i];
      if ( ncols>=this.cols ) {
        this._logger.debug('PropertyTable.build: new row');
        this.body.appendChild(desc);
        this.body.appendChild(data);
        this.body.appendChild(fill);
        desc = document.createElement('tr');
        data = document.createElement('tr');
        fill = document.createElement('tr');
        ncols = 0;
      }
      ncols = ncols + n.colSpan;
      fill.appendChild(n.spacer);
      desc.appendChild(n.description);
      data.appendChild(n);
    }
    this.body.appendChild(desc);
    this.body.appendChild(data);
    this.body.appendChild(fill);
    return this.subscribe();
  };

  table.build_horizontal = function() {
    this._logger.debug('PropertyTable.build: start to build table with '+this.items.length+' items');
    for (var i=0; i < this.items.length; ++i)  {
      var n = this.items[i];
      var row   = document.createElement('tr');
      this.body.appendChild(row);
      if ( n.colSpan > 1 )  {
	row.appendChild(n.description);
	n.description.colSpan = 2;
	row   = document.createElement('tr');
	this.body.appendChild(row);
	row.appendChild(n);
	n.colSpan = 2;
      }
      else {
	row.appendChild(n.description);
	row.appendChild(n);
      }
    }
    return this.subscribe();
  };

  table.appendChild(table.body);
  table._logger.debug('PropertyTable.init: Creating property table...Done.');
  return table;
};

/** @class ControlsStatusDisplay
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
var ControlsStatusDisplay = function(partition,provider,logger) {
  // public stuff
  table       = document.createElement('table');
  table.body  = document.createElement('tbody');

  // private variables
  table._provider = provider;
  table._logger = logger;

  table.width = '95%';
  table.partition = partition;
  table.className = 'FSMTable';
  table.fsm = FSMItem(table.partition,table._logger,true);
  table.fsm.__tag = 'ControlsStatusDisplay';

  table.row = document.createElement('tr');
  table.row.appendChild(table.fsm.label);
  table.row.appendChild(table.fsm);
  table.row.appendChild(table.fsm.lock); 
  table.body.appendChild(table.row);
  table.appendChild(table.body);

  /** Set callback function if the detector item is clicked
   *
   *  @param      caller callback handler
   *  @param      call callback function
   *
   *  @return On success reference to self, null otherwise
   */
  table.setCallback = function(caller, call)   {
    this.fsm.setCallback(caller,call);
  };

  table.fsm.subscribe(table._provider);
  table._logger.debug('ControlsStatusDisplay: Constructor OK.');
  return table;
};

/** @class RunStatusDisplay
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
var RunStatusDisplay = function(partition,provider,logger) {
  table = document.createElement('table');
  table.body = document.createElement('tbody');
  table.experiment_run_header = null;
  table.ctrl_status = null;
  table.run_properties = null;
  table.fsm_items = null;
  table.det_items = null;
  table.trg_items = null;
  table.detDisplay = null;
  table.trgDisplay = null;
  table.det_row = null;
  table.trg_row = null;
  table.statusDisplay = null;

  table._partition = partition;
  table._provider = provider;
  table._logger = logger;

  table._logger.debug('RunStatusDisplay.init: Creating display.. Partition:'+table._partition);

  table.body.className = 'RunStatusPanel';
  table.className = 'RunStatusPanel';
  table.appendChild(table.body);

  /// Build widget and display it as an HTML table
  var row     = null;
  var td      = null;
  
  table._logger.debug('RunStatusDisplay.build: Creating table...');

  row = document.createElement('tr');
  table.experiment_run_header = document.createElement('td');
  table.experiment_run_header.innerHTML = '';
  table.experiment_run_header.colSpan = 2;
  row.appendChild(table.experiment_run_header);
  //td = Cell(lhcb_online_picture(),1,null);
  //td = Cell('',1,null);
  //td.style.textAlign = 'right';
  //td.rowSpan = 1;
  //row.appendChild(td);
  table.body.appendChild(row);

  row = document.createElement('tr');
  table.ctrl_status = document.createElement('td');
  row.appendChild(table.ctrl_status);
  table.run_properties = document.createElement('td');
  table.run_properties.rowSpan = 2;
  row.appendChild(table.run_properties);  
  table.body.appendChild(row);
  
  row = document.createElement('tr');
  table.fsm_items = document.createElement('td');
  table.fsm_items.width = '40%';
  row.appendChild(table.fsm_items);
  table.body.appendChild(row);

  table.det_row = document.createElement('tr');
  table.det_items = Cell('',2,null);
  table.det_items.width = '100%';
  table.det_items.textAlign = 'left';
  table.det_row.appendChild(table.det_items);
  table.body.appendChild(table.det_row);

  table.trg_row = document.createElement('tr');
  table.trg_items = Cell('',2,null);
  table.trg_row.appendChild(table.trg_items);
  table.body.appendChild(table.trg_row);
  table._logger.info('RunStatusDisplay.build: Created table...Done.');

  table.createProperties = function(this_obj)  {
    this_obj.runPropertyDisplay = PropertyTable(this_obj._provider, 
					    this_obj._logger, 
					    2,
					    'PropertyTableItem',
					    'PropertyTableValue');
    var prop = this_obj.runPropertyDisplay;
    var prefix = 'lbWeb.'+this_obj._partition+'_RunInfo.';
    prop.add(prefix+'general.runNumber',          'Run number',1);
    prop.add(prefix+'general.runType',            'Run type',1);

    prop.add(prefix+'general.runStartTime',       'Run start time',1);
    prop.add(prefix+'general.runStopTime',        'Run duration',1);

    prop.add(prefix+'general.dataType',           'Data type',1);
    prop.add(prefix+'Storage.storeFlag',          'Data destination',1);

    prop.add(prefix+'TFC.nTriggers',              'TFC: Number of L&Oslash; events',1);
    prop.addFormat(prefix+'TFC.triggerRate',      'TFC: L&Oslash; Trigger Rate',1,'%8.2f Hz');
    prop.addFormat(prefix+'TFC.runTriggerRate',   'TFC: Integrated L&Oslash; trigger rate',1,'%8.2f Hz');
    prop.addFormat(prefix+'TFC.deadTime',         'TFC: Dead-time',1,'%8.2f %%');
    prop.addFormat(prefix+'TFC.runDeadTime',      'TFC: Integrated dead-time',1,'%8.2f %%');


    prop.add(prefix+'HLTFarm.nSubFarms',          'HLT: Number of Subfarms',1);
    prop.add(prefix+'HLTFarm.architecture',       'HLT: Architecture',1);
    prop.add(prefix+'HLTFarm.hltNTriggers',       'HLT: Number of accepted events',1);
    prop.addFormat(prefix+'HLTFarm.hltRate',      'HLT: Accept Rate',1,'%8.2f Hz');
    prop.addFormat(prefix+'HLTFarm.runHltRate',   'HLT: Integrated accept rate',1,'%8.2f Hz');

    if ( this_obj._partition=='LHCb' || this_obj._partition=='TRG' )  {
      prop.add(prefix+'Trigger.L0Type',           'L&Oslash; configuration  (L&Oslash;-TCK)',2);
    }
    prop.add(prefix+'Trigger.TCKLabel',           'HLT configuration (HLT-TCK)',2);

    prop.build_horizontal();
    this_obj.run_properties.appendChild(this_obj.runPropertyDisplay);
  };

  /** Set header row into the display
   *
   *  @return Reference to self
   */
  table.setHeader = function() {
    this.experiment_run_header.innerHTML = detector_run_header(this._partition);
    return this;
  };
  /** Attach display items. 
   * Note: Display must be "built" before. Otherwise the service updates do not find the
   * necessary html identifiers in in the documents,
   *
   *  @return Reference to self
   */
  table.attach = function(systems)   {
    this.statusDisplay = ControlsStatusDisplay(this._partition,this._provider,this._logger);
    this.ctrl_status.appendChild(this.statusDisplay);
    if ( this.createProperties )  {
      this.createProperties(this)
    }
    this.fsmDisplay = FSMDisplay(this._provider,this._logger,null);
    var used_systems = [];
    for(var i=0; i<systems.length;++i) {
      if ( systems[i]=='LHCb_HV')continue;
      used_systems.push(systems[i]);
    }
    this.fsmDisplay.setup(systems).build();
    this.fsm_items.appendChild(this.fsmDisplay);
    return this;
  };

  table.attachDetectors = function(systems) {
    if ( this.detDisplay != null ) {      
      this.det_items.removeChild(this.detDisplay);
    }
    this.detDisplay = FSMDisplay(this._provider,this._logger,'Subdetectors');
    this.detDisplay.setup(systems).buildHorizontal(7);
    this.det_items.appendChild(this.detDisplay);
    return this;
  };

  table.attachTriggers = function(systems) {
    if ( this.trgDisplay != null ) {
      this.trg_items.removeChild(this.trgDisplay);
    }
    this.trgDisplay = FSMDisplay(this._provider,this._logger,'Triggers');
    this.trgDisplay.setup(systems).buildHorizontal(6);
    this.trg_items.appendChild(this.trgDisplay);
    return this;
  };

  table._logger.info('RunStatusDisplay.init: Creating display...Done.');
  return table;
};

/** @class FSMStatusDisplay
 *  
 *  @author  M.Frank
 *  @version 1.0
 */
var FSMStatusDisplay = function(partition,provider,logger) {
  table = document.createElement('TABLE');
  table.body  = document.createElement('TBODY');
  table.experiment_run_header = null;
  table.ctrl_status = null;
  table.fsm_items = null;
  table.detData = null;
  table.statusDisplay = null;

  table._partition = partition;
  table._provider = provider;
  table._logger = logger;

  table._logger.debug('FSMStatusDisplay.init: Creating display... Partition:'+table._partition);

  table.width = '100%';
  table.className = 'FSMStatusPanel';
  table.body.className = 'FSMStatusPanel';
  var row = document.createElement('tr');
  table.experiment_run_header = document.createElement('td');
  row.appendChild(table.experiment_run_header);
  row.appendChild(document.createElement('td'));
  table.body.appendChild(row);
  
  row = document.createElement('tr');
  table.ctrl_status = document.createElement('td');
  row.appendChild(table.ctrl_status);
  row.appendChild(document.createElement('td'));
  table.body.appendChild(row);
  
  row = document.createElement('tr');
  table.fsm_items = document.createElement('td');
  row.appendChild(table.fsm_items);
  table.body.appendChild(row);
  table.appendChild(table.body);

  /** Attach display items. 
   * Note: Display must be "built" before. Otherwise the service updates do not find the
   * necessary html identifiers in in the documents,
   *
   *  @return Reference to self
   */
  table.attach = function(systems)   {
    this.experiment_run_header.innerHTML = detector_header(this._partition);
    this.statusDisplay = ControlsStatusDisplay(this._partition,this._provider,this._logger);
    this.ctrl_status.appendChild(this.statusDisplay);
    this.fsmDisplay = FSMDisplay(this._provider,this._logger,'Subsystems');
    this.fsm_items.appendChild(this.fsmDisplay);
    this.fsmDisplay.setup(systems).build();
    table._logger.info('FSMStatusDisplay.attach: Children created:'+systems);
    return this;
  };
  table._logger.info('FSMStatusDisplay.init: Creating display...Done.');
  return table;
};

if ( _debugLoading ) alert('Script lhcb.display.items.cpp loaded successfully');

