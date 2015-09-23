
var lbExt = new Object();

/** @function element
 *  
 *  Note:
 *  "element" sets up a ExtJs component for lbcomet.
 *
 *  @author  M.Frank
 *  @version 1.0
 */
lbExt.element = function(el)  {
  /// Attach address to object
  /**  @param addr   Topic address for comet service
   *
   *   @return On success reference to self, null otherwise
   */
  el.address = function(addr)  {
    this.addr = addr;
    return this;
  };
  /// Attach format for value->string conversion
  /**  @param fmt   Data format as string for sprintf
   *
   *   @return On success reference to self, null otherwise
   */
  el.format = function(fmt)  {
    this.fmt  = fmt;
    return this;
  };
  /// Standard callback for data provider for updates
  /**  @param data   Data value as string
   *
   *   @return On success reference to self, null otherwise
   */
  el.set = function(data)  {
    return this.display(data);
  };
  /// Subscribe item to receive data from data provider object
  /** @param      provider  Data provider object
   *
   *  @return On success reference to self, null otherwise
   */
  el.subscribe = function(provider) {
    provider.subscribe(this.addr,this);
    return this;
  };
  /// Attach conversion function for value->string conversion
  /**  @param func Data conversion function. Must return a string.
   *
   *   @return On success reference to self, null otherwise
   */
  el.convert = function(func)  {
    this.conversion = func;
    return this;
  };
  /// Attach parasitic data listener to the comet topic.
  /**  @param func Callback function to be called.
   *
   *   @return On success reference to self, null otherwise
   */
  el.listen = function(object,func)  {
    object.listeners.push({object: this,func: func});
    return this;
  };
  /// Primitive extraction function for received data
  /** @param      data    Display data
   *
   *  @return On success reference to self, null otherwise
   */
  el.data = function(data)  {
    var item_data;
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
    return item_data;
  };
  /// Display rendering function for received data
  /** @param      data    Display data
   *
   *  @return On success reference to self, null otherwise
   */
  el.display = function(data)  {
    var i, d, item_data = this.data(data);
    if ( this.listeners )  {
      for(i=0; i<this.listeners.length; ++i)   {
	this.listeners[i].func(data);
	this.listeners[i].object.display(data);
      }
    }
    if ( this.conversion != null )   {
      this.conversion.target = this;
      item_data = this.conversion(item_data);
    }
    if ( this.fmt != null ) {
      item_data = sprintf(this.fmt,item_data);
    }
    if ( ''+item_data == 'undefined' ) {
      item_data = '&lt;undefined&gt;';
    }
    this.update(item_data);
    return this;
  };
  /// Data rendering callback
  /** @param      data    Display data
   *
   *  @return On success reference to self, null otherwise
   */
  el.callback = function(func)  {
    this.set = func;
    return this;
  };
  el.listeners = [];
  return el;
};


/// Build LHC operator comments panel
/**
 *   +-------- LHC operator comments ----------
 *   |LHC in limbo, awaiting decision ...      
 *   |
 *   +-----------------------------------------
 *
 * @return Reference to ExtJs Panel object containing all data items of this widget
 *
 * @author  M.Frank
 * @version 1.0
*/
lbExt.LHCComment = function(opts)  {
  var options = opts.options;
  var panel = new Ext.form.FieldSet({
    renderTo: opts.parent ? opts.parent : document.body,
	id:       opts.id,
	title:    'LHC Operator comments',
	width:        options.width ? options.width : '100%',
	frame:       false,
	borderStyle: {color: 'black', borderWidth: 1},
	bodyStyle:    'padding:2px 2px 0',
	autoHeight:   false,
	defaultType:  'textfield',
	closable: false,
	collapsed: false,
	collapsible: false,
	hideCollapsTool: true
	});
  if ( options.style ) panel.style = options.style;
  lbExt.element(panel)
  .address('lbWeb.LHCCOM/LHC.LHC.RunControl.Page1Comment')
  .convert(function(data) {return data.replace(/\n/g,'<BR>');});
  return panel;
};

/** Build table with LHC summary information
 *   
 * @return Reference to HTML table containing all data items of this widget
 *
 * @author  M.Frank
 * @version 1.0
 */
