_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.listener.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadScript('lhcb.display.partition.cpp');
_loadScript('lhcb.display.zoom.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.status','css');
_loadFile('lhcb.display.fsm','css');

_loadFile('../ExtJs/resources/css/ext-all','css');
_loadFile('../ExtJs/examples/shared/examples','css');
_loadFile('../ExtJs/examples/grid/grid-examples','css');

_loadScript('ExtJs/adapter/ext/ext-base.js');
_loadScript('ExtJs/ext-all.js');
//_loadScript('ExtJs/ext-all-debug.js');

var s_display_font_size = null;
var s_org_display_font_size = null;

function fileLabels(data) {
  return Math.floor(data/10,10)*10;
};

var hlt2Properties = function(table)  {
  table.runPropertyDisplay = PropertyTable(table._provider, 
					   table._logger, 
					   2,
					   'PropertyTableItem',
					   'PropertyTableValue');
  var prop = table.runPropertyDisplay;
  var prefix = 'lbWeb.'+table._partition+'_RunInfo.';
  prop.add(prefix+'general.runNumber',          'Run number',1);
  prop.add(prefix+'Trigger.HLTType',            'Trigger configuration',1);
  prop.add(prefix+'general.runType',            'Activity',1);

  prop.add(prefix+'general.runStartTime',       'Run start time',1);
  prop.add(prefix+'general.dataType',           'Data type',1);
  prop.add(prefix+'HLTFarm.nSubFarms',          'HLT: Number of Subfarms',1);
  prop.add(prefix+'HLTFarm.architecture',       'HLT: Architecture',1);
  prop.add(prefix+'HLTFarm.hltNTriggers',       'HLT: Number of Accept events',1);
  prop.addFormat(prefix+'HLTFarm.hltRate',      'HLT: Accept Rate',1,'%8.2f Hz');
  prop.addFormat(prefix+'HLTFarm.runHltRate',   'HLT: Integrated accept rate',1,'%8.2f Hz');
  prop.build_horizontal();
  table.run_properties.appendChild(table.runPropertyDisplay);
  table.run_properties.width='50%';
  table.ctrl_status.width='50%';
};

/** Online widgets. DAQ efficiency
 *
 *  @author M.Frank
 */
var hlt2RunsTable2 = function(options,title,dp) {
  var c, tr, tab = document.createElement('table');
  var tb = document.createElement('tbody');

  tab.className  = 'MonitorPage';
  tooltips.set(tb,'HLT2 deferred runs.');
  tab.appendChild(tb);
  if ( 1||options.style ) {
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell(title,1,options.style));
  }
  tab.runs = StyledItem(dp,'Text-Right','%s');
  tb.appendChild(tr=document.createElement('tr'));
  tr.appendChild(c=Cell('',1,'Text-Right'));
  tab.runs.table = document.createElement('table');
  tab.runs.table.width='100%';
  tab.runs.table.appendChild(document.createElement('tbody'));
  c.appendChild(tab.runs.table);

  tab.runs.conversion = function(data)  {
    var r, c, tr, tab = this.table;
    var tb = document.createElement('tbody');
    var runs = data.split('|');
    tab.className  = tb.className = 'MonitorPage';
    tb.appendChild(tr=document.createElement('tr'));
    for(r=0; r<3; r++)  {
      tr.appendChild(c=Cell('Run number',1,'MonitorDataHeader'));
      tr.appendChild(c=Cell('No.files', 1,'MonitorDataHeader'));
    }
    for	(r = 0; r < runs.length; r++) {
      if ( (r%3)==0 )  {
	tr = document.createElement('tr');
	tb.appendChild(tr);
      }
      var rr = runs[r].split('/');
      tr.appendChild(Cell(rr[0],1,'PropertyTableItem'));
      tr.appendChild(Cell(rr[1],1,'PropertyTableValue'));
    }
    tab.replaceChild(tb,tab.childNodes[0]);
    return '';
  };

  tab.subscribe = function(provider) {
    provider.subscribeItem(this.runs);
  };
  return tab;
};

/** Online widgets. DAQ efficiency
 *
 *  @author M.Frank
 */
