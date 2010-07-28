_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** COOLING web widgets
 *
 *  @author  M.Frank
 */
var CoolingStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('Cooling Plant State');

  table.options = {logo:    null,
        	   logo_url:null,
		   title:   'Cooling Plant Status',
		   tips:    'Cooling Plant status'
  };

  table.attachWidgets = function() {
    var opts = {style:'Arial12pt',legend:true,logger:this.logger};
    this.left.addItem(lhcb.widgets.DSS.CaVPlantStatus('It',opts));
    this.left.addItem(lhcb.widgets.DSS.CaVPlantStatus('Tt',opts));
    this.left.addItem(lhcb.widgets.DSS.MixedWaterStatus(opts));
    this.left.addSpacer('30px');

    opts.showStatus = true;
    this.right.addItem(lhcb.widgets.DSS.CaVPlantStatus('Ot',opts));
    this.right.addItem(lhcb.widgets.DSS.CaVPlantStatus('Rich',opts));
  };
  return table;
};

var cooling_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var cooling_body = function()   { return lhcb.widgets.SubdetectorPage.start(CoolingStatus); };
