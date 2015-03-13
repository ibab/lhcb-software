_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** TRIGGER web widgets
 *
 *  @author  M.Frank
 */
var TriggerStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('LHCb Triggers Overview');

  table.options = {logo:    '',
        	   logo_url:null,
		   title:   'Trigger Overview',
		   tips:    'LHCb Trigger overview:<br>-- L0DU counters<br>-- ODIN counters<br>-- HLT counters<br>-- Calibration farm'
  };


  table.attachWidgets = function() {
    var opts = {system:'LHCb',style:'Arial12pt',legend:true,logger:this.logger};
    this.odin        = lhcb.widgets.ODINRateMonitor(opts);
    this.l0du        = lhcb.widgets.L0DURateMonitor(opts);
    this.hlt         = lhcb.widgets.HLTRateMonitor(opts);
    this.trg         = lhcb.widgets.TriggerSetup(opts);

    this.left.addItem(this.trg);
    //this.left.addSpacer('50px');
    this.left.addItem(this.odin);
    this.left.addItem(this.hlt);

    this.right.addItem(this.l0du);
    //this.right.addSpacer('50px');
  };
  return table;
};

var trigger_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var trigger_body = function()   { return lhcb.widgets.SubdetectorPage.start(TriggerStatus); };