lbExt.LHCSummary = function(opts) {
  var options = opts.options;
  var panel = new Ext.form.FieldSet({
    renderTo: opts.parent ? opts.parent : document.body,
	id:            opts.id,
	title:        'LHC status summary',
	width:         options.width ? options.width : '100%',
	frame:         opts.frame,
	layout:       'table',
	//layoutConfig: { columnWidth: 0.24, columns: 4 },
	layoutConfig: { columns: 4 },
	autoHeight:   false,
	defaultType:  'box',
	closable: false,
	collapsed: false,
	collapsible: false,
	hideCollapsTool: true,
	defaults: { defaultType: 'box',
	  style: {textAlign: 'center',
	    width:         120,
	    paddingTop:    5,
	    paddingBottom: 5
	    },
	  },
	cls:'TextBold',
	items: [{ html: 'Energy:',         cls: 'MonitorDataHeader', ctCls:'TextBold'  },
		{ html: 'undefined' },  // 1
		{ html: 'Avg.Luminosity:', cls: 'MonitorDataHeader', ctCls:'TextBold'  },
		{ html: 'undefined' },  // 3
		{ html: 'Intensity [e]:',  cls: 'MonitorDataHeader', ctCls:'TextBold'  },
		{ html: 'undefined' },  // 5
		{ html: '&larr;1 Beam 2&rarr;', ctCls:'TextBold' },
		{ html: 'undefined' },  // 7
		{ html: 'Lifetime [h]:',   cls: 'MonitorDataHeader', ctCls:'TextBold'  },
		{ html: 'undefined' },  // 9
		{ html: '&larr;1 Beam 2&rarr;', ctCls:'TextBold' },
		{ html: 'undefined' },  // 11
		{ html: 'Handshakes:',          cls:'TextBold'   },
		{ html: 'Dump',                 cls: 'MonitorDataHeader', ctCls:'TextBold' },
		{ html: 'Adjust',               cls: 'MonitorDataHeader', ctCls:'TextBold' },
		{ html: 'Injection',            cls: 'MonitorDataHeader', ctCls:'TextBold' },
		{ html: ''          }, 
		{ html: 'undefined' },   // 17
		{ html: 'undefined' },   // 18
		{ html: 'undefined' }],  // 19
	buttons: [],
	itemCls:  'TextBold'
	});
  if ( options.style ) panel.style = options.style;
  lbExt.element(panel.items.get(1)).address('lbWeb.LHCCOM/LHC.LHC.Beam.Energy').format('%7.0f GeV');
  lbExt.element(panel.items.get(3)).address('lbWeb.LHCCOM/LHC.LHCb.Beam.Luminosity.LuminosityAverage').format('%9.2e');
  lbExt.element(panel.items.get(5)).address('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam1.averageBeamIntensity').format('%9.2e');
  lbExt.element(panel.items.get(7)).address('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam2.averageBeamIntensity').format('%9.2e');
  lbExt.element(panel.items.get(9)).address('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam1.bestLifetime').format('%9.2e');
  lbExt.element(panel.items.get(11)).address('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam2.bestLifetime').format('%9.2e');
  lbExt.element(panel.items.get(17)).address('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_BEAMDUMP');
  lbExt.element(panel.items.get(18)).address('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_ADJUST');
  lbExt.element(panel.items.get(19)).address('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_INJECTION');

  /** Subscribe all required data item to receive data from data provider object
   *
   *  @param      provider  Data provider object
   *
   *  @return On success reference to self, null otherwise
   */
  panel.subscribe = function(provider)  {
    var o, i;
    for(i=0; i<this.items.getCount(); ++i)  {
      o = panel.items.get(i);
      if ( o.address && o.subscribe ) o.subscribe(provider);
    }
  };
  return panel;
};

/// Build LHC operator comments panel
/**
 *     +---- Shift crew : None --------------------------------------------------------
 *     |
 *     |
 *     +---- Comment updated on 18-SEP-2000 16:28 -------------------------------------
 *
 * @return Reference to ExtJs Panel object containing all data items of this widget
 *
 * @author  M.Frank
 * @version 1.0
*/
lbExt.LHCb_shift_comments = function(opts)  {
  var options = opts.options;
  var panel = new Ext.form.FieldSet({
    renderTo: opts.parent ? opts.parent : document.body,
	id:       opts.id,
	title:    'Shift comments from logbook',
	width:        options.width ? options.width : '100%',
	frame:       false,
	borderStyle: {color: 'black', borderWidth: 1},
	bodyStyle:    'padding:2px 2px 0',
	ctCls: 'PaddingSides10pt',
	autoHeight:   false,
	defaultType:  'textfield',
	closable: false,
	collapsed: false,
	collapsible: false,
	hideCollapsTool: true
	});
  if ( options.style ) panel.style = options.style;
  panel.style.paddingTop = 5;
  panel.style.paddingBottom = 5;

  lbExt.element(panel)
  .address('lbWeb.shiftComments')
  .convert(function(data) {return data.replace(/\n/g,'<BR>');});
  return panel;
};

