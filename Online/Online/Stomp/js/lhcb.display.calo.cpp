_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


/** CALO web widgets
 *
 *  @author  M.Frank
 */
var CaloStatus = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('CALO Operation State');

  table.options = {logo:     null, //lhcb_logo('http://lbtwiki.cern.ch/pub/CALO/WebHome/LOGO.png'),
        	   logo_url: 'http://cern.ch/lhcb-public/en/Detector/Calorimeters-en.html',
		   title:    'CALO Detector Status',
		   tips:     'CALO operations status:<br>-- High/Low voltage status<br>-- Infrastructure'
  };


  lhcb.widgets.calo = new Object();

  /** CALO web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.calo.Systems = function(options) {
    var tab = lhcb.widgets.SubdetectorSystem(options);
    tab.caloLV = function() {
      var tb = this.body('Calorimeter Low Voltage Status');
      tb.appendChild(this.addFSMRow(['ECAL_DCS_LV_A',   'ECAL_DCS_LV_C']));
      tb.appendChild(this.addFSMRow(['HCAL_DCS_LV_A',   'HCAL_DCS_LV_C']));
      tb.appendChild(this.addFSMRow(['PRS_DCS_LV_A',    'PRS_DCS_LV_C']));
      tb.appendChild(this.addFSMRow(['PS_DCS_VFE_LV_A', 'PS_DCS_VFE_LV_C']));
      tb.appendChild(this.addFSMRow(['SPD_DCS_VFE_LV_A','SPD_DCS_VFE_LV_C']));
      return this;
    };
    tab.caloMV = function() {
      var tb = this.body('Calorimeter Voltage Status');
      tb.appendChild(this.addFSMRow(['ECAL_MV_A',   'ECAL_MV_C']));
      tb.appendChild(this.addFSMRow(['HCAL_MV_A',   'HCAL_MV_C']));
      tb.appendChild(this.addFSMRow(['PRS_MV_A',    'PRS_MV_C']));
      return this;
    };
    tab.ecalHV = function() {
      var tb = this.body('ECAL High Voltage Status');
      tb.appendChild(this.addFSMRow(['ECAL_HV_A_OT', 'ECAL_HV_C_OT']));
      tb.appendChild(this.addFSMRow(['ECAL_HV_A_MD', 'ECAL_HV_C_MD']));
      tb.appendChild(this.addFSMRow(['ECAL_HV_A_IN', 'ECAL_HV_C_IN']));
      tb.appendChild(this.addFSMRow(['ECAL_INTEG_A', 'ECAL_INTEG_C']));
      return this;
    };
    tab.ecalTell1 = function() {
      var tb = this.body('ECAL Tell1 Board Status');
      tb.appendChild(this.addFSMRow(['ECTELL14', 'ECTELL01']));
      tb.appendChild(this.addFSMRow(['ECTELL15', 'ECTELL02']));
      tb.appendChild(this.addFSMRow(['ECTELL16', 'ECTELL03']));
      tb.appendChild(this.addFSMRow(['ECTELL17', 'ECTELL04']));
      tb.appendChild(this.addFSMRow(['ECTELL18', 'ECTELL05']));
      tb.appendChild(this.addFSMRow(['ECTELL19', 'ECTELL06']));
      tb.appendChild(this.addFSMRow(['ECTELL20', 'ECTELL07']));
      tb.appendChild(this.addFSMRow(['ECTELL21', 'ECTELL08']));
      tb.appendChild(this.addFSMRow(['ECTELL22', 'ECTELL09']));
      tb.appendChild(this.addFSMRow(['ECTELL23', 'ECTELL10']));
      tb.appendChild(this.addFSMRow(['ECTELL24', 'ECTELL11']));
      tb.appendChild(this.addFSMRow(['ECTELL25', 'ECTELL12']));
      tb.appendChild(this.addFSMRow(['ECTELL26', 'ECTELL13']));
      return this;
    };
    tab.hcalHV = function() {
      var tb = this.body('HCAL High Voltage Boards');
      tb.appendChild(this.addFSMRow(['HCAL_HVboard1', 'HCAL_HVboard5']));
      tb.appendChild(this.addFSMRow(['HCAL_HVboard2', 'HCAL_HVboard6']));
      tb.appendChild(this.addFSMRow(['HCAL_HVboard3', 'HCAL_HVboard7']));
      tb.appendChild(this.addFSMRow(['HCAL_HVboard4', 'HCAL_HVboard8']));
      return this;
    };
    tab.hcalTell1 = function() {
      var tb = this.body('HCAL Tell1 Board Status');
      tb.appendChild(this.addFSMRow(['HCTELL05', 'HCTELL01']));
      tb.appendChild(this.addFSMRow(['HCTELL06', 'HCTELL02']));
      tb.appendChild(this.addFSMRow(['HCTELL07', 'HCTELL03']));
      tb.appendChild(this.addFSMRow(['HCTELL08', 'HCTELL04']));
      return this;
    };
    tab.prsTell1 = function() {
      var tb = this.body('PRS Tell1 Board Status');
      tb.appendChild(this.addFSMRow(['PSTELL05', 'PSTELL01']));
      tb.appendChild(this.addFSMRow(['PSTELL06', 'PSTELL02']));
      tb.appendChild(this.addFSMRow(['PSTELL07', 'PSTELL03']));
      tb.appendChild(this.addFSMRow(['PSTELL08', 'PSTELL04']));
      return this;
    };
    tab.prsDCSST = function() {
      var tb = this.body('PRS DCS Stations');
      tb.appendChild(this.addFSMRow(['PSDCST1_A', 'PSDCST1_C']));
      return this;
    };
    return tab;
  };

  table.beforeComment = function() {
    var opts = {style:'Arial12pt',legend:true,calo:true,logger:this.logger};
    this.ecalDAQ = lhcb.widgets.SystemSummary({system: 'ECAL',
							 title: 'ECAL Subsystem State',
							 split: false,
							 items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
							 legend:true,
							 rowing:true,
							 logger:this.logger});
    
    this.hcalDAQ = lhcb.widgets.SystemSummary({system: 'HCAL',
							 title: 'HCAL Subsystem State',
							 split: false,
							 items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
							 legend:true,
							 rowing:true,
							 logger:this.logger});
    this.prsDAQ  = lhcb.widgets.SystemSummary({system: 'PRS',
							 title: 'PRS Subsystem State',
							 split: false,
							 items: ['DCS','DAI','DAQ','RunInfo','HLT','TFC','Storage','Monitoring'],
							 legend:true,
							 rowing:true,
							 logger:this.logger});
    
    var td, tr, tab, tb, tr0;
    tab = document.createElement('table');
    tb = document.createElement('tbody');
    tr0 = document.createElement('tr');
    td0 = document.createElement('td');

    this.addRow([this.hvSummary({system:'ECAL',hv:true,lv:false,nosplit:true}),
		 this.hvSummary({system:'HCAL',hv:true,lv:false,nosplit:true})]);
    this.addRow([lhcb.widgets.calo.Systems(opts).ecalHV(),
		 lhcb.widgets.calo.Systems(opts).hcalHV()]);
    this.addRow([lhcb.widgets.calo.Systems(opts).caloLV(),
		 lhcb.widgets.calo.Systems(opts).caloMV()]);
    this.addRow([lhcb.widgets.calo.Systems(opts).ecalTell1(),
		 [lhcb.widgets.calo.Systems(opts).hcalTell1(),
		  lhcb.widgets.calo.Systems(opts).prsTell1(),
		  lhcb.widgets.calo.Systems(opts).prsDCSST()]]);

    this.tbody.appendChild(tr0);
    td0.appendChild(tab);
    td0.colSpan = 2;
    tr0.appendChild(td0);
    tab.appendChild(tb);

    tr = document.createElement('tr');
    tr.appendChild(td=Cell('ECAL',1,'MonitorDataHeader'));
    td.style.width = '38%';
    tr.appendChild(td=Cell('HCAL',1,'MonitorDataHeader'));
    td.style.width = '38%';
    tr.appendChild(td=Cell('PRS',1,'MonitorDataHeader'));
    td.style.width = '50%';
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.ecalDAQ);
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.hcalDAQ);
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.prsDAQ);
    tb.appendChild(tr);

    this.subscriptions.push(this.prsDAQ);
    this.subscriptions.push(this.ecalDAQ);
    this.subscriptions.push(this.hcalDAQ);
  };

  table.attachWidgets = function() {
    //var opts = {style:'Arial12pt',legend:true,calo:true,logger:this.logger};
    //this.right.addItem(lhcb.widgets.LHCStateSummary(opts));
  };
  return table;
};

var calo_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var calo_body = function() { return lhcb.widgets.SubdetectorPage.start(CaloStatus); };
