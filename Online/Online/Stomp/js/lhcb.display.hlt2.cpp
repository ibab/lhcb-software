//_loadScript('lhcb.display.data.cpp');
_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.listener.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadScript('lhcb.display.partition.cpp');
_loadScript('lhcb.display.zoom.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.status','css');
_loadFile('lhcb.display.fsm','css');

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
}

/** Online widgets. DAQ efficiency
 *
 *  @author M.Frank
 */
var hlt2Runs2Process = function(options,title,dp) {
  var c, tr, tab = document.createElement('table');
  var tb = document.createElement('tbody');
  tab.className = 'MonitorPage';

  tab.width='100%';
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

var showDeferredState = function(table) {
  var opts = {'style': 'Arial12pt', 'legend': true };
  var tab = document.createElement('table');
  var body = document.createElement('tbody');
  var row = document.createElement('tr');
  var cell; 

  body.className = 'VerticalAlignTop';
  row.className = 'VerticalAlignTop';
    
  table.deferredHLT2 =
    hlt2Runs2Process(opts,
		     'HLT2 run to be processed:',
		     'lbWeb.LHCb_RunInfoSplitHLT.RunsLeftCorr');
  table.deferredHLT2.subscribe(table.provider);

  cell = document.createElement('td');
  cell.width = '50%';
  cell.appendChild(table.deferredHLT2);
  row.appendChild(cell);

  cell = document.createElement('td');
  cell.width = '50%';
  table.deferredHLT1 = lhcb.widgets.DeferredHLT(opts);
  table.deferredHLT1.subscribe(table.provider);
  cell.appendChild(table.deferredHLT1);
  row.appendChild(cell);

  cell = Cell('&nbsp;',1);
  //cell.width = '10%';
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
