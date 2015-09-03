if ( !lhcb.widgets ) {

  lhcb.widgets = new Object();

  /// Check initialization
  lhcb.widgets.check = function() {};
  
  /// Set document location to LHCb DAQ status page
  lhcb.widgets.goto_lhcb_page = function(name) {
    var item = function() {    document.location = this.url; };
    item.url =  lhcb.constants.lhcb_display_url(name);
    return item;
  };
  /// Set document location to LHCb DAQ status page
  lhcb.widgets.goto_lhcb_daq_page = function() {
    document.location =  lhcb.constants.lhcb_display_url("status&system=LHCb");
  };
  /// Set document location to LHC operations page
  lhcb.widgets.goto_lhc_operations_page = function() {
    document.location =  lhcb.constants.operations_url("LHC3");
  };

  lhcb.widgets.green_smiley  = function() { 
    return '<IMG src="'+lhcb.constants.mkStaticImage('smileys/smiley4162.gif').src+'" height="25"></IMG>';   
  };
  lhcb.widgets.yellow_smiley = function() {
    return '<IMG src="'+lhcb.constants.mkStaticImage('smileys/smiley1807.gif').src+'" height="25"></IMG>';   
  };
  lhcb.widgets.red_smiley    = function() {
    return '<IMG src="'+lhcb.constants.mkStaticImage('smileys/free-mad-smileys-120.gif').src+'" height="25"></IMG>';
  };
  lhcb.widgets.blue_smiley   = function() { 
    return '<IMG src="'+lhcb.constants.mkStaticImage('smileys/smiley153.gif').src+'" height="15"></IMG>';   
  };

  lhcb.widgets.button = function(value,onclick) {
    var but, cell = document.createElement('td');
    if ( _isInternetExplorer() ) {
      cell.appendChild(but=document.createElement('button'));
    }
    else {
      cell.appendChild(but=document.createElement('input'));
      but.type = 'button';
    }
    but.value = value;
    but.onclick = onclick;
    return [cell,but];
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
  
    tab.className = tb.className = 'MonitorSubHeader';
  
    tab.body = tb;
    tb.appendChild(tr=document.createElement('tr'));
  
    tr.appendChild(td=document.createElement('td'));
    td.innerHTML = 'LHC&nbsp;Fill';
  
    tab.lhcFillNumber = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.FillNumber',null,null);
    tr.appendChild(tab.lhcFillNumber);
  
    tr.appendChild(td=document.createElement('td'));
    td.innerHTML = 'State:';
  
    tab.lhcBeamMode = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.BeamMode',null,null);
    tr.appendChild(tab.lhcBeamMode);
  
    tr.appendChild(td=document.createElement('td'));
    td.innerHTML = '';
  
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
 
  /** Build table with summary of the LHC
   *   
   * @return Reference to HTML table containing all data items of this widget
   *
   * @author  M.Frank
   * @version 1.0
   */
  lhcb.widgets.LHC_header2 = function() {
    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');
  
    tab.className = tb.className = 'MonitorSubHeader';
  
    tab.body = tb;
    tb.appendChild(tr=document.createElement('tr'));
  
    tr.appendChild(td=document.createElement('td'));
    td.innerHTML = 'LHC&nbsp;Fill';
    td.className = 'Text-Right';
  
    tab.lhcFillNumber = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.FillNumber','Text-Left',null);
    tr.appendChild(tab.lhcFillNumber);
  
    tab.lhcBeamMode = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.BeamMode','Text-Right','State:%s&nbsp;/&nbsp;');
    tr.appendChild(tab.lhcBeamMode);

    tab.lhcMachineMode = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.MachineMode','Text-Left',null);
    tr.appendChild(tab.lhcMachineMode);
  
    tr.appendChild(td=document.createElement('td'));
    td.innerHTML = '';
  
    tab.appendChild(tb);

    /** Subscribe all required data item to receive data from data provider object
     *
     *  @param      provider  Data provider object
     *
     *  @return On success reference to self, null otherwise
     */
    tab.subscribe = function(provider) {
      this.lhcBeamMode.subscribe(provider);
      this.lhcMachineMode.subscribe(provider);
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
    //tab.height    = '120px';

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
    tab.className = tb.className = 'MonitorPage';

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
    tab.className = tb.className = 'MonitorPage';

    tab.comments   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Plan',null,null);
    tab.comments.conversion = function(data) { 
      //alert(data);
      var s = data.replace(/\n/g,'<BR>');
      return s;
    };
    tab.comments.style.fontWeight = 'normal';

    if ( options.style) {
      tr = document.createElement('tr');
      tb.appendChild(tr);
      tr.appendChild(Cell('Plan of the day:',null,options.style));
    }
    tb.appendChild(tr=document.createElement('tr'));
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
    var td, tr, tab = document.createElement('table');
    var tb  = document.createElement('tbody');

    tab.style.height = tb.style.height = '100%';
    tab.className = tb.className = 'MonitorSubHeader';

    tab.runNumber = StyledItem('lbWeb.'+sys+'_RunInfo.general.runNumber',null,null);
    tab.runState = FSMItem('lbWeb.'+sys,logger,true);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(td=document.createElement('td'));
    td.innerHTML = 'Run';    
    tr.appendChild(tab.runNumber);
    tr.appendChild(td=document.createElement('td'));
    td.innerHTML = ' is ';
    tr.appendChild(tab.runState);
    tr.appendChild(tab.runState.lock);
    tr.appendChild(td=document.createElement('td'));
    td.innerHTML = '';

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
  lhcb.widgets.Det_Run_Summary = function(opts) {
    var sys = opts.system;
    var logger = opts.logger;
    var tb, td, tr, rates, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className  = tb.className = 'MonitorPage';
    tb.cellSpacing = 0;
    tb.cellPadding = 0;

    tab.runType        = StyledItem('lbWeb.'+sys+'_RunInfo.general.runType',     null,null);
    tab.runStart       = StyledItem('lbWeb.'+sys+'_RunInfo.general.runStartTime',null,'Run started at: %s');
    tab.nTriggers      = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.nTriggers',      'Text-Right',null);
    //tab.hltNTriggers   = StyledItem('lbWeb.'+sys+'_RunInfo.HLTFarm.hltNTriggers',null,null);
    tab.deadTime       = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.deadTime',        null,'%8.2f %%');
    tab.deadTimeRun    = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.runDeadTime',     null,'Run:%8.2f %%');
    tab.trgConfig      = StyledItem('lbWeb.'+sys+'_RunInfo.Trigger.TCKLabel',    null,null);
    tab.magnetField    = StyledItem('lbWeb.lbHyst.B',                            'Text-Right','%7.4f');
    tab.magnetPolarity = StyledItem('lbWeb.lbHyst.Polarity',                     null, null);
    tab.hvState        = StyledItem('lbWeb.LHCb_LHC_HV.FSM.state','MonitorDataHeader',null);
    tab.hvState.conversion = function(data) {  return 'LV & HV:'+(data.split('/')[1]); };
    tab.veloPosition   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Specific.VELO.Position','MonitorDataHeader','VELO %s');
    tab.figureOfMerit1 = StyledItem('lbWeb.BCM_DP_S0.RS2_REL',                  'Text-Right', 'S0:%7.3f');
    tab.figureOfMerit2 = StyledItem('lbWeb.BCM_DP_S0.RS32_REL',                 'Text-Right', '%7.3f');
    tab.figureOfMerit3 = StyledItem('lbWeb.BCM_DP_S1.RS2_REL',                  'Text-Right', 'S1:%7.3f');
    tab.figureOfMerit4 = StyledItem('lbWeb.BCM_DP_S1.RS32_REL',                 'Text-Right', '%7.3f');
    tab.bcmBeamPermit1 = StyledItem('lbWeb.BCM_Interface.BeamPermit.getStatus', 'Text-Right', null);
    tab.bcmBeamPermit2 = StyledItem('lbWeb.BCM_Interface.InjPermit1.getStatus', 'Text-Right', null);
    tab.bcmBeamPermit3 = StyledItem('lbWeb.BCM_Interface.InjPermit2.getStatus', 'Text-Right', null);
    tab.interActionRate= StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.TriggerRates.TrgRate_calo_bb','Text-Right','%7.0f Hz');

    tab.l0Rate         = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.triggerRate',    'Text-Right','%6.0f');
    tab.l0RateRun      = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.runTriggerRate', 'Text-Right','%6.0f');
    tab.hltRate        = StyledItem('lbWeb.'+sys+'_RunInfo.HLTFarm.hltRate',    'Text-Right','%4.0f');
    tab.hltRateRun     = StyledItem('lbWeb.'+sys+'_RunInfo.HLTFarm.runHltRate', 'Text-Right','%4.0f');
    tab.physicsRateEx  = StyledItem('lbWeb.LHCB_STATS.PhysEx',   'Text-Right',   '%4.0f');
    tab.physicsRateIn  = StyledItem('lbWeb.LHCB_STATS.PhysIn',   'Text-Right',   '%4.0f');
    tab.LumiRateEx     = StyledItem('lbWeb.LHCB_STATS.LumiEx',   'Text-Right',   '%4.0f');
    tab.LumiRateIn     = StyledItem('lbWeb.LHCB_STATS.LumiIn',   'Text-Right',   '%4.0f');
    tab.LumiRateEx     = StyledItem('lbWeb.LHCB_STATS.LumiEx',   'Text-Right',   '%4.0f');
    tab.LumiRateIn     = StyledItem('lbWeb.LHCB_STATS.LumiIn',   'Text-Right',   '%4.0f');
    tab.minbRateEx     = StyledItem('lbWeb.LHCB_STATS.MBiasEx',  'Text-Right',   '%4.0f');
    tab.minbRateIn     = StyledItem('lbWeb.LHCB_STATS.MBiasIn',  'Text-Right',   '%4.0f');
    tab.BeamGasEx      = StyledItem('lbWeb.LHCB_STATS.BeamGasEx','Text-Right',   '%4.0f');
    tab.BeamGasIn      = StyledItem('lbWeb.LHCB_STATS.BeamGasIn','Text-Right',   '%4.0f');
    tab.RandEx         = StyledItem('lbWeb.LHCB_STATS.RandEx',   'Text-Right',   '%4.0f');
    tab.RandIn         = StyledItem('lbWeb.LHCB_STATS.RandIn',   'Text-Right',   '%4.0f');
    tab.instantLumi    = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_GP', null, '%7.2f &mu;b<sup>-1</sup>/s');

    // Run status
    tb.appendChild(tr=document.createElement('tr'));
    tab.runType.colSpan = 2;
    tr.appendChild(tab.runType);
    tab.runStart.colSpan = 6;
    tr.appendChild(tab.runStart);
    tab.runStart.style.textAlign = 'left';

    // Magnet status
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(td=Cell('Magnet [&nbsp;T&nbsp;]:',2,'MonitorDataHeader'));
    td.width = '15%';
    tr.appendChild(tab.magnetField);
    tr.appendChild(Cell('Polarity:',2,'MonitorDataHeader'));
    tr.appendChild(tab.magnetPolarity);
    tab.magnetPolarity.conversion = function(data) {
      if ( data>0 ) return '+&nbsp;(Down)';
      return '-&nbsp;(Up)';
    };
    tr.appendChild(tab.hvState);

    // Background status
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('BCM BKG [&permil;]:',2,'MonitorDataHeader'));
    tr.appendChild(tab.figureOfMerit1);
    tr.appendChild(td=tab.figureOfMerit2);
    td.colSpan = 2;
    tr.appendChild(tab.figureOfMerit3);
    tr.appendChild(tab.figureOfMerit4);

    // Beam permits
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('Beam Permits:',2,'MonitorDataHeader'));
    tr.appendChild(tab.bcmBeamPermit1);
    tr.appendChild(td=tab.bcmBeamPermit2);
    td.colSpan = 2;
    tr.appendChild(tab.bcmBeamPermit3);
    tr.appendChild(tab.veloPosition);    // Velo position

    // Luminosity and interaction information
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('Int.rate:',2,'MonitorDataHeader'));
    tr.appendChild(tab.interActionRate);
    tr.appendChild(Cell('Inst.Lumi:',2,'MonitorDataHeader'));
    tr.appendChild(tab.instantLumi);
    tab.instantLumi.colSpan = 2;

    // Event processing information
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('L0 Events:',2,'MonitorDataHeader'));
    tr.appendChild(tab.nTriggers);
    tr.appendChild(Cell('Dead&nbsp;time:',2,'MonitorDataHeader'));
    tr.appendChild(tab.deadTime);
    tr.appendChild(tab.deadTimeRun);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(rates=document.createElement('td'));
    rates.style.width='100%';
    rates.colSpan = 7;
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(td=Cell('TCK:',1,'MonitorDataHeader'));
    td.width = '5%';
    tab.trgConfig.colSpan=8;
    tr.appendChild(tab.trgConfig);
    tab.appendChild(tb);


    var t=document.createElement('table');
    tb = document.createElement('tbody');

    t.className  = tb.className = 'MonitorPage';
    tb.cellSpacing = 0;
    tb.cellPadding = 0;
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(td=Cell('Rates',   1,'MonitorDataHeader'));
    td.width = '12%';
    tr.appendChild(td=Cell('L0&nbsp;[Hz]',  1,'MonitorDataHeaderRight'));
    td.width = '12%';
    tr.appendChild(td=Cell('HLT&nbsp;[Hz]', 1,'MonitorDataHeaderRight'));
    td.width = '12%';
    tr.appendChild(td=Cell('Phys',2,'MonitorDataHeaderRight'));
    td.width = '20%';
    tr.appendChild(td=Cell('MinB',1,'MonitorDataHeaderRight'));
    td.width = '12%';
    tr.appendChild(td=Cell('Lumi',1,'MonitorDataHeaderRight'));
    td.width = '12%';
    tr.appendChild(td=Cell('BGas',1,'MonitorDataHeaderRight'));
    td.width = '12%';
    tr.appendChild(td=Cell('Rndm',1,'MonitorDataHeaderRight'));
    td.width = '12%';

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(td=Cell('Now:<BR>Run:',   1,'MonitorDataHeader'));
    td.rowSpan = 2;
    tr.appendChild(tab.l0Rate);
    tr.appendChild(tab.hltRate);
    tr.appendChild(td=Cell('Inc&nbsp;[Hz]:<BR>Exc&nbsp;[Hz]:', 1,'MonitorDataHeaderLeft'));
    td.rowSpan = 2;
    td.style.width = '10%';
    tr.appendChild(tab.physicsRateIn);
    tr.appendChild(tab.minbRateIn);
    tr.appendChild(tab.LumiRateIn);
    tr.appendChild(tab.BeamGasIn);
    tr.appendChild(tab.RandIn);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(tab.l0RateRun);
    tr.appendChild(tab.hltRateRun);
    tr.appendChild(tab.physicsRateEx);
    tr.appendChild(tab.minbRateEx);
    tr.appendChild(tab.LumiRateEx);
    tr.appendChild(tab.BeamGasEx);
    tr.appendChild(tab.RandEx);
    t.appendChild(tb);
    rates.appendChild(t);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.runType);
      provider.subscribeItem(this.runStart);
      provider.subscribeItem(this.nTriggers);
      //provider.subscribeItem(this.hltNTriggers);
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
      provider.subscribeItem(this.hvState);
      
      provider.subscribeItem(this.figureOfMerit1);
      provider.subscribeItem(this.figureOfMerit2);
      provider.subscribeItem(this.figureOfMerit3);
      provider.subscribeItem(this.figureOfMerit4);

      provider.subscribeItem(this.instantLumi);
      provider.subscribeItem(this.interActionRate);

      provider.subscribeItem(this.physicsRateIn);
      provider.subscribeItem(this.physicsRateEx);
      provider.subscribeItem(this.LumiRateIn);
      provider.subscribeItem(this.LumiRateEx);
      provider.subscribeItem(this.minbRateIn);
      provider.subscribeItem(this.minbRateEx);
      provider.subscribeItem(this.BeamGasEx);    
      provider.subscribeItem(this.BeamGasIn);    
      provider.subscribeItem(this.RandEx);
      provider.subscribeItem(this.RandIn);

      provider.subscribeItem(this.trgConfig);
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
    tr.appendChild(c=Cell('LHCb LV & HV is:',1,null));    
    c.style.width  = '120px';
    this.hvState   = StyledItem('lbWeb.LHCb_LHC_HV.FSM.state',null,null);
    //this.hvState = StyledItem('lbWeb.LHCb_HV.FSM.state',null,null);
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
    tb.onclick = function() { document.location = lhcb.constants.lhcb_display_url("lhc");};
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
    tab.onclick = function() { document.location = lhcb.constants.lhcb_display_url("lhc");};
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

    tab.runType        = StyledItem('lbWeb.'+sys+'_RunInfo.general.runType',     null, null);
    tab.nTriggers      = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.nTriggers',       'Text-Right', null);
    tab.hltNTriggers   = StyledItem('lbWeb.'+sys+'_RunInfo.HLTFarm.hltNTriggers','Text-Right', null);
    tab.l0Rate         = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.triggerRate',     'Text-Right','%8.2f Hz');
    tab.l0RateRun      = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.runTriggerRate',  'Text-Right','%8.2f Hz');
    tab.hltRate        = StyledItem('lbWeb.'+sys+'_RunInfo.HLTFarm.hltRate',     'Text-Right','%8.2f Hz');
    tab.hltRateRun     = StyledItem('lbWeb.'+sys+'_RunInfo.HLTFarm.runHltRate',  'Text-Right','%8.2f Hz');
    tab.deadTime       = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.deadTime',        'Text-Right','%8.2f %%');
    tab.deadTimeRun    = StyledItem('lbWeb.'+sys+'_RunInfo.TFC.runDeadTime',     'Text-Right','%8.2f %%');
    tab.interActionRate= StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.TriggerRates.TrgRate_calo_bb', 'Text-Right', '%7.1f Hz');
    tab.instantLumi    = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Internal.Luminosity.LumiInst_GP', 'Text-Right', '%7.2f Hz/&mu;b');
    tab.physRateEx = StyledItem('lbWeb.LHCB_STATS.PhysEx',   'Text-Right','%7.0f Hz');
    tab.physRateIn = StyledItem('lbWeb.LHCB_STATS.PhysIn',   'Text-Right','%7.0f Hz');
    tab.minbRateEx = StyledItem('lbWeb.LHCB_STATS.MBiasEx',  'Text-Right','%7.0f Hz');
    tab.minbRateIn = StyledItem('lbWeb.LHCB_STATS.MBiasIn',  'Text-Right','%7.0f Hz');
    tab.LumiEx     = StyledItem('lbWeb.LHCB_STATS.LumiEx',   'Text-Right','%7.0f Hz');
    tab.LumiIn     = StyledItem('lbWeb.LHCB_STATS.LumiIn',   'Text-Right','%7.0f Hz');
    tab.BeamGasEx  = StyledItem('lbWeb.LHCB_STATS.BeamGasEx','Text-Right','%7.0f Hz');
    tab.BeamGasIn  = StyledItem('lbWeb.LHCB_STATS.BeamGasIn','Text-Right','%7.0f Hz');
    tab.RandEx     = StyledItem('lbWeb.LHCB_STATS.RandEx',   'Text-Right','%7.0f Hz');
    tab.RandIn     = StyledItem('lbWeb.LHCB_STATS.RandIn',   'Text-Right','%7.0f Hz');

    if ( options.style ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Data taking rates and instantaneous luminosity',5,options.style));
    }

    // Run status
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('Activity:',null,'MonitorDataHeader'));
    tr.appendChild(tab.runType);
    tab.runType.colSpan = 4;
    tab.runType.style.textAlign = 'left';

    // Luminosity and interaction information
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('Interaction rate:',null,'MonitorDataHeader'));
    tab.interActionRate.colSpan = 2;
    tr.appendChild(tab.interActionRate);
    tr.appendChild(Cell('Inst.Lumi:',1,'MonitorDataHeader'));
    tr.appendChild(tab.instantLumi);

    // Event processing information
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('L0 Events:',null,'MonitorDataHeader'));
    tab.nTriggers.colSpan = 2;
    tr.appendChild(tab.nTriggers);
    tr.appendChild(Cell('accepted:',1,'MonitorDataHeader'));
    tr.appendChild(tab.hltNTriggers);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('L0 rate:',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Now:',1,'Text-Right'));
    tr.appendChild(tab.l0Rate);
    tr.appendChild(Cell('Run:',1,'Text-Right'));
    tr.appendChild(tab.l0RateRun);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('HLT rate:',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Now:',1,'Text-Right'));
    tr.appendChild(tab.hltRate);
    tr.appendChild(Cell('Run:',1,'Text-Right'));
    tr.appendChild(tab.hltRateRun);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('Dead-time:',null,'MonitorDataHeader'));
    tr.appendChild(Cell('Now:',1,'Text-Right'));
    tr.appendChild(tab.deadTime);
    tr.appendChild(Cell('Run:',1,'Text-Right'));
    tr.appendChild(tab.deadTimeRun);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('Physics:',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Exclusive:',1,'Text-Right'));
    tr.appendChild(tab.physRateEx);
    tr.appendChild(Cell('Inclusive:',1,'Text-Right'));
    tr.appendChild(tab.physRateIn);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('Minimum Bias:',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Exclusive:',1,'Text-Right'));
    tr.appendChild(tab.minbRateEx);
    tr.appendChild(Cell('Inclusive:',1,'Text-Right'));
    tr.appendChild(tab.minbRateIn);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('Beam gas:',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Exclusive:',1,'Text-Right'));
    tr.appendChild(tab.BeamGasEx);
    tr.appendChild(Cell('Inclusive:',1,'Text-Right'));
    tr.appendChild(tab.BeamGasIn);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('Lumi triggers:',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Exclusive:',1,'Text-Right'));
    tr.appendChild(tab.LumiEx);
    tr.appendChild(Cell('Inclusive:',1,'Text-Right'));
    tr.appendChild(tab.LumiIn);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('Random triggers:',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Exclusive:',1,'Text-Right'));
    tr.appendChild(tab.RandEx);
    tr.appendChild(Cell('Inclusive:',1,'Text-Right'));
    tr.appendChild(tab.RandIn);

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

      provider.subscribeItem(this.physRateEx);    
      provider.subscribeItem(this.physRateIn);    
      provider.subscribeItem(this.minbRateEx);    
      provider.subscribeItem(this.minbRateIn);    
      provider.subscribeItem(this.LumiEx);    
      provider.subscribeItem(this.LumiIn);    
      provider.subscribeItem(this.BeamGasEx);    
      provider.subscribeItem(this.BeamGasIn);    
      provider.subscribeItem(this.RandEx);
      provider.subscribeItem(this.RandIn);
      return this;
    };
    return tab;
  };

  lhcb.widgets.MagnetSummary = function(options) {
    var tb, tr, tab = document.createElement('table');
    tab.className  = 'MonitorPage';
    
    tb = document.createElement('tbody');
    tooltips.set(tb,'Magnet summary<br>Click to move to magnet page.');
    tb.onclick = function() { document.location = lhcb.constants.lhcb_display_url("magnet");};
    tb.className  = 'MonitorPage';
    tb.height    = '120px';
    if ( options.style ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Magnet Status',3,options.style));
    }
    if ( options.legend ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Set Current',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Measured Current',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Polarity',1,'MonitorDataHeader'));
    }
    tb.appendChild(tr=document.createElement('tr'));
    tab.magSet      = StyledItem('lbWeb.LbMagnet.SetCurrent',null,'%7.0f A');
    tab.magRead     = StyledItem('lbWeb.LbMagnet.Current',null,'%7.0f A');
    tab.magPolarity = StyledItem('lbWeb.LbMagnet.Polarity',null,null);
    tab.magPolarity.conversion = function(data) {      return data>0 ? 'Down' : 'Up';    };
    tr.appendChild(tab.magSet);
    tr.appendChild(tab.magRead);
    tr.appendChild(tab.magPolarity);

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
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Cooling Plant Status',5,options.style));
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

    tb.appendChild(tr=document.createElement('tr'));
    if ( options.legend ) tr.appendChild(Cell('Alarms',1,'MonitorDataHeader'));

    tr.appendChild(tab.itCoolingAlarms);
    tr.appendChild(tab.ttCoolingAlarms);
    tr.appendChild(tab.otCoolingAlarms);
    tr.appendChild(tab.richCoolingAlarms);

    tb.appendChild(tr=document.createElement('tr'));
    if ( options.legend ) tr.appendChild(Cell('Faults',1,'MonitorDataHeader'));

    tr.appendChild(tab.itCoolingFaults);
    tr.appendChild(tab.ttCoolingFaults);
    tr.appendChild(tab.otCoolingFaults);
    tr.appendChild(tab.richCoolingFaults);

    /*
    tb.appendChild(tr=document.createElement('tr'));
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
    tab.onclick = function() { document.location = lhcb.constants.lhcb_display_url("bcm&sensors=1");};
    tab.className = tb.className = 'MonitorPage';

    if ( options.style ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(c=Cell('Background Status and Beam Permits:',7,options.style));
      c.style.width='100%';
    }

    tab.bcmBeamPermit1 = StyledItem('lbWeb.BCM_Interface.BeamPermit.getStatus',null,null);
    tab.bcmBeamPermit2 = StyledItem('lbWeb.BCM_Interface.InjPermit1.getStatus',null,null);
    tab.bcmBeamPermit3 = StyledItem('lbWeb.BCM_Interface.InjPermit2.getStatus',null,null);
    tb.appendChild(tr=document.createElement('tr'));
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

    tab.figureOfMerit1 = StyledItem('lbWeb.BCM_DP_S0.RS2_REL',             null, '%7.3f');
    tab.figureOfMerit2 = StyledItem('lbWeb.BCM_DP_S0.RS32_REL',            null, '%7.3f');
    tab.figureOfMerit3 = StyledItem('lbWeb.BCM_DP_S1.RS2_REL',             null, '%7.3f');
    tab.figureOfMerit4 = StyledItem('lbWeb.BCM_DP_S1.RS32_REL',            null, '%7.3f');
    tb.appendChild(tr=document.createElement('tr'));
    if ( options.legend ) {
      tr.appendChild(Cell('FoM',null,'MonitorDataHeader'));
    }
    tr.appendChild(Cell('S0.RS2/32:',1,'MonitorDataHeader'));
    tr.appendChild(tab.figureOfMerit1);
    tr.appendChild(tab.figureOfMerit2);
    tr.appendChild(Cell('S1.RS2/32:',1,'MonitorDataHeader'));
    tr.appendChild(tab.figureOfMerit3);
    tr.appendChild(tab.figureOfMerit4);
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
    var tr, cell, tab = document.createElement('table');
    var tb = document.createElement('tbody');

    tab.className =  tb.className = 'MonitorPage';
    tooltips.set(tb,'Safety status of the subdetectors');
    if ( options.style ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Safety',3,options.style));
    }
    if ( options.legend ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Subdetector',1,'MonitorDataHeader'));
      tr.appendChild(Cell('State',1,'MonitorDataHeader'));
      tr.appendChild(Cell('',1,null));
    }
    var opt = (!options.legend && !options.style) ? ' Safety Status' : '';
    if ( options.all || options.rich ) tab.richSafety = FSMItem('lbWeb.RICH_Safety',options.logger,true);
    if ( options.all || options.muon ) tab.muonSafety = FSMItem('lbWeb.MUON_Safety',options.logger,true);
    if ( options.all || options.tt   ) tab.ttSafety   = FSMItem('lbWeb.TT_Safety',options.logger,true);
    if ( options.all || options.it   ) tab.itSafety   = FSMItem('lbWeb.IT_Safety',options.logger,true);
    if ( options.all || options.tt   ) tb.appendChild(lhcb.widgets.mkFSMitem1(tab.ttSafety,'TT'+opt));
    if ( options.all || options.it   ) tb.appendChild(lhcb.widgets.mkFSMitem1(tab.itSafety,'IT'+opt));
    if ( options.all || options.rich ) tb.appendChild(lhcb.widgets.mkFSMitem1(tab.richSafety,'RICH'+opt));
    if ( options.all || options.muon ) tb.appendChild(lhcb.widgets.mkFSMitem1(tab.muonSafety,'MUON'+opt));
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      if (this.ttSafety)   provider.subscribeItem(this.ttSafety);
      if (this.itSafety)   provider.subscribeItem(this.itSafety);
      if (this.richSafety) provider.subscribeItem(this.richSafety);
      if (this.muonSafety) provider.subscribeItem(this.muonSafety);
    };

    return tab;
  };

  /** Online widgets. RICH gas pressures
   *
   *  @author M.Frank
   */
  lhcb.widgets.RICHPressures = function(options) {
    var tb, tr, cell, tab = document.createElement('table');

    tb = document.createElement('tbody');
    tooltips.set(tb,'Rich parameters');
    tab.className =  tb.className = 'MonitorPage';

    if ( options.style ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Rich1 Gas',2,options.style));
      tr.appendChild(Cell('Rich2 Gas',2,options.style));
    }
    if ( options.legend ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Pressure',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Temperature',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Pressure',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Temperature',1,'MonitorDataHeader'));
    }
    if ( options.tips    ) tooltips.set(tb,options.tips);
    if ( options.onclick ) tb.onclick = options.onclick; 

    tb.appendChild(tr);
    tab.rich1Pressure = StyledItem('lbWeb.LHCb_RunInfoCond.RICH1.R1HltGasParameters.Pressure',null,'%7.1f hPa');
    tab.rich1Temp     = StyledItem('lbWeb.LHCb_RunInfoCond.RICH1.R1HltGasParameters.Temperature',null,'%7.1f K');
    tab.rich2Pressure = StyledItem('lbWeb.LHCb_RunInfoCond.RICH2.R2HltGasParameters.Pressure',null,'%7.2f hPa');
    tab.rich2Temp     = StyledItem('lbWeb.LHCb_RunInfoCond.RICH2.R2HltGasParameters.Temperature',null,'%7.1f K');

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(tab.rich1Pressure);
    tr.appendChild(tab.rich1Temp);
    tr.appendChild(tab.rich2Pressure);
    tr.appendChild(tab.rich2Temp);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.rich1Pressure);
      provider.subscribeItem(this.rich1Temp);
      provider.subscribeItem(this.rich2Pressure);
      provider.subscribeItem(this.rich2Temp);
    };
    return tab;
  };
  lhcb.widgets.rich = new Object();
  lhcb.widgets.rich.Pressures = lhcb.widgets.RICHPressures;

  /** Online widgets. HV Summary for subdetector(s) from ECS project
   *
   *  @author M.Frank
   */
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
      tb.appendChild(this.addState('HRC',  false, 'HV', 'HV'));
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
      tb.className = 'MonitorPage';
      tb.height    = '120px';
      if ( options.split ) this.split = true;
      tooltips.set(tb,'HT summary of LHCb');
      if ( options.style ) {
	tb.appendChild(tr=document.createElement('tr'));
	tr.appendChild(Cell('Voltages',6,options.style));
      }
      if ( options.top ) {
	tb.appendChild(tr=document.createElement('tr'));
	tr.appendChild(Cell('System',1,'MonitorDataHeader'));
	tr.appendChild(Cell('State',1,'MonitorDataHeader'));
	tr.appendChild(Cell('Requested',1,'MonitorDataHeader'));

	this.hvState   = FSMItem('lbWeb.LHCb_LHC_HV',this.logger,true);
	this.hvRequest = StyledItem('lbWeb.LHCb_LHC_HV_Info.requestedState',null,null);
	
	tb.appendChild(tr=document.createElement('tr'));
	tr.appendChild(Cell('LHCb HV &amp; LV',1,'MonitorDataHeader'));
	tr.appendChild(this.hvState);
	tr.appendChild(this.hvRequest);
	tr.appendChild(this.hvState.lock);
	this.hvState.lock.className = null;
	this.appendChild(tb);
      }

      tb = document.createElement('tbody');
      tooltips.set(tab,'High voltage state of the subdetectors');
      tb.className  = 'MonitorPage';
      tb.height    = '120px';
      if ( options.hv ) {
	tb.appendChild(tr=document.createElement('tr'));
	tr.appendChild(Cell('High Voltage',6,'Arial12pt'));
      }
      if ( options.hv_legend ) {
	tb.appendChild(tr=document.createElement('tr'));
	tr.appendChild(cell=Cell('Subdetector',1,'MonitorDataHeader'));
	tr.appendChild(cell=Cell('State',1,'MonitorDataHeader'));
	tr.appendChild(cell=Cell('Request',1,'MonitorDataHeader'));
	cell.style.width = '20%';
	tr.appendChild(Cell(this.split ? 'HV State Side A / C' : 'HV State',2,'MonitorDataHeader'));
      }
      this.addHV(tb);
      this.appendChild(tb);
      
      tb = document.createElement('tbody');
      tooltips.set(tb,'Low voltage state of the subdetectors');
      tb.className  = 'MonitorPage';
      tb.height    = '120px';
      if ( options.lv ) {
	tb.appendChild(tr=document.createElement('tr'));
	tr.appendChild(Cell('Low Voltage',6,'Arial12pt'));
      }
      if ( options.lv_legend ) {
	tb.appendChild(tr=document.createElement('tr'));
	tr.appendChild(Cell('Subdetector',1,'MonitorDataHeader'));
	tr.appendChild(Cell('State',1,'MonitorDataHeader'));
	tr.appendChild(cell=Cell('Request',1,'MonitorDataHeader'));
	tr.appendChild(Cell('LV State Side A / Side C',2,'MonitorDataHeader'));
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
      this.subscribe_i(provider, 'HV', 'HRC');

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
    var tb  = document.createElement('tbody');
    var tr  = document.createElement('tr');
    var sys = options.system;
    var split = options.split;
    var cell;
    tab.className = tb.className   = 'MonitorPage';
    tab.style.height = tb.style.height = '100%';
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
	if ( system )
	  this[name]  = FSMItem('lbWeb.'+system+'_'+name,options.logger,true);
	else
	  this[name]  = FSMItem('lbWeb.'+name,options.logger,true);
	tr.appendChild(this[name]);
	tr.appendChild(this[name].lock);
	this[name].style.width = '30%';
	this.subscriptions.push(this[name]);
      }
      return tr;
    };
    for(var i=0; i<options.items.length;++i)
      tb.appendChild(tab.addSystem(sys,options.items[i]));
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      for(var i=0; i<this.subscriptions.length;++i)
	provider.subscribeItem(this.subscriptions[i]);
    };
    return tab;
  };

  /** Online widgets. DAQ efficiency
   *
   *  @author M.Frank
   */
  lhcb.widgets.DAQEfficiency = function(options) {
    var tr, tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var type = options.efficiency_type;
    if ( !type ) type = 'Lumi';
    tab.className  = tb.className = 'MonitorPage';
    
    tooltips.set(tb,'Run efficiency summary normalized to<br> '+type+' of datataking during this fill.');
    tb.height    = '120px';
    if ( options.style ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Efficiency normalized to '+type,3,options.style));
    }
    if ( options.legend ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('HV Ready',   1,'MonitorDataHeader'));
      tr.appendChild(Cell('VELO IN',    1,'MonitorDataHeader'));
      tr.appendChild(Cell('DAQ stopped',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Dead time',  1,'MonitorDataHeader'));
    }
    tab.hv      = StyledItem('lbWeb.LHCbEfficiency.Results'+type+'.HV',null,'%7.2f %%');
    tab.velo    = StyledItem('lbWeb.LHCbEfficiency.Results'+type+'.VELO',null,'%7.2f %%');
    tab.daq     = StyledItem('lbWeb.LHCbEfficiency.Results'+type+'.DAQ',null,'%7.2f %%');
    tab.dead    = StyledItem('lbWeb.LHCbEfficiency.Results'+type+'.DAQLiveTime',null,'%7.2f %%');
    tab.total   = StyledItem('lbWeb.LHCbEfficiency.'+type+'Total',null,'%7.2f %%');
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(tab.hv);
    tr.appendChild(tab.velo);
    tr.appendChild(tab.daq);
    tr.appendChild(tab.dead);
    tr.appendChild(tab.total);
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

  /** Online widgets. L0DU monitor
   *
   *  @author M.Frank
   */
  lhcb.widgets.L0DURateMonitor = function(options) {
    var tb, tr, cell, tab = document.createElement('table');
    
    tab.className  = 'MonitorPage';
    tb = document.createElement('tbody');
    tooltips.set(tb,'Trigegr counters from the L0 Decision Unit (L0DU)');
    tb.className  = 'MonitorPage';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('L0 Decision Unit trigger counters',4,options.style));
      tb.appendChild(tr);
    }
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Trigger channel',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Number of Triggers',1,'MonitorDataHeader'));
      //tr.appendChild(Cell('Avg.rate [Hz]',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Inst.Rate [Hz]',1,'MonitorDataHeader'));
      tb.appendChild(tr);
    }
    tab.nTrigChan = StyledItem('lbWeb.Monitor_L0DU_1.Rates.nTrigChannels',null,null);
    tab.nTrigTot  = StyledItem('lbWeb.Monitor_L0DU_1.Rates.nTrigTot',null,null);
    tab.nBC       = StyledItem('lbWeb.Monitor_L0DU_1.nBC',null,null);
    
    tab.addRow = function(name) {
      var name_col;
      var tr = document.createElement('tr');
      tr.appendChild(name_col=Cell(name,1,'MonitorDataHeader'));
      name_col.style.width = '40%';

      tr.nTriggers = document.createElement('td');
      tr.instRate  = document.createElement('td');
      tr.nTriggers.style.textAlign = 'right';
      tr.instRate.style.textAlign = 'right';
      tr.instRate.innerHTML = '--';

      tr.nTriggers.style.width = '30%';
      tr.instRate.style.width = '30%';

      tr.appendChild(tr.nTriggers);
      tr.appendChild(tr.instRate);
      return tr;
    };
    tab.trg_rows = new Array();
    for(var i=0; i<32; ++i) {
      tab.trg_rows.push(tab.addRow('Channel '+(i+1)));
      tb.appendChild(tab.trg_rows[i]);
    }
    tab.appendChild(tb);

    tb = document.createElement('tbody');
    tb.className  = 'MonitorPage';
    tb.appendChild(tab.trg_total=tab.addRow('Total'));
    tab.trg_total.nTriggers.className = 'MonitorDataHeader';
    tab.trg_total.instRate.className = 'MonitorDataHeader';
    //tab.trg_total.avgRate.className = 'MonitorDataHeader';
    tab.appendChild(tb);

    tab.nTrigChan.tab = tab;
    tab.last_readings = null;
    tab.curr_readings = null;
    tab.last_time = 0;
    tab.curr_time = null;

    tab.nTrigChan.display = function(data) {
      var v = data[1].split(' | ',20);
      var val, diff, freq;
      var tot = 0, last_tot;
      var tab = this.tab;
      var len = v.length>tab.trg_rows.length ? tab.trg_rows.length : v.length;
      if ( tab.curr_readings ) {
	tab.last_readings = tab.curr_readings;
	tab.last_time = tab.curr_time;
      }
      tab.curr_time = (new Date()).getTime();
      tab.curr_readings = new Array();
      diff = (tab.curr_time-tab.last_time)/1000;
      for(var i=0; i<len;++i) {
	val = parseInt(v[i]);
	tab.trg_rows[i].nTriggers.innerHTML = v[i];
	tab.curr_readings[i] = val;
	if ( tab.last_readings ) {
	  freq = (val-tab.last_readings[i])/diff;
	  tab.trg_rows[i].instRate.innerHTML = sprintf('%10.2f Hz',freq);
	}
	tot = tot + val;
      }
      if ( tab.last_readings ) {
	last_tot = parseInt(tab.trg_total.nTriggers.innerHTML);
	freq = (tot-last_tot)/diff;
	tab.trg_total.instRate.innerHTML = sprintf('%10.2f Hz',freq);
      }
      tab.trg_total.nTriggers.innerHTML = tot;
    };

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.nTrigChan);
      provider.subscribeItem(this.nTrigTot);
      provider.subscribeItem(this.nBC);
    };

    return tab;
  };

  /** Online widgets. ODIN Rate monitor
   *
   *  @author M.Frank
   */
  lhcb.widgets.ODINRateMonitor = function(options) {
    var tb, tr, cell, tab = document.createElement('table');
    
    tab.className  = 'MonitorPage';
    tb = document.createElement('tbody');
    tooltips.set(tb,'Trigger counters from ODIN');
    tb.className  = 'MonitorPage';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('ODIN trigger counters',4,options.style));
      tb.appendChild(tr);
    }
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Event type',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Triggers',1,'MonitorDataHeader'));
      //tr.appendChild(Cell('Triggers',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Inst. rate [Hz]',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Run rate [Hz]',1,'MonitorDataHeader'));

      tb.appendChild(tr);
    }
    tab.nTriggersL = StyledItem('lbWeb.LHCb_RunInfo.TFC.triggerRates.nTriggersL',null,'%10.0f');
    //tab.nTriggersU = StyledItem('lbWeb.LHCb_RunInfo.TFC.triggerRates.nTriggersU',null,'%10.0f');
    tab.names = StyledItem('lbWeb.LHCb_RunInfo.TFC.triggerRates.names',null,null);
    tab.rates = StyledItem('lbWeb.LHCb_RunInfo.TFC.triggerRates.rates',null,'%10.2f');
    tab.runRates = StyledItem('lbWeb.LHCb_RunInfo.TFC.triggerRates.runRates',null,'%10.2f');

    tab.appendChild(tb);

    tab.trg_row = new Object();

    tab.names.tab = tab;
    tab.names.display = function(data) {
      var td, tr, v = data[1].split(' | ',20);
      var upda = false;
      var tab = this.tab;
      tab.trg_rows = v;
      for(var i=0; i<v.length; ++i) {
	if ( !tab.trg_row[v[i]] )  {
	  var st = '';
	  if ( v[i].indexOf('AllCalib')>=0 ) st = 'MonitorDataHeader';
	  else if ( v[i].indexOf('Physics_Total')>=0 ) st = 'MonitorDataHeader';
	  else if ( v[i].indexOf('Physics_Accepted')>=0 ) st = 'MonitorDataHeader';
	  else if ( v[i].indexOf('AllHLT_Accepted')>=0 ) st = 'MonitorDataHeader';

	  tab.trg_row[v[i]] = document.createElement('tr');
	  tr = tab.trg_row[v[i]];
	  tr.col_name    = Cell(v[i],1,'MonitorDataHeader');
	  tr.col_trL     = document.createElement('td');
	  //tr.col_trU     = document.createElement('td');
	  tr.col_runRate = document.createElement('td');
	  tr.col_rate    = document.createElement('td');
	  tr.col_trL.className = st;
	  tr.col_rate.className = st;
	  tr.col_runRate.className = st;
	  tr.col_trL.style.textAlign = 'right';
	  tr.col_rate.style.textAlign = 'right';
	  tr.col_runRate.style.textAlign = 'right';
	  
	  tr.appendChild(tr.col_name);
	  tr.appendChild(tr.col_trL);
	  //tr.appendChild(tr.col_trU);
	  tr.appendChild(tr.col_rate);
	  tr.appendChild(tr.col_runRate);
	  upda = true;
	}
      }
      if ( upda ) {
	var n;
	var tb_tot = document.createElement('tbody');
	var tb_acc = document.createElement('tbody');
	var tb_cal = document.createElement('tbody');
	if ( tab.odin_body_total  ) tab.deleteElementById('odin_body_total');
	if ( tab.odin_body_accept ) tab.deleteElementById('odin_body_accept');
	if ( tab.odin_body_calib  ) tab.deleteElementById('odin_body_calib');
	tab.odin_body_total = tb_tot;
	tab.odin_body_accept = tb_acc;
	tab.odin_body_total = tb_cal;
	for(var i=0; i<tab.trg_rows.length; ++i) {
	  n = tab.trg_rows[i];
	  if ( n.indexOf('_Total')>0 ) {
	    tr = tab.trg_row[n];
	    tb_tot.appendChild(tr);
	  }
	}
	tb_acc.appendChild(Cell('ODIN accepted triggers',4,'Arial12pt'));
	tb_cal.appendChild(Cell('Calibration triggers',4,'Arial12pt'));
	for(var i=0; i<tab.trg_rows.length; ++i) {
	  n = tab.trg_rows[i];
	  var id = n.indexOf('Calib');
	  if ( n.indexOf('_Accepted')>0 && id<0 ) {
	    tr = tab.trg_row[n];
	    tb_acc.appendChild(tr);
	  }
	  else if ( id>= 0 ) {
	    tr = tab.trg_row[n];
	    tb_cal.appendChild(tr);
	  }
	}
	tab.appendChild(tb_tot);
	tab.appendChild(tb_acc);
	tab.appendChild(tb_cal);
      }
      return this;
    };

    tab.nTriggersL.tab = tab;
    tab.nTriggersL.display = function(data) {
      var v = data[1].split(' | ',20);
      var t = this.tab;
      if ( t.trg_rows.length == v.length )  {
	var r = t.trg_row;
	var k = t.trg_rows;
	for(var i=0; i<v.length; ++i) {
	  r[k[i]]['col_trL'].innerHTML = v[i];
	}
      }
    };
    /*
    tab.nTriggersU.tab = tab;
    tab.nTriggersU.display = function(data) {
      var v = data[1].split(' | ',20);
      var t = this.tab;
      if ( t.trg_rows.length == v.length )  {
	var r = t.trg_row;
	var k = t.trg_rows;
	for(var i=0; i<v.length; ++i) {
	  r[k[i]]['col_trU'].innerHTML = v[i];
	}
      }
    };
    */
    tab.rates.tab = tab;
    tab.rates.display = function(data) {
      var v = data[1].split(' | ',20);
      var t = this.tab;
      if ( t.trg_rows.length == v.length )  {
	var r = t.trg_row;
	var k = t.trg_rows;
	var fmt = t.rates.format;
	for(var i=0; i<v.length; ++i) {
	  var f = parseFloat(v[i]);
	  r[k[i]].col_rate.innerHTML = sprintf(fmt,f);
	}
      }
    };

    tab.runRates.tab = tab;
    tab.runRates.display = function(data) {
      var v = data[1].split(' | ',20);
      var t = this.tab;
      var fmt = t.runRates.format;
      if ( t.trg_rows.length == v.length )  {
	var r = t.trg_row;
	var k = t.trg_rows;
	for(var i=0; i<v.length; ++i) {
	  var f = parseFloat(v[i]);
	  r[k[i]].col_runRate.innerHTML = sprintf(fmt,f);
	}
      }
    };

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.names);
      provider.subscribeItem(this.nTriggersL);
      //provider.subscribeItem(this.nTriggersU);
      provider.subscribeItem(this.rates);
      provider.subscribeItem(this.runRates);
    };

    return tab;
  };

  /** Online widgets. HLT Rate monitor
   *
   *  @author M.Frank
   */
  lhcb.widgets.HLTRateMonitor = function(options) {
    var tb, tr, cell, tab = document.createElement('table');
    
    tab.className  = 'MonitorPage';
    tb = document.createElement('tbody');
    tooltips.set(tb,'HLT trigger counters');
    tb.className  = 'MonitorPage';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('HLT Trigger counters',3,options.style));
      tb.appendChild(tr);
    }
    if ( options.legend ) {
      tr = document.createElement('tr');
      tr.appendChild(cell=Cell('Event Type',    1,'MonitorDataHeader'));
      cell.style.width = '40%';
      tr.appendChild(cell=Cell('Triggers Incl.',1,'MonitorDataHeader'));
      cell.style.width = '30%';
      tr.appendChild(cell=Cell('Triggers Excl.',1,'MonitorDataHeader'));
      cell.style.width = '30%';
      tb.appendChild(tr);
    }
    tab.update_totals = function(data) {
      this.data = parseFloat(data);
      tab.totalHandler();
      return data;
    };
    tab.BeamGasEx = StyledItem('lbWeb.LHCB_STATS.BeamGasEx',null,'%10.2f Hz');
    tab.BeamGasIn = StyledItem('lbWeb.LHCB_STATS.BeamGasIn',null,'%10.2f Hz');
    tab.LumiEx    = StyledItem('lbWeb.LHCB_STATS.LumiEx',null,'%10.2f Hz');
    tab.LumiIn    = StyledItem('lbWeb.LHCB_STATS.LumiIn',null,'%10.2f Hz');
    tab.MBiasEx   = StyledItem('lbWeb.LHCB_STATS.MBiasEx',null,'%10.2f Hz');
    tab.MBiasIn   = StyledItem('lbWeb.LHCB_STATS.MBiasIn',null,'%10.2f Hz');
    tab.PhysEx    = StyledItem('lbWeb.LHCB_STATS.PhysEx',null,'%10.2f Hz');
    tab.PhysIn    = StyledItem('lbWeb.LHCB_STATS.PhysIn',null,'%10.2f Hz');
    tab.RandEx    = StyledItem('lbWeb.LHCB_STATS.RandEx',null,'%10.2f Hz');
    tab.RandIn    = StyledItem('lbWeb.LHCB_STATS.RandIn',null,'%10.2f Hz');

    tab.BeamGasEx.conversion = tab.update_totals;
    tab.BeamGasIn.conversion = tab.update_totals;
    tab.LumiEx.conversion    = tab.update_totals;
    tab.LumiIn.conversion    = tab.update_totals;
    tab.MBiasEx.conversion   = tab.update_totals;
    tab.MBiasIn.conversion   = tab.update_totals;
    tab.PhysEx.conversion    = tab.update_totals;
    tab.PhysIn.conversion    = tab.update_totals;
    tab.RandEx.conversion    = tab.update_totals;
    tab.RandIn.conversion    = tab.update_totals;

    tab.BeamGasEx.style.textAlign = 'right';
    tab.BeamGasIn.style.textAlign = 'right';
    tab.LumiEx.style.textAlign    = 'right';
    tab.LumiIn.style.textAlign    = 'right';
    tab.MBiasEx.style.textAlign   = 'right';
    tab.MBiasIn.style.textAlign   = 'right';
    tab.PhysEx.style.textAlign    = 'right';
    tab.PhysIn.style.textAlign    = 'right';
    tab.RandEx.style.textAlign    = 'right';
    tab.RandIn.style.textAlign    = 'right';

    tr = document.createElement('tr');
    tr.appendChild(Cell('Physics',1,'MonitorDataHeader'));
    tr.appendChild(tab.PhysIn);
    tr.appendChild(tab.PhysEx);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Min.Bias',1,'MonitorDataHeader'));
    tr.appendChild(tab.MBiasIn);
    tr.appendChild(tab.MBiasEx);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Lumi',1,'MonitorDataHeader'));
    tr.appendChild(tab.LumiIn);
    tr.appendChild(tab.LumiEx);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Beam Gas',1,'MonitorDataHeader'));
    tr.appendChild(tab.BeamGasIn);
    tr.appendChild(tab.BeamGasEx);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Random',1,'MonitorDataHeader'));
    tr.appendChild(tab.RandIn);
    tr.appendChild(tab.RandEx);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Total',1,'MonitorDataHeader'));
    tr.appendChild(tab.totalIn=Cell('0',1,'MonitorDataHeader'));
    tr.appendChild(tab.totalEx=Cell('0',1,'MonitorDataHeader'));
    tab.totalIn.style.textAlign = 'right';
    tab.totalEx.style.textAlign = 'right';
    tb.appendChild(tr);

    tab.totalHandler = function() {
      var fEx = 0.0, fIn = 0.0;
      if ( tab.BeamGasEx.data ) fEx = fEx + tab.BeamGasEx.data;
      if ( tab.LumiEx.data    ) fEx = fEx + tab.LumiEx.data;
      if ( tab.MBiasEx.data   ) fEx = fEx + tab.MBiasEx.data;
      if ( tab.PhysEx.data    ) fEx = fEx + tab.PhysEx.data;
      if ( tab.RandEx.data    ) fEx = fEx + tab.RandEx.data;
      if ( tab.BeamGasIn.data ) fIn = fIn + tab.BeamGasIn.data;
      if ( tab.LumiIn.data    ) fIn = fIn + tab.LumiIn.data;
      if ( tab.MBiasIn.data   ) fIn = fIn + tab.MBiasIn.data;
      if ( tab.PhysIn.data    ) fIn = fIn + tab.PhysIn.data;
      if ( tab.RandIn.data    ) fIn = fIn + tab.RandIn.data;
      tab.totalIn.innerHTML = sprintf(tab.RandIn.format,fIn);
      tab.totalEx.innerHTML = sprintf(tab.RandEx.format,fEx);
    };

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.BeamGasEx);
      provider.subscribeItem(this.BeamGasIn);
      provider.subscribeItem(this.LumiEx);
      provider.subscribeItem(this.LumiIn);
      provider.subscribeItem(this.MBiasEx);
      provider.subscribeItem(this.MBiasIn);
      provider.subscribeItem(this.PhysEx);
      provider.subscribeItem(this.PhysIn);
      provider.subscribeItem(this.RandEx);
      provider.subscribeItem(this.RandIn);
    };

    return tab;
  };
    
  /** Online widgets. Trigger setup and TCKs
   *
   *  @author M.Frank
   */
  lhcb.widgets.TriggerSetup = function(options) {
    var td, tr, tab = document.createElement('table');
    var tb = document.createElement('tbody');
    tab.className  = tb.className = 'MonitorPage';
    tb.cellSpacing = tb.cellPadding = 0;

    tab.l0Type        = StyledItem('lbWeb.LHCb_RunInfo.Trigger.L0Type',null, null);
    tab.HLTType       = StyledItem('lbWeb.LHCb_RunInfo.Trigger.HLTType',null, null);
    tab.TCK           = StyledItem('lbWeb.LHCb_RunInfo.Trigger.TCKLabel',null, null);

    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('Trigger setup',options.legend ? 2 : 1,options.style));
      tb.appendChild(tr);
    }

    tr = document.createElement('tr');
    if ( options.legend ) tr.appendChild(Cell('L0 Configuration:',null,'MonitorDataHeader'));
    tr.appendChild(tab.l0Type);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    if ( options.legend ) tr.appendChild(Cell('HLT Configuration:',null,'MonitorDataHeader'));
    tr.appendChild(tab.HLTType);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    if ( options.legend ) tr.appendChild(Cell('TCK:',1,'MonitorDataHeader'));
    tr.appendChild(tab.TCK);
    tb.appendChild(tr);

    tab.appendChild(tb);
    tooltips.set(tab,'Trigger setup and status<br>Click to go to DAQ page');
    tab.onclick = lhcb.widgets.goto_lhcb_daq_page;

    tab.subscribe = function(provider) {
      provider.subscribeItem(tab.TCK);
      provider.subscribeItem(tab.l0Type);
      provider.subscribeItem(tab.HLTType);
    };
    return tab;
  };


  /** Online widgets. DAQ efficiency
   *
   *  @author M.Frank
   */
  lhcb.widgets.DeferredHLT = function(options) {
    var c, tr, tab = document.createElement('table');
    var tb = document.createElement('tbody');
    tab.className  = tb.className = 'MonitorPage';
    
    tooltips.set(tb,'HLT1 Deferred activity for this fill.');
    tb.height    = '120px';
    if ( 1||options.style ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('HLT1 deferred activity monitor:',3,options.style));
    }
    if ( options.legend ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Disk:',          1,'MonitorDataHeader'));
      tr.appendChild(Cell('Blocks total',   1,'MonitorDataHeaderRight'));
      tr.appendChild(Cell('Blocks free',    1,'MonitorDataHeaderRight'));
      tr.appendChild(Cell('Usage',          1,'MonitorDataHeaderRight'));
      tr.appendChild(Cell('Files',          1,'MonitorDataHeaderRight'));
    }
    tab.MEPRate      = StyledItem('lbWeb.LHCb_RunInfoSplitHLT.TotalHLTRate','Text-Right','%d Hz');
    tab.DiskSize     = StyledItem('lbWeb.LHCb_RunInfoSplitHLT.DiskSize','Text-Right',null);
    tab.DiskFree     = StyledItem('lbWeb.LHCb_RunInfoSplitHLT.DiskFree','Text-Right',null);
    tab.NFilesLeft   = StyledItem('lbWeb.LHCb_RunInfoSplitHLT.NFilesLeft','Text-Right',null);
    tab.DiskUsage    = StyledItem('None','Text-Right',null);
    tab.DiskFreeData = 1.0;
    tab.DiskSizeData = 1.0;
    tab.DiskFree.parent = tab;
    tab.DiskFree.conversion = function(data) {
      this.parent.DiskFreeData = parseFloat(data);
      this.parent.DiskUsage.innerHTML = 
        sprintf('%7.4f %%',100.0*(1.0-this.parent.DiskFreeData/this.parent.DiskSizeData));
      return data;
    };
    tab.DiskSize.parent = tab;
    tab.DiskSize.conversion = function(data) { 
      this.parent.DiskSizeData = parseFloat(data); 
      return data;
    };
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('',1,'Text-Right'));
    tr.appendChild(tab.DiskSize);
    tr.appendChild(tab.DiskFree);
    tr.appendChild(tab.DiskUsage);
    tr.appendChild(tab.NFilesLeft);
    if ( options.legend ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(c=Cell('HLT1 Rates:',1,   'MonitorDataHeader'));
      tr.appendChild(c=Cell('MEPRx', 1,   'MonitorDataHeaderRight'));
      c.style.width = '30%';
    }
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('',1,'Text-Right'));
    tr.appendChild(tab.MEPRate);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.MEPRate);
      provider.subscribeItem(this.DiskSize);
      provider.subscribeItem(this.DiskFree);
      provider.subscribeItem(this.NFilesLeft);
    };
    return tab;
  };

  /** Online widgets. Base class to show subdetector FSM groups
   *
   *  @author M.Frank
   */
  lhcb.widgets.SubdetectorSystem = function(options) {
    var tab = document.createElement('table');
    tab.subscriptions=[];
    tab.logger = options.logger;
    tab.className = 'MonitorPage';

    tab.addFSMRow = function(names) {
      var tr = document.createElement('tr');
      for(var i=0; i<names.length; ++i) {
	var name = names[i];
	if ( name ) {
	  var cell = Cell(name,1,'MonitorDataHeader');
	  var item = FSMItem('lbWeb.'+name,this.logger,true);
	  this[name] = item;
	  tr.appendChild(cell);
	  tr.appendChild(item);
	  tr.appendChild(item.lock);
	  this.subscriptions.push(item);
	}
	else {
	  tr.appendChild(Cell('',3,null));
	}
      }
      return tr;
    };

    tab.body = function(title) {
      var tb = document.createElement('tbody');
      tb.className   = 'MonitorPage';
      if ( options.style ) {
	var tr = document.createElement('tr');
	tr.appendChild(Cell(title,6,options.style));
	tb.appendChild(tr);
      }
      this.appendChild(tb);
      return tb;
    };

    tab.subscribe = function(provider) {
      for(var i=0; i<this.subscriptions.length;++i)
	provider.subscribeItem(this.subscriptions[i]);
    };
    
    return tab;
  };


  lhcb.widgets.SubdetectorPage = function(name, msg) {
    var table       = document.createElement('table');
    table.body      = document.createElement('tbody');
    table.system    = name;
    table.title     = name;
    table.messages  = msg;
    table.provider  = null;
    table.logger    = null;
    if ( table.system.indexOf(' ')>0 ) table.system = name.substr(0,name.indexOf(' '));
    table.add = function() {
      var td = document.createElement('td'), tr = document.createElement('tr');
      td.setAttribute('colSpan',2);
      tr.appendChild(td);
      this.body.appendChild(tr);
      return td;
    };
    table.className = table.body.className = 'MonitorOuterFrame';
    table.display = table.add();
    table.logDisplay = table.add();
    table.appendChild(table.body);
    table.subscriptions = new Array();

    table.hvSummary = function(options) {
      var tab = lhcb.widgets.HVSummary(this.logger);
      tab.split = true;
      if ( options.nosplit ) tab.split = false;
      tab.system = this.system;
      if ( options.system ) tab.system = options.system;
      tab.addHV = function(tb) {};
      tab.addLV = function(tb) {};
      if ( options.hv )  {
	tab.addHV = function(tb) { 
	  if ( this.system instanceof Array ) {
	    for(var i=0; i<this.system.length; ++i)
	      tb.appendChild(this.addState(this.system[i], this.split, 'HV', 'HV'));
	  }
	  else {
	    tb.appendChild(this.addState(this.system, this.split, 'HV', 'HV'));
	  }
	};
      }
      if ( options.lv )  {
	tab.addLV = function(tb) { 
	  if ( this.system instanceof Array ) {
	    for(var i=0; i<this.system.length; ++i)
	      tb.appendChild(this.addState(this.system[i], this.split, 'LV', 'DCS_Local'));
	  }
	  else {
	    tb.appendChild(this.addState(this.system, this.split, 'LV', 'DCS_Local'));
	  }
	};
      }
      return tab.buildTable({style:'Arial12pt',top:false,hv:options.hv,hv_legend:options.hv,lv:options.lv});
    };

    table.addRow = function(items) {
      var mx=1, tb = this.tbody, tr, td, item;
      tb.appendChild(tr=document.createElement('tr'));
      for(var j=0; j<items.length; ++j) {
	if ( items[j] instanceof Array ) {
	  if ( items[j].length>mx ) mx = items[j].length;
	} 
      }
      for(var i=0; i<items.length; ++i) {
	tr.appendChild(td=document.createElement('td'));
	td.style.verticalAlign = 'top';
	if ( items[i] instanceof Array ) {
	  for(var k=0, len=items[i].length; k<len; ++k) {
	    td.appendChild(items[i][k]);
	    this.subscriptions.push(items[i][k]);
	    if ( k == len-1 ) {
	      td.style.verticalAlign = 'bottom';
	      continue;
	    }
	    tb.appendChild(tr=document.createElement('tr'));
	    tr.appendChild(td=document.createElement('td'));
	    td.style.verticalAlign = 'top';
	  }
	} 
	else {
	  td.rowSpan = mx;
	  td.appendChild(items[i]);
	  this.subscriptions.push(items[i]);
	}
      }
    };

    /// Empty placeholder
    table.attachWidgets = function() {  };
    table.beforeComment = function() {  };

    table.build = function(options) {
      var tab = document.createElement('table');
      var tb = document.createElement('tbody');
      var t1, tb1, tr1, td1, cell, d = new Date();

      tab.className = tb.className = 'MonitorInnerFrame';

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
      cell = Cell(this.title+'&nbsp;'+lhcb_online_picture(),1,'MonitorBigHeader');
      cell.style.textAlign = 'right';
      cell.onclick = function() { document.location = "http://lhcb.cern.ch";};
      tr1.appendChild(cell);
      if ( options.tips ) {
	tooltips.set(this.heading,options.tips);
      }
      tb.appendChild(this.heading);

      this.timerHandler = function() {document.getElementById('current_time').innerHTML = (new Date()).toString(); };
      setInterval(this.timerHandler,2000);

      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(td=document.createElement('td'));
      td.style.width = '55%';
      td.appendChild(this.lhc_header=lhcb.widgets.LHC_header());
      tr.appendChild(td=document.createElement('td'));
      td.appendChild(this.run_header=lhcb.widgets.Det_Run_header('LHCb',this.logger));
      td.width = '45%';

      this.lhc_header.style.height = '60px';
      this.run_header.style.height = '60px';
      this.subscriptions.push(this.lhc_header);
      this.subscriptions.push(this.run_header);


      tb.appendChild(tr=document.createElement('tr'));
      // Left half of the display
      tr.appendChild(td=document.createElement('td'));
      td.appendChild(t1=document.createElement('table'));
      t1.appendChild(this.left=document.createElement('tbody'));
      t1.className = this.left.className = 'MonitorPage';
      t1.style.border = this.left.style.border = 'none';
      this.left.style.verticalAlign = 'top';

      // Right hand of the display
      tr.appendChild(td=document.createElement('td'));
      td.appendChild(t1=document.createElement('table'));
      t1.appendChild(this.right=document.createElement('tbody'));
      t1.className = this.right.className = 'MonitorPage';
      t1.style.border = this.right.style.border = 'none';
      this.right.style.verticalAlign = 'top';

      this.left.addItem = function(item) {
	var tr1, td1;
	this.appendChild(tr1=document.createElement('tr'));
	tr1.appendChild(td1=document.createElement('td'));
	td1.style.verticalAlign = 'top';
	td1.appendChild(item);
	this.parent.subscriptions.push(item);
	return td1;
      };
      this.left.addSpacer = function(height) {
	var tr1, td1;
	this.appendChild(tr1=document.createElement('tr'));
	tr1.appendChild(td1=document.createElement('td'));
	if ( height ) td1.style.height=height;
	return td1;
      };
      this.right.addItem = this.left.addItem;
      this.right.addSpacer = this.left.addSpacer;
      this.left.parent = this.right.parent = this;

      this.tbody = tb;
      this.attachWidgets();
      this.beforeComment();

      // Finally add suggestions text
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('',null,null));
      tr.appendChild(Cell('Comments and suggestions to M.Frank CERN/LHCb',2,'MonitorTinyHeader'));

      tb.appendChild(tr);
      tab.appendChild(tb);
      this.display.appendChild(tab);
      return this;
    };

    table.subscribe = function() {
      for(var i=0; i<this.subscriptions.length; ++i)
	this.subscriptions[i].subscribe(this.provider);
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
    if ( msg > 0 ) display.logger = new OutputLogger(display.logDisplay, 200, LOG_INFO, 'StatusLogger');
    else           display.logger = new OutputLogger(display.logDisplay,  -1, LOG_INFO, 'StatusLogger');
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

  ///=====================================================================================
  ///
  ///  VELO Widgets
  ///
  ///=====================================================================================
  lhcb.widgets.velo = new Object();

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
      tr.appendChild(Cell('XA',1,'MonitorDataHeader'));
      tr.appendChild(Cell('XC',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Y',1,'MonitorDataHeader'));
      tb.appendChild(tr);
    }
    if ( options.tips ) {
      tooltips.set(tb,options.tips);
    }
    if ( options.onclick ) {
      tb.onclick = options.onclick; 
    }
    tab.veloState   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Specific.VELO.Position', null, null);
    //tab.veloOpening = StyledItem('lbWeb.veloMove_Xle.Comm.ResolPos',null,'%7.2f mm');
    tab.veloOpening = StyledItem('lbWeb.veloMove_Xle.Potm.Av',null,'%7.2f mm');
    //tab.veloCenter  = StyledItem('lbWeb.veloMove_Xri.Comm.ResolPos',null,'%7.2f mm');
    tab.veloCenter  = StyledItem('lbWeb.veloMove_Xri.Potm.Av',null,'%7.2f mm');
    //tab.veloY       = StyledItem('lbWeb.veloMove_Yy.Comm.ResolPos',null,'%7.2f mm');
    tab.veloY       = StyledItem('lbWeb.veloMove_Yy.Potm.Av',null,'%7.2f mm');
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
    if ( options.tips ) {
      tooltips.set(tb,options.tips);
    }
    if ( options.onclick ) {
      tb.onclick = options.onclick; 
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
    tab.ap411 = StyledItem('lbWeb.veloVac_Barotrons.AP411.Pressure', null, "%7.1f");
    tab.dp411 = StyledItem('lbWeb.veloVac_Barotrons.DP411.Pressure', null, "%7.1e");
    tab.pe411 = StyledItem('lbWeb.veloVac_Meters.PE411.Vacuum', null, "%7.1e");
    tab.pe412 = StyledItem('lbWeb.veloVac_Meters.PE412.Vacuum', null, "%7.1e");
    tab.pe421 = StyledItem('lbWeb.veloVac_Meters.PE421.Vacuum', null, "%7.1e");
    tab.pe422 = StyledItem('lbWeb.veloVac_Meters.PE422.Vacuum', null, "%7.1e");
    if ( options.style ) {
      tr.appendChild(Cell('Velo Vacuum Status: Pressures in [mbar]',4,options.style));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    if ( options.legend ) {
      tr.appendChild(Cell('AP411',1,'MonitorDataHeader'));
      tr.appendChild(Cell('DP411',1,'MonitorDataHeader'));
      tr.appendChild(Cell('PE411',1,'MonitorDataHeader'));
      tr.appendChild(Cell('PE412',1,'MonitorDataHeader'));
      tr.appendChild(Cell('PE421',1,'MonitorDataHeader'));
      tr.appendChild(Cell('PE422',1,'MonitorDataHeader'));
      tb.appendChild(tr);
    }
    if ( options.tips ) {
      tooltips.set(tb,options.tips);
    }
    if ( options.onclick ) {
      tb.onclick = options.onclick; 
    }
    tr = document.createElement('tr');
    tr.appendChild(tab.ap411);
    tab.ap411.style.width = '17%';
    tr.appendChild(tab.dp411);
    tab.dp411.style.width = '17%';
    tr.appendChild(tab.pe411);
    tab.pe411.style.width = '17%';
    tr.appendChild(tab.pe412);
    tab.pe412.style.width = '17%';
    tr.appendChild(tab.pe421);
    tab.pe421.style.width = '17%';
    tr.appendChild(tab.pe422);
    tab.pe422.style.width = '17%';
    tb.appendChild(tr);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.ap411);
      provider.subscribeItem(this.dp411);
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
    //tab.cool   = FSMItem('lbWeb.VELO_COOLING',options.logger,true);
    tab.coolA  = FSMItem('lbWeb.VEA_COOL',options.logger,true);
    tab.coolC  = FSMItem('lbWeb.VEC_COOL',options.logger,true);
    tab.setPA  = StyledItem('lbWeb.VTCS_TL_AC101.SETPOINT',null,'%7.1f <sup>o</sup>C');
    tab.setPC  = StyledItem('lbWeb.VTCS_TR_AC101.SETPOINT',null,'%7.1f <sup>o</sup>C');
    tab.T0A    = StyledItem('lbWeb.VeloDpFct_Aside_P_NTC2.Average',null,'%7.1f <sup>o</sup>C');
    tab.T1A    = StyledItem('lbWeb.VeloDpFct_Aside_P_NTC3.Average',null,'%7.1f <sup>o</sup>C');
    tab.T0C    = StyledItem('lbWeb.VeloDpFct_Cside_P_NTC2.Average',null,'%7.1f <sup>o</sup>C');
    tab.T1C    = StyledItem('lbWeb.VeloDpFct_Cside_P_NTC3.Average',null,'%7.1f <sup>o</sup>C');
    if ( options.style ) {
      tr.appendChild(Cell('Velo Infrastructure State',8,options.style));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    if ( options.legend ) {
      tr.appendChild(cell=Cell('Vacuum',2,'MonitorDataHeader'));
      cell.style.width='15%';
      tr.appendChild(cell=Cell('A<BR>Side',1,'MonitorDataHeader'));
      cell.style.width='10%';
      cell.rowSpan = 2;
      tr.appendChild(cell=Cell('Cooling',2,'MonitorDataHeader'));
      cell.style.width='15%';
      tr.appendChild(Cell('SetPoint',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Avg.NTC0',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Avg.NTC1',1,'MonitorDataHeader'));
      //tr.appendChild(Cell('Cooling',2,'MonitorDataHeader'));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    if ( options.tips ) {
      tooltips.set(tb,options.tips);
    }
    if ( options.onclick ) {
      tb.onclick = options.onclick; 
    }
    //tr.appendChild(tab.cool);
    //tr.appendChild(tab.cool.lock);

    tr.appendChild(tab.vacuum);
    tr.appendChild(tab.vacuum.lock);
    tab.vacuum.lock.style.width = '2%';
    tr.appendChild(tab.coolA);
    tr.appendChild(tab.coolA.lock);
    tab.coolA.lock.style.width = '2%';
    tr.appendChild(tab.setPA);
    tr.appendChild(tab.T0A);
    tr.appendChild(tab.T1A);

    tb.appendChild(tr);

    tr = document.createElement('tr');
    if ( options.legend ) {
      tr.appendChild(Cell('Motion',2,'MonitorDataHeader'));
      tr.appendChild(cell=Cell('C<BR>Side',1,'MonitorDataHeader'));
      cell.rowSpan = 2;
      tr.appendChild(Cell('Cooling',2,'MonitorDataHeader'));
      tr.appendChild(Cell('SetPoint',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Avg.NTC0',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Avg.NTC1',1,'MonitorDataHeader'));
      tb.appendChild(tr);
      tr = document.createElement('tr');
    }
    tr.appendChild(tab.motion);
    tr.appendChild(tab.motion.lock);
    tr.appendChild(tab.coolC);
    tr.appendChild(tab.coolC.lock);
    tr.appendChild(tab.setPC);
    tr.appendChild(tab.T0C);
    tr.appendChild(tab.T1C);
    tr.appendChild(cell=Cell('&nbsp;',2,null));
    tb.appendChild(tr);

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.motion);
      provider.subscribeItem(this.vacuum);
      //provider.subscribeItem(this.cool);
      provider.subscribeItem(this.coolA);
      provider.subscribeItem(this.setPA);
      provider.subscribeItem(this.T0A);
      provider.subscribeItem(this.T1A);
      provider.subscribeItem(this.coolC);
      provider.subscribeItem(this.setPC);
      provider.subscribeItem(this.T0C);
      provider.subscribeItem(this.T1C);
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

  ///=====================================================================================
  ///
  ///  DSS Widgets
  ///
  ///=====================================================================================
  lhcb.widgets.DSS = new Object();

  /** DSS web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.DSS.MixedWaterStatus = function(options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr, cell;

    tab.logger = options.logger;
    tab.subscriptions=[];
    tab.className = 'MonitorPage';
    tb.className   = 'MonitorPage';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell('DSS Mixed water temperature',6,options.style));
      tb.appendChild(tr);
    }
    tab.d1d2 = StyledItem('lbWeb.dip/LHCb/DSS/PT_TE_BARRACK_Mixed_Water_D1D2_In.__DIP_DEFAULT__',null,'%7.2f <sup>o</sup>C');
    tab.d3A = StyledItem('lbWeb.dip/LHCb/DSS/PT_TE_BARRACK_Mixed_Water_D3A_In.__DIP_DEFAULT__',null,'%7.2f <sup>o</sup>C');
    tab.d3B = StyledItem('lbWeb.dip/LHCb/DSS/PT_TE_BARRACK_Mixed_Water_D3B_In.__DIP_DEFAULT__',null,'%7.2f <sup>o</sup>C');

    tab.bunker = StyledItem('lbWeb.dip/LHCb/DSS/PT_TE_BUNKER_Mixed_Water_Cooling.__DIP_DEFAULT__',null,'%7.2f <sup>o</sup>C');
    tab.caloA = StyledItem('lbWeb.dip/LHCb/DSS/PT_TE_CALO_Mixed_Water_Aside.__DIP_DEFAULT__',null,'%7.2f <sup>o</sup>C');
    tab.caloC = StyledItem('lbWeb.dip/LHCb/DSS/PT_TE_CALO_Mixed_Water_Cside.__DIP_DEFAULT__',null,'%7.2f <sup>o</sup>C');

    tab.uxaA = StyledItem('lbWeb.dip/LHCb/DSS/PT_TE_UXA~A_Mixed_Water_Cooling.__DIP_DEFAULT__',null,'%7.2f <sup>o</sup>C');
    tab.uxaB1 = StyledItem('lbWeb.dip/LHCb/DSS/PT_TE_UXA~B1_Mixed_Water_Cooling.__DIP_DEFAULT__',null,'%7.2f <sup>o</sup>C');
    tab.uxaC = StyledItem('lbWeb.dip/LHCb/DSS/PT_TE_UXA~C_Mixed_Water_Cooling.__DIP_DEFAULT__',null,'%7.2f <sup>o</sup>C');

    tab.subscriptions.push(tab.d1d2);
    tab.subscriptions.push(tab.d3A);
    tab.subscriptions.push(tab.d3B);
    tab.subscriptions.push(tab.bunker);
    tab.subscriptions.push(tab.caloA);
    tab.subscriptions.push(tab.caloC);
    tab.subscriptions.push(tab.uxaA);
    tab.subscriptions.push(tab.uxaB1);
    tab.subscriptions.push(tab.uxaC);
    
    tr = document.createElement('tr');
    tr.appendChild(Cell('D1/D2:',1,null));
    tr.appendChild(tab.d1d2);
    tr.appendChild(Cell('D3-A:',1,null));
    tr.appendChild(tab.d3A);
    tr.appendChild(Cell('D3-B:',1,null));
    tr.appendChild(tab.d3B);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Bunker:',1,null));
    tr.appendChild(tab.bunker);
    tr.appendChild(Cell('Calo-A:',1,null));
    tr.appendChild(tab.caloA);
    tr.appendChild(Cell('Calo-C:',1,null));
    tr.appendChild(tab.caloC);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('UXA-A:',1,null));
    tr.appendChild(tab.uxaA);
    tr.appendChild(Cell('UXA-B1:',1,null));
    tr.appendChild(tab.uxaB1);
    tr.appendChild(Cell('UXA-C:',1,null));
    tr.appendChild(tab.uxaC);
    tb.appendChild(tr);

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      for(var i=0; i<this.subscriptions.length;++i) {
	provider.subscribeItem(this.subscriptions[i]);
      }
    };
    return tab;
  };

  /** DSS web widgets
   *
   *  @author  M.Frank
   */
  lhcb.widgets.DSS.CaVPlantStatus = function(sys, options) {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr, cell;

    tab.subscriptions=[];
    tab.logger = options.logger;
    tab.className = tb.className = 'MonitorPage';
    if ( options.style ) {
      tr = document.createElement('tr');
      tr.appendChild(Cell(sys+' Cooling plant status',4,options.style));
      tb.appendChild(tr);
    }

    tab.OTspecific = function(tb) {
      this.outletPressure = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.Parameters.param03',null,'%7.2f bar');
      this.outletTemperature = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.Parameters.param04',null,'%7.2f <sup>o</sup>C');
      this.mixedWaterIn  = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.Measurements.param13',null,'%7.2f <sup>o</sup>C');
      this.mixedWaterOut = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.Measurements.param14',null,'%7.2f <sup>o</sup>C');
      this.subscriptions.push(this.outletPressure);
      this.subscriptions.push(this.outletTemperature);
      this.subscriptions.push(this.mixedWaterIn);
      this.subscriptions.push(this.mixedWaterOut);
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(cell=Cell('Outlet',2,'MonitorDataHeader'));
      tr.appendChild(Cell('Press:',1,null));
      tr.appendChild(this.outletPressure);
      tr.appendChild(Cell('Temp:',1,null));
      tr.appendChild(this.outletTemperature);

      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Mixed water',2,'MonitorDataHeader'));
      tr.appendChild(Cell('In:',1,null));
      tr.appendChild(this.mixedWaterIn);
      tr.appendChild(Cell('Out:',1,null));
      tr.appendChild(this.mixedWaterOut);
    };

    tab.TTspecific = function(tb) {
      this.tt_manifold_temp01 = StyledItem('lbWeb.CaV/TtPlant.Actual.Measurements.param11',null,'%7.2f &#186;C');
      this.tt_manifold_temp02 = StyledItem('lbWeb.CaV/TtPlant.Actual.Measurements.param12',null,'%7.2f &#186;C');
      this.tt_manifold_temp03 = StyledItem('lbWeb.CaV/TtPlant.Actual.Measurements.param13',null,'%7.2f &#186;C');
      this.tt_manifold_temp04 = StyledItem('lbWeb.CaV/TtPlant.Actual.Measurements.param14',null,'%7.2f &#186;C');
      this.tt_c6f14flow       = StyledItem('lbWeb.CaV/TtPlant.Actual.Measurements.param18',null,'%7.2f l/h');
      this.mixedWaterIn  = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.Measurements.param16',null,'%7.2f <sup>o</sup>C');
      this.mixedWaterOut = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.Measurements.param17',null,'%7.2f <sup>o</sup>C');
      this.mixedWaterFlow = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.Measurements.param15',null,'%7.1f l/h');

      this.subscriptions.push(this.tt_manifold_temp01);
      this.subscriptions.push(this.tt_manifold_temp02);
      this.subscriptions.push(this.tt_manifold_temp03);
      this.subscriptions.push(this.tt_manifold_temp04);
      this.subscriptions.push(this.tt_c6f14flow);
      this.subscriptions.push(this.mixedWaterIn);
      this.subscriptions.push(this.mixedWaterOut);
      this.subscriptions.push(this.mixedWaterFlow);

      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(cell=Cell('Manifold<br>Temperatures',1,'MonitorDataHeader'));
      cell.rowSpan = "2";
      tr.appendChild(Cell('Temp.1:',1,null));
      tr.appendChild(this.tt_manifold_temp01);
      tr.appendChild(Cell('Temp.2:',1,null));
      tr.appendChild(this.tt_manifold_temp02);

      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Temp.3:',1,null));
      tr.appendChild(this.tt_manifold_temp03);
      tr.appendChild(Cell('Temp.4:',1,null));
      tr.appendChild(this.tt_manifold_temp04);

      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Mixed water',1,'MonitorDataHeader'));
      tr.appendChild(Cell('In:',1,null));
      tr.appendChild(this.mixedWaterIn);
      tr.appendChild(Cell('Out:',1,null));
      tr.appendChild(this.mixedWaterOut);

      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Flow',1,'MonitorDataHeader'));
      tr.appendChild(Cell('C6F14:',1,null));
      tr.appendChild(this.tt_c6f14flow);
      tr.appendChild(Cell('Mixed:',1,null));
      tr.appendChild(this.mixedWaterFlow);
    };

    tab.ITspecific = function(tb) {
      this.it_manifold_temp01 = StyledItem('lbWeb.CaV/ItPlant.Actual.Measurements.param01',null,'%7.2f &#186;C');
      this.it_manifold_temp02 = StyledItem('lbWeb.CaV/ItPlant.Actual.Measurements.param02',null,'%7.2f &#186;C');
      this.it_c6f14flow = StyledItem('lbWeb.CaV/ItPlant.Actual.Measurements.param06',null,'%7.2f l/h');
      this.mixedWaterIn  = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.Measurements.param04',null,'%7.2f <sup>o</sup>C');
      this.mixedWaterOut = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.Measurements.param05',null,'%7.2f <sup>o</sup>C');
      this.mixedWaterFlow = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.Measurements.param03',null,'%7.1f l/h');
      this.subscriptions.push(this.it_manifold_temp01);
      this.subscriptions.push(this.it_manifold_temp02);
      this.subscriptions.push(this.it_c6f14flow);
      this.subscriptions.push(this.mixedWaterIn);
      this.subscriptions.push(this.mixedWaterOut);
      this.subscriptions.push(this.mixedWaterFlow);

      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(cell=Cell('Manifold',1,'MonitorDataHeader'));
      tr.appendChild(Cell('Temp.1:',1,null));
      tr.appendChild(this.it_manifold_temp01);
      tr.appendChild(Cell('Temp.2:',1,null));
      tr.appendChild(this.it_manifold_temp02);

      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Mixed water',1,'MonitorDataHeader'));
      tr.appendChild(Cell('In:',1,null));
      tr.appendChild(this.mixedWaterIn);
      tr.appendChild(Cell('Out:',1,null));
      tr.appendChild(this.mixedWaterOut);

      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(Cell('Flow',1,'MonitorDataHeader'));
      tr.appendChild(Cell('C6F14:',1,null));
      tr.appendChild(this.it_c6f14flow);
      tr.appendChild(Cell('Mixed:',1,null));
      tr.appendChild(this.mixedWaterFlow);
    };

    if      ( sys=='Ot'   ) tab.specific = tab.OTspecific;
    else if ( sys == 'It' ) tab.specific = tab.ITspecific;
    else if ( sys == 'Tt' ) tab.specific = tab.TTspecific;
    if ( tab.specific ) {
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(cell=document.createElement('td'));
      cell.colSpan = 6;
      var t1, tb1;
      cell.appendChild(t1=document.createElement('table'));
      t1.appendChild(tb1=document.createElement('tbody'));
      t1.className = tb1.className = 'MonitorInnerFrame';
      tab.specific(tb1);
    }

    tab.fault    = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.fault',null,null);
    tab.alarm    = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.alarm',null,null);
    tab.fault.conversion = tab.alarm.conversion = function(data) 
    { return data=="FALSE" ? lhcb.widgets.green_smiley() : lhcb.widgets.red_smiley();  };
    tab.setLEDs = function(data) {
      if ( this.leds ) {
	for(var i=4; i<12; ++i) {
	  var m = data&(1<<i);
	  var l = this.leds[i-4];
	  l.style.color = data&(1<<i) ? 'black' : 'white';
	  l.bgColor     = data&(1<<i) ? '#00CC99' : '#333333';
	  l.innerHTML   = data&(1<<i) ? 'ON' : 'OFF';
	}
      }
    };
    tab.runState = StyledItem('lbWeb.CaV/'+sys+'Plant.Actual.status',null,null);
    tab.runState.parent = tab;
    tab.runState.conversion = function(data) {
      var t='';
      var v0 = parseInt(data);
      var v = v0&0xF;
      for(var i=0; i<4; ++i) {
	var m = v&(1<<i);
	if ( 0 != m ) {
	  if ( (v^m) != 0 ) {
	    this.className = 'FwDEAD';
	    this.parent.setLEDs(v0);
	    return 'Invalid ('+data+')';
	  }
	  else if ( i == 2 )  {
	    this.className = 'FwStateOKPhysics';
	    this.parent.setLEDs(v0);
	    return 'Run mode';
	  }
	  else if ( i == 0 )  {
	    this.className = 'FwStateAttention3';
	    this.parent.setLEDs(v0);
	    return 'System OFF';
	  }
	  else {
	    this.className = 'FwDEAD';
	    this.parent.setLEDs(v0);
	    if ( i == 0      ) return 'System OFF';
	    else if ( i == 1 ) return 'Stand-by mode';
	    else if ( i == 3 ) return 'Recovering';
	  }
	}
      }
      this.className = 'FwDEAD';
      return 'Invalid:'+data;
    };

    tab.subscriptions.push(tab.runState);

    tab.subscriptions.push(tab.fault);
    tab.subscriptions.push(tab.alarm);
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(Cell('State:',1,null));
    tr.appendChild(tab.runState);
    tr.appendChild(Cell('Alarms:',1,null));
    tr.appendChild(tab.alarm);
    tr.appendChild(Cell('Faults:',1,null));
    tr.appendChild(tab.fault);


    if ( options.showStatus ) {

      if ( options.style ) {
	tr = document.createElement('tr');
	tr.appendChild(Cell('Plant status bits:',4,options.style));
	tb.appendChild(tr);
      }
      tab.leds = [];
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(cell=Cell('Leak flow rate:',2,null));
      tr.appendChild(cell=Cell('',1,'Text-Center'));
      tab.leds.push(cell);
      tr.appendChild(cell=Cell('Leak search',2,null));
      tr.appendChild(cell=Cell('',1,'Text-Center'));
      tab.leds.push(cell);

      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(cell=Cell('Safety heater:',2,null));
      tr.appendChild(cell=Cell('',1,'Text-Center'));
      tab.leds.push(cell);
      tr.appendChild(cell=Cell('Reserved:',2,null));
      tr.appendChild(cell=Cell('',1,'Text-Center'));
      tab.leds.push(cell);

      var r1, r2;
      tb.appendChild(tr=document.createElement('tr'));
      tr.appendChild(r1=cell=Cell('Maintenance:',2,null));
      tr.appendChild(cell=Cell('',1,'Text-Center'));
      tab.leds.push(cell);
      tr.appendChild(r2=cell=Cell('TCR Control:',2,null));
      tr.appendChild(cell=Cell('',1,'Text-Center'));
      tab.leds.push(cell);

      // Reserved ones...
      tab.leds.push(cell=Cell('',1,'Text-Center'));
      tab.leds.push(cell=Cell('',1,'Text-Center'));
      tab.leds.push(cell=Cell('',1,'Text-Center'));
    }

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
