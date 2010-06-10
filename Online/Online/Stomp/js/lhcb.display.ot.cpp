_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** OT web widgets
 *
 *  @author  M.Frank
 */
var OtStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('OT Operation State');

  table.options = {logo:    lhcb_logo('http://cern.ch/lhcb-public/Objects/Detector/Tracking-system-diagram-2.jpg'),
        	   logo_url:'http://cern.ch/lhcb-public/en/Detector/Trackers-en.html',
		   title:   'OT Detector Status',
		   tips:    'OT operations status:<br>-- High/Low voltage status<br>-- Tell1 Board Status<br>-- Infrastructure'
  };


  lhcb.widgets.ot = new Object();

  /** OT web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.ot.Systems = function(options) {
    var tab = lhcb.widgets.SubdetectorSystem(options);
    tab.addStations = function(title,id) {
      var tb = this.body(title);
      tb.appendChild(tab.addFSMRow(['OTA_'+id+'_T1', 'OTC_'+id+'_T1']));
      tb.appendChild(tab.addFSMRow(['OTA_'+id+'_T2', 'OTC_'+id+'_T2']));
      tb.appendChild(tab.addFSMRow(['OTA_'+id+'_T3', 'OTC_'+id+'_T3']));
      return this;
    };
    tab.addHV = function() { return this.addStations('High Voltage per Station','HV'); };
    tab.addLV = function() { return this.addStations('Low Voltage per Station', 'DCS_LV'); };
    tab.addFE = function() { return this.addStations('Front end (FE) Status per Station','DAQ_FEE'); };
    tab.addTell1 = function() { return this.addStations('TELL1 Board Status per Station','DAQ_TELL1'); };
    return tab;
  };

  table.attachWidgets = function() {
    var opts = {style:'Arial12pt',legend:true,logger:this.logger};
    var otDAQ = lhcb.widgets.SystemSummary({style:'Arial12pt',
						    system: 'OT',
						    title: 'OT Subsystem State',
						    split: true,
						    items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
						    legend:true,
						    rowing:true,
						    logger:this.logger});
    this.left.addItem(lhcb.widgets.LHCStateSummary(opts));
    this.left.addItem(this.hvSummary({hv:true,lv:false}));
    this.left.addItem(lhcb.widgets.ot.Systems(opts).addHV());
    this.left.addItem(lhcb.widgets.ot.Systems(opts).addLV());
    this.left.addItem(lhcb.widgets.ot.Systems(opts).addTell1());
    this.left.addItem(lhcb.widgets.ot.Systems(opts).addFE());
    //this.left.addSpacer('30px');

    this.right.addItem(lhcb.widgets.DSS.MixedWaterStatus(opts));
    this.right.addItem(lhcb.widgets.DSS.CaVPlantStatus('Ot',opts));
    this.right.addItem(otDAQ);
  };
  return table;
};

var ot_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var ot_body = function()   { return lhcb.widgets.SubdetectorPage.start(OtStatus); };