/// Build table with HLT1 defer disk statistics
/**  
 *  @author  M.Frank
 *  @version 1.0
 */
lbExt.Hlt1DeferredStatus = function(opts)  {
  var options = opts.options;
  var panel = new Ext.Panel({
    renderTo: opts.parent ? opts.parent : document.body,
	frame:  options.frame,
	style:  options.style,
	width:  options.width ? options.width : '100%',
	title:  options.title ? options.title : 'HLT1 deferred activity monitor',
	layout: 'table',
	layoutConfig: { columnWidth: 0.2, columns: 5 },
	container: parent,
	bodyStyle: 'padding:10px 10px 10px 10px',
	borderStyle: 'solid',
	defaults: { defaultType: 'textfield', 
	  height: 30,
	  style: {textAlign: 'center',
	    verticalAlign: 'middle',
	    marginRight: 20,
	    width: '100%' },
	   },
	items: [{html: 'Disk status',  xtype: 'box', cls: 'MonitorDataHeader', height: 60, rowspan: 2 },
		{html: 'Blocks total', xtype: 'box', cls: 'MonitorDataHeader'  },
		{html: 'Blocks free',  xtype: 'box', cls: 'MonitorDataHeader'  },
		{html: 'Usage',        xtype: 'box', cls: 'MonitorDataHeader'  },
		{html: 'Files',        xtype: 'box', cls: 'MonitorDataHeader'  },
		{html: 'undefined',    xtype: 'box' },
		{html: 'undefined',    xtype: 'box' }, // 6
		{html: 'undefined',    xtype: 'box' },
		{html: 'undefined',    xtype: 'box' }, // 8
		{html: 'HLT1 rate',    xtype: 'box', cls: 'MonitorDataHeader'  },
		{html: 'undefined',    xtype: 'box', cls: 'MonitorDataItem'  },
		{html: '',             xtype: 'box' },
		{html: '',             xtype: 'box' },
		{html: '',             xtype: 'box' }],
	  buttons: []
	});

  if ( options.is ) panel.id = options.id;
  else Ext.id(panel,'Hlt1-deferred-activity');

  panel.totalHLTRate = lbExt.element(panel.items.get(10))
  .address('lbWeb.LHCb_RunInfoSplitHLT.TotalHLTRate').format('%d Hz');
  panel.NFilesLeft = lbExt.element(panel.items.get(8)).address('lbWeb.LHCb_RunInfoSplitHLT.NFilesLeft');
  panel.disk_free = lbExt.element(panel.items.get(6)).address('lbWeb.LHCb_RunInfoSplitHLT.DiskFree');
  panel.disk_size = lbExt.element(panel.items.get(5)).address('lbWeb.LHCb_RunInfoSplitHLT.DiskSize');
  panel.usage = lbExt.element(panel.items.get(7))
  .listen(panel.disk_free,function(data)  { panel.usage.disk_free = parseFloat(data[1]); })
  .listen(panel.disk_size,function(data)  { panel.usage.disk_size = parseFloat(data[1]); })
  .convert(function(data)  {
      if ( this.disk_free && this.disk_size )
	return sprintf('%.2f %%',100.0*this.disk_free/this.disk_size);
      return null;
    });
  /// Subscribe comet items to the service to receive data
  panel.subscribe = function(provider)  {
    this.disk_free.subscribe(provider);
    this.disk_size.subscribe(provider);
    this.NFilesLeft.subscribe(provider);
    this.totalHLTRate.subscribe(provider);
    return this;
  };
  return panel;
};


/** Online widgets. Load HLT2 run info from datapoint for displays.
 *
 *  @author M.Frank
 */
