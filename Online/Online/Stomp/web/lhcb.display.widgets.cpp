if ( !lhcb.widgets ) {

  lhcb.widgets = new Object();
  
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
    td.innerHTML = 'LHC Fill';
    tr.appendChild(td);
  
    tab.lhcFillNumber = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.FillNumber',null,null);
    tr.appendChild(tab.lhcFillNumber);
  
    td = document.createElement('td');
    td.innerHTML = 'is in state:';
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
    }
    return tab;
  }
 
  /** Build table with LHC summary information
   *   
   * @return Reference to HTML table containing all data items of this widget
   *
   * @author  M.Frank
   * @version 1.0
   */
  lhcb.widgets.LHC_summary = function() {

    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className = 'MonitorData';
    tb.className  = 'MonitorData';
    tab.width     = '100%';
    tab.height    = '120px';

    tab.energy        = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Energy',null,"%7.1f GeV");
    //tab.BeamMode      = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.BeamMode',null,null);
    tab.intensity1    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam1.totalIntensity',null,"%9.2e");
    tab.intensity2    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam2.totalIntensity',null,"%9.2e");
    tab.lifetime1     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam1.primitiveLifetime',null,"%7.2f");
    tab.lifetime2     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam2.primitiveLifetime',null,"%7.2f");
    tab.avgLumi       = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Beam.Luminosity.LuminosityAverage',null,"%9.2e");
    tab.LHCbDump     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_BEAMDUMP',null,null);
    tab.LHCbAdjust   = StyledItem('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_ADJUST',null,null);
    tab.LHCbInject   = StyledItem('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_INJECTION',null,null);

    tab.energy.conversion = function(value) {  return value*120.0/1000.0; }

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('LHC status summary',4,'MonitorDataHeader'));

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Energy:',null,'MonitorDataHeader'));
    tr.appendChild(tab.energy);
    tr.appendChild(Cell('Avg.Luminosity:',null,'MonitorDataHeader'));
    tr.appendChild(tab.avgLumi);

    tr = document.createElement('tr');
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Intensity [e]:',null,'MonitorDataHeader'));
    tr.appendChild(tab.intensity1);
    tr.appendChild(Cell('&lt;1 Beam 2&gt;',null,null));
    tr.appendChild(tab.intensity2);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Lifetime [h]:',null,'MonitorDataHeader'));
    tr.appendChild(tab.lifetime1);
    tr.appendChild(Cell('&lt;1 Beam 2&gt;',null,null));
    tr.appendChild(tab.lifetime2);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    td = Cell('Handshakes:',null,'MonitorDataHeader');
    td.rowSpan = 2;
    tr.appendChild(td);
    tr.appendChild(Cell('Dump',null,null));
    tr.appendChild(Cell('Adjust',null,null));
    tr.appendChild(Cell('Injection',null,null));

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
    }
    return tab;
  }

  /**
     Build table like this:

     +-------- LEP operator comments ----------
     |LEP in limbo, awaiting decision ...      
     |
     +-----------------------------------------
  */
  lhcb.widgets.LHC_operator_comments = function() {
    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.width  = '100%';
    tab.height = '100%';
    tb.height  = '100%';
    tab.className = 'MonitorData';
    tb.className = 'MonitorData';
    
    tab.comments   = StyledItem('lbWeb.Background.lhcComments',null,null);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('LHC operator comments:',null,'MonitorDataHeader'));
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(tab.comments);
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
    }
    return tab;
  }

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

    tab.className = 'MonitorData';
    tb.className = 'MonitorData';
    tab.width = '100%';

    tab.comments   = StyledItem('lbWeb.shiftComments',null,null);
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
    }
    return tab;
  }

  /** 
   *   
   * @return Reference to HTML table containing all data items of this widget
   *
   * @author  M.Frank
   * @version 1.0
   */
  lhcb.widgets.Det_Run_header = function(sys,logger) {
    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.width = '100%';
    tab.className = 'MonitorSubHeader';
    tb.className = 'MonitorSubHeader';

    tr = document.createElement('tr');
    tb.appendChild(tr);

    td = document.createElement('td');
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
    }

    return tab;
  }

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

    tab.className  = 'MonitorData';
    tab.width      = '100%';
    tb.className   = 'MonitorData';
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
    tab.magnetField    = StyledItem('lbWeb.lbHyst.B',                               null,'%7.4f');
    tab.magnetPolarity = StyledItem('lbWeb.lbHyst.Polarity',                             null, null);
    tab.veloPosition   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.RunControl.VeloPos',          null, null);
    tab.figureOfMerit1 = StyledItem('lbWeb.BCM_DP_S0.RS2_REL',                           null, 'S0:%7.3f');
    tab.figureOfMerit2 = StyledItem('lbWeb.BCM_DP_S0.RS32_REL',                          null, '%7.3f');
    tab.figureOfMerit3 = StyledItem('lbWeb.BCM_DP_S1.RS2_REL',                           null, 'S1:%7.3f');
    tab.figureOfMerit4 = StyledItem('lbWeb.BCM_DP_S1.RS32_REL',                          null, '%7.3f');
    tab.bcmBeamPermit1 = StyledItem('lbWeb.BCM_Interface.BeamPermit.getStatus',          null, null);
    tab.bcmBeamPermit2 = StyledItem('lbWeb.BCM_Interface.InjPermit1.getStatus',          null, null);
    tab.bcmBeamPermit3 = StyledItem('lbWeb.BCM_Interface.InjPermit2.getStatus',          null, null);

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

    // Run status information
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
      return this;
    }
    return tab;
  }

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
  }
  
  if ( _debugLoading ) alert('Script lhcb.display.widgets.cpp loaded successfully');
}
