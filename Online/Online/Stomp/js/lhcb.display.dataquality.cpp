_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');

var dq_summary = function()  {

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
    var DAQ = lhcb.widgets.SystemSummary({style:'Arial12pt',
	  system: null,
	  title: 'DataQuality Subsystem State',
	  split: false,
	  items: ['MONA1001_R','MONA1002_R','MONA1003_R','MONA1004_R','MONA10_R_Work'],
	  legend:true,
	  rowing:true,
	  logger:this.logger});
    var summary = dq_summary()
    this.left.addItem(DAQ);
    this.right.addItem(summary);
  };
  return table;
};

var dataquality_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var dataquality_body = function()   { return lhcb.widgets.SubdetectorPage.start(DQstatus); };
