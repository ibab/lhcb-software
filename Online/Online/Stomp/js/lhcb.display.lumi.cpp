_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** LUMI web widgets
 *
 *  @author  M.Frank
 */
var LumiStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('Luminosity and Datataking State');

  table.options = {logo:    '',
        	   logo_url:null,
		   title:   'Luminosity and Datataking Status',
		   tips:    'LHCb operations status:<br>-- LHC status<br>-- Magnet<br>-- Velo<br>-- Background<br>-- Data rates &amp; Luminosity'
  };


  lhcb.widgets.lumi = new Object();

  table.attachWidgets = function() {
    var opts = {system:'LHCb',style:'Arial12pt',legend:true,logger:this.logger};
    this.lhc_state   = lhcb.widgets.LHCStateSummary(opts);
    this.shift       = lhcb.widgets.LHC_operator_comments();
    this.plan        = lhcb.widgets.LHCb_PlanOfDay({style:'MonitorDataHeader',legend:true,logger:this.logger});
    this.velo        = lhcb.widgets.velo.positionSummary(opts);
    this.trg         = lhcb.widgets.TriggerSetup(opts);
    this.run         = lhcb.widgets.RateSummary(opts);
    this.effi_1      = lhcb.widgets.DAQEfficiency(opts);
    opts.efficiency_type = 'Time';
    //opts.style = null;
    this.effi_2      = lhcb.widgets.DAQEfficiency(opts);
    this.deferred    = lhcb.widgets.DeferredHLT(opts);
    this.left.addItem(this.run);
    this.left.addSpacer('30px');
    this.left.addItem(this.effi_1);
    this.left.addItem(this.effi_2);
    this.left.addItem(this.deferred);


    this.right.addItem(this.lhc_state);
    this.right.addItem(this.trg);
    this.right.addItem(this.velo);
    this.right.addItem(this.shift);
    this.right.addItem(this.plan);
  };
  return table;
};

var lumi_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var lumi_body = function()   { return lhcb.widgets.SubdetectorPage.start(LumiStatus); };