lbExt.hlt2RunsLoader = function(opts)   {
  var loader = new Object({lastUpdate: 0, store: opts.store, columns: opts.columns});
  loader.prepareRuns = function(data)  {
    var r, rr, runs = [], line = [], rundata = data.split('|');
    for (r = 0; r < rundata.length; r++) {
      if ( (r%this.columns) == 0 && line.length>0 )  {
	runs.push(line);
	line = [];
      }
      rr = rundata[r].split('/');
      var nfile = parseInt(rr[1]);
      if ( nfile > 1 )  {
	line.push(parseInt(rr[0]));
	line.push(nfile);
      }
    }
    if ( line.length > 0 ) runs.push(line);
    return runs;
  };
  lbExt.element(loader).address(opts.datapoint).callback(function(items)  {
      var now = (new Date()).getTime();
      if ( (now - this.lastUpdate) > 1000*10 )  { 
	var runs = this.prepareRuns(items[1]);
	this.store.loadData(runs);
	this.lastUpdate = now;
      }
    });
  return loader;
};

/** Online widgets. Load HLT2 run info table
 *
 *  @author M.Frank
 */
lbExt.hlt2RunsTable = function(opts) {
  var options = opts.options;
  var store = new Ext.data.ArrayStore({
    fields: [{name: 'r1'},
	     {name: 'f1', type: 'integer'},
	     {name: 'r2'},
	     {name: 'f2', type: 'integer'},
	     {name: 'r3'},
	     {name: 'f3', type: 'integer'}
	     ]});
  var loader = lbExt.hlt2RunsLoader({datapoint:opts.datapoint, store: store, columns: 3});
  var panel = new Ext.grid.GridPanel({
    renderTo: opts.parent ? opts.parent : document.body,
	id:       opts.id,
	title:    opts.title,
	frame:    options.frame,
	style:    options.style,
	width:    options.width ? options.width : '100%',
	store: store,
	columns: [{id:'r1',header: 'Run',   dataIndex: 'r1', sortable: true  },
		  {id:'f1',header: 'Files', dataIndex: 'f1', sortable: true  },
		  {id:'r2',header: 'Run',   dataIndex: 'r2', sortable: true  },
		  {id:'f2',header: 'Files', dataIndex: 'f2', sortable: true  },
		  {id:'r3',header: 'Run',   dataIndex: 'r3', sortable: true  },
		  {id:'f3',header: 'Files', dataIndex: 'f3', sortable: true  }],
	autoExpandColumn: 3,
	buttonAlign: 'right',
	stripeRows: true,
	autoExpandColumn: 'f3',
	boxMaxWidth:  80,
	height: 350,
	// config options for stateful behavior
	stateful: true,
	stateId: 'grid',
	animCollapse: true,
	closable: false,
	collapsed: false,
	collapsible: true,
	hideCollapseTool: false,
	collapseFirst: true
	});
  return loader;
};

function lbExt__hlt2RunsHisto__FileLabels(data) {
  //return Math.floor(data/10,10)*10;
  return Math.floor(data);
};

/** Online widgets. Display histogram with HLT2 runs to be processed.
 *
 *  @author M.Frank
 */
lbExt.hlt2RunsHistogram = function(opts) {
  var options = opts.options;
  var store = new Ext.data.ArrayStore({fields: ['run','files'], data: [['0',1],['1',1]]});
  var loader = lbExt.hlt2RunsLoader({datapoint:opts.datapoint, store: store, columns: 1});
  Ext.onReady(function(){
      var panel = new Ext.Panel({
	renderTo: opts.parent ? opts.parent : document.body,
	    id:       opts.id,
	    title:    opts.title,
	    frame:    options.frame,
	    style:    options.style,
	    width:    options.width ? options.width : '100%',
	    boxMaxHeight: 350,
	    height: 250,
	    items: {
	  xtype:  'columnchart',
	      store:   loader.store,
	      yField: 'files',
	      url:    'ExtJs/resources/charts.swf',
	      xField: 'run',
	      xAxis:   new Ext.chart.CategoryAxis({title: 'Run Number'}),
	      yAxis:   new Ext.chart.NumericAxis({
		title: 'Files',
		    scale: 'logarithmic',
		    majorUnit: 100,
		    hideOverlappingLabels: true,
		    labelFunction: 'lbExt__hlt2RunsHisto__FileLabels',
		    snapToUnits : true
		    }),
	      extraStyle: {xAxis: { labelRotation: -90  } },

	      animCollapse: true,
	      closable: false,
	      collapsed: false,
	      collapsible: true,
	      hideCollapseTool: false,
	      collapseFirst: true
	      }
	});
      panel.loader = loader;
      panel.subscribe = function(provider)  {
	this.loader.subscribe(provider);
      };
      Ext.id(panel,'container');
    });
  return loader;
}
