_loadScript('lhcb.display.tooltips.cpp');
_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.constants.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** VELO web widgets
 *
 *  @author  M.Frank
 */
var VeloStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('VELO Operation State');

  table.options = {logo:     lhcb_logo('http://cern.ch/lhcb-vd/assets/images/Velo.jpg'),
        	   logo_url: 'http://cern.ch/lhcb-public/en/Detector/VELO-en.html',
		   title:    'VELO Detector Status',
		   tips:     'VELO operations status:<br>-- High/Low voltage status<br>-- Position<br>-- Move state<br>-- Infrastructure'
  };

  table.attachWidgets = function() {
    var opts = {style:'Arial12pt',legend:true,logger:this.logger};
    this.veloVoltages       = this.hvSummary({hv:true,lv:true});
    this.veloLV             = lhcb.widgets.velo.LVBoards(opts);
    this.lhc_state          = lhcb.widgets.LHCStateSummary(opts);
    this.veloPosition       = lhcb.widgets.velo.positionSummary(opts);
    this.veloMoving         = lhcb.widgets.velo.moveSummary({legend:true,logger:this.logger});
    this.veloVacuum         = lhcb.widgets.velo.vacuumSummary(opts);
    this.veloInfrastructure = lhcb.widgets.velo.infrastructureSummary(opts);
    this.veloDAQ            = lhcb.widgets.SystemSummary({style:'Arial12pt',
								  system: 'VELO',
								  title: 'VELO Subsystem State',
								  split: true,
								  items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
								  legend:true,
								  rowing:true,
								  logger:this.logger});

    this.left.addItem(this.veloVoltages);
    this.left.addItem(this.veloLV);

    this.right.addItem(this.lhc_state);
    this.right.addItem(this.veloPosition);
    this.right.addItem(this.veloMoving);
    this.right.addItem(this.veloVacuum);
    this.right.addItem(this.veloInfrastructure);
    this.right.addItem(this.veloDAQ);
  };
  return table;
};

var velo_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var velo_body = function() { return lhcb.widgets.SubdetectorPage.start(VeloStatus); };
