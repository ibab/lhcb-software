_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.listener.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadScript('lhcb.display.partition.cpp');
_loadScript('lhcb.display.zoom.cpp');
_loadScript('lhcb.display.extWidgets.cpp');


_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.status','css');
_loadFile('lhcb.display.fsm','css');

_loadFile('../ExtJs/resources/css/ext-all','css');
_loadFile('../ExtJs/examples/shared/examples','css');
_loadFile('../ExtJs/examples/grid/grid-examples','css');

_loadScript('ExtJs/adapter/ext/ext-base.js');
//_loadScript('ExtJs/ext-all.js');
_loadScript('ExtJs/ext-all-debug.js');


var s_display_font_size = null;
var s_org_display_font_size = null;



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


var lhcStatus = function(options)  {
  //return lbExt.Hlt1DeferredStatus(options);
  //return lbExt.hlt2RunsTable(options);
  //return lbExt.hlt2RunsHistogram(options);
  //return lbExt.LHCComment(options);
  //return lbExt.LHCSummary(options);
  options.options.width = '95%';
  return lbExt.LHCb_shift_comments(options);
}

var showDeferredState = function(table) {
  var opts = {'style': 'Arial12pt', 'frame': true, width: '100%' };
  var tab = document.createElement('table');
  var body = document.createElement('tbody');
  var row, cell; 
  tab.appendChild(body);
  table.bottom.appendChild(tab);

  tab.className = 'VerticalAlignTop';
  body.appendChild(row = document.createElement('tr'));
  row.appendChild(cell = Cell('',1));
  cell.style.width='100%';
  tab.lhcStatus = lhcStatus({parent:cell,
	title:'Runs table',
	datapoint:'lbWeb.LHCb_RunInfoSplitHLT.RunsLeftCorr',
	options: opts} );
  tab.lhcStatus.subscribe(table.provider);
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

var test_unload = function()  {
  dataProviderReset();
};

var test_body = function()  {
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
  //selector.selectBox.add('LHCb2','LHCb2',true);
  //createDisplay(selector); 
  //selector.hideInput();
  //selector.showLHCstate();
  showDeferredState(selector);
  selector.provider.start();
};

if ( _debugLoading ) alert('Script lhcb.display.status.cpp loaded successfully');
