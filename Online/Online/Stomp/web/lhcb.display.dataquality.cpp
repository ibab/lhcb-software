_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');

/** Data Quality web widgets
 *
 *  @author  M.Frank
 */
DQ_FSM = function(options) {
  var tab = document.createElement('table');
  var tb = document.createElement('tbody');
  var tr, td, i;

  tab.logger = options.logger;
  tab.className = tb.className   = 'MonitorPage';
  if ( options.legend ) {
    tr = document.createElement('tr');
    tr.style.height = 35;
    tr.appendChild(Cell('Controller',1,'MonitorDataHeader'));
    tr.appendChild(Cell('FSM Node State',2,'MonitorDataHeader'));
    tr.appendChild(Cell('Architecture',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Last Command',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Task-Count',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Partition',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Ctrl State',1,'MonitorDataHeader'));
    tb.appendChild(tr);
  }
  tab.items = [];
  for(i=1; i<5; ++i)   {
    var item, ctr = sprintf('MONA10%02d_R',i);
    tr = document.createElement('tr');
    tr.appendChild(Cell(ctr,1));

    item = FSMItem('lbWeb.'+ctr,options.logger,true);
    tab.items.push(item);
    item.style.width='20%';
    tr.appendChild(item);
    tr.appendChild(item.lock);

    item = StyledItem('lbWeb.'+ctr+'_Controller.architecture');
    tab.items.push(item);
    tr.appendChild(item);

    item = StyledItem('lbWeb.'+ctr+'_Controller.command');
    tab.items.push(item);
    tr.appendChild(item);

    item = StyledItem('lbWeb.'+ctr+'_Controller.count');
    tab.items.push(item);
    tr.appendChild(item);

    item = StyledItem('lbWeb.'+ctr+'_Controller.partName');
    tab.items.push(item);
    tr.appendChild(item);

    item = StyledItem('lbWeb.'+ctr+'_Controller.state');
    tab.items.push(item);
    tr.appendChild(item);

    tb.appendChild(tr);
  }
  tr = document.createElement('tr');
  tr.appendChild(Cell('MONA10_R_Work',1));
  item = FSMItem('lbWeb.MONA10_R_Work',options.logger,true);
  tab.items.push(item);
  tr.appendChild(item);
  tr.appendChild(item.lock);
  tr.appendChild(Cell('',5));
  tb.appendChild(tr);

  tab.appendChild(tb);

  tab.subscribe = function(provider) {
    for(var i=0; i<this.items.length; ++i)
      provider.subscribeItem(this.items[i]);
  };
  return tab;
};

var DQ_Summary = function(options)  {
  var tab = document.createElement('table');
  var tb = document.createElement('tbody');
  var tr, td, item;

  tab.logger = options.logger;
  tab.className = tb.className   = 'MonitorPage';
  if ( options.legend ) {
    tr = document.createElement('tr');
    tr.style.height = 35;
    tr.appendChild(Cell('Automatic',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Current Run',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Work',1,'MonitorDataHeader'));
    tb.appendChild(tr);
  }

  tab.items = [];
  tr = document.createElement('tr');
  tb.appendChild(tr);

  item = StyledItem('lbWeb.MONA10_R_Work.automaticRampUp');
  item.conversion = function(data)  {
    if ( data == '1' ) return 'Autostart ENABLED';
    return 'Autostart DISABLED';
  };
  tab.items.push(item);
  tr.appendChild(item);

  item = StyledItem('lbWeb.MONA10_R_Work.runNumber');
  tab.items.push(item);
  tr.appendChild(item);

  item = StyledItem('lbWeb.MONA10_R_Work.work');
  item.conversion = function(data)  {
    if ( data == '0' ) return 'Nothing todo';
    return 'Work waiting....';
  };
  tab.items.push(item);
  tr.appendChild(item);

  tab.appendChild(tb);

  tab.subscribe = function(provider) {
    for(var i=0; i<this.items.length; ++i)
      provider.subscribeItem(this.items[i]);
  };
  return tab;  
};

/** DataQuality web widgets
 *
 *  @author  M.Frank
 */
var DQstatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('DataQuality Operation State');

  table.options = {logo:    lhcb_logo('http://lhcb.web.cern.ch/lhcb/lhcb_page/lhcb_logo.png'),
        	   logo_url:'http://cern.ch/lhcb-public',
		   title:   'Online DataQuality Status',
		   tips:    'DataQuality operations status'
  };

  lhcb.widgets.dq = new Object();
  table.attachWidgets = function()   {
    var opts = {style:'Arial12pt',legend:true,logger:this.logger,showStatus:false};
    var sum = DQ_Summary({legend:true,logger:this.logger});
    var fsm = DQ_FSM({legend:true,logger:this.logger});
    this.left.addItem(sum);
    this.bottom.addItem(fsm);
  };
  return table;
};

var dataquality_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var dataquality_body = function()   { return lhcb.widgets.SubdetectorPage.start(DQstatus); };