var hlt2RunsTable = function(options,title,datapoint) {
  var tab = document.createElement('div');
  tab.id = 'run-table';
  tab.runs = StyledItem(datapoint,'Text-Right','%s');
  tab.runs.conversion = function(data)  {
    var r, rr, runs = [], line = [], rundata = data.split('|');
    for	(r = 0; r < rundata.length; r++) {
      if ( (r%3) == 0 && line.length>0 )  {
	runs.push(line);
	line = [];
      }
      rr = rundata[r].split('/');
      line.push(rr[0]);
      line.push(parseInt(rr[1]));
    }
    if ( line.length > 0 ) runs.push(line);
    this.store.loadData(runs);
    return '';
  };
  tab.subscribe = function(provider) {
    provider.subscribeItem(this.runs);
  };
  // create the data store
  var store = new Ext.data.ArrayStore({
    fields: [{name: 'r1'},
	     {name: 'f1', type: 'integer'},
	     {name: 'r2'},
	     {name: 'f2', type: 'integer'},
	     {name: 'r3'},
	     {name: 'f3', type: 'integer'}
	     ]});
  tab.runs.store = store;
  store.loadData(['1',0,'2',0,'3',0]);
  Ext.onReady(function(){
      var panel = new Ext.grid.GridPanel({
	store: store,
	    renderTo:  tab,
	    columns: [
		      {id:'r1',header: 'Run',   dataIndex: 'r1'},
		      {id:'f1',header: 'Files', dataIndex: 'f1'},
		      {id:'r2',header: 'Run',   dataIndex: 'r2'},
		      {id:'f2',header: 'Files', dataIndex: 'f2'},
		      {id:'r3',header: 'Run',   dataIndex: 'r3'},
		      {id:'f3',header: 'Files', dataIndex: 'f3'}
		      ],
	    stripeRows: true,
	    autoExpandColumn: 'r1',
	    boxMaxHeight: 350,
	    height: 350,
	    title: title,
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
    });
  // render the grid to the specified div in the page
  //grid.render('run-table');
  return tab;
};

var hlt2RunsHisto = function(options,title,datapoint) {
  var tab = document.createElement('div');
  tab.id = 'container';
  tab.runs = StyledItem(datapoint,'Text-Right','%s');
  tab.runs.conversion = function(data)  {
    var r, rr, runs = [], rundata = data.split('|');
    for	(r = 0; r < rundata.length; r++) {
      rr = rundata[r].split('/');
      runs.push([rr[0],parseFloat(rr[1])]);
    }
    this.store.loadData(runs);
    return '';
  };
  tab.subscribe = function(provider) {
    provider.subscribeItem(this.runs);
  };

  tab.runs.store = new Ext.data.ArrayStore({fields: ['run','files'], data: [[0,1],[1,1]] });
  Ext.onReady(function(){
      var panel = new Ext.Panel({
	    width:    '100%',
	    boxMaxHeight: 350,
	    height: 250,
	    renderTo:  tab,
	    title:     title,
	    items: {
   	      xtype:  'columnchart',
	      store:   tab.runs.store,
	      yField: 'files',
	      url:    'ExtJs/resources/charts.swf',
	      xField: 'run',
	      xAxis:   new Ext.chart.CategoryAxis({title: 'Run Number'}),
	      yAxis:   new Ext.chart.NumericAxis({
                title: 'Files',
		    scale: 'logarithmic',
		    majorUnit: 100,
		    hideOverlappingLabels: true,
		    labelFunction: 'fileLabels',
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
    });
  return tab;
}



var showDeferredState = function(table) {
  var opts = {'style': 'Arial12pt', 'legend': true };
  var tab = document.createElement('table');
  var body = document.createElement('tbody');
  var row, cell; 

  tab.style.width = '100%';

  tab.className = 'VerticalAlignTop';
  table.currRuns = hlt2RunsHisto(opts,
				 'Runs currently processing',
				 'lbWeb.LHCb_RunInfoSplitHLT.RunsLeftCorr');
  table.currRuns.subscribe(table.provider);
  cell = Cell('',2);
  cell.appendChild(table.currRuns);
  row = document.createElement('tr');
  row.appendChild(cell);
  body.appendChild(row);

  table.allRuns = hlt2RunsHisto(opts,
				'All runs with files to be processed',
				'lbWeb.LHCb_RunInfoSplitHLT.RunsLeftCorr');
  table.allRuns.subscribe(table.provider);
  cell = Cell('',2);
  cell.appendChild(table.allRuns);
  row = document.createElement('tr');
  row.appendChild(cell);
  body.appendChild(row);

  table.allRunsTable = hlt2RunsTable(opts,
				     'All runs with files to be processed',
				     'lbWeb.LHCb_RunInfoSplitHLT.RunsLeftCorr');
  table.allRunsTable.subscribe(table.provider);
  cell = Cell('',1);
  cell.style.width = '50%';
  cell.appendChild(table.allRunsTable);
  row = document.createElement('tr');
  row.appendChild(cell);

  table.deferred = lhcb.widgets.DeferredHLT(opts);
  table.deferred.subscribe(table.provider);
  cell = Cell('',1,'VerticalAlignTop');
  cell.style.width = '50%';
  cell.appendChild(table.deferred);
  row.appendChild(cell);

  cell = Cell('&nbsp;',1);
  row.appendChild(cell);

  body.appendChild(row);
  tab.appendChild(body);
  table.bottom.appendChild(tab);
};

var createDisplay = function(selector)   {
  var partition = selector.selectBox.get_value();
  var opts = {'size_call': selector.size_call};
  if ( null != selector.listener )   {
    selector.listener.close();
    selector.provider.start();
  }
  selector.callbackNo = 3;
  selector.listener = new DetectorListener(selector.logger,selector.provider,selector.display,selector.messages,this);
  selector.listener.createProperties = hlt2Properties;
  selector.listener.trace = false;
  selector.listener.start(partition,'lbWeb.'+partition+'.FSM.children');
  selector.heading.innerHTML = partition+' Run Status Display';
  setWindowTitle(partition+' Run Status');
};

var hlt2_unload = function()  {
  dataProviderReset();
};

var hlt2_body = function()  {
  var prt  = the_displayObject['external_print'];
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector = new PartitionSelector(msg);

  s_display_font_size = the_displayObject['size'];
  body.appendChild(selector);
  body.className = 'MainBody';

  if ( msg > 0 )
    selector.logger = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'RunStatusLogger');
  else
    selector.logger = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'RunStatusLogger');
  if ( prt ) selector.logger.print = prt;
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  setWindowTitle('LHCb2 Run Status');
  selector.selectBox.add('LHCb2','LHCb2',true);
  createDisplay(selector); 
  selector.hideInput();
  selector.showLHCstate();
  showDeferredState(selector);
  selector.provider.start();
};

if ( _debugLoading ) alert('Script lhcb.display.status.cpp loaded successfully');
