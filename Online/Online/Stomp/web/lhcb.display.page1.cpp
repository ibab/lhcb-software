_loadScript('lhcb.display.items.cpp');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');

/*
----------------------------------------------------------------  8-SEP 18:41:05
 LEP Fill 8986 is SHUTDOWN               | ALEPH run 58472 is INACTIVE
--------------------------------------------------------------------------------
 Energy   6.971  nominal  -1.000 GeV     | RUN_196 , Started  2-NOV-2000 08:02
 Current (mA)   0.000 e+  *   0.000 e-   |           Last 4 mn     Run    Fill
 Luminosity : Now      0.00 10^30        | Events            0       0       0
        This fill      0.00 nb-1         | LCAL              0       0       0
 Collimators are Physics                 | QQbar             0       0       0
-------- LEP operator comments ----------| L1 rate Hz    0.000   0.000 Ineff.(%)
LEP in limbo, awaiting decision ...      | L2 rate Hz    0.000   0.000 Op   0.00
                                         | L3 rate Hz    0.000   0.000 DAQ  0.00
                                         | Deadtime %     0.00    0.00 Dt   0.00
                                         | Downtime %     0.00    0.00 Tot  0.00
                                         | Magnet          3.1    12.1     9.9
                                         | BKG    TPC    -0.80  SAMBA     0.00
----- Shift crew : None --------------------------------------------------------
----- Comment updated on 18-SEP-2000 16:28 -------------------------------------
*/

