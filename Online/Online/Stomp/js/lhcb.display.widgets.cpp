if ( !lhcb.widgets ) {

  lhcb.widgets = new Object();
  lhcb.widgets.velo = new Object();
  
  /// Set document location to LHCb DAQ status page
  lhcb.widgets.goto_lhcb_page = function(name) {
    var item = function() {    document.location = this.url; };
    item.url =  "lhcb.display.htm?type="+name;
    return item;
  };
  /// Set document location to LHCb DAQ status page
  lhcb.widgets.goto_lhcb_daq_page = function() {
    document.location =  "lhcb.display.htm?type=status&system=LHCb";
  };
  /// Set document location to LHC operations page
  lhcb.widgets.goto_lhc_operations_page = function() {
    document.location =  "http://op-webtools.web.cern.ch/op-webtools/vistar/vistars.php?usr=LHC3";
  };

  lhcb.widgets.subscribeItem = function(provider,item) {
    if ( item ) {
      provider.subscribe(item.name,item);
    }
  };

  lhcb.widgets.mkFSMitem1 = function(item,label) {
    var tr = document.createElement('tr');
    if ( label )
      tr.appendChild(Cell(label,1,'FSMLabel'));
    else
      tr.appendChild(item.label);
    tr.appendChild(item);
    tr.appendChild(item.lock);
    item.lock.className = null;
    return tr;
  };


  /** Build table with summary of the LHC
   *   
   * @return Reference to HTML table containing all data items of this widget
   *
   * @author  M.Frank
   * @version 1.0
   */
  lhcb.widgets.LHC_header = function() {
    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');
  
    tab.width = '100%';
    tab.className = 'MonitorSubHeader';
    tb.className = 'MonitorSubHeader';
  
    tr = document.createElement('tr');
    tb.appendChild(tr);
  
    td = document.createElement('td');
    td.innerHTML = 'LHC&nbsp;Fill';
    tr.appendChild(td);
  
    tab.lhcFillNumber = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.FillNumber',null,null);
    tr.appendChild(tab.lhcFillNumber);
  
    td = document.createElement('td');
    td.innerHTML = 'is&nbsp;in&nbsp;state:';
    tr.appendChild(td);
  
    tab.lhcBeamMode = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.BeamMode',null,null);
    tr.appendChild(tab.lhcBeamMode);
  
    td = document.createElement('td');
    td.innerHTML = '';
    tr.appendChild(td);
  
    tab.appendChild(tb);

    /** Subscribe all required data item to receive data from data provider object
     *
     *  @param      provider  Data provider object
     *
     *  @return On success reference to self, null otherwise
     */
    tab.subscribe = function(provider) {
      this.lhcBeamMode.subscribe(provider);
      this.lhcFillNumber.subscribe(provider);
      return this;
    };
    return tab;
  };
 
  /** Build table with LHC summary information
   *   
   * @return Reference to HTML table containing all data items of this widget
   *
   * @author  M.Frank
   * @version 1.0
   */
  lhcb.widgets.LHC_summary = function() {
    var c, tr, tab = document.createElement('table');
    var tb = document.createElement('tbody');

    tab.className = tb.className  = 'MonitorPage';
    tab.width     = '100%';
    tab.height    = '120px';

    tab.energy       = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Energy',null,"%7.0f GeV");
    //tab.intensity1   = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam1.totalIntensity',null,"%9.2e");
    //tab.intensity2   = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam2.totalIntensity',null,"%9.2e");
    //tab.lifetime1    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam1.primitiveLifetime',null,"%7.2f");
    //tab.lifetime2    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam2.primitiveLifetime',null,"%7.2f");
    tab.intensity1    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam1.averageBeamIntensity',null,'%9.2e');
    tab.intensity2    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam2.averageBeamIntensity',null,'%9.2e');
    tab.lifetime1     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam1.bestLifetime',null,'%7.2f h');
    tab.lifetime2     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam2.bestLifetime',null,'%7.2f h');

    tab.avgLumi      = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Beam.Luminosity.LuminosityAverage',null,"%9.2e");
    tab.LHCbDump     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_BEAMDUMP',null,null);
    tab.LHCbAdjust   = StyledItem('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_ADJUST',null,null);
    tab.LHCbInject   = StyledItem('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_INJECTION',null,null);

    tab.energy.conversion = function(value) {  return value*120.0/1000.0; };

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('LHC status summary',4,'MonitorDataHeader'));

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(c=Cell('Energy:',null,'MonitorDataHeader'));
    c.style.width = '30%';
    tr.appendChild(tab.energy);
    tr.appendChild(Cell('Avg.Luminosity:',null,'MonitorDataHeader'));
    tr.appendChild(tab.avgLumi);

    tr = document.createElement('tr');
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Intensity [e]:',null,'MonitorDataHeader'));
    tr.appendChild(tab.intensity1);
    tr.appendChild(Cell('&larr;1 Beam 2&rarr;',null,'Text-Center'));
    tr.appendChild(tab.intensity2);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Lifetime [h]:',null,'MonitorDataHeader'));
    tr.appendChild(tab.lifetime1);
    tr.appendChild(Cell('&larr;1 Beam 2&rarr;',null,'Text-Center'));
    tr.appendChild(tab.lifetime2);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    c = Cell('Handshakes:',null,'MonitorDataHeader');
    c.rowSpan = 2;
    tr.appendChild(c);
    tr.appendChild(Cell('Dump',null,'MonitorDataHeader'));
    tr.appendChild(Cell('Adjust',null,'MonitorDataHeader'));
    tr.appendChild(Cell('Injection',null,'MonitorDataHeader'));

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(tab.LHCbDump);
    tr.appendChild(tab.LHCbAdjust);
    tr.appendChild(tab.LHCbInject);
    tab.LHCbDump.width   = '70px';
    tab.LHCbAdjust.width = '70px';
    tab.LHCbInject.width = '70px';

    /*
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(tab.LumiFill);
    */


    tab.appendChild(tb);

    /** Subscribe all required data item to receive data from data provider object
     *
     *  @param      provider  Data provider object
     *
     *  @return On success reference to self, null otherwise
     */
    tab.subscribe = function(provider) {
      this.energy.subscribe(provider);
      this.intensity1.subscribe(provider);
      this.intensity2.subscribe(provider);
      this.lifetime1.subscribe(provider);
      this.lifetime2.subscribe(provider);
      this.avgLumi.subscribe(provider);
      this.LHCbDump.subscribe(provider);
      this.LHCbAdjust.subscribe(provider);
      this.LHCbInject.subscribe(provider);
      return this;
    };
    return tab;
  };

  /**
     Build table like this:

     +-------- LHC operator comments ----------
     |LHC in limbo, awaiting decision ...      
     |
     +-----------------------------------------
  */
  lhcb.widgets.LHC_operator_comments = function() {
    var tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.width  = '100%';
    tab.height = tb.height  = '100%';
    tab.className = tb.className = 'MonitorPage';
    
    tab.comments   = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.Page1Comment',null,null);
    tab.comments.style.fontWeight = 'normal';
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('LHC operator comments:',null,'MonitorDataHeader'));
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(tab.comments);
    tab.comments.conversion = function(data) { return data.replace(/\n/g,'<BR>');  };
    tab.comments.height = '60px';

    tab.appendChild(tb);

    /** Subscribe all required data item to receive data from data provider object
     *
     *  @param      provider  Data provider object
     *
     *  @return On success reference to self, null otherwise
     */
    tab.subscribe = function(provider) {
      this.comments.subscribe(provider);
      return this;
    };
    return tab;
  };

  /**
     Build table like this:

     +---- Shift crew : None --------------------------------------------------------
     |
     |
     +---- Comment updated on 18-SEP-2000 16:28 -------------------------------------
  */
  lhcb.widgets.LHCb_shift_comments = function() {
    var tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className = 'MonitorPage';
    tb.className = 'MonitorPage';
    tab.width = '100%';

    tab.comments   = StyledItem('lbWeb.shiftComments',null,null);
    tab.comments.conversion = function(data) { return data.replace(/\n/g,'<BR>');    };
    tab.comments.style.fontWeight = 'normal';

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Shift comments:',null,'MonitorDataHeader'));
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(tab.comments);
    tab.comments.height = '100px';
    tab.appendChild(tb);

    /** Subscribe all required data item to receive data from data provider object
     *
     *  @param      provider  Data provider object
     *
     *  @return On success reference to self, null otherwise
     */
    tab.subscribe = function(provider) {
      this.comments.subscribe(provider);
      return this;
    };
    return tab;
  };

  lhcb.widgets.LHCb_PlanOfDay = function(options) {
    var tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className = 'MonitorPage';
    tb.className = 'MonitorPage';
    tab.width = '100%';

    tab.comments   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Plan',null,null);
    tab.comments.conversion = function(data) { return data.replace(/\n/g,'<BR>');    };
    tab.comments.style.fontWeight = 'normal';

    if ( options.style) {
      tr = document.createElement('tr');
      tb.appendChild(tr);
      tr.appendChild(Cell('Plan of the day:',null,options.style));
    }
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(tab.comments);
    tab.comments.height = '100px';
    tab.appendChild(tb);

    /** Subscribe all required data item to receive data from data provider object
     *
     *  @param      provider  Data provider object
     *
     *  @return On success reference to self, null otherwise
     */
    tab.subscribe = function(provider) {
      this.comments.subscribe(provider);
      return this;
    };
    return tab;
  };

  /** 
   *   
   * @return Reference to HTML table containing all data items of this widget
   *
   * @author  M.Frank
   * @version 1.0
   */
  lhcb.widgets.Det_Run_header = function(sys,logger) {
    var tab = document.createElement('table');
    var tb  = document.createElement('tbody');
    var tr  = document.createElement('tr');
    var td  = document.createElement('td');

    tab.style.width = tb.style.width = '100%';
    tab.style.height = tb.style.height = '100%';
    tab.className = tb.className = 'MonitorSubHeader';

    tb.appendChild(tr);

    td.innerHTML = 'Run';
    tr.appendChild(td);
    
    tab.runNumber = StyledItem('lbWeb.'+sys+'_RunInfo.general.runNumber',null,null);
    tr.appendChild(tab.runNumber);
    
    td = document.createElement('td');
    td.innerHTML = ' is ';
    tr.appendChild(td);
    
    tab.runState = FSMItem('lbWeb.'+sys,logger,true);
    tr.appendChild(tab.runState);
    tr.appendChild(tab.runState.lock);

    td = document.createElement('td');
    td.innerHTML = '';
    tr.appendChild(td);

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribe(this.runNumber.name,this.runNumber);
      provider.subscribe(this.runState.name,this.runState);
      return this;
    };
    return tab;
  };

  /**
     Build table like this:
     +-----------------------------------------
     | RUN_196 , Started  2-NOV-2000 08:02
     |           Last 4 mn     Run    Fill
     | Events            0       0       0
     | LCAL              0       0       0
     | QQbar             0       0       0
     | L1 rate Hz    0.000   0.000 Ineff.(%)
     | L2 rate Hz    0.000   0.000 Op   0.00
     | L3 rate Hz    0.000   0.000 DAQ  0.00
     | Deadtime %     0.00    0.00 Dt   0.00
     | Downtime %     0.00    0.00 Tot  0.00
     | Magnet          3.1    12.1     9.9
     | BKG    TPC    -0.80  SAMBA     0.00
     +-----------------------------------------
  */
  lhcb.widgets.Det_Run_Summary = function(sys) {

    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className  = tb.className = 'MonitorPage';
    tab.width      = '100%';
    tb.cellSpacing = 0;
    tb.cellPadding = 0;

    tab.runType        = StyledItem('lbWeb.'+sys+'_RunInfo.general.runType',     null,null);
    tab.runStart       = StyledItem('lbWeb.'+sys+'_RunInfo.general.runStartTime',null,'Started at: %s');
    tab.nTriggers      = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.nTriggers',       null,null);
    tab.hltNTriggers   = StyledItem('lbWeb.'+sys+'_RunInfo.HLTFarm.hltNTriggers',null,null);
    tab.l0Rate         = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.triggerRate',     null,'%8.2f Hz');
    tab.l0RateRun      = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.runTriggerRate',  null,'%8.2f Hz');
    tab.hltRate        = StyledItem('lbWeb.'+sys+'_RunInfo.HLTFarm.hltRate',     null,'%8.2f Hz');
    tab.hltRateRun     = StyledItem('lbWeb.'+sys+'_RunInfo.HLTFarm.runHltRate',  null,'%8.2f Hz');
    tab.deadTime       = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.deadTime',        null,'%8.2f %%');
    tab.deadTimeRun    = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.runDeadTime',     null,'%8.2f %%');
    tab.magnetField    = StyledItem('lbWeb.lbHyst.B',                            null,'%7.4f');
    tab.magnetPolarity = StyledItem('lbWeb.lbHyst.Polarity',                     null, null);
    tab.veloPosition   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Specific.VELO.Position',null, null);
    tab.figureOfMerit1 = StyledItem('lbWeb.BCM_DP_S0.RS2_REL',                           null, 'S0:%7.3f');
    tab.figureOfMerit2 = StyledItem('lbWeb.BCM_DP_S0.RS32_REL',                          null, '%7.3f');
    tab.figureOfMerit3 = StyledItem('lbWeb.BCM_DP_S1.RS2_REL',                           null, 'S1:%7.3f');
    tab.figureOfMerit4 = StyledItem('lbWeb.BCM_DP_S1.RS32_REL',                          null, '%7.3f');
    tab.bcmBeamPermit1 = StyledItem('lbWeb.BCM_Interface.BeamPermit.getStatus',          null, null);
    tab.bcmBeamPermit2 = StyledItem('lbWeb.BCM_Interface.InjPermit1.getStatus',          null, null);
    tab.bcmBeamPermit3 = StyledItem('lbWeb.BCM_Interface.InjPermit2.getStatus',          null, null);
    tab.interActionRate= StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.TriggerRates.TrgRateLumi_GP', null, '%7.1f Hz');
    tab.instantLumi    = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_GP', null, '%7.2f Hz/&mu;b');

    // Run status
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(tab.runType);
    tab.runStart.colSpan = 4;
    tr.appendChild(tab.runStart);
    tab.runStart.style.textAlign = 'left';

    // Magnet status
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Magnet [&nbsp;T&nbsp;]:',1,'MonitorDataHeader'));
    tr.appendChild(tab.magnetField);
    tr.appendChild(Cell('Polarity:',1,'MonitorDataHeader'));
    tr.appendChild(tab.magnetPolarity);
    tab.magnetPolarity.conversion = function(data) {
      if ( data>0 ) return '+&nbsp;(Down)';
      return '-&nbsp;(Up)';
    };

    // Background status
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('BCM BKG [&permil;]:',1,'MonitorDataHeader'));
    tr.appendChild(tab.figureOfMerit1);
    tr.appendChild(tab.figureOfMerit2);
    tr.appendChild(tab.figureOfMerit3);
    tr.appendChild(tab.figureOfMerit4);

    // Beam permits
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Beam Permits:',null,'MonitorDataHeader'));
    tr.appendChild(tab.bcmBeamPermit1);
    tr.appendChild(tab.bcmBeamPermit2);
    tr.appendChild(tab.bcmBeamPermit3);

    // Velo position
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Velo position:',1,'MonitorDataHeader'));
    tr.appendChild(tab.veloPosition);

    // Luminosity and interaction information
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Interaction rate:',null,'MonitorDataHeader'));
    tr.appendChild(tab.interActionRate);
    tr.appendChild(Cell('Inst.Lumi:',1,'MonitorDataHeader'));
    tr.appendChild(tab.instantLumi);

    // Event processing information
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('L0 Events:',null,'MonitorDataHeader'));
    tr.appendChild(tab.nTriggers);
    tr.appendChild(Cell('accepted:',1,'MonitorDataHeader'));
    tr.appendChild(tab.hltNTriggers);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('',1,null));
    tr.appendChild(Cell('Now',2,'MonitorDataHeader'));
    tr.appendChild(Cell('Run',2,'MonitorDataHeader'));

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('L0 rate:',1,'MonitorDataHeader'));
    tab.l0Rate.colSpan = 2;
    tab.l0RateRun.colSpan = 2;
    tr.appendChild(tab.l0Rate);
    tr.appendChild(tab.l0RateRun);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('HLT rate:',1,'MonitorDataHeader'));
    tab.hltRate.colSpan = 2;
    tab.hltRateRun.colSpan = 2;
    tr.appendChild(tab.hltRate);
    tr.appendChild(tab.hltRateRun);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Dead-time:',null,'MonitorDataHeader'));
    tab.deadTime.colSpan = 2;
    tab.deadTimeRun.colSpan = 2;
    tr.appendChild(tab.deadTime);
    tr.appendChild(tab.deadTimeRun);

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.runType);
      provider.subscribeItem(this.runStart);
      provider.subscribeItem(this.nTriggers);
      provider.subscribeItem(this.hltNTriggers);
      provider.subscribeItem(this.l0Rate);
      provider.subscribeItem(this.l0RateRun);
      provider.subscribeItem(this.hltRate);
      provider.subscribeItem(this.hltRateRun);
      provider.subscribeItem(this.deadTime);
      provider.subscribeItem(this.deadTimeRun);
      provider.subscribeItem(this.veloPosition);
      
      provider.subscribeItem(this.bcmBeamPermit1);
      provider.subscribeItem(this.bcmBeamPermit2);
      provider.subscribeItem(this.bcmBeamPermit3);
      
      provider.subscribeItem(this.magnetField);
      provider.subscribeItem(this.magnetPolarity);
      
      provider.subscribeItem(this.figureOfMerit1);
      provider.subscribeItem(this.figureOfMerit2);
      provider.subscribeItem(this.figureOfMerit3);
      provider.subscribeItem(this.figureOfMerit4);

      provider.subscribeItem(this.instantLumi);
      provider.subscribeItem(this.interActionRate);
      return this;
    };
    return tab;
  };

  /** Build table with HT summary state of the LHCb detector
   *   
   * @author  M.Frank
   * @version 1.0
   */
  lhcb.widgets.LHCb_HT_header = function() {
    var c, tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');
  
    tab.width     = '100%';
    tab.className = 'MonitorData';
    tb.className  = 'MonitorData';
  
    tr = document.createElement('tr');
    tr.appendChild(Cell('Detector HV Status',2,'MonitorDataHeader'));
    tb.appendChild(tr);
  
    tr = document.createElement('tr');
    tr.appendChild(c=Cell('LHCb HT is:',1,null));    
    c.style.width = '120px';
    //this.hvState = StyledItem('lbWeb.LHCb.HVState',null,null);
    this.hvState = StyledItem('lbWeb.LHCb_HV.FSM.state',null,null);
    this.hvState.conversion = function(val) {
      if ( val ) {
	var v=val.split('/',4);
	if ( v.length > 1 ) return v[1];
      }
      return val;
    };
    tr.appendChild(this.hvState);
    tb.appendChild(tr);
    /*
    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Updated:',1,null));    
    c.style.width = '120px';
    this.hvStateUpdate = StyledItem('lbWeb.LHCb.HVStateUpdate',null,null);
    tr.appendChild(this.hvStateUpdate);
    tb.appendChild(tr);
    */
    tab.appendChild(tb);
    return tab;
  };

  /// Create Cell item containing the TTC prepulse information
  lhcb.widgets.rf2ttcPrepulses = function() {
    var item = StyledItem('lbWeb.RF2TTC/rf2ttc.Parameter.Readings.DIP.INJECTION_PREPULSE','Text-Right',null);
    item.conversion = function(v) { return (v=='TRUE') ? 'Yes' : 'No'; };
    return item;
  };

  /// Create Cell item containing the TTC clock status
  lhcb.widgets.rf2ttcStatus = function() {
    var item = StyledItem('lbWeb.RF2TTC/rf2ttc.Parameter.Settings.Errors.FREQ_STATUS',null,null);
    item.conversion = function(v) {  return (v=='TRUE') ? 'Clock OK' : 'Bad Clock'; };
    return item
  };

  /// Create Cell item containing the TTC clock state
  lhcb.widgets.rf2ttcState = function() {
    var item = StyledItem('lbWeb.RF2TTC/rf2ttc.State.RunState',null,null);
    item.data = 3;
    item.conversion = function(state) {
      this.data = state;
      if(state == 3 || state == 5)    //state Ready or Running
	return 'Ready';
      else if(state == 1 || state == 2) // Not Ready or Configuring
        return 'Not Ready';
      return 'Off';
    };
    return item
  };

  /// Create Cell item containing the TTC clock type. Requires the state for interpretation!
  lhcb.widgets.rf2ttcSource = function(st) {
    var item = StyledItem('lbWeb.RF2TTC/rf2ttc.Parameter.Settings.Selection.SELECT_SOURCE','Text-Right',null);
    item.State  = st;
    item.State.data = 3;
    item.conversion = function(source)  {
      var state = state = this.State.data;
      if(state == 3 || state == 5) {  //state Ready or Running
	if(source == 0)
	  return 'Int. LHCb clock'; 
	else if (source == 1)
          return 'Ext. LHC clock: locked on RF Ref'; 
	else if (source == 2)
          return 'Ext. LHC clock: locked on Beam 1'; 
	else if (source == 3)
          return 'Ext. LHC clock: locked on Beam 2';
      }
      else if(state == 1 || state == 2) // Not Ready or Configuring
        return 'Not configured';
      else if(state == 0)
        return 'Problem with the clock';
      else if(state == -1)
        return 'No Clock Monitoring';
      return 'System not used or PVSS project not connected';
    };
    return item;
  };

  lhcb.widgets.LHCStateSummary = function(options) {
    var tb, tr, cell, tab = document.createElement('table');
    tab.className  = 'MonitorPage';

    tb = document.createElement('tbody');
    tooltips.set(tb,'LHC state summary<br>Click to move to LHC summary information.');
    tb.onclick = function() { document.location = "lhcb.display.htm?type=lhc";};
    tb.className  = 'MonitorPage';
    tb.height    = '120px';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('LHC State',3,options.style));
      tb.appendChild(tr);
    }
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Mode',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Fill Number',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Energy',1,'MonitorDataHeader'));
      tb.appendChild(tr);
    }
    tab.lhcMode = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.MachineMode',null,null);
    tab.lhcFillNumber    = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.FillNumber',null,null);
    tab.lhcEnergy        = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Energy',null,'%7.1f GeV');
    tab.lhcEnergy.conversion = function(data) {      return (data*120.0)/1000.0;    };
    tr = document.createElement('tr');
    tr.appendChild(tab.lhcMode);
    tr.appendChild(tab.lhcFillNumber);
    tr.appendChild(tab.lhcEnergy);
    tb.appendChild(tr);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.lhcMode);
      provider.subscribeItem(this.lhcFillNumber);
      provider.subscribeItem(this.lhcEnergy);
    };
    return tab;
  };


  lhcb.widgets.ClockSummary = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');
    var cell = Cell('LHC clock:',null,'MonitorDataHeader');

    tooltips.set(tab,'TTC clock information<br>Click to see LHC status');
    tab.onclick = function() { document.location = "lhcb.display.htm?type=lhc";};
    tab.lhcPrepulses   = lhcb.widgets.rf2ttcPrepulses();
    tab.lhcClockState  = lhcb.widgets.rf2ttcState();
    tab.lhcClock       = lhcb.widgets.rf2ttcSource(tab.lhcClockState);
    tab.lhcStatus      = lhcb.widgets.rf2ttcStatus();

    tab.className = tb.className   = 'MonitorPage';
    if ( options.style ) {
      tr.appendChild(Cell('LHC Clock Status',4,options.style));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }

    tr.appendChild(cell);
    cell.style.width = '25%';
    tr.appendChild(tab.lhcClock);
    tab.lhcClock.colSpan = 5;
    tab.style.textAlign = 'left';
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(cell=Cell('Clock state:',null,'MonitorDataHeader'));
    tr.appendChild(tab.lhcClockState);
    tab.lhcClockState.style.textAlign = 'left';
    tr.appendChild(cell=Cell('/',null,null));
    tr.appendChild(tab.lhcStatus);
    tr.appendChild(cell=Cell('Prepulses:',null,'MonitorDataHeader'));
    cell.style.width = '15%';
    tr.appendChild(tab.lhcPrepulses);
    tab.lhcPrepulses.style.width = '15%';
    tb.appendChild(tr);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.lhcClock);
      provider.subscribeItem(this.lhcStatus);
      provider.subscribeItem(this.lhcClockState);
      provider.subscribeItem(this.lhcPrepulses);
    };
    return tab;
  };

  lhcb.widgets.RateSummary = function(options) {

    var tb, td, tr, tab = document.createElement('table');
    var sys = options.system;
    tb = document.createElement('tbody');

    tab.className  = tb.className = 'MonitorPage';
    tab.width      = '100%';
    tb.cellSpacing = 0;
    tb.cellPadding = 0;

    tab.runType        = StyledItem('lbWeb.'+sys+'_RunInfo.general.runType',     null,null);
    tab.nTriggers      = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.nTriggers',       null,null);
    tab.hltNTriggers   = StyledItem('lbWeb.'+sys+'_RunInfo.HLTFarm.hltNTriggers',null,null);
    tab.l0Rate         = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.triggerRate',     null,'%8.2f Hz');
    tab.l0RateRun      = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.runTriggerRate',  null,'%8.2f Hz');
    tab.hltRate        = StyledItem('lbWeb.'+sys+'_RunInfo.HLTFarm.hltRate',     null,'%8.2f Hz');
    tab.hltRateRun     = StyledItem('lbWeb.'+sys+'_RunInfo.HLTFarm.runHltRate',  null,'%8.2f Hz');
    tab.deadTime       = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.deadTime',        null,'%8.2f %%');
    tab.deadTimeRun    = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.runDeadTime',     null,'%8.2f %%');
    tab.interActionRate= StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.TriggerRates.TrgRateLumi_GP', null, '%7.1f Hz');
    tab.instantLumi    = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_GP', null, '%7.2f Hz/&mu;b');

    if ( options.style ) {
      tr = document.createElement('tr');
      tb.appendChild(tr);
      tr.appendChild(Cell('Data taking rates and instantaneous luminosity',5,options.style));
    }

    // Run status
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Activity:',null,'MonitorDataHeader'));
    tr.appendChild(tab.runType);
    tab.runType.colSpan = 4;
    tab.runType.style.textAlign = 'left';

    // Luminosity and interaction information
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Interaction rate:',null,'MonitorDataHeader'));
    tr.appendChild(tab.interActionRate);
    tr.appendChild(Cell('Inst.Lumi:',1,'MonitorDataHeader'));
    tr.appendChild(tab.instantLumi);

    // Event processing information
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('L0 Events:',null,'MonitorDataHeader'));
    tr.appendChild(tab.nTriggers);
    tr.appendChild(Cell('accepted:',1,'MonitorDataHeader'));
    tr.appendChild(tab.hltNTriggers);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('',1,null));
    tr.appendChild(Cell('Now',2,'MonitorDataHeader'));
    tr.appendChild(Cell('Run',2,'MonitorDataHeader'));

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('L0 rate:',1,'MonitorDataHeader'));
    tab.l0Rate.colSpan = 2;
    tab.l0RateRun.colSpan = 2;
    tr.appendChild(tab.l0Rate);
    tr.appendChild(tab.l0RateRun);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('HLT rate:',1,'MonitorDataHeader'));
    tab.hltRate.colSpan = 2;
    tab.hltRateRun.colSpan = 2;
    tr.appendChild(tab.hltRate);
    tr.appendChild(tab.hltRateRun);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Dead-time:',null,'MonitorDataHeader'));
    tab.deadTime.colSpan = 2;
    tab.deadTimeRun.colSpan = 2;
    tr.appendChild(tab.deadTime);
    tr.appendChild(tab.deadTimeRun);

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.runType);
      provider.subscribeItem(this.nTriggers);
      provider.subscribeItem(this.hltNTriggers);
      provider.subscribeItem(this.l0Rate);
      provider.subscribeItem(this.l0RateRun);
      provider.subscribeItem(this.hltRate);
      provider.subscribeItem(this.hltRateRun);
      provider.subscribeItem(this.deadTime);
      provider.subscribeItem(this.deadTimeRun);
      provider.subscribeItem(this.instantLumi);
      provider.subscribeItem(this.interActionRate);
      return this;
    };
    return tab;
  };

  lhcb.widgets.MagnetSummary = function(options) {
    var tb, tr, tab = document.createElement('table');
    tab.className  = 'MonitorPage';
    
    tb = document.createElement('tbody');
    tooltips.set(tb,'Magnet summary<br>Click to move to magnet page.');
    tb.onclick = function() { document.location = "lhcb.display.htm?type=magnet";};
    tb.className  = 'MonitorPage';
    tb.height    = '120px';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Magnet Status',3,options.style));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    if ( options.legend ) {
      tr.appendChild(Cell('Set Current',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Measured Current',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Polarity',1,'MonitorDataHeader'));
    }
    tb.appendChild(tr);
    tab.magSet      = StyledItem('lbWeb.LbMagnet.SetCurrent',null,'%7.0f A');
    tab.magRead     = StyledItem('lbWeb.LbMagnet.Current',null,'%7.0f A');
    tab.magPolarity = StyledItem('lbWeb.LbMagnet.Polarity',null,null);
    tab.magPolarity.conversion = function(data) {      return data>0 ? 'Down' : 'Up';    };
    tr = document.createElement('tr');
    tr.appendChild(tab.magSet);
    tr.appendChild(tab.magRead);
    tr.appendChild(tab.magPolarity);
    tb.appendChild(tr);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.magSet);
      provider.subscribeItem(this.magRead);
      provider.subscribeItem(this.magPolarity);
    };
    return tab;
  };

  lhcb.widgets.CoolingSummary = function(options) {
    var c, tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');
    tab.className = tb.className   = 'MonitorPage';

    tooltips.set(tab,'Cooling information summary<br>for various subdetectors');    
    // Velo position
    tab.itCoolingAlarms    = StyledItem('lbWeb.CaV/ItPlant.Actual.alarm', null, null);
    tab.otCoolingAlarms    = StyledItem('lbWeb.CaV/OtPlant.Actual.alarm', null, null);
    tab.ttCoolingAlarms    = StyledItem('lbWeb.CaV/TtPlant.Actual.alarm', null, null);
    tab.richCoolingAlarms  = StyledItem('lbWeb.CaV/RichPlant.Actual.alarm', null, null);

    tab.itCoolingFaults    = StyledItem('lbWeb.CaV/ItPlant.Actual.fault', null, null);
    tab.otCoolingFaults    = StyledItem('lbWeb.CaV/OtPlant.Actual.fault', null, null);
    tab.ttCoolingFaults    = StyledItem('lbWeb.CaV/TtPlant.Actual.fault', null, null);
    tab.richCoolingFaults  = StyledItem('lbWeb.CaV/RichPlant.Actual.fault', null, null);
    /*
    tab.itCoolingStatus    = StyledItem('lbWeb.CaV/ItPlant.Actual.status', null, null);
    tab.otCoolingStatus    = StyledItem('lbWeb.CaV/OtPlant.Actual.status', null, null);
    tab.ttCoolingStatus    = StyledItem('lbWeb.CaV/TtPlant.Actual.status', null, null);
    tab.richCoolingStatus  = StyledItem('lbWeb.CaV/RichPlant.Actual.status', null, null);
    */
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Cooling Plant Status',5,options.style));
      tb.appendChild(tr);
    }
    tr = document.createElement('tr');
    tb.appendChild(tr);
    if ( options.legend ) {
      tr.appendChild(c=Cell('Cooling',1,'MonitorDataHeader'));
      c.style.width = '25%';
    }
    tr.appendChild(Cell('IT',1,'MonitorDataHeader'));
    tr.appendChild(Cell('TT',1,'MonitorDataHeader'));
    tr.appendChild(Cell('OT',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Rich',1,'MonitorDataHeader'));

    tr = document.createElement('tr');
    tb.appendChild(tr);
    if ( options.legend ) tr.appendChild(Cell('Alarms',1,'MonitorDataHeader'));

    tr.appendChild(tab.itCoolingAlarms);
    tr.appendChild(tab.ttCoolingAlarms);
    tr.appendChild(tab.otCoolingAlarms);
    tr.appendChild(tab.richCoolingAlarms);

    tr = document.createElement('tr');
    tb.appendChild(tr);
     if ( options.legend ) tr.appendChild(Cell('Faults',1,'MonitorDataHeader'));

    tr.appendChild(tab.itCoolingFaults);
    tr.appendChild(tab.ttCoolingFaults);
    tr.appendChild(tab.otCoolingFaults);
    tr.appendChild(tab.richCoolingFaults);

    /*
    tr = document.createElement('tr');
    tb.appendChild(tr);
     if ( options.legend ) tr.appendChild(Cell('Status',1,'MonitorDataHeader'));

    tr.appendChild(tab.itCoolingStatus);
    tr.appendChild(tab.ttCoolingStatus);
    tr.appendChild(tab.otCoolingStatus);
    tr.appendChild(tab.richCoolingStatus);
    */
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.itCoolingAlarms);
      provider.subscribeItem(this.ttCoolingAlarms);
      provider.subscribeItem(this.otCoolingAlarms);
      provider.subscribeItem(this.richCoolingAlarms);
      provider.subscribeItem(this.itCoolingFaults);
      provider.subscribeItem(this.ttCoolingFaults);
      provider.subscribeItem(this.otCoolingFaults);
      provider.subscribeItem(this.richCoolingFaults);
      //provider.subscribeItem(this.itCoolingStatus);
      //provider.subscribeItem(this.ttCoolingStatus);
      //provider.subscribeItem(this.otCoolingStatus);
      //provider.subscribeItem(this.richCoolingStatus);
    };
    return tab;
  };

  lhcb.widgets.BackgroundSummary = function(options) {
    var c, tr;
    var tab = document.createElement('table');
    var tb  = document.createElement('tbody');

    tooltips.set(tab,'Background summary<br>Click to access BCM information');
    tab.onclick = function() { document.location = "lhcb.display.htm?type=bcm&sensors=1";};
    tab.className = tb.className = 'MonitorPage';

    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(c=Cell('Background Status and Beam Permits:',7,options.style));
      c.style.width='100%';
      tb.appendChild(tr);
    }

    tab.bcmBeamPermit1 = StyledItem('lbWeb.BCM_Interface.BeamPermit.getStatus',null,null);
    tab.bcmBeamPermit2 = StyledItem('lbWeb.BCM_Interface.InjPermit1.getStatus',null,null);
    tab.bcmBeamPermit3 = StyledItem('lbWeb.BCM_Interface.InjPermit2.getStatus',null,null);
    tr = document.createElement('tr');
    if ( options.legend ) {
      tr.appendChild(c=Cell('Permits',1,'MonitorDataHeader'));
      c.style.width = '25%';
    }
    tr.appendChild(tab.bcmBeamPermit1);
    tab.bcmBeamPermit1.colSpan = 2;
    tr.appendChild(tab.bcmBeamPermit2);
    tab.bcmBeamPermit2.colSpan = 2;
    tr.appendChild(tab.bcmBeamPermit3);
    tab.bcmBeamPermit3.colSpan = 2;
    tb.appendChild(tr);

    tab.figureOfMerit1 = StyledItem('lbWeb.BCM_DP_S0.RS2_REL',             null, '%7.3f');
    tab.figureOfMerit2 = StyledItem('lbWeb.BCM_DP_S0.RS32_REL',            null, '%7.3f');
    tab.figureOfMerit3 = StyledItem('lbWeb.BCM_DP_S1.RS2_REL',             null, '%7.3f');
    tab.figureOfMerit4 = StyledItem('lbWeb.BCM_DP_S1.RS32_REL',            null, '%7.3f');
    tr = document.createElement('tr');
    if ( options.legend ) {
      tr.appendChild(Cell('FoM',null,'MonitorDataHeader'));
    }
    tr.appendChild(Cell('S0.RS2/32:',1,'MonitorDataHeader'));
    tr.appendChild(tab.figureOfMerit1);
    tr.appendChild(tab.figureOfMerit2);
    tr.appendChild(Cell('S1.RS2/32:',1,'MonitorDataHeader'));
    tr.appendChild(tab.figureOfMerit3);
    tr.appendChild(tab.figureOfMerit4);
    tb.appendChild(tr);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.bcmBeamPermit1);
      provider.subscribeItem(this.bcmBeamPermit2);
      provider.subscribeItem(this.bcmBeamPermit3);
      
      provider.subscribeItem(this.figureOfMerit1);
      provider.subscribeItem(this.figureOfMerit2);
      provider.subscribeItem(this.figureOfMerit3);
      provider.subscribeItem(this.figureOfMerit4);
    };
    return tab;
  };

  lhcb.widgets.SafetySummary = function(options) {
    var tb, tr, cell, tab = document.createElement('table');
    
    tab.className  = 'MonitorPage';
    tb = document.createElement('tbody');
    tooltips.set(tb,'Safety status of the subdetectors');
    tb.className  = 'MonitorPage';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Safety',3,options.style));
      tb.appendChild(tr);
    }
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Subdetector',1,'MonitorDataHeader'));
      tr.appendChild(Cell('State',1,'MonitorDataHeader'));
      tr.appendChild(Cell('',1,null));
      tb.appendChild(tr);
    }
    tab.richSafety = FSMItem('lbWeb.RICH_SAFETY',options.logger,true);
    tab.muonSafety = FSMItem('lbWeb.MUON_Safety',options.logger,true);
    tab.ttSafety   = FSMItem('lbWeb.TT_Safety',options.logger,true);
    tab.itSafety   = FSMItem('lbWeb.IT_Safety',options.logger,true);
    tb.appendChild(lhcb.widgets.mkFSMitem1(tab.ttSafety,'TT'));
    tb.appendChild(lhcb.widgets.mkFSMitem1(tab.itSafety,'IT'));
    tb.appendChild(lhcb.widgets.mkFSMitem1(tab.richSafety,'RICH'));
    tb.appendChild(lhcb.widgets.mkFSMitem1(tab.muonSafety,'MUON'));
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.ttSafety);
      provider.subscribeItem(this.itSafety);
      provider.subscribeItem(this.richSafety);
      provider.subscribeItem(this.muonSafety);
    };

    return tab;
  };

  lhcb.widgets.RICHPressures = function(options) {
    var tb, tr, cell, tab = document.createElement('table');
    tab.className  = 'MonitorPage';
    tb = document.createElement('tbody');
    tooltips.set(tb,'Rich parameters');
    tb.className  = 'MonitorPage';
    tb.height    = '120px';

    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Rich1 Gas',2,options.style));
      tr.appendChild(Cell('Rich2 Gas',2,options.style));
      tb.appendChild(tr);
    }
    tr = document.createElement('tr');
    tr.appendChild(Cell('Pressure:',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Temperatur',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Pressure:',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Temperatur',1,'MonitorDataHeader'));
    tb.appendChild(tr);
    tab.rich1Pressure = StyledItem('lbWeb.LHCb_RunInfoCond.RICH1.R1HltGasParameters.Pressure',null,'%7.1f hPa');
    tab.rich1Temp     = StyledItem('lbWeb.LHCb_RunInfoCond.RICH1.R1HltGasParameters.Temperature',null,'%7.1f K');
    tab.rich2Pressure = StyledItem('lbWeb.LHCb_RunInfoCond.RICH2.R2HltGasParameters.Pressure',null,'%7.2f hPa');
    tab.rich2Temp     = StyledItem('lbWeb.LHCb_RunInfoCond.RICH2.R2HltGasParameters.Temperature',null,'%7.1f K');

    tr = document.createElement('tr');
    tr.appendChild(tab.rich1Pressure);
    tr.appendChild(tab.rich1Temp);
    tr.appendChild(tab.rich2Pressure);
    tr.appendChild(tab.rich2Temp);
    tb.appendChild(tr);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.rich1Pressure);
      provider.subscribeItem(this.rich1Temp);
      provider.subscribeItem(this.rich2Pressure);
      provider.subscribeItem(this.rich2Temp);
    };
    return tab;
  };

  lhcb.widgets.HVSummary = function(logger) {
    var tb, tr, cell, tab = document.createElement('table');
    tab.className  = 'MonitorPage';
    tab.logger = logger;

    tab.addState = function(nam,split,m,q) {
      var cell,tr = document.createElement('tr');
      tr.appendChild(cell=Cell(nam,1,null));
      cell.style.width='120px';
      this[m+'LHC'+nam]   = FSMItem('lbWeb.'+nam+'_LHC_'+m,this.logger,true);
      this[m+'LHCRQ'+nam] = StyledItem('lbWeb.'+nam+'_LHC_'+m+'_Comm.PrevLhcState',null,null);
      tr.appendChild(this[m+'LHC'+nam]);      
      tr.appendChild(this[m+'LHCRQ'+nam]);
      
      if ( split ) {
	this[m+'A'+nam] = FSMItem('lbWeb.'+nam+'A_'+q,this.logger,true);
	this[m+'C'+nam] = FSMItem('lbWeb.'+nam+'C_'+q,this.logger,true);
	tr.appendChild(this[m+'A'+nam]);
	tr.appendChild(this[m+'C'+nam]);
      }
      else {
	this[m+nam] = FSMItem('lbWeb.'+nam+'_'+q,this.logger,true);
	tr.appendChild(this[m+nam]);
      }
      tr.appendChild(this[m+'LHC'+nam].lock);
      this[m+'LHC'+nam].lock.className = null;
      tooltips.set(tr,'HT state of the '+nam+' subdetector');
      return tr;
    };

    tab.addHV = function(tb) {
      tb.appendChild(this.addState('VELO',  true, 'HV', 'HV'));
      tb.appendChild(this.addState('TT',   false, 'HV', 'HV'));
      tb.appendChild(this.addState('IT',   false, 'HV', 'HV'));
      tb.appendChild(this.addState('OT',    true, 'HV', 'HV'));
      tb.appendChild(this.addState('RICH1',false, 'HV', 'HV'));
      tb.appendChild(this.addState('RICH2',false, 'HV', 'HV'));
      tb.appendChild(this.addState('PRS',  false, 'HV', 'HV'));
      tb.appendChild(this.addState('ECAL', false, 'HV', 'HV'));
      tb.appendChild(this.addState('HCAL', false, 'HV', 'HV'));
      tb.appendChild(this.addState('MUON',  true, 'HV', 'HV'));
    };

    tab.addLV = function(tb) {
      tb.appendChild(this.addState('VELO',   true, 'LV', 'DCS_Local'));
      tb.appendChild(this.addState('TT',    false, 'LV', 'DCS_Local'));
      tb.appendChild(this.addState('IT',    false, 'LV', 'DCS_Local'));
      tb.appendChild(this.addState('RICH1', false, 'LV', 'DCS_Local'));
      tb.appendChild(this.addState('RICH2', false, 'LV', 'DCS_Local'));
    };

    tab.buildTable = function(options) {
      var tb, tr, cell;
      tb = document.createElement('tbody');
      tb.className  = 'MonitorPage';
      tb.height    = '120px';
      tooltips.set(tb,'HT summary of LHCb');
      //alert('opts:'+options);
      if ( options.style ) {
	tr = document.createElement('tr');
	tr.appendChild(Cell('Voltages',6,options.style));
	tb.appendChild(tr);
      }
      if ( options.top ) {
	tr = document.createElement('tr');
	tr.appendChild(Cell('System',1,'MonitorDataHeader'));
	tr.appendChild(Cell('State',1,'MonitorDataHeader'));
	tr.appendChild(Cell('Requested',1,'MonitorDataHeader'));
	tb.appendChild(tr);
	tr = document.createElement('tr');
	this.hvState  = FSMItem('lbWeb.LHCb_LHC_HV',this.logger,true);
	this.hvRequest = StyledItem('lbWeb.LHCb_LHC_HV_Info.requestedState',null,null);
	
	tr.appendChild(Cell('LHCb HV &amp; LV',1,'MonitorDataHeader'));
	tr.appendChild(this.hvState);
	tr.appendChild(this.hvRequest);
	tr.appendChild(this.hvState.lock);
	this.hvState.lock.className = null;
	tb.appendChild(tr);
	this.appendChild(tb);
      }

      tb = document.createElement('tbody');
      tooltips.set(tab,'High voltage state of the subdetectors');
      tb.className  = 'MonitorPage';
      tb.height    = '120px';
      if ( options.hv ) {
	tr = document.createElement('tr');
	tr.appendChild(Cell('High Voltage',6,'Arial12pt'));
	tb.appendChild(tr);
      }
      if ( options.hv_legend ) {
	tr = document.createElement('tr');
	tr.appendChild(cell=Cell('Subdetector',1,'MonitorDataHeader'));
	tr.appendChild(cell=Cell('State',1,'MonitorDataHeader'));
	tr.appendChild(cell=Cell('Request',1,'MonitorDataHeader'));
	cell.style.width = '20%';
	tr.appendChild(cell=Cell('HV State Side A / Side C',2,'MonitorDataHeader'));
	tb.appendChild(tr);
      }
      this.addHV(tb);
      this.appendChild(tb);
      
      tb = document.createElement('tbody');
      tooltips.set(tb,'Low voltage state of the subdetectors');
      tb.className  = 'MonitorPage';
      tb.height    = '120px';
      if ( options.lv ) {
	tr = document.createElement('tr');
	tr.appendChild(Cell('Low Voltage',6,'Arial12pt'));
	tb.appendChild(tr);
      }
      if ( options.lv_legend ) {
	tr = document.createElement('tr');
	tr.appendChild(Cell('Subdetector',1,'MonitorDataHeader'));
	tr.appendChild(Cell('State',1,'MonitorDataHeader'));
	tr.appendChild(cell=Cell('Request',1,'MonitorDataHeader'));
	tr.appendChild(Cell('LV State Side A / Side C',2,'MonitorDataHeader'));
	tb.appendChild(tr);
      }
      this.addLV(tb);
      this.appendChild(tb);
      return this;
    };

    tab.build = function(header_style) {
      return this.buildTable({style:header_style,top:true,hv:true,hv_legend:true,lv:true,lv_legend:true});
    };

    tab.subscribe_j = lhcb.widgets.subscribeItem;

    tab.subscribe_i = function(provider,typ,nam) {
      this.subscribe_j(provider, this[typ+'LHC'+nam]);
      this.subscribe_j(provider, this[typ+'LHCRQ'+nam]);
      this.subscribe_j(provider, this[typ+nam]);
      this.subscribe_j(provider, this[typ+'A'+nam]);
      this.subscribe_j(provider, this[typ+'C'+nam]);
    };
    tab.subscribe = function(provider) {
      this.subscribe_i(provider, 'HV', 'VELO');
      this.subscribe_i(provider, 'HV', 'TT');
      this.subscribe_i(provider, 'HV', 'IT');
      this.subscribe_i(provider, 'HV', 'OT');
      this.subscribe_i(provider, 'HV', 'RICH1');
      this.subscribe_i(provider, 'HV', 'RICH2');
      this.subscribe_i(provider, 'HV', 'PRS');
      this.subscribe_i(provider, 'HV', 'ECAL');
      this.subscribe_i(provider, 'HV', 'HCAL');
      this.subscribe_i(provider, 'HV', 'MUON');

      this.subscribe_i(provider, 'LV', 'VELO');
      this.subscribe_i(provider, 'LV', 'TT');
      this.subscribe_i(provider, 'LV', 'IT');
      this.subscribe_i(provider, 'LV', 'RICH1');
      this.subscribe_i(provider, 'LV', 'RICH2');
      if ( this['hvState']  )  provider.subscribeItem(this.hvState);
      if ( this['hvRequest'] ) provider.subscribeItem(this.hvRequest);
    };

    return tab;
  };


  /** SystemSummary class
   *
   *  @author M.Frank
   */
  lhcb.widgets.SystemSummary = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');
    var split = options.split;
    var sys = options.system;
    var cell;
    tab.className = tb.className   = 'MonitorPage';

    if ( options.style ) {
      var tit = options.title;
      if ( !tit ) tit = 'VELO Subsystem State';
      if ( options.rowing ) 
	tr.appendChild(cell=Cell(tit,5,options.style));
      else 
	tr.appendChild(Cell(tit,4,options.style));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    if ( options.legend ) {
      if ( options.rowing ) tr.appendChild(cell=Cell('',1,null));
      if ( split ) {
	tr.appendChild(cell=Cell('Side A',2,'MonitorDataHeader'));
	tr.appendChild(cell=Cell('Side C',2,'MonitorDataHeader'));
	tb.appendChild(tr);
      }
    }
    tab.subscriptions=[];
    tab.addSystem = function(system,name) {
      tr = document.createElement('tr');
      if ( options.rowing ) tr.appendChild(cell=Cell(name,1,'MonitorDataHeader'));
      if ( split ) {
	this[name+'A'] = FSMItem('lbWeb.'+system+'A_'+name,options.logger,true);
	this[name+'C'] = FSMItem('lbWeb.'+system+'C_'+name,options.logger,true);
	tr.appendChild(this[name+'A']);
	tr.appendChild(this[name+'A'].lock);
	this[name+'A'].style.width = '30%';
	tr.appendChild(this[name+'C']);
	tr.appendChild(this[name+'C'].lock);
	this[name+'C'].style.width = '30%';
	this.subscriptions.push(this[name+'A']);
	this.subscriptions.push(this[name+'C']);
      }
      else {
	this[name]  = FSMItem('lbWeb.'+system+'_'+name,options.logger,true);
	tr.appendChild(this[name]);
	tr.appendChild(this[name].lock);
	this[name].style.width = '30%';
	this.subscriptions.push(this[name]);
      }
      return tr;
    };
    for(var i=0; i<options.items.length;++i) {
      tb.appendChild(tab.addSystem(sys,options.items[i]));
    }
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      for(var i=0; i<this.subscriptions.length;++i) {
	provider.subscribeItem(this.subscriptions[i]);
      }
    };
    return tab;
  };

  lhcb.widgets.DAQEfficiency = function(options) {
    var tb, tr, tab = document.createElement('table');
    var type = options.efficiency_type;
    if ( !type ) type = 'Lumi';
    tab.className  = 'MonitorPage';
    
    tb = document.createElement('tbody');
    tooltips.set(tb,'Run efficiency summary normalized to<br>integrated luminosity of datataking during this fill.');
    tb.className  = 'MonitorPage';
    tb.height    = '120px';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Efficiency normalized to '+type,3,options.style));
      tb.appendChild(tr);
    }
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('HV Ready',1,'MonitorDataHeader'));
      tr.appendChild(Cell('VELO IN',1,'MonitorDataHeader'));
      tr.appendChild(Cell('DAQ stopped',1,'MonitorDataHeader'));
      tr.appendChild(Cell('dead time',1,'MonitorDataHeader'));
    }
    tb.appendChild(tr);
    tab.hv      = StyledItem('lbWeb.LHCbEfficiency.Results'+type+'.HV',null,'%7.2f %%');
    tab.velo    = StyledItem('lbWeb.LHCbEfficiency.Results'+type+'.VELO',null,'%7.2f %%');
    tab.daq     = StyledItem('lbWeb.LHCbEfficiency.Results'+type+'.DAQ',null,'%7.2f %%');
    tab.dead    = StyledItem('lbWeb.LHCbEfficiency.Results'+type+'.DAQLiveTime',null,'%7.2f %%');
    tab.total   = StyledItem('lbWeb.LHCbEfficiency.'+type+'Total',null,'%7.2f %%');
    tr = document.createElement('tr');
    tr.appendChild(tab.hv);
    tr.appendChild(tab.velo);
    tr.appendChild(tab.daq);
    tr.appendChild(tab.dead);
    tr.appendChild(tab.total);
    tb.appendChild(tr);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.hv);
      provider.subscribeItem(this.velo);
      provider.subscribeItem(this.daq);
      provider.subscribeItem(this.dead);
      provider.subscribeItem(this.total);
    };
    return tab;
  };

  lhcb.widgets.SubdetectorPage = function(name, msg) {
    var table       = document.createElement('table');
    table.body      = document.createElement('tbody');
    table.system    = name;
    table.messages  = msg;
    table.provider  = null;
    table.logger    = null;
    table.add = function() {
      var tr = document.createElement('tr');
      var td = document.createElement('td');
      td.setAttribute('colSpan',2);
      tr.appendChild(td);
      this.body.appendChild(tr);
      return td;
    };
    table.className = table.body.className = 'MonitorPage';
    table.display = table.add();
    table.logDisplay = table.add();
    table.appendChild(table.body);
    table.subscriptions = new Array();

    table.hvSummary = function(options) {
      var tab = lhcb.widgets.HVSummary(this.logger);
      tab.system = this.system;
      tab.addHV = function(tb) {};
      if ( options.hv ) tab.addHV = function(tb) { tb.appendChild(this.addState(this.system, true, 'HV', 'HV'));};

      tab.addLV = function(tb) {};
      if ( options.lv ) tab.addLV = function(tb) { tb.appendChild(this.addState(this.system, true, 'LV', 'DCS_Local')); };
      return tab.buildTable({style:'Arial12pt',top:false,hv:options.hv,hv_legend:options.hv,lv:options.lv});
    };

    /// Empty placeholder
    table.attachWidgets = function() {  };

    table.build = function(options) {
      var tab = document.createElement('table');
      var tb = document.createElement('tbody');
      var t1, tb1, tr1, td1, cell, d = new Date();

      tab.className = tb.className = 'MonitorPage';
      tab.width = tb.width  = '100%';
      tab.style.fontSize = '90%';

      this.heading = document.createElement('tr');
      this.heading.appendChild(td=document.createElement('td'));
      td.style.width = '100%';
      td.colSpan = 3;
      td.appendChild(t1=document.createElement('table'));
      t1.appendChild(tb1=document.createElement('tbody'));
      t1.width  = '100%';

      tb1.appendChild(tr1=document.createElement('tr'));
      tr1.appendChild(td1=document.createElement('td'));
      if ( options.logo ) {
	td1.appendChild(cell=Cell(options.logo,null,null));
	if ( options.logo_url ) {
	  cell.url = options.logo_url;
	  cell.onclick = function() { document.location = this.url; };
	}
      }
      tr1.appendChild(this.head_date=Cell(d.toString(),1,'MonitorTinyHeader'));
      this.head_date.id = 'current_time';
      this.head_date.textAlign = 'right';
      cell = Cell(this.system+'&nbsp;'+lhcb_online_picture(),1,'MonitorBigHeader');
      cell.style.textAlign = 'right';
      cell.onclick = function() { document.location = "http://lhcb.cern.ch";};
      tr1.appendChild(cell);
      if ( options.tips ) {
	tooltips.set(this.heading,options.tips);
      }
      tb.appendChild(this.heading);

      this.timerHandler = function() {document.getElementById('current_time').innerHTML = (new Date()).toString(); };
      setInterval(this.timerHandler,2000);

      tr = document.createElement('tr');

      tr.appendChild(td=document.createElement('td'));
      td.style.width = '55%';
      td.appendChild(this.lhc_header=lhcb.widgets.LHC_header());

      tr.appendChild(td=document.createElement('td'));
      td.appendChild(this.run_header=lhcb.widgets.Det_Run_header('LHCb',this.logger));
      td.width = '45%';

      this.lhc_header.style.height = '60px';
      this.run_header.style.height = '60px';
      tb.appendChild(tr);
      this.subscriptions.push(this.lhc_header);
      this.subscriptions.push(this.run_header);


      tr = document.createElement('tr');
      // Left half of the display
      tr.appendChild(td=document.createElement('td'));
      td.appendChild(t1=document.createElement('table'));
      t1.appendChild(this.left=document.createElement('tbody'));
      t1.className = this.left.className = 'MonitorPage';
      t1.style.border = this.left.style.border = 'none';

      // Right hand of the display
      tr.appendChild(td=document.createElement('td'));
      td.appendChild(t1=document.createElement('table'));
      t1.appendChild(this.right=document.createElement('tbody'));
      t1.className = this.right.className = 'MonitorPage';
      t1.style.border = this.right.style.border = 'none';

      this.left.addItem = function(item) {
	var tr1, td1;
	this.appendChild(tr1=document.createElement('tr'));
	tr1.appendChild(td1=document.createElement('td'));
	td1.appendChild(item);
	this.parent.subscriptions.push(item);
      };
      this.left.addSpacer = function(height) {
	var tr1, td1;
	this.appendChild(tr1=document.createElement('tr'));
	tr1.appendChild(td1=document.createElement('td'));
	td1.style.height=height;
      };
      this.right.addItem = this.left.addItem;
      this.right.addSpacer = this.left.addSpacer;
      this.left.parent = this.right.parent = this;

      this.attachWidgets();

      tb.appendChild(tr);

      // Finally add suggestions text
      tr = document.createElement('tr');
      tr.appendChild(Cell('',null,null));
      tr.appendChild(Cell('Comments and suggestions to M.Frank CERN/LHCb',2,'MonitorTinyHeader'));
      tb.appendChild(tr);

      tab.appendChild(tb);
      this.display.appendChild(tab);
      return this;
    };

    table.subscribe = function() {
      for(var i=0; i<this.subscriptions.length; ++i) {
	this.subscriptions[i].subscribe(this.provider);
      }
    };
    return table;
  };

  lhcb.widgets.SubdetectorPage.start = function(creator) {
    var msg  = the_displayObject['messages'];
    var body = document.getElementsByTagName('body')[0];
    var tips = init_tooltips(body);
    var display = creator(msg);

    body.appendChild(display);
    body.className = 'MainBody';
    if ( display.options.title ) setWindowTitle(display.options.title);
    if ( msg > 0 )
    display.logger = new OutputLogger(display.logDisplay, 200, LOG_INFO, 'StatusLogger');
    else
    display.logger = new OutputLogger(display.logDisplay,  -1, LOG_INFO, 'StatusLogger');
    display.provider = new DataProvider(display.logger);
    display.provider.topic = '/topic/status';
    display.build(display.options);
    display.subscribe();
    display.provider.start();
    body.style.cursor = 'default';
    return display;
  };

  lhcb.widgets.SubdetectorPage.stop = function(creator) {
    dataProviderReset();
  };

  /** VELO web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.velo.positionSummary = function(options) {
    var tb, tr, cell, tab = document.createElement('table');
    tab.className  = 'MonitorPage';
    tb = document.createElement('tbody');
    tooltips.set(tb,'Velo Position');
    tb.className  = 'MonitorPage';
    tb.height    = '120px';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Velo Position',4,options.style));
      tb.appendChild(tr);
    }
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Position',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Opening',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Center',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Y:',1,'MonitorDataHeader'));
      tb.appendChild(tr);
    }
    tab.veloState   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Specific.VELO.Position', null, null);
    tab.veloOpening = StyledItem('lbWeb.LHCb_RunInfoCond.VELO.CurrentPosition.Opening',null,'%7.2f mm');
    tab.veloCenter  = StyledItem('lbWeb.LHCb_RunInfoCond.VELO.CurrentPosition.Center',null,'%7.2f mm');
    tab.veloY       = StyledItem('lbWeb.LHCb_RunInfoCond.VELO.CurrentPosition.Y',null,'%7.2f mm');
    tr = document.createElement('tr');
    tr.appendChild(tab.veloState);
    tr.appendChild(tab.veloOpening);
    tr.appendChild(tab.veloCenter);
    tr.appendChild(tab.veloY);
    tb.appendChild(tr);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.veloState);
      provider.subscribeItem(this.veloOpening);
      provider.subscribeItem(this.veloCenter);
      provider.subscribeItem(this.veloY);
    };
    return tab;
  };

  /** VELO web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.velo.moveSummary = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');
    var cell;
    tab.className = tb.className   = 'MonitorPage';
  
    tab.injectionAllowed = StyledItem('lbWeb.veloMove_RdBool.InjectionAllowed', null, null);
    tab.movableAllowed   = StyledItem('lbWeb.veloMove_RdBool.MovableDevicesAllowed', null, null);
    tab.safeBeam         = StyledItem('lbWeb.veloMove_RdBool.SafeStableBeam', null, null);
    if ( options.style ) {
      tr.appendChild(Cell('Velo Move Status',4,options.style));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    if ( options.legend ) {
      tr.appendChild(cell=Cell('Injection Allowed',1,'MonitorDataHeader'));
      tr.appendChild(cell=Cell('Movable Devices Allowed',1,'MonitorDataHeader'));
      tr.appendChild(cell=Cell('Safe Stable Beam',1,'MonitorDataHeader'));
      tb.appendChild(tr);
    }
    tr = document.createElement('tr');
    tr.appendChild(tab.injectionAllowed );
    tab.injectionAllowed.style.width = '33%';
    tr.appendChild(tab.movableAllowed);
    tab.movableAllowed.style.width = '33%';
    tr.appendChild(tab.safeBeam);
    tab.safeBeam.style.width = '33%';
    tb.appendChild(tr);
    tab.appendChild(tb);
  
    tab.subscribe = function(provider) {
      provider.subscribeItem(this.injectionAllowed);
      provider.subscribeItem(this.movableAllowed);
      provider.subscribeItem(this.safeBeam);      
    };
    return tab;
  };

  /** VELO web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.velo.vacuumSummary = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');

    tab.className = tb.className   = 'MonitorPage';
    tab.pe411 = StyledItem('lbWeb.veloVac_Meters.PE411.Vacuum', null, "%7.2e");
    tab.pe412 = StyledItem('lbWeb.veloVac_Meters.PE412.Vacuum', null, "%7.2e");
    tab.pe421 = StyledItem('lbWeb.veloVac_Meters.PE421.Vacuum', null, "%7.2e");
    tab.pe422 = StyledItem('lbWeb.veloVac_Meters.PE422.Vacuum', null, "%7.2e");
    if ( options.style ) {
      tr.appendChild(Cell('Velo Vacuum Status: Pressures in [hPa]',4,options.style));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    if ( options.legend ) {
      tr.appendChild(Cell('PE411',1,'MonitorDataHeader'));
      tr.appendChild(Cell('PE412',1,'MonitorDataHeader'));
      tr.appendChild(Cell('PE421',1,'MonitorDataHeader'));
      tr.appendChild(Cell('PE422',1,'MonitorDataHeader'));
      tb.appendChild(tr);
    }
    tr = document.createElement('tr');
    tr.appendChild(tab.pe411);
    tab.pe411.style.width = '25%';
    tr.appendChild(tab.pe412);
    tab.pe412.style.width = '25%';
    tr.appendChild(tab.pe421);
    tab.pe421.style.width = '25%';
    tr.appendChild(tab.pe422);
    tab.pe422.style.width = '25%';
    tb.appendChild(tr);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.pe411);
      provider.subscribeItem(this.pe412);
      provider.subscribeItem(this.pe421);
      provider.subscribeItem(this.pe422);
    };
    return tab;
  };

  /** VELO web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.velo.infrastructureSummary = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');
    var cell;

    tab.className = tb.className   = 'MonitorPage';
    tab.motion = FSMItem('lbWeb.VELO_MOTION',options.logger,true);
    tab.vacuum = FSMItem('lbWeb.VELO_VACUUM',options.logger,true);
    tab.cool   = FSMItem('lbWeb.VELO_COOLING',options.logger,true);
    tab.coolA  = FSMItem('lbWeb.VEA_COOL',options.logger,true);
    tab.coolC  = FSMItem('lbWeb.VEC_COOL',options.logger,true);

    if ( options.style ) {
      tr.appendChild(Cell('Velo Infrastructure State',3,options.style));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    if ( options.legend ) {
      tr.appendChild(Cell('Cooling',2,'MonitorDataHeader'));
      tr.appendChild(Cell('Side A',2,'MonitorDataHeader'));
      tr.appendChild(Cell('Side C',2,'MonitorDataHeader'));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    tr.appendChild(tab.cool);
    tr.appendChild(tab.cool.lock);
    tab.cool.style.width = '33%';
    tr.appendChild(tab.coolA);
    tr.appendChild(tab.coolA.lock);
    tab.coolA.style.width = '33%';
    tr.appendChild(tab.coolC);
    tr.appendChild(tab.coolC.lock);
    tab.coolC.style.width = '33%';
    tb.appendChild(tr);

    tr = document.createElement('tr');
    if ( options.legend ) {
      tr.appendChild(Cell('Vacuum',2,'MonitorDataHeader'));
      tr.appendChild(Cell('Motion',2,'MonitorDataHeader'));
      tr.appendChild(Cell('',2,null));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    tr.appendChild(tab.vacuum);
    tr.appendChild(tab.vacuum.lock);
    tab.vacuum.style.width = '30%';
    tr.appendChild(tab.motion);
    tr.appendChild(tab.motion.lock);
    tab.motion.style.width = '30%';
    tr.appendChild(cell=Cell('',2,null));
    cell.style.width='100%';
    tb.appendChild(tr);

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.motion);
      provider.subscribeItem(this.vacuum);
      provider.subscribeItem(this.cool);
      provider.subscribeItem(this.coolA);
      provider.subscribeItem(this.coolC);
    };
    return tab;
  };

  /** VELO web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.velo.LVBoards = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr, cell;

    tab.logger = options.logger;
    tab.className = tb.className   = 'MonitorPage';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Velo LV Board State',3,options.style));
      tb.appendChild(tr);
    }
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(cell=Cell('Board',1,'MonitorDataHeader'));
      tr.appendChild(cell=Cell('SideA',2,'MonitorDataHeader'));
      tr.appendChild(cell=Cell('SideC',2,'MonitorDataHeader'));
      tb.appendChild(tr);
    }
    tab.subscriptions=[];
    tab.addBoard = function(name) {
      var cell, tr = document.createElement('tr');
      tr.appendChild(cell=Cell(name,1,'MonitorDataHeader'));
      cell.style.width='30%';
      this['A_'+name] = FSMItem('lbWeb.VELOA_'+name,this.logger,true);
      this['C_'+name] = FSMItem('lbWeb.VELOC_'+name,this.logger,true);

      tr.appendChild(this['A_'+name]);
      tr.appendChild(this['A_'+name].lock);

      tr.appendChild(this['C_'+name]);
      tr.appendChild(this['C_'+name].lock);
      this.subscriptions.push(this['A_'+name]);
      this.subscriptions.push(this['C_'+name]);
      return tr;
    };

    tb.appendChild(tab.addBoard('DCS_LV_PS'));
    tb.appendChild(tab.addBoard('DCS_LV_EB'));
    tb.appendChild(tab.addBoard('DCS_LV_EB00'));
    tb.appendChild(tab.addBoard('DCS_LV_EB01'));
    tb.appendChild(tab.addBoard('DCS_LV_EB02'));
    tb.appendChild(tab.addBoard('DCS_LV_EB03'));
    tb.appendChild(tab.addBoard('DCS_LV_EB04'));
    tb.appendChild(tab.addBoard('DCS_LV_EB05'));
    tb.appendChild(tab.addBoard('DCS_LV_EB06'));
    tb.appendChild(tab.addBoard('DCS_LV_EB07'));
    tb.appendChild(tab.addBoard('DCS_LV_EB08'));
    tb.appendChild(tab.addBoard('DCS_LV_EB09'));

    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Velo HV Board State',3,options.style));
      tb.appendChild(tr);
    }
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(cell=Cell('Board',1,'MonitorDataHeader'));
      tr.appendChild(cell=Cell('SideA',2,'MonitorDataHeader'));
      tr.appendChild(cell=Cell('SideC',2,'MonitorDataHeader'));
      tb.appendChild(tr);
    }
    tb.appendChild(tab.addBoard('HV_BOARD00'));
    tb.appendChild(tab.addBoard('HV_BOARD01'));
    tb.appendChild(tab.addBoard('HV_BOARD02'));
    tb.appendChild(tab.addBoard('HV_BOARD03'));
    tb.appendChild(tab.addBoard('HV_BOARD04'));
    tb.appendChild(tab.addBoard('HV_BOARD05'));

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      for(var i=0; i<this.subscriptions.length;++i) {
	provider.subscribeItem(this.subscriptions[i]);
      }
    };
    return tab;
  };


  if ( _debugLoading ) alert('Script lhcb.display.widgets.cpp loaded successfully');
}
