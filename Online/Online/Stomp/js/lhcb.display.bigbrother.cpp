_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');

function _loadWidgets(){
  /*
    tab.intensity2    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam2.averageBeamIntensity',null,'%9.2e');
    tab.lumiInst      = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_GP',null,"%9.2e");
    tab.lumiDel       = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntDel_GP',null,"%9.2e");
    tab.lumiRec       = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_GP',null,"%9.2e");
  */
  lhcb.widgets.fillTimes = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');
    var cell;

    tooltips.set(tab,'Current fill statistics<br>Click to see LHC status');
    tab.onclick = function() { document.location = lhcb.constants.lhcb_display_url("lhc");};

    tab.className = tb.className   = 'MonitorPage';
    tb.style.width = tb.style.height = '100%';
    if ( options.fontSize ) {
      tab.style.fontSize = tb.style.fontSize = options.fontSize;
    }
    if ( options.style ) {
      tr.appendChild(Cell('Fill Status',4,options.style));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    tr.appendChild(Cell('Start', null,'MonitorDataHeader'));
    tr.appendChild(tab.startTime=Cell('', null,null));
    tr.appendChild(Cell('Beam1', null,null));
    tr.appendChild(Cell('Beam2', null,null));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Ramped',null,'MonitorDataHeader'));
    tr.appendChild(tab.rampedTime=Cell('', null,null));
    tr.appendChild(tab.rampedIntB1=Cell('', null,null));
    tr.appendChild(tab.rampedIntB2=Cell('', null,null));
    tab.rampedTime.style.width='35%';
    tab.rampedIntB1.style.width='20%';
    tab.rampedIntB2.style.width='20%';
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Stable',null,'MonitorDataHeader'));
    tr.appendChild(tab.stableTime=Cell('', null,null));
    tr.appendChild(tab.stableIntB1=Cell('', null,null));
    tr.appendChild(tab.stableIntB2=Cell('', null,null));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Now',   null,'MonitorDataHeader'));
    tr.appendChild(tab.nowTime=Cell('', null,null));
    tr.appendChild(tab.nowIntB1=Cell('', null,null));
    tr.appendChild(tab.nowIntB2=Cell('', null,null));
    tb.appendChild(tr);

    tab.appendChild(tb);

    tab.fillTimes       = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam1.averageBeamIntensity',null,null);
    tab.fillIntensities = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam1.averageBeamIntensity',null,'%9.2e');
    tab.fillTimes.target = this;
    tab.fillIntensities.target = this;
    tab.fillTimes.display = function(data) {
      var v = data[1].split(' | ',20);
      if ( v.length > 3 ) {
      }
    };
    tab.fillIntensities.display = function(data) {
      var v = data[1].split(' | ',20);
      if ( v.length > 3 ) {
      }
    };
    tab.subscribe = function(provider) {
      provider.subscribeItem(this.fillTimes);
      provider.subscribeItem(this.fillIntensities);
    };

    return tab;
  };
  lhcb.widgets.fillLuminosity = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');
    var cell;

    tooltips.set(tab,'Current Luminosity statistics<br>Click to see LHC status');
    tab.onclick = function() { document.location = lhcb.constants.lhcb_display_url("lhc");};

    tab.className = tb.className   = 'MonitorPage';
    tb.style.width = tb.style.height = '100%';
    if ( options.fontSize ) {
      tab.style.fontSize = tb.style.fontSize = options.fontSize;
    }
    if ( options.style ) {
      tr.appendChild(Cell('Luminosity Statistics',2,options.style));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    tab.lumiNOW  = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_GP','Text-Right','%9.3f &mu;b<sup>-1</sup>/s');
    tab.lumiFILL = StyledItem('lbWeb.LHCbEfficiency.LumiTotal','Text-Right','%9.3f &mu;b<sup>-1</sup>/s');

    tr.appendChild(Cell('Luminosity NOW', null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiNOW);
    tab.lumiNOW.style.width='40%';
    tb.appendChild(tr);
    /*
    tr = document.createElement('tr');
    tr.appendChild(Cell('Last 30 min.',null,'MonitorDataHeader'));
    tr.appendChild(tab.lumi30min=Cell('', null,null));
    tb.appendChild(tr);
    */
    tr = document.createElement('tr');
    tr.appendChild(Cell('Fill from start',null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiFILL);
    tb.appendChild(tr);

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.lumiNOW);
      provider.subscribeItem(this.lumiFILL);
    };
    return tab;
  };


  lhcb.widgets.interactionRates = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');
    var cell;

    tooltips.set(tab,'Current Interaction rate statistics<br>Click to see LHC status');
    tab.onclick = function() { document.location = lhcb.constants.lhcb_display_url("lhc");};
    tab.className = tb.className = 'MonitorPage';
    tb.style.width = tb.style.height = '100%';

    tab.lumiPP     = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_hadron',   'Text-Right','%7.3f&nbsp;');
    tab.lumiPPrec  = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_hadron', 'Text-Right','%7.3f&nbsp;');
    tab.lumiHad    = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_pp_rate',  'Text-Right','%7.3f&nbsp;');
    tab.lumiHadrec = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_pp_rate','Text-Right','%7.3f&nbsp;');
    tab.lumiSMu    = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_smu',      'Text-Right','%7.3f&nbsp;');
    tab.lumiSMurec = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_smu',    'Text-Right','%7.3f&nbsp;');
    tab.lumiDMu    = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_dimu',     'Text-Right','%7.3f&nbsp;');
    tab.lumiDMurec = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_dimu',   'Text-Right','%7.3f&nbsp;');
    tab.lumiGP     = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_GP',       'Text-Right','%7.3f&nbsp;');
    tab.lumiGPrec  = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_GP',     'Text-Right','%7.3f&nbsp;');

    if ( options.fontSize )
      tab.style.fontSize = tb.style.fontSize = options.fontSize;
    if ( options.style )
      tr.appendChild(Cell('Interaction Rates',null,options.style));
    else
      tr.appendChild(Cell('', null,null));

    tr.appendChild(cell=Cell('Inst [&mu;b<sup>-1</sup>/s]&nbsp;&nbsp;&nbsp;', null,'MonitorDataHeader'));
    cell.style.textAlign='right';
    tr.appendChild(cell=Cell('Rec [&mu;b<sup>-1</sup>]&nbsp;&nbsp;&nbsp;', null,'MonitorDataHeader'));
    cell.style.textAlign='right';
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('pp Inelastic', null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiPP);
    tr.appendChild(tab.lumiPPrec);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Hadron', null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiHad);
    tr.appendChild(tab.lumiHadrec);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Single Muon', null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiSMu);
    tr.appendChild(tab.lumiSMurec);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Di-Muon', null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiDMu);
    tr.appendChild(tab.lumiDMurec);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('GP', null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiGP);
    tr.appendChild(tab.lumiGPrec);
    tb.appendChild(tr);


    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.lumiPP);
      provider.subscribeItem(this.lumiPPrec);
      provider.subscribeItem(this.lumiHad);
      provider.subscribeItem(this.lumiHadrec);
      provider.subscribeItem(this.lumiSMu);
      provider.subscribeItem(this.lumiSMurec);
      provider.subscribeItem(this.lumiDMu);
      provider.subscribeItem(this.lumiDMurec);
      provider.subscribeItem(this.lumiGP);
      provider.subscribeItem(this.lumiGPrec);
    };
    return tab;
  };

  lhcb.widgets.efficiencyMatrix = function(options) {
    var tb, tr, tab = document.createElement('table');
    var type = 'Lumi';
    tb = document.createElement('tbody');
    
    tooltips.set(tb,'Run efficiency summary normalized to<br> '+type+' of datataking during this fill.');
    tab.className  = tb.className  = 'MonitorPage';

    if ( options.fontSize ) {
      tab.style.fontSize = tb.style.fontSize = options.fontSize;
    }
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Experiment efficiency',3,options.style));
      tb.appendChild(tr);
    }

    tab.lumiGPrec  = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_GP',null,'%7.3f');
    tab.lumiGPdel  = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntDel_GP',null,'%7.3f');
    tab.eff_HV     = StyledItem('lbWeb.LHCbEfficiency.ResultsLumi.HV',null,'%7.2f %%');
    tab.eff_VELO   = StyledItem('lbWeb.LHCbEfficiency.ResultsLumi.VELO',null,'%7.2f %%');
    tab.eff_DAQ    = StyledItem('lbWeb.LHCbEfficiency.ResultsLumi.DAQ',null,'%7.2f %%');
    tab.eff_DEAD   = StyledItem('lbWeb.LHCbEfficiency.ResultsLumi.DAQLiveTime',null,'%7.2f %%');
    tab.eff_total  = StyledItem('lbWeb.LHCbEfficiency.LumiTotal',null,'%7.2f %%');

    tab.update_lumi = function(data) {
      this.data=parseFloat(data[1]);
      this.target.updateMatrix();
    };
    tab.lumiGPrec.target = tab.eff_HV.target = tab.eff_VELO.target = tab.eff_DAQ.target = tab.eff_DEAD.target = tab;
    tab.lumiGPrec.display = tab.eff_HV.display = tab.eff_VELO.display = tab.eff_DAQ.display = tab.eff_DEAD.display = tab.update_lumi;

    tab.updateMatrix = function() {
      var val0, val1, val2, val3;
      
      //alert('this.eff_HV.data:'+this.eff_HV.data+'\nthis.lumiGPrec.data:'+this.lumiGPrec.data);      
      /*
      this.eff_HV.data = 0.1;
      this.eff_VELO.data = 0.05;
      this.eff_DAQ.data = 0.01;
      this.eff_DEAD.data = 0.005;
      this.lumiGPrec.data = 10;
      */
      if ( ''+this.lumiGPrec.data ) {
	this.lumi_DELIVERED.innerHTML = sprintf(this.lumiGPrec.format,this.lumiGPrec.data);
	if ( ''+this.eff_HV.data ) {
	  //if ( this.eff_HV.data==0 ) this.eff_HV.data = 1;
	  val0 = 1.-this.eff_HV.data;
	  this.eff_HV.innerHTML = sprintf('%7.2f %%',(1.-val0)*100.);
	  this.lumi_HV.innerHTML = sprintf(this.lumiGPrec.format,val0*this.lumiGPrec.data);

	  if ( ''+this.eff_VELO.data ) {
	    //if ( this.eff_VELO.data==0 ) this.eff_VELO.data = 1;
	    val1 = 1.-this.eff_VELO.data;
	    val0 = val0*val1;
	    this.eff_VELO.innerHTML = sprintf('%7.2f %%',(1.-val1)*100.);
	    this.eff_VELO_HV.innerHTML = sprintf('%7.2f %%',(1.-val0)*100.);
	    this.lumi_VELO.innerHTML = sprintf(this.lumiGPrec.format,val0*this.lumiGPrec.data);

	    if ( ''+this.eff_DAQ.data ) {
	      //if ( this.eff_DAQ.data==0 ) this.eff_DAQ.data = 1;
	      val2 = 1.-this.eff_DAQ.data;
	      val1 = val1*val2;
	      val0 = val0*val2;
	      this.eff_DAQ_VELO_HV.innerHTML = sprintf('%7.2f %%',(1.-val0)*100.);
	      this.eff_DAQ_VELO.innerHTML = sprintf('%7.2f %%',(1.-val1)*100.);
	      this.eff_DAQ.innerHTML = sprintf('%7.2f %%',(1.-val2)*100.);
	      this.lumi_DAQ.innerHTML = sprintf(this.lumiGPrec.format,val0*this.lumiGPrec.data);

	      if ( ''+this.eff_DEAD.data ) {
		//if ( this.eff_DEAD.data==0 ) this.eff_DEAD.data = 1;
		val3 = 1.-this.eff_DEAD.data;
		val2 = val2*val3;
		val1 = val1*val3;
		val0 = val0*val3;
		this.lumi_DEAD.innerHTML = sprintf(this.lumiGPrec.format,val0*this.lumiGPrec.data);
		this.eff_DEAD_DAQ_VELO_HV.innerHTML = sprintf('%7.2f %%',(1.-val0)*100.);
		this.eff_DEAD_DAQ_VELO.innerHTML = sprintf('%7.2f %%',(1.-val1)*100.);
		this.eff_DEAD_DAQ.innerHTML = sprintf('%7.2f %%',(1.-val2)*100.);
		this.eff_DEAD.innerHTML = sprintf('%7.2f %%',(1.-val3)*100.);
	      }
	    }
	  }
	}
      }
    };

    tr = document.createElement('tr');
    tr.appendChild(Cell('Delivered',1,'MonitorDataHeader'));
    tr.appendChild(tab.lumi_DELIVERED=Cell('',1,null));
    tr.appendChild(Cell('&mu;b<sup>-1</sup>',1,null));
    tr.appendChild(Cell('',5,null));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('HV Ready',1,'MonitorDataHeader'));
    tr.appendChild(tab.lumi_HV=Cell('',1,null));
    tr.appendChild(Cell('&mu;b<sup>-1</sup>',1,null));
    tr.appendChild(tab.eff_HV);
    tr.appendChild(Cell('&lt;---  Detector operation inefficiency',4,'MonitorDataHeader'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('VELO IN',1,'MonitorDataHeader'));
    tr.appendChild(tab.lumi_VELO=Cell('',1,null));
    tr.appendChild(Cell('&mu;b<sup>-1</sup>',1,null));
    tr.appendChild(tab.eff_VELO_HV=Cell('',1,null));
    tr.appendChild(tab.eff_VELO);
    tr.appendChild(Cell('&lt;--- VELO operation inefficiency',3,'MonitorDataHeader'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Running',1,'MonitorDataHeader'));
    tr.appendChild(tab.lumi_DAQ=Cell('',1,null));
    tr.appendChild(Cell('&mu;b<sup>-1</sup>',1,null));
    tr.appendChild(tab.eff_DAQ_VELO_HV=Cell('',1,null));
    tr.appendChild(tab.eff_DAQ_VELO=Cell('',1,null));
    tr.appendChild(tab.eff_DAQ);
    tr.appendChild(Cell('&lt;--- DAQ inefficiency',2,'MonitorDataHeader'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Recorded',1,'MonitorDataHeader'));
    //tr.appendChild(tab.lumiGPrec);
    tr.appendChild(tab.lumi_DEAD=Cell('',1,null));
    tr.appendChild(Cell('&mu;b<sup>-1</sup>',1,null));
    tr.appendChild(tab.eff_DEAD_DAQ_VELO_HV=Cell('',1,null));
    tr.appendChild(tab.eff_DEAD_DAQ_VELO=Cell('',1,null));
    tr.appendChild(tab.eff_DEAD_DAQ=Cell('',1,null));
    tr.appendChild(tab.eff_DEAD);
    tr.appendChild(Cell('&lt;--- Dead time',1,'MonitorDataHeader'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tab.appendChild(tb);
    
    tab.subscribe = function(provider) {
      provider.subscribeItem(this.lumiGPrec);
      provider.subscribeItem(this.lumiGPdel);
      provider.subscribeItem(this.eff_HV);
      provider.subscribeItem(this.eff_VELO);
      provider.subscribeItem(this.eff_DAQ);
      provider.subscribeItem(this.eff_DEAD);
      provider.subscribeItem(this.eff_total);
    };
    return tab;
  };

};


/** BIGBROTHER web widget
 *
 *  @author  M.Frank
 */
var BigBrother = function(msg)   {
  var table = lhcb.widgets.SubdetectorPage('Fill and Performance Status');
  _loadWidgets();

  table.options = {logo:    '',
        	   logo_url:null,
		   title:   'Fill and Performance Status',
		   tips:    'LHCb Fill and Performance status<br>'
  };


  table.beforeComment = function() {
    var tab, tb, td, tr = document.createElement('tr');
    var opts = {system:'LHCb',style:'Arial12pt',legend:true,logger:this.logger,fontSize:'90%'};
    var fillTimes   = lhcb.widgets.fillTimes({legend:true,logger:this.logger,fontSize:'90%'});
    var fillLumi    = lhcb.widgets.fillLuminosity(opts);
    var interactionRates = lhcb.widgets.interactionRates(opts);
    var effi_matrix = lhcb.widgets.efficiencyMatrix(opts);

    tr.vAlign='top';
    td = document.createElement('td');
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(fillTimes);
    td = document.createElement('td');
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(fillLumi);
    td = document.createElement('td');
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(interactionRates);

    tab = document.createElement('table');
    tab.appendChild(tb = document.createElement('tbody'));    
    tab.style.width=tb.style.width='100%';
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(td=document.createElement('td'));
    td.colSpan = 3;
    td.appendChild(effi_matrix);
    tb.appendChild(tr);


    tr.appendChild(td);

    tr = document.createElement('tr');
    tr.appendChild(td=document.createElement('td'));
    td.colSpan = 2;
    td.appendChild(tab);

    this.tbody.appendChild(tr);
    this.subscriptions.push(fillTimes);
    this.subscriptions.push(fillLumi);
    this.subscriptions.push(interactionRates);
    this.subscriptions.push(effi_matrix);
  };
  
  /*
  table.attachWidgets = function() {
    var td, tr, tb, tab;
    var opts = {system:'LHCb',style:'Arial12pt',legend:true,logger:this.logger,fontSize:'90%'};
    var lhc_state   = lhcb.widgets.LHCStateSummary(opts);

    var effi_1      = lhcb.widgets.DAQEfficiency(opts);
    opts.efficiency_type = 'Time';
    var effi_2      = lhcb.widgets.DAQEfficiency(opts);

    this.left.addSpacer('50px');

    this.left.addItem(effi_1);
    this.left.addItem(effi_2);

    this.right.addSpacer('50px');


    this.right.addItem(lhc_state);
  };
  */
  return table;
};

var bigbrother_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var bigbrother_body = function()   { return lhcb.widgets.SubdetectorPage.start(BigBrother); };