var Page1 = function(msg, sys)   {
  var tr, td, tb, tab;
  table           = document.createElement('table');
  table.body      = document.createElement('tbody');
  table.messages  = msg;
  table.system    = sys;
  table.provider  = null;
  table.logger    = null;

  table.add = function() {
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.setAttribute('colSpan',2);
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  }

  table.className = 'MonitorPage';
  table.body.className = 'MonitorPage';
  table.body.cellpadding = 0;
  table.body.cellspacing = 0;
  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  table.LHC_header = function() {
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
    
    this.lhcFillNumber = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.FillNumber',null,null);
    tr.appendChild(this.lhcFillNumber);
    
    td = document.createElement('td');
    td.innerHTML = 'is in state:';
    tr.appendChild(td);
    
    this.lhcBeamMode = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.BeamMode',null,null);
    tr.appendChild(this.lhcBeamMode);

    td = document.createElement('td');
    td.innerHTML = '';
    tr.appendChild(td);

    tab.appendChild(tb);
    return tab;
  }
  
  table.LHCb_header = function() {
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
    
    this.runNumber = StyledItem('lbWeb.'+this.system+'_RunInfo.general.runNumber',null,null);
    tr.appendChild(this.runNumber);
    
    td = document.createElement('td');
    td.innerHTML = ' is ';
    tr.appendChild(td);
    
    this.runState = FSMItem('lbWeb.'+this.system,this.logger,true);
    //this.runState = StyledItem('lbWeb.'+this.system+'.FSM.state',null,null);
    tr.appendChild(this.runState);
    tr.appendChild(this.runState.lock);

    td = document.createElement('td');
    td.innerHTML = '';
    tr.appendChild(td);

    tab.appendChild(tb);
    return tab;
  }

  /**
     Build table like this:
     +----------------------------------------
     | Energy   6.971  nominal  -1.000 GeV     
     | Current (mA)   0.000 e+  *   0.000 e-   
     | Luminosity : Now      0.00 10^30        
     |        This fill      0.00 nb-1         
     | Collimators are Physics                 
     +----------------------------------------
  */
  table.LHC_summary = function() {

    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className = 'MonitorData';
    tb.className  = 'MonitorData';
    tab.width     = '100%';
    tab.height    = '120px';

    this.lhcEnergy        = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Energy',null,"%7.1f GeV");
    //this.lhcBeamMode      = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.BeamMode',null,null);
    this.lhcIntensity1    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam1.totalIntensity',null,"%9.2e");
    this.lhcIntensity2    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam2.totalIntensity',null,"%9.2e");
    this.lhcLifetime1     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam1.primitiveLifetime',null,"%7.2f");
    this.lhcLifetime2     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam2.primitiveLifetime',null,"%7.2f");
    this.lhcAvgLumi       = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Beam.Luminosity.LuminosityAverage',null,"%9.2e");
    this.lhc_LHCbDump     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_BEAMDUMP',null,null);
    this.lhc_LHCbAdjust   = StyledItem('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_ADJUST',null,null);
    this.lhc_LHCbInject   = StyledItem('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_INJECTION',null,null);

    this.lhcEnergy.conversion = function(value) {  return value*120.0/1000.0; }

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('LHC status summary',4,'MonitorDataHeader'));

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Energy:',null,'MonitorDataHeader'));
    tr.appendChild(this.lhcEnergy);
    tr.appendChild(Cell('Avg.Luminosity:',null,'MonitorDataHeader'));
    tr.appendChild(this.lhcAvgLumi);

    tr = document.createElement('tr');
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Intensity [e]:',null,'MonitorDataHeader'));
    tr.appendChild(this.lhcIntensity1);
    tr.appendChild(Cell('&lt;1 Beam 2&gt;',null,null));
    tr.appendChild(this.lhcIntensity2);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Lifetime [h]:',null,'MonitorDataHeader'));
    tr.appendChild(this.lhcLifetime1);
    tr.appendChild(Cell('&lt;1 Beam 2&gt;',null,null));
    tr.appendChild(this.lhcLifetime2);

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
    tr.appendChild(this.lhc_LHCbDump);
    tr.appendChild(this.lhc_LHCbAdjust);
    tr.appendChild(this.lhc_LHCbInject);
    this.lhc_LHCbDump.width   = '70px';
    this.lhc_LHCbAdjust.width = '70px';
    this.lhc_LHCbInject.width = '70px';

    /*
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(this.lhcLumiFill);
    */


    tab.appendChild(tb);
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
  table.LHCb_summary = function() {

    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className  = 'MonitorData';
    tab.width      = '100%';
    tb.className   = 'MonitorData';
    tb.cellSpacing = 0;
    tb.cellPadding = 0;

    this.runType        = StyledItem('lbWeb.'+this.system+'_RunInfo.general.runType',     null,null);
    this.runStart       = StyledItem('lbWeb.'+this.system+'_RunInfo.general.runStartTime',null,'Started at: %s');
    this.nTriggers      = StyledItem('lbWeb.'+this.system+'_RunInfo.TFC.nTriggers',       null,null);
    this.hltNTriggers   = StyledItem('lbWeb.'+this.system+'_RunInfo.HLTFarm.hltNTriggers',null,null);
    this.l0Rate         = StyledItem('lbWeb.'+this.system+'_RunInfo.TFC.triggerRate',     null,'%8.2f Hz');
    this.l0RateRun      = StyledItem('lbWeb.'+this.system+'_RunInfo.TFC.runTriggerRate',  null,'%8.2f Hz');
    this.hltRate        = StyledItem('lbWeb.'+this.system+'_RunInfo.HLTFarm.hltRate',     null,'%8.2f Hz');
    this.hltRateRun     = StyledItem('lbWeb.'+this.system+'_RunInfo.HLTFarm.runHltRate',  null,'%8.2f Hz');
    this.deadTime       = StyledItem('lbWeb.'+this.system+'_RunInfo.TFC.deadTime',        null,'%8.2f %%');
    this.deadTimeRun    = StyledItem('lbWeb.'+this.system+'_RunInfo.TFC.runDeadTime',     null,'%8.2f %%');
    this.magnetField    = StyledItem('lbWeb.BSensor0.Babs',                               null,'%7.4f');
    this.magnetPolarity = StyledItem('lbWeb.Magnet.Polarity',                             null, null);
    this.veloPosition   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.RunControl.VeloPos',          null, null);
    this.figureOfMerit1 = StyledItem('lbWeb.BCM_DP_S0.RS2_REL',                           null, 'S0:%7.3f');
    this.figureOfMerit2 = StyledItem('lbWeb.BCM_DP_S0.RS32_REL',                          null, '%7.3f');
    this.figureOfMerit3 = StyledItem('lbWeb.BCM_DP_S1.RS2_REL',                           null, 'S1:%7.3f');
    this.figureOfMerit4 = StyledItem('lbWeb.BCM_DP_S1.RS32_REL',                          null, '%7.3f');
    this.bcmBeamPermit1 = StyledItem('lbWeb.BCM_Interface.BeamPermit.getStatus',          null, null);
    this.bcmBeamPermit2 = StyledItem('lbWeb.BCM_Interface.InjPermit1.getStatus',          null, null);
    this.bcmBeamPermit3 = StyledItem('lbWeb.BCM_Interface.InjPermit2.getStatus',          null, null);

    // Run status
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(this.runType);
    this.runStart.colSpan = 4;
    tr.appendChild(this.runStart);
    this.runStart.style.textAlign = 'left';

    // Magnet status
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Magnet [T]:',1,'MonitorDataHeader'));
    tr.appendChild(this.magnetField);
    tr.appendChild(Cell('Polarity:',1,'MonitorDataHeader'));
    tr.appendChild(this.magnetPolarity);

    // Background status
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('BCM BKG [&permil;]:',1,'MonitorDataHeader'));
    tr.appendChild(this.figureOfMerit1);
    tr.appendChild(this.figureOfMerit2);
    tr.appendChild(this.figureOfMerit3);
    tr.appendChild(this.figureOfMerit4);

    // Beam permits
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('BCM Permits:',null,'MonitorDataHeader'));
    tr.appendChild(this.bcmBeamPermit1);
    tr.appendChild(this.bcmBeamPermit2);
    tr.appendChild(this.bcmBeamPermit3);

    // Velo position
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Velo position:',1,'MonitorDataHeader'));
    tr.appendChild(this.veloPosition);

    // Run status information
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('L0 Events:',null,'MonitorDataHeader'));
    tr.appendChild(this.nTriggers);
    tr.appendChild(Cell('accepted:',1,'MonitorDataHeader'));
    tr.appendChild(this.hltNTriggers);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('',1,null));
    tr.appendChild(Cell('Now',2,'MonitorDataHeader'));
    tr.appendChild(Cell('Run',2,'MonitorDataHeader'));

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('L0 rate:',1,'MonitorDataHeader'));
    this.l0Rate.colSpan = 2;
    this.l0RateRun.colSpan = 2;
    tr.appendChild(this.l0Rate);
    tr.appendChild(this.l0RateRun);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('HLT rate:',1,'MonitorDataHeader'));
    this.hltRate.colSpan = 2;
    this.hltRateRun.colSpan = 2;
    tr.appendChild(this.hltRate);
    tr.appendChild(this.hltRateRun);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Dead-time:',null,'MonitorDataHeader'));
    this.deadTime.colSpan = 2;
    this.deadTimeRun.colSpan = 2;
    tr.appendChild(this.deadTime);
    tr.appendChild(this.deadTimeRun);

    tab.appendChild(tb);
    return tab;
  }

  /**
     Build table like this:

     +-------- LEP operator comments ----------
     |LEP in limbo, awaiting decision ...      
     |
     +-----------------------------------------
  */
  table.LHC_operator = function() {
    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.width  = '100%';
    tab.height = '100%';
    tb.height  = '100%';
    tab.className = 'MonitorData';
    tb.className = 'MonitorData';
    
    this.lhcComments   = StyledItem('lbWeb.Background.lhcComments',4,null);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('LHC operator comments:',null,'MonitorDataHeader'));
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(this.lhcComments);
    this.lhcComments.height = '60px';

    tab.appendChild(tb);
    return tab;
  }

  /**
     Build table like this:

     +---- Shift crew : None --------------------------------------------------------
     |
     |
     +---- Comment updated on 18-SEP-2000 16:28 -------------------------------------
  */
  table.LHCb_shift = function() {
    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className = 'MonitorData';
    tb.className = 'MonitorData';
    tab.width = '100%';

    this.shiftComments   = StyledItem('lbWeb.shiftComments',4,null);
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Shift comments:',null,'MonitorDataHeader'));
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(this.shiftComments);
    this.shiftComments.height = '100px';
    tab.appendChild(tb);
    return tab;
  }

  table.subscribeItem = function(item) {
    this.provider.subscribe(item.name,item);
  }
  table.subscribe = function() {
    this.subscribeItem(this.runNumber);
    this.subscribeItem(this.runState);
    this.subscribeItem(this.runType);
    this.subscribeItem(this.runStart);
    this.subscribeItem(this.nTriggers);
    this.subscribeItem(this.hltNTriggers);
    this.subscribeItem(this.l0Rate);
    this.subscribeItem(this.l0RateRun);
    this.subscribeItem(this.hltRate);
    this.subscribeItem(this.hltRateRun);
    this.subscribeItem(this.deadTime);
    this.subscribeItem(this.deadTimeRun);
    this.subscribeItem(this.veloPosition);
    //this.subscribeItem(this.);
    //this.subscribeItem(this.);

    this.subscribeItem(this.lhcBeamMode);
    this.subscribeItem(this.lhcFillNumber);
    this.subscribeItem(this.lhcEnergy);
    this.subscribeItem(this.lhcIntensity1);
    this.subscribeItem(this.lhcIntensity2);
    this.subscribeItem(this.lhcLifetime1);
    this.subscribeItem(this.lhcLifetime2);
    this.subscribeItem(this.lhcAvgLumi);
    this.subscribeItem(this.lhc_LHCbDump);
    this.subscribeItem(this.lhc_LHCbAdjust);
    this.subscribeItem(this.lhc_LHCbInject);

    this.subscribeItem(this.bcmBeamPermit1);
    this.subscribeItem(this.bcmBeamPermit2);
    this.subscribeItem(this.bcmBeamPermit3);

    this.subscribeItem(this.lhcComments);


    this.subscribeItem(this.magnetField);
    this.subscribeItem(this.magnetPolarity);

    this.subscribeItem(this.figureOfMerit1);
    this.subscribeItem(this.figureOfMerit2);
    this.subscribeItem(this.figureOfMerit3);
    this.subscribeItem(this.figureOfMerit4);

    this.subscribeItem(this.shiftComments);
    return this;
  }

  table.build = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var d = new Date();

    tab.width='100%';
    tb.width = '100%';
    this.heading = document.createElement('tr');
    var cell = Cell(lhcb_online_picture()+'&nbsp;'+this.system+' Page1',1,'MonitorBigHeader');
    cell.style.textAlign = 'left';
    cell.style.width = '360px';
    this.heading.appendChild(cell);
    this.head_date = Cell(d.toString(),1,'MonitorTinyHeader');
    this.head_date.textAlign = 'right';
    this.head_date.id = 'current_time';
    this.head_date.style.width = '360px';
    this.heading.appendChild(this.head_date);
    tb.appendChild(this.heading);

    this.timerHandler = function() {document.getElementById('current_time').innerHTML = (new Date()).toString(); }
    setInterval(this.timerHandler,2000);
    
    tr = document.createElement('tr');
    tb.appendChild(tr);
    
    td = document.createElement('td');
    td.appendChild(this.LHC_header());
    tr.appendChild(td);
    
    td = document.createElement('td');
    td.appendChild(this.LHCb_header());
    tr.appendChild(td);
    
    
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.appendChild(this.LHC_summary());    
    tr.appendChild(td);

    td = document.createElement('td');
    var sum = this.LHCb_summary();
    sum.height = '230px';
    td.appendChild(sum);
    td.rowSpan = 2;
    tr.appendChild(td);
    tb.appendChild(tr);
    // LHC operator comments
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.appendChild(this.LHC_operator());    
    tr.appendChild(td);
    tb.appendChild(tr);
    // Shift crew comments
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.appendChild(this.LHCb_shift());
    td.colSpan = 2;
    tr.appendChild(td);
    tb.appendChild(tr);

    // Finally add suggestions text
    tr = document.createElement('tr');
    tr.appendChild(Cell('',1,'MonitorTinyHeader'));
    tr.appendChild(Cell('Comments and suggestions to M.Frank CERN/LHCb',1,'MonitorTinyHeader'));
    tb.appendChild(tr);

    tab.appendChild(tb);
    this.display.appendChild(tab);
    return this;
  }

  return table;
}

var page1_unload = function()  {
  dataProviderReset();
  //alert('Connection reset.\n\nBye, Bye my friend....');
}


var page1_body = function()  {
  var msg = the_displayObject['messages'];
  var sys = the_displayObject['system'];
  var body = document.getElementsByTagName('body')[0];
  var selector;

  if ( sys == null ) sys = 'LHCb';
  selector = Page1(msg, sys);
  body.appendChild(selector);
  setWindowTitle(sys+' Page 1');
  //msg = 1;
  if ( msg > 0 )
    selector.logger   = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'StatusLogger');
  else
    selector.logger   = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'StatusLogger');
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  //selector.start('PARTITIONS','lbWeb.PARTITIONS');
  selector.build();
  selector.subscribe();
  selector.provider.start();
}

if ( _debugLoading ) alert('Script lhcb.display.page1.cpp loaded successfully');
