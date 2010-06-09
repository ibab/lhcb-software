_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');

function _loadWidgets() {

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
      tr.appendChild(Cell('Fill Status',2,options.style));
      tr.appendChild(Cell('Beam Intensities',2,'MonitorDataHeader'));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }

    tab.currFill       = StyledItem('lbWeb.LHCbPerformance.CurrentFill',null,null);
    tab.currFillStart  = StyledItem('lbWeb.LHCbPerformance.CurrentFillStart',null,null);
    tab.currFillRamped = StyledItem('lbWeb.LHCbPerformance.CurrentFillRamped',null,null);
    tab.currFillStable = StyledItem('lbWeb.LHCbPerformance.CurrentFillStable',null,null);
    tab.currFillEnded  = StyledItem('lbWeb.LHCbPerformance.CurrentFillEnded',null,null);

    tab.currFill.fillNo  = -1;
    tab.currFill.state   = 'None';
    tab.currFill.parent  = tab;
    tab.currFill.display = function(data) {
      var v = data[1].split('/',20);
      var t = this.parent;
      if ( v.length>3 ) {
	t.currFill.fillNo    = parseInt(v[0]);
	t.currFill.state     = v[8];
	tab.now.innerHTML    = 'Now ('+t.currFill.fillNo+')';
	t.nowTime.innerHTML  = v[1].substr(v[1].indexOf('-')+1);
	t.nowIntB1.innerHTML = sprintf('%7.2e',parseFloat(v[9]));
	t.nowIntB2.innerHTML = sprintf('%7.2e',parseFloat(v[10]));

      }
    };
    tab.currFillStart.parent = tab;
    tab.currFillStart.display = function(data) {
      var v = data[1].split('/',20);
      if ( v.length>0 ) this.parent.startTime.innerHTML = v[1].substr(v[1].indexOf('-')+1);;
    };
    tab.currFillRamped.parent = tab;
    tab.currFillRamped.display = function(data) {
      var v = data[1].split('/',20);
      var t = this.parent;
      if ( v.length>9 ) {
	t.rampedTime.colSpan = 1;
	t.rampedTime.innerHTML  = v[1].substr(v[1].indexOf('-')+1);
	t.rampedIntB1.innerHTML = sprintf('%7.2e',parseFloat(v[9]));
	t.rampedIntB2.innerHTML = sprintf('%7.2e',parseFloat(v[10]));
      }
      else {
	var fillNo = parseInt(v[0]);
	t.rampedTime.colSpan = 3;
	t.rampedTime.innerHTML = ' Fill '+t.currFill.fillNo+' was never ramped';
      }
    };
    tab.currFillStable.parent = tab;
    tab.currFillStable.display = function(data) {
      var v = data[1].split('/',20);
      var t = this.parent;
      if ( v.length>9 ) {
	t.stableTime.colSpan = 1;
	t.stableTime.innerHTML  = v[1].substr(v[1].indexOf('-')+1);
	t.stableIntB1.innerHTML = sprintf('%7.2e',parseFloat(v[9]));
	t.stableIntB2.innerHTML = sprintf('%7.2e',parseFloat(v[10]));
      }
      else {
	var fillNo = parseInt(v[0]);
	t.stableTime.colSpan = 3;
	t.stableTime.innerHTML = ' Fill '+t.currFill.fillNo+' was never stable';
	t.stableIntB1.innerHTML= '';
      }
    };

    tab.currFillEnded.parent = tab;
    tab.currFillEnded.display = function(data) {
      var v = data[1].split('/',20);
      var t = this.parent;
      if ( v.length>9 ) {
	t.endedTime.colSpan = 1;
	t.endedTime.innerHTML  = v[1].substr(v[1].indexOf('-')+1);
	t.endedIntB1.innerHTML = sprintf('%7.2e',parseFloat(v[9]));
	t.endedIntB2.innerHTML = sprintf('%7.2e',parseFloat(v[10]));
      }
      else {
	var fillNo = parseInt(v[0]);
	t.endedTime.colSpan = 3;
	if ( fillNo < t.currFill.fillNo )
  	  t.endedTime.innerHTML = ' Fill '+t.currFill.fillNo+' in preparation';
	else if ( t.stableIntB1.innerHTML=='' )
  	  t.endedTime.innerHTML = ' Fill '+t.currFill.fillNo+' in preparation';
	else
	  t.endedTime.innerHTML = ' Fill '+t.currFill.fillNo+' still running';
      }
    };
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
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Stable',null,'MonitorDataHeader'));
    tr.appendChild(tab.stableTime=Cell('', null,null));
    tr.appendChild(tab.stableIntB1=Cell('', null,null));
    tr.appendChild(tab.stableIntB2=Cell('', null,null));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Ended',   null,'MonitorDataHeader'));
    tr.appendChild(tab.endedTime=Cell('', null,null));
    tr.appendChild(tab.endedIntB1=Cell('', null,null));
    tr.appendChild(tab.endedIntB2=Cell('', null,null));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(tab.now=Cell('Now',   null,'MonitorDataHeader'));
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
      provider.subscribeItem(this.currFill);
      provider.subscribeItem(this.currFillStart);
      provider.subscribeItem(this.currFillRamped);
      provider.subscribeItem(this.currFillStable);
      provider.subscribeItem(this.currFillEnded);
      //provider.subscribeItem(this.fillTimes);
      //provider.subscribeItem(this.fillIntensities);
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
      tr.appendChild(Cell('Luminosity Statistics',3,options.style));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    tab.lumiDEL     = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntDel_GP','Text-Right','%9.3f');
    tab.lumiREC     = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_GP','Text-Right','%9.3f');
    tab.lumiNOW     = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_GP','Text-Right','%9.3f');
    tab.neventsRec  = StyledItem('lbWeb.LHCb_RunInfo.HLTFarm.hltNTriggers','Text-Right',null);
    tab.neventsIn   = StyledItem('lbWeb.LHCb_RunInfo.TFC.nTriggers','Text-Right',null);

    tab.lumi = StyledItem('lbWeb.LHCbPerformance.CurrentLuminosities',null,null);
    tab.lumi.parent = tab;
    tab.lumi.display = function(data) {
      var v = data[1].split('/',20);
      var t = this.parent;
      t.lumiNOW.display(    [22,v[10]]);
      t.lumiREC.display(    [22,v[11]]);
      t.lumiDEL.display(    [22,v[12]]);
    };

    tr.appendChild(Cell('Lumi NOW', null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiNOW);
    //tab.lumiNOW.style.width='40%';
    tr.appendChild(cell=Cell('&mu;b<sup>-1</sup>/s',null,'Text-Left'));
    cell.style.width='25%';
    tb.appendChild(tr);
    /*
    tr = document.createElement('tr');
    tr.appendChild(Cell('Last 30 min.',null,'MonitorDataHeader'));
    tr.appendChild(tab.lumi30min=Cell('', null,null));
    tr.appendChild(Cell('&mu;b<sup>-1</sup>/s',null,'Text-Left'));
    tb.appendChild(tr);
    */
    tr = document.createElement('tr');
    tr.appendChild(Cell('Fill from start',null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiDEL);
    tr.appendChild(Cell('&mu;b<sup>-1</sup>',null,'Text-Left'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Fill recorded',null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiREC);
    tr.appendChild(Cell('&mu;b<sup>-1</sup>',null,'Text-Left'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('No. L&Oslash; events',null,'MonitorDataHeader'));
    tr.appendChild(tab.neventsIn);
    tr.appendChild(Cell('',null,'Text-Left'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Evts.recorded',null,'MonitorDataHeader'));
    tr.appendChild(tab.neventsRec);
    tr.appendChild(Cell('',null,'Text-Left'));
    tb.appendChild(tr);

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      /*
      provider.subscribeItem(this.lumiNOW);
      provider.subscribeItem(this.lumiDEL);
      provider.subscribeItem(this.lumiREC);
      */
      provider.subscribeItem(this.lumi);
      provider.subscribeItem(this.neventsRec);
      provider.subscribeItem(this.neventsIn);
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

    tab.fillNo = -1;
    tab.lumiPP     = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_hadron',   'Text-Right','%7.3f');
    tab.lumiPPrec  = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_hadron', 'Text-Right','%7.3f');
    tab.lumiHad    = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_pp_rate',  'Text-Right','%7.3f');
    tab.lumiHadrec = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_pp_rate','Text-Right','%7.3f');
    tab.lumiSMu    = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_smu',      'Text-Right','%7.3f');
    tab.lumiSMurec = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_smu',    'Text-Right','%7.3f');
    tab.lumiDMu    = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_dimu',     'Text-Right','%7.3f');
    tab.lumiDMurec = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_dimu',   'Text-Right','%7.3f');
    tab.lumiGP     = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_GP',       'Text-Right','%7.3f');
    tab.lumiGPrec  = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_GP',     'Text-Right','%7.3f');
    tab.lumi = StyledItem('lbWeb.LHCbPerformance.CurrentLuminosities',null,null);
    tab.lumi.parent = tab;
    tab.lumi.display = function(data) {
      var v = data[1].split('/',20);
      var t = this.parent;
      t.fillNo = parseInt(v[0]);
      t.lumiPP.display(     [22,v[2]]);
      t.lumiPPrec.display(  [22,v[3]]);
      t.lumiHad.display(    [22,v[4]]);
      t.lumiHadrec.display( [22,v[5]]);
      t.lumiSMu.display(    [22,v[6]]);
      t.lumiSMurec.display( [22,v[7]]);
      t.lumiDMu.display(    [22,v[8]]);
      t.lumiDMurec.display( [22,v[9]]);
      t.lumiGP.display(     [22,v[10]]);
      t.lumiGPrec.display(  [22,v[11]]);
    };


    if ( options.fontSize )
      tab.style.fontSize = tb.style.fontSize = options.fontSize;
    if ( options.style )
      tr.appendChild(Cell('Interaction Rates',null,options.style));
    else
      tr.appendChild(Cell('', null,null));

    tr.appendChild(cell=Cell('Instantaneous', 2,'MonitorDataHeader'));
    cell.style.textAlign='right';
    tr.appendChild(cell=Cell('Recorded', 2,'MonitorDataHeader'));
    cell.style.textAlign='right';
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('pp Inelastic', null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiPP);
    tr.appendChild(Cell('&mu;b<sup>-1</sup>/s', null,'Text-Right'));
    tr.appendChild(tab.lumiPPrec);
    tr.appendChild(Cell('&mu;b<sup>-1</sup>', null,'Text-Right'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Hadron', null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiHad);
    tr.appendChild(Cell('&mu;b<sup>-1</sup>/s', null,'Text-Right'));
    tr.appendChild(tab.lumiHadrec);
    tr.appendChild(Cell('&mu;b<sup>-1</sup>', null,'Text-Right'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Single Muon', null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiSMu);
    tr.appendChild(Cell('&mu;b<sup>-1</sup>/s', null,'Text-Right'));
    tr.appendChild(tab.lumiSMurec);
    tr.appendChild(Cell('&mu;b<sup>-1</sup>', null,'Text-Right'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Di-Muon', null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiDMu);
    tr.appendChild(Cell('&mu;b<sup>-1</sup>/s', null,'Text-Right'));
    tr.appendChild(tab.lumiDMurec);
    tr.appendChild(Cell('&mu;b<sup>-1</sup>', null,'Text-Right'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Best estimate', null,'MonitorDataHeader'));
    tr.appendChild(tab.lumiGP);
    tr.appendChild(Cell('&mu;b<sup>-1</sup>/s', null,'Text-Right'));
    tr.appendChild(tab.lumiGPrec);
    tr.appendChild(Cell('&mu;b<sup>-1</sup>', null,'Text-Right'));
    tb.appendChild(tr);


    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.lumi);
      /*
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
      */
    };
    return tab;
  };

  lhcb.widgets.fillEfficiencyMatrix = function(options) {
    var cell, tb, tr, tab = document.createElement('table');
    var type = 'Lumi';
    tb = document.createElement('tbody');
    
    tooltips.set(tab,'Run efficiency summary normalized to<br> '+type+' of datataking during this fill.');
    tab.className  = tb.className  = 'MonitorPage';

    if ( options.fontSize ) {
      tab.style.fontSize = tb.style.fontSize = options.fontSize;
    }
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Experiment efficiency',3,options.style));
      tb.appendChild(tr);
    }

    tab.lumiGPrec  = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_GP','Text-Right','%7.3f');
    tab.lumiGPdel  = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntDel_GP','Text-Right','%7.3f');
    tab.eff_HV     = StyledItem('lbWeb.LHCbEfficiency.ResultsLumi.HV','Text-Right','%7.2f %%');
    tab.eff_VELO   = StyledItem('lbWeb.LHCbEfficiency.ResultsLumi.VELO','Text-Right','%7.2f %%');
    tab.eff_DAQ    = StyledItem('lbWeb.LHCbEfficiency.ResultsLumi.DAQ','Text-Right','%7.2f %%');
    tab.eff_DEAD   = StyledItem('lbWeb.LHCbEfficiency.ResultsLumi.DAQLiveTime','Text-Right','%7.2f %%');
    tab.eff_total  = StyledItem('lbWeb.LHCbEfficiency.LumiTotal','Text-Right','%7.2f %%');
    tab.lumi = StyledItem('lbWeb.LHCbPerformance.CurrentLuminosities',null,null);

    tab.update_lumi2 = function(data) {
      var v = data[1].split('/',20);
      this.target.lumiGPrec.data = parseFloat(v[11]);
      this.target.lumiGPdel.data = parseFloat(v[12]);
      this.target.updateMatrix();
    };
    tab.lumi.target = tab;
    tab.lumi.display = tab.update_lumi2;

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
    tr.appendChild(cell=Cell('Delivered',1,'MonitorDataHeader'));
    cell.style.width = "30%";
    tr.appendChild(tab.lumi_DELIVERED=Cell('',1,'Text-Right'));
    tr.appendChild(Cell('&mu;b<sup>-1</sup>',1,null));
    tr.appendChild(Cell('',5,null));
    tb.appendChild(tr);

    var left = '&nbsp;&nbsp;&nbsp;<IMG SRC="'+lhcb.constants.lhcb_static('Images/Left.png')+'" HEIGHT="8" WIDTH="120"/>';
    tr = document.createElement('tr');
    tr.appendChild(Cell('&nbsp;HV Ready',1,'MonitorDataHeader'));
    tr.appendChild(tab.lumi_HV=Cell('',1,'Text-Right'));
    tr.appendChild(Cell('&mu;b<sup>-1</sup>',1,null));
    tr.appendChild(tab.eff_HV);
    tr.appendChild(Cell(left+' Detector operation inefficiency',4,'MonitorDataHeader'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('&nbsp;VELO IN',1,'MonitorDataHeader'));
    tr.appendChild(tab.lumi_VELO=Cell('',1,'Text-Right'));
    tr.appendChild(Cell('&mu;b<sup>-1</sup>',1,null));
    tr.appendChild(tab.eff_VELO_HV=Cell('',1,'Text-Right'));
    tr.appendChild(tab.eff_VELO);
    tr.appendChild(cell=Cell(left+' VELO operation inefficiency',3,'MonitorDataHeader'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('&nbsp;Running',1,'MonitorDataHeader'));
    tr.appendChild(tab.lumi_DAQ=Cell('',1,'Text-Right'));
    tr.appendChild(Cell('&mu;b<sup>-1</sup>',1,null));
    tr.appendChild(tab.eff_DAQ_VELO_HV=Cell('',1,'Text-Right'));
    tr.appendChild(tab.eff_DAQ_VELO=Cell('',1,'Text-Right'));
    tr.appendChild(tab.eff_DAQ);
    tr.appendChild(Cell(left+' DAQ inefficiency',2,'MonitorDataHeader'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('&nbsp;Recorded',1,'MonitorDataHeader'));
    //tr.appendChild(tab.lumiGPrec);
    tr.appendChild(tab.lumi_DEAD=Cell('',1,'Text-Right'));
    tr.appendChild(Cell('&mu;b<sup>-1</sup>',1,null));
    tr.appendChild(tab.eff_DEAD_DAQ_VELO_HV=Cell('',1,'Text-Right'));
    tr.appendChild(tab.eff_DEAD_DAQ_VELO=Cell('',1,'Text-Right'));
    tr.appendChild(tab.eff_DEAD_DAQ=Cell('',1,'Text-Right'));
    tr.appendChild(tab.eff_DEAD);
    tr.appendChild(Cell(left+' Dead time',1,'MonitorDataHeader'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tab.appendChild(tb);
    
    tab.subscribe = function(provider) {
      //provider.subscribeItem(this.lumiGPrec);
      //provider.subscribeItem(this.lumiGPdel);
      provider.subscribeItem(this.lumi);
      provider.subscribeItem(this.eff_HV);
      provider.subscribeItem(this.eff_VELO);
      provider.subscribeItem(this.eff_DAQ);
      provider.subscribeItem(this.eff_DEAD);
      provider.subscribeItem(this.eff_total);
    };
    return tab;
  };

  /** Fill history display of the LHCb DAQ for web browsers.

     @author   M.Frank
     @version  1.0
   */
  lhcb.widgets.fillHistory = function(options) {
    var cell, tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');
    
    tooltips.set(tab,'History of recent fills.');
    tab.className  = tb.className  = 'MonitorPage';

    if ( options.fontSize ) {
      tab.style.fontSize = tb.style.fontSize = options.fontSize;
    }
    tab.showInefficiencies = 0;
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Luminosity and performace status of recent fills',3,options.style));
      tr.appendChild(cell=Cell('Show Eff.',1,null));
      cell.parent = tab;
      cell.onclick = function() {
	if ( this.parent.showInefficiencies==0 )
	  this.innerHTML = 'Breakdown';
	else if ( this.parent.showInefficiencies==1 )
	  this.innerHTML = 'Show Ineff.';
	else if ( this.parent.showInefficiencies==2 )
	  this.innerHTML = 'Show Eff.';
	++this.parent.showInefficiencies;
	if ( this.parent.showInefficiencies > 2 )this.parent.showInefficiencies=0;
	this.parent.current.display(this.parent.current.data);
	this.parent.hist.display(this.parent.hist.data);
      };
      cell.style.borderStyle = 'outset';
      cell.style.borderColor = '#999999';
      cell.style.borderWidth='thick';
      tr.appendChild(tab.eff_header=Cell('',4,'MonitorDataHeaderRED'));
      cell.style.textAlign = 'center';
      tr.appendChild(cell=Cell('Infficiencies',2,'MonitorDataHeaderRED'));
      cell.style.textAlign = 'center';
      tb.appendChild(tr);
    }

    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.fillNo = -1;
      tr.appendChild(tr.fill=Cell('Fill&nbsp;',null,'MonitorDataHeaderRight'));
      tr.appendChild(tr.date=Cell('Date&nbsp;',null,'MonitorDataHeaderRight'));
      tr.appendChild(tr.lumi_total=Cell('Lumi Delivered&nbsp;',null,'MonitorDataHeaderRight'));
      tr.appendChild(tr.lumi_logged=Cell('Lumi on Disk&nbsp;',null,'MonitorDataHeaderRight'));
      tr.appendChild(tr.hvon=Cell('% HV ON&nbsp;',null,'MonitorDataHeaderRight'));
      tr.appendChild(tr.veloin=Cell('% Velo IN&nbsp;',null,'MonitorDataHeaderRight'));
      tr.appendChild(tr.running=Cell('% Running&nbsp;',null,'MonitorDataHeaderRight'));
      tr.appendChild(tr.logged=Cell('% Logged&nbsp;',null,'MonitorDataHeaderRight'));
      tr.appendChild(tr.daq=Cell('DAQ&nbsp;',2,'MonitorDataHeaderCenter'));
      tb.appendChild(tr);
    }

    tab.setLine = function(tr, data) {
      var v = data.split('/',20);
      var d = [0.,
	       0.,
	       parseFloat(v[2]),
	       parseFloat(v[3]),
	       parseFloat(v[4]),
	       parseFloat(v[5]),
	       parseFloat(v[6])];
      var tot = d[2]/100.;
      var veloin = d[4];
      if ( tot < 1e-5 ) tot = 1.;

      if ( tr.lineNo != this.currRow.lineNo && parseInt(v[0])==this.currRow.fillNo ) {
	tr.fill.innerHTML='';
	tr.date.innerHTML='';
	tr.lumi_total.innerHTML='';
	tr.lumi_logged.innerHTML='';
	tr.hvon.innerHTML='';
	tr.veloin.innerHTML='';
	tr.running.innerHTML='';
	tr.logged.innerHTML='';
	tr.daq.innerHTML='';
	tr.daq2=innerHTML='';
	return tr;
      }
      tr.fillNo = parseInt(v[0]);
      tr.fill.innerHTML = v[0];
      tr.date.innerHTML = v[1].substr(v[1].indexOf('-')+1);
      tr.lumi_total.innerHTML  = sprintf('%7.2f &mu;b<sup>-1</sup>',d[2]);
      tr.lumi_logged.innerHTML = sprintf('%7.2f &mu;b<sup>-1</sup>',d[6]);

      if ( tab.showInefficiencies==0 ) {
	// For cumulative inefficiencies
	tab.eff_header.innerHTML = 'Cumulative Inefficiencies';
	tr.hvon.innerHTML        = sprintf('%7.2f %%',100.-d[3]/tot);
	tr.veloin.innerHTML      = sprintf('%7.2f %%',100.-d[4]/tot);
	tr.running.innerHTML     = sprintf('%7.2f %%',100.-d[5]/tot);
	tr.logged.innerHTML      = sprintf('%7.2f %%',100.-d[6]/tot);
      }
      else if ( tab.showInefficiencies==1 ) {
	// For cumulative efficiencies
	tab.eff_header.innerHTML = 'Cumulative Efficiencies';
	tr.hvon.innerHTML        = sprintf('%7.2f %%',d[3]/tot);
	tr.veloin.innerHTML      = sprintf('%7.2f %%',d[4]/tot);
	tr.running.innerHTML     = sprintf('%7.2f %%',d[5]/tot);
	tr.logged.innerHTML      = sprintf('%7.2f %%',d[6]/tot);
      }
      else if ( tab.showInefficiencies==2 ) {
	// For cumulative efficiencies
	tab.eff_header.innerHTML = 'Inefficiency breakdown';
	tr.hvon.innerHTML        = sprintf('%7.2f %%',d[2]>1e-9 ? 100*(1.-(d[3]/d[2])): 0.);
	tr.veloin.innerHTML      = sprintf('%7.2f %%',d[3]>1e-9 ? 100*(1.-(d[4]/d[3])): 0.);
	tr.running.innerHTML     = sprintf('%7.2f %%',d[4]>1e-9 ? 100*(1.-(d[5]/d[4])): 0.);
	tr.logged.innerHTML      = sprintf('%7.2f %%',d[5]>1e-9 ? 100*(1.-(d[6]/d[5])): 0.);
      }
      tr.daq.innerHTML         = sprintf('%7.2f %%  /  ',(veloin<1e-9) ? 0. : (1.-(d[5]/veloin))*100.);
      tr.daq2.innerHTML        = sprintf('%7.2f &mu;b<sup>-1</sup>',d[4]-d[5]);
      return tr;
    };
    tab.lineNo = 0;
    tab.addLine = function(data) {
      var tr = document.createElement('tr');
      tr.lineNo = ++tab.lineNo;
      tr.appendChild(tr.fill=Cell('',null,'Text-Right'));
      tr.appendChild(tr.date=Cell('',null,'Text-Right'));
      tr.appendChild(tr.lumi_total=Cell('',null,'Text-Right'));
      tr.appendChild(tr.lumi_logged=Cell('',null,'Text-Right'));
      tr.appendChild(tr.hvon=Cell('',null,'Text-Right'));
      tr.appendChild(tr.veloin=Cell('',null,'Text-Right'));
      tr.appendChild(tr.running=Cell('',null,'Text-Right'));
      tr.appendChild(tr.logged=Cell('',null,'Text-Right'));
      tr.appendChild(tr.daq=Cell('',null,'Text-Right'));
      tr.appendChild(tr.daq2=Cell('',null,'Text-Left'));
      return (data) ? this.setLine(tr,data) : tr;
    };

    tb.appendChild(tab.currRow = tab.addLine(null));
    tab.current = StyledItem('lbWeb.LHCbPerformance.CurrentFill',null,null);
    tab.current.parent = tab;
    tab.currRow.fillNo = -1;
    tab.currRow.lineNo = ++tab.lineNo;
    tab.current.display = function(data) {
      this.data = data;
      this.parent.setLine(this.parent.currRow,data[1]);
    };

    tab.hist = StyledItem('lbWeb.LHCbPerformance.History',null,null);
    tab.lines = new Array();
    tab.hist.tbody = tb;
    tab.hist.parent = tab;
    tab.hist.display = function(data) {
      tab.hist.data = data;
      var tr, rm = [], t = this.parent, b = this.tbody, v = data[1].split(' | ',20);
      for(var i=0; i<v.length; ++i) {
	if ( i < t.lines.length ) {
	  t.setLine(t.lines[i], v[i]);
	}
	else {
	  b.appendChild(tr=t.addLine(v[i]));
	  t.lines.push(tr);
	}
      }
      for(var j=v.length; j<t.lines.length; ++j)
	rm.push(t.lines[j]);
      for(var k=0; k<rm.length; ++k)
	b.removeChild(rm[k]);
    };

    tab.appendChild(tb);
    
    tab.subscribe = function(provider) {
      provider.subscribeItem(this.current);
      provider.subscribeItem(this.hist);
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

  table.options = {logo:       '',
        	   logo_url:   null,
		   title:      table.title,
		   tips_no:    'LHCb Fill and Performance status'
  };


  table.beforeComment = function() {
    var tab, tb, td, tr = document.createElement('tr');
    var opts        = {system:'LHCb',style:'Arial12pt',legend:true,logger:this.logger};
    var fillTimes   = lhcb.widgets.fillTimes(opts);
    var fillLumi    = lhcb.widgets.fillLuminosity(opts);
    var interactionRates = lhcb.widgets.interactionRates(opts);
    var effi_matrix = lhcb.widgets.fillEfficiencyMatrix(opts);
    var history     = lhcb.widgets.fillHistory(opts);

    tr.vAlign='top';
    td = document.createElement('td');
    tr.appendChild(td=document.createElement('td'));
    td.style.width = '35%';
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


    tr = document.createElement('tr');
    tr.appendChild(td=document.createElement('td'));
    td.colSpan = 3;
    td.appendChild(history);
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
    this.subscriptions.push(history);
  };
  return table;
};

var bigbrother_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var bigbrother_body = function()   { return lhcb.widgets.SubdetectorPage.start(BigBrother); };
