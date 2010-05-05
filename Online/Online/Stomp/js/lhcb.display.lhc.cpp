_loadScript('lhcb.display.tooltips.cpp');
_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');

var LHCStatus = function(msg)   {
  var tr, td, tb, tab;
  table           = document.createElement('table');
  table.body      = document.createElement('tbody');
  table.messages  = msg;
  table.provider  = null;
  table.logger    = null;
  table.haveBeamType = null;

  table.add = function() {
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.setAttribute('colSpan',2);
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  };

  table.className = 'MonitorPage';
  table.body.className = 'MonitorPage';
  table.body.cellpadding = table.body.cellspacing = 0;
  table.style.width = table.body.style.width = '100%';

  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  table.LHC_Operations_Info = function() {
    var c, tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.style.width = tb.style.width = '100%';
    tab.className = tb.className = 'MonitorPage';

    this.energy        = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Energy',null,'%7.1f GeV');
    this.fillNumber    = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.FillNumber',null,null);
    this.machineMode   = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.MachineMode',null,null);
    this.beamMode      = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.BeamMode',null,null);
    this.safeBeam1     = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.SafeBeam.Beam1',null,null);
    this.safeBeam2     = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.SafeBeam.Beam2',null,null);
    //this.intensity1    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam1.totalIntensity',null,'%9.2e');
    this.intensity1    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam1.averageBeamIntensity',null,'%9.2e');
    //this.intensity2    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam2.totalIntensity',null,'%9.2e');
    this.intensity2    = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam2.averageBeamIntensity',null,'%9.2e');
    //this.lifetime1     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam1.primitiveLifetime',null,'%7.2f hours');
    this.lifetime1     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam1.bestLifetime',null,'%7.2f hours');
    //this.lifetime2     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam2.primitiveLifetime',null,'%7.2f hours');
    this.lifetime2     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.IntensityPerBunch.Beam2.bestLifetime',null,'%7.2f hours');

    this.energy.conversion = function(data) {      return (data*120.0)/1000.0;    };
    if ( this.haveBeamType ) {
      this.beamType1     = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.BeamType.Beam1',null,null);
      this.beamType2     = StyledItem('lbWeb.LHCCOM/LHC.LHC.RunControl.BeamType.Beam2',null,null);
      this.beamType1.conversion = function(data) {
	if (data==0)      return 'No Beam';
	else if (data==1) return 'Protons';
	else if (data==2) return 'Heavy Ions';
	else              return 'Unknown';
      };
      this.beamType2.conversion = this.beamType1.conversion;
    }
    this.safeBeam1.conversion = function(data) {
      if (data==0)      return 'No Beam';
      else if (data==1) return 'Present';
      else if (data==2) return 'Safe';
      else if (data==3) return 'Stable';
      else if (data==4) return 'VELO Allowed';
      else              return 'Unknown';
    };
    this.safeBeam2.conversion = this.safeBeam1.conversion;

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('LHC Status:',null,'MonitorDataHeaderRED'));
    c.style.width = '20%';
    tr.appendChild(c=Cell('Fill:',null,'MonitorDataHeader'));
    c.style.width = '15%';
    this.fillNumber.style.width='25%';
    tr.appendChild(this.fillNumber);
    tr.appendChild(c=Cell('Energy:',null,'MonitorDataHeader'));
    c.style.width = '15%';
    tr.appendChild(this.energy);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('',null,null));
    tr.appendChild(c=Cell('Beam mode:',null,'MonitorDataHeader'));
    tr.appendChild(this.beamMode);
    tr.appendChild(c=Cell('Machine mode:',null,'MonitorDataHeader'));
    tr.appendChild(this.machineMode);
    tb.appendChild(tr);
    if ( this.haveBeamType ) {
      tr = document.createElement('tr');
      tr.appendChild(c=Cell('Beam Type:',null,'MonitorDataHeader'));
      tr.appendChild(Cell('Beam 1:',null,'MonitorDataHeader'));
      tr.appendChild(this.beamType1);
      tr.appendChild(Cell('Beam2:',null,'MonitorDataHeader'));
      tr.appendChild(this.beamType2);
      tb.appendChild(tr);
    }
    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Beam State:',null,'MonitorDataHeader'));
    tr.appendChild(Cell('Beam 1:',null,'MonitorDataHeader'));
    tr.appendChild(this.safeBeam1);
    tr.appendChild(Cell('Beam2:',null,'MonitorDataHeader'));
    tr.appendChild(this.safeBeam2);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Intensity:',null,'MonitorDataHeader'));
    tr.appendChild(Cell('Beam 1:',null,'MonitorDataHeader'));
    tr.appendChild(this.intensity1);
    tr.appendChild(Cell('Beam2:',null,'MonitorDataHeader'));
    tr.appendChild(this.intensity2);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Lifetime:',null,'MonitorDataHeader'));
    tr.appendChild(Cell('Beam 1:',null,'MonitorDataHeader'));
    tr.appendChild(this.lifetime1);
    tr.appendChild(Cell('Beam2:',null,'MonitorDataHeader'));
    tr.appendChild(this.lifetime2);
    tb.appendChild(tr);

    tab.appendChild(tb);
    return tab;
  };


  table.Magnet_summary = function() {
    var c, tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');

    tab.style.width = tb.style.width = '100%';
    tab.className = tb.className = 'MonitorPage';

    tr.appendChild(c=Cell('Magnet Status:',null,'MonitorDataHeaderRED'));
    c.style.width = '20%';
    c.rowSpan = 2;

    tr.appendChild(c=Cell('Field:',null,'MonitorDataHeader'));
    c.style.width='10%';
    this.magnetField = StyledItem('lbWeb.lbHyst.B',null,'%7.3f T');
    this.magnetField.style.width='25%';
    tr.appendChild(this.magnetField);
    tr.appendChild(c=Cell('Polarity:',null,'MonitorDataHeader'));
    c.style.width='10%';
    this.magnetPolarity = StyledItem('lbWeb.lbHyst.Polarity',null,null);
    this.magnetPolarity.conversion = function(data) {
      if ( data>0 ) return '+&nbsp;(Down)';
      return '-&nbsp;(Up)';
    };

    tr.appendChild(this.magnetPolarity);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Current:',null,'MonitorDataHeader'));
    this.magnetCurrent = StyledItem('lbWeb.LbMagnet.Current',null,'%7.0f A');
    tr.appendChild(this.magnetCurrent);
    tr.appendChild(c=Cell('Set:',null,'MonitorDataHeader'));
    this.magnetCurrentSet = StyledItem('lbWeb.LbMagnet.SetCurrent',null,'%7.0f A');
    tr.appendChild(this.magnetCurrentSet);
    tb.appendChild(tr);

    tab.appendChild(tb);
    return tab;
  };

  table.Background_summary = function() {
    var c, tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.width = '100%';
    tab.className = 'MonitorPage';
    tb.className = 'MonitorPage';

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Background, Permit &amp; RF Status:',7,'MonitorDataHeaderRED'));
    tb.appendChild(tr);


    this.LHCDump     = StyledItem('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_BEAMDUMP', null,null);
    this.LHCAdjust   = StyledItem('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_ADJUST',   null,null);
    this.LHCInject   = StyledItem('lbWeb.LHCCOM/LHC.LHC.Handshake.LHC_INJECTION',null,null);
    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Hand shakes (LHC):',1,'MonitorDataHeader'));
    c.style.width = '20%';
    tr.appendChild(c=Cell('Dump',1,'MonitorDataHeader'));
    tr.appendChild(this.LHCDump);
    tr.appendChild(Cell('  Adjust:',1,'MonitorDataHeader'));
    tr.appendChild(this.LHCAdjust);
    tr.appendChild(Cell(' Injection:',1,'MonitorDataHeader'));
    tr.appendChild(this.LHCInject);
    tb.appendChild(tr);

    this.LHCbDump     = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Handshake.LHCB_BEAMDUMP', null,null);
    this.LHCbAdjust   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Handshake.LHCB_ADJUST',   null,null);
    this.LHCbInject   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Handshake.LHCB_INJECTION',null,null);
    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Hand shakes (LHCb):',1,'MonitorDataHeader'));
    tr.appendChild(c=Cell('Dump',1,'MonitorDataHeader'));
    tr.appendChild(this.LHCbDump);
    tr.appendChild(Cell('  Adjust:',1,'MonitorDataHeader'));
    tr.appendChild(this.LHCbAdjust);
    tr.appendChild(Cell(' Injection:',1,'MonitorDataHeader'));
    tr.appendChild(this.LHCbInject);
    tb.appendChild(tr);

    this.bcmBeamPermit1 = StyledItem('lbWeb.BCM_Interface.BeamPermit.getStatus', null, null);
    this.bcmBeamPermit2 = StyledItem('lbWeb.BCM_Interface.InjPermit1.getStatus', null, null);
    this.bcmBeamPermit3 = StyledItem('lbWeb.BCM_Interface.InjPermit2.getStatus', null, null);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Beam permits:',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Permit1',1,'MonitorDataHeader'));
    tr.appendChild(this.bcmBeamPermit1);
    tr.appendChild(Cell('Inj.permit 1',1,'MonitorDataHeader'));
    tr.appendChild(this.bcmBeamPermit2);
    tr.appendChild(Cell('Inj.permit 2',1,'MonitorDataHeader'));
    tr.appendChild(this.bcmBeamPermit3);
    tb.appendChild(tr);

    this.figureOfMerit1 = StyledItem('lbWeb.BCM_DP_S0.RS2_REL',            null, '%7.3f');
    this.figureOfMerit2 = StyledItem('lbWeb.BCM_DP_S0.RS32_REL',           null, '%7.3f');
    this.figureOfMerit3 = StyledItem('lbWeb.BCM_DP_S1.RS2_REL',            null, '%7.3f');
    this.figureOfMerit4 = StyledItem('lbWeb.BCM_DP_S1.RS32_REL',           null, '%7.3f');

    tr = document.createElement('tr');
    tr.appendChild(Cell('BCM Figure of Merit [&permil;]',null,'MonitorDataHeader'));
    tr.appendChild(Cell('S0.RS2/32:',1,'MonitorDataHeader'));
    tr.appendChild(this.figureOfMerit1);
    tr.appendChild(this.figureOfMerit2);
    tr.appendChild(Cell('S1.RS2/32:',1,'MonitorDataHeader'));
    tr.appendChild(this.figureOfMerit3);
    tr.appendChild(this.figureOfMerit4);
    tb.appendChild(tr);
    
    this.rfrxF40B1      = StyledItem('lbWeb.LHCCOM/LHC.LHCb.RFRX.F40_B1',  null, 'Beam1:%7.0f kHz');
    this.rfrxF40B1rev   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.RFRX.FREV_B1', null, 'Rev:%7.2f kHz');
    this.rfrxF40B2      = StyledItem('lbWeb.LHCCOM/LHC.LHCb.RFRX.F40_B2',  null, 'Beam2:%7.0f kHz');
    this.rfrxF40B2rev   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.RFRX.FREV_B2', null, 'Rev:%7.2f kHz');
    this.rfrxF40rev     = StyledItem('lbWeb.LHCCOM/LHC.LHCb.RFRX.F40_REF', null, '%7.0f kHz');

    tr = document.createElement('tr');
    tr.appendChild(Cell('RFRX [F40]',null,'MonitorDataHeader'));
    tr.appendChild(this.rfrxF40B1);
    tr.appendChild(this.rfrxF40B1rev);
    tr.appendChild(this.rfrxF40B2);
    tr.appendChild(this.rfrxF40B2rev);
    tr.appendChild(Cell('Reference:',1,'MonitorDataHeader'));
    tr.appendChild(this.rfrxF40rev);
    tb.appendChild(tr);
    
    tab.appendChild(tb);
    return tab;
  };

  table.TED_summary = function() {
    var cell, tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.width = '100%';
    tab.className = 'MonitorPage';
    tb.className  = 'MonitorPage';
    //          lbWeb.LHCCOM/LHC.LHC.Machine.CollimatorPositions.TDI_4R8.lvdt_gap_upstream
    var base = 'lbWeb.LHCCOM/LHC.LHC.Machine.CollimatorPositions.';
    this.TDI_4R8_gap   = StyledItem(base+'TDI_4R8.lvdt_gap_upstream',  'Text-Center','Gap:%8.2f mm');
    this.TDI_4R8_left  = StyledItem(base+'TDI_4R8.lvdt_left_upstream', 'Text-Center','Left:%8.2f mm');
    this.TDI_4R8_right = StyledItem(base+'TDI_4R8.lvdt_right_upstream','Text-Center','Right:%8.2f mm');
    base = 'lbWeb.LHCCOM/LHC.LHC.Machine.TEDPosition.';
    this.TEDTI2  = StyledItem(base+'TEDTI2.position', 'Text-Center','TEDTI2: %s');
    this.TEDTI8  = StyledItem(base+'TEDTI8.position', 'Text-Center','TEDTI8: %s');
    this.TEDTT40 = StyledItem(base+'TEDTT40.position','Text-Center','TEDTT40: %s');
    this.TEDTT60 = StyledItem(base+'TEDTT60.position','Text-Center','TEDTT60: %s');
    this.ted_conversion = function(value) {
      var v = '';//' ('+value+')';
      if ( value == 0 ) return 'Moving'+v;
      else if ( value == 1 ) return 'Installation'+v;
      else if ( value == 2 ) return 'Open'+v;
      else if ( value == 3 ) return 'Closed'+v;
      else if ( value == 4 ) return 'Transport'+v;
      else if ( value == 5 ) return 'Undefined'+v;
      return 'Unknown ('+value+')';
    };
    this.TEDTI2.conversion = this.ted_conversion;
    this.TEDTI8.conversion = this.ted_conversion;
    this.TEDTT40.conversion = this.ted_conversion;
    this.TEDTT60.conversion = this.ted_conversion;

    // TEDs
    tr = document.createElement('tr');    
    tb.appendChild(tr);
    tr.appendChild(cell=Cell('TED Status',1,'MonitorDataHeaderRED'));
    cell.style.width = '20%';
    tr.appendChild(this.TEDTI2);
    tr.appendChild(this.TEDTI8);
    tr.appendChild(this.TEDTT40);
    tr.appendChild(this.TEDTT60);

    // TDI 4R8
    tr = document.createElement('tr');    
    tb.appendChild(tr);
    tr.appendChild(cell=Cell('TDI 4R8 position:',1,'MonitorDataHeaderRED'));
    tr.appendChild(this.TDI_4R8_left);
    tr.appendChild(this.TDI_4R8_gap);
    tr.appendChild(this.TDI_4R8_right);

    tab.appendChild(tb);
    return tab;
  };

  table.Clock_summary = function() {
    var c, tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');

    tab.width     = '100%';
    tab.className = 'MonitorPage';
    tb.className  = 'MonitorPage';

    tb.appendChild(tr);

    this.lhcClock            = lhcb.widgets.rf2ttcStatus();
    this.rf2ttcPrePulses     = lhcb.widgets.rf2ttcPrepulses();
    this.rf2ttcRunState      = lhcb.widgets.rf2ttcState();
    this.rf2ttcSource        = lhcb.widgets.rf2ttcSource(this.rf2ttcRunState);

    tr.appendChild(c=Cell('LHC Timing:',null,'MonitorDataHeaderRED'));
    c.style.width = '20%';
    tr.appendChild(this.rf2ttcSource);

    tr.appendChild(this.lhcClock);
    tr.appendChild(c=Cell('/',null,'Text-Center'));
    tr.appendChild(this.rf2ttcRunState);
    tr.appendChild(c=Cell('Prepulses:',null,'Text-Right'));
    tr.appendChild(this.rf2ttcPrePulses);
    this.rf2ttcPrePulses.style.width='10%';

    tab.appendChild(tb);
    return tab;
  };

  /**
  */
  table.Experiment_summary = function(logger) {
    var c, tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className   = 'MonitorPage';
    tab.style.width = '100%';
    tb.className    = 'MonitorPage';
    tb.style.width  = '100%';
    tb.cellSpacing  = 0;
    tb.cellPadding  = 0;

    // Velo position
    this.runState      = FSMItem('lbWeb.LHCb',logger,true);
    this.l0Rate        = StyledItem('lbWeb.LHCb_RunInfo.TFC.triggerRate',null,'%7.1f Hz');
    this.expStatus     = StyledItem('lbWeb.LHCCOM/LHC.LHCb.ExptStatus.Status',null,null);
    this.veloPosition  = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Specific.VELO.Position',null, 'Position:%s');
    this.veloSafe      = StyledItem('lbWeb.veloMove_RdBool.SafeStableBeam',null, null);
    this.veloInject    = StyledItem('lbWeb.veloMove_RdBool.InjectionAllowed',null, null);
    this.veloMoveDevs  = StyledItem('lbWeb.veloMove_RdBool.MovableDevicesAllowed',null, null);

    this.veloSafe.conversion = function(data) { return data>0 ? 'State: Safe state' : 'State: NOT safe'; };
    this.veloInject.conversion = function(data) { return data>0 ? 'Injection allowed' : 'Injection VETO ON'; };
    this.veloMoveDevs.conversion = function(data) { return data>0 ? 'No movable devices allowed' : 'Movable devices allowed'; };
    this.veloMoveDevs.colSpan = 2;

    tr = document.createElement('tr');
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Experiment Status:',null,'MonitorDataHeaderRED'));
    c.style.width = '20%';
    this.expStatus.style.width='80px';
    tr.appendChild(this.expStatus);

    tr.appendChild(c=Cell('RunState:',null,'MonitorDataHeader'));
    c.style.width='15%';
    this.runState.style.width='20%';
    this.runState.style.textAlign='center';
    tr.appendChild(this.runState);

    tr.appendChild(c=Cell('L0 rate:',null,'MonitorDataHeader'));
    c.style.width='10%';
    tr.appendChild(this.l0Rate);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Velo',null,'MonitorDataHeader'));

    //c.style.width='15%';
    //this.veloPosition.style.width='5%';
    tr.appendChild(this.veloPosition);
    tr.appendChild(this.veloSafe);
    tr.appendChild(this.veloInject);
    tr.appendChild(this.veloMoveDevs);


    tb.appendChild(tr);

    tab.appendChild(tb);
    return tab;
  };

  /**
  */
  table.Trigger_summary = function(logger) {
    var c, tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className  = 'MonitorPage';
    tab.width      = '100%';
    tb.className   = 'MonitorPage';
    tb.width       = '100%';
    tb.cellSpacing = 0;
    tb.cellPadding = 0;

    this.l0Type        = StyledItem('lbWeb.LHCb_RunInfo.Trigger.L0Type',null, null);
    this.HLTType       = StyledItem('lbWeb.LHCb_RunInfo.Trigger.HLTType',null, null);
    this.TCK           = StyledItem('lbWeb.LHCb_RunInfo.Trigger.TCKLabel',null, null);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Trigger:',null,'MonitorDataHeaderRED'));
    c.style.width = '20%';
    c.rowSpan = 2;
    tr.appendChild(c=Cell('L0:',null,'MonitorDataHeader'));
    c.style.width = '10%';
    this.l0Type.style.width = '150px';
    tr.appendChild(this.l0Type);
    tr.appendChild(c=Cell('HLT:',null,'MonitorDataHeader'));
    c.style.width = '10%';
    tr.appendChild(this.HLTType);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('TCK:',1,'MonitorDataHeader'));
    tr.appendChild(this.TCK);
    this.TCK.colSpan = 3;
    tb.appendChild(tr);

    tab.appendChild(tb);
    tooltips.set(tab,'Trigger setup and status<br>Click to go to DAQ page');
    tab.onclick = lhcb.widgets.goto_lhcb_daq_page;
    return tab;
  };

  /**
  */
  table.Cooling_summary = function() {
    var c, tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className  = 'MonitorPage';
    tab.width      = '100%';
    tb.className   = 'MonitorPage';
    tb.cellSpacing = 0;
    tb.cellPadding = 0;

    // Velo position
    this.itCoolingAlarms  = StyledItem('lbWeb.CaV/ItPlant.Actual.alarm', null, null);
    this.otCoolingAlarms  = StyledItem('lbWeb.CaV/OtPlant.Actual.alarm', null, null);
    this.ttCoolingAlarms  = StyledItem('lbWeb.CaV/TtPlant.Actual.alarm', null, null);
    this.richCoolingAlarms  = StyledItem('lbWeb.CaV/RichPlant.Actual.alarm', null, null);

    this.itCoolingFaults  = StyledItem('lbWeb.CaV/ItPlant.Actual.fault', null, null);
    this.otCoolingFaults  = StyledItem('lbWeb.CaV/OtPlant.Actual.fault', null, null);
    this.ttCoolingFaults  = StyledItem('lbWeb.CaV/TtPlant.Actual.fault', null, null);
    this.richCoolingFaults  = StyledItem('lbWeb.CaV/RichPlant.Actual.fault', null, null);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(c=Cell('Cooling',1,'MonitorDataHeaderRED'));
    c.style.width = '20%';
    tr.appendChild(Cell('IT',1,'MonitorDataHeader'));
    tr.appendChild(Cell('TT',1,'MonitorDataHeader'));
    tr.appendChild(Cell('OT',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Rich',1,'MonitorDataHeader'));

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Alarms',1,'MonitorDataHeader'));

    tr.appendChild(this.itCoolingAlarms);
    tr.appendChild(this.ttCoolingAlarms);
    tr.appendChild(this.otCoolingAlarms);
    tr.appendChild(this.richCoolingAlarms);

    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Faults',1,'MonitorDataHeader'));

    tr.appendChild(this.itCoolingFaults);
    tr.appendChild(this.ttCoolingFaults);
    tr.appendChild(this.otCoolingFaults);
    tr.appendChild(this.richCoolingFaults);

    tab.appendChild(tb);
    tooltips.set(tab,'Hardware status<br>Click to go to hardware status page');
    tab.onclick     = function() { document.location = "lhcb.display.htm?type=detstatus";};
    return tab;
  };

  table.subscribeItem = function(item) {
    this.provider.subscribe(item.name,item);
  };
  table.subscribe = function() {

    // LHC_operations_Info
    this.subscribeItem(this.energy);
    this.subscribeItem(this.fillNumber);
    this.subscribeItem(this.machineMode);
    this.subscribeItem(this.beamMode);
    if ( this.haveBeamType ) {
      this.subscribeItem(this.beamType1);
      this.subscribeItem(this.beamType2);
    }
    this.subscribeItem(this.safeBeam1);
    this.subscribeItem(this.safeBeam2);
    this.subscribeItem(this.intensity1);
    this.subscribeItem(this.intensity2);
    this.subscribeItem(this.lifetime1);
    this.subscribeItem(this.lifetime2);

    // Clock_summary
    this.subscribeItem(this.lhcClock);
    this.subscribeItem(this.rf2ttcSource);
    this.subscribeItem(this.rf2ttcRunState);
    this.subscribeItem(this.rf2ttcPrePulses);

    // TED_summary:
    this.subscribeItem(this.TEDTI2);
    this.subscribeItem(this.TEDTI8);
    this.subscribeItem(this.TEDTT40);
    this.subscribeItem(this.TEDTT60);
    this.subscribeItem(this.TDI_4R8_left);
    this.subscribeItem(this.TDI_4R8_gap);
    this.subscribeItem(this.TDI_4R8_right);

    /// Magnet_summary
    this.subscribeItem(this.magnetField);
    this.subscribeItem(this.magnetPolarity);
    this.subscribeItem(this.magnetCurrent);
    this.subscribeItem(this.magnetCurrentSet);

    // Background, Permits & RF
    this.subscribeItem(this.LHCDump);
    this.subscribeItem(this.LHCAdjust);
    this.subscribeItem(this.LHCInject);

    this.subscribeItem(this.LHCbDump);
    this.subscribeItem(this.LHCbAdjust);
    this.subscribeItem(this.LHCbInject);

    this.subscribeItem(this.bcmBeamPermit1);
    this.subscribeItem(this.bcmBeamPermit2);
    this.subscribeItem(this.bcmBeamPermit3);

    this.subscribeItem(this.figureOfMerit1);
    this.subscribeItem(this.figureOfMerit2);
    this.subscribeItem(this.figureOfMerit3);
    this.subscribeItem(this.figureOfMerit4);

    this.subscribeItem(this.rfrxF40B1);
    this.subscribeItem(this.rfrxF40B1rev);
    this.subscribeItem(this.rfrxF40B2);
    this.subscribeItem(this.rfrxF40B2rev);
    this.subscribeItem(this.rfrxF40rev);

    this.subscribeItem(this.runState);
    this.subscribeItem(this.l0Rate);
    this.subscribeItem(this.expStatus);
    this.subscribeItem(this.veloPosition);
    this.subscribeItem(this.veloSafe);
    this.subscribeItem(this.veloInject);
    this.subscribeItem(this.veloMoveDevs);
    this.subscribeItem(this.l0Type);
    this.subscribeItem(this.HLTType);
    this.subscribeItem(this.TCK);

    this.subscribeItem(this.itCoolingAlarms);
    this.subscribeItem(this.itCoolingFaults);
    this.subscribeItem(this.otCoolingAlarms);
    this.subscribeItem(this.otCoolingFaults);
    this.subscribeItem(this.ttCoolingAlarms);
    this.subscribeItem(this.ttCoolingFaults);
    this.subscribeItem(this.richCoolingAlarms);
    this.subscribeItem(this.richCoolingFaults);
  };

  table.build = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var t1, tb1, tr1, td1, t2, tb2, tr2, td2, d = new Date();

    tab.width ='100%';
    tb.width  = '100%';

    tb.className = tab.className = 'MonitorPage';
    this.heading = document.createElement('tr');
    var cell = Cell('<IMG src="http://lhc.web.cern.ch/lhc/images/LHC.gif" height="64"/> LHC machine and beam status',1,'MonitorBigHeader');
    cell.style.textAlign = 'left';
    cell.style.width = '60%';
    this.heading.appendChild(cell);
    tooltips.set(cell,'LHC status summary.<br>Click to see LHC operations page');
    cell.onclick = lhcb.widgets.goto_lhc_operations_page;
    this.head_date    = Cell(d.toString(),1,'MonitorTinyHeader');
    this.head_date.id = 'current_time';
    this.head_date.textAlign = 'right';
    this.head_date.style.width = '250px';
    this.heading.appendChild(this.head_date);
    tb.appendChild(this.heading);
    td1 = Cell('<IMG src="'+_fileBase+'/Images/Beams.jpg" height="64" width="64"/>',1,null);
    this.heading.appendChild(td1);

    this.timerHandler = function() {document.getElementById('current_time').innerHTML = (new Date()).toString(); };
    setInterval(this.timerHandler,2000);

    tr = document.createElement('tr');
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(t1=document.createElement('table'));
    td.style.width = '100%';
    td.colSpan = 3;
    t1.appendChild(tb1=document.createElement('tbody'));
    t1.style.width='100%';
    tb1.style.width='100%';
    t1.style.fontSize = tb1.style.fontSize = '90%';
    //-------------------------------------------------
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.LHC_Operations_Info());
    tooltips.set(td1,'LHC operations information<br>Click to see LHC operations page');
    td1.onclick = lhcb.widgets.goto_lhc_operations_page;
    //-------------------------------------------------
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Clock_summary());
    tooltips.set(td1,'TTC clock information');
    //-------------------------------------------------
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.TED_summary());
    tooltips.set(td1,'LHCb run status<br>Click to see collimator and TED information.');
    td1.onclick = function() { document.location = "lhcb.display.htm?type=collimators";};
    //-------------------------------------------------
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Background_summary());
    tooltips.set(td1,'LHCb background status<br>Click to see BCM information.');
    td1.onclick = function() { document.location = "lhcb.display.htm?type=bcm&sensors=1";};
    //-------------------------------------------------
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(t2=document.createElement('table'));
    t2.appendChild(tb2=document.createElement('tbody'));
    tb2.appendChild(tr2=document.createElement('tr'));
    tb2.width = t2.width = '100%';
    var cell = Cell(lhcb_online_picture()+' Experiment status',1,'MonitorBigHeader');    
    cell.style.textAlign = 'left';
    tr2.appendChild(cell);
    cell = Cell('<IMG src="'+_fileBase+'/Images/LHCb/LHCbTrackSimulation.jpg" height="52"/>',1,'MonitorBigHeader');
    cell.style.textAlign = 'right';
    tr2.appendChild(cell);
    //-------------------------------------------------
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Experiment_summary(this.logger));
    td1.onclick = lhcb.widgets.goto_lhcb_daq_page;
    tooltips.set(td1,'Experiment status<br>Click to go to DAQ page');
    //-------------------------------------------------
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Trigger_summary(this.logger));
    td1.onclick = lhcb.widgets.goto_lhcb_daq_page;
    tooltips.set(td1,'LHCb trigger status<br>Click to see full run status information.');
    //-------------------------------------------------
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Magnet_summary());
    tooltips.set(td1,'Brief overview information<br>of the LHCb magnet status<br>Click to see magnet page.');    
    td1.onclick = function() { document.location = "lhcb.display.htm?type=magnet";};
    //-------------------------------------------------
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.Cooling_summary());
    tooltips.set(td1,'Cooling information summary<br>for various subdetectors');    
    //-------------------------------------------------
    tb.appendChild(tr);
    // Finally add suggestions text
    tr = document.createElement('tr');
    tr.appendChild(cell=Cell('Comments and suggestions to M.Frank CERN/LHCb',3,'MonitorTinyHeader'));
    cell.style.textAlign='right';
    tb.appendChild(tr);

    tab.appendChild(tb);
    this.display.appendChild(tab);
    return this;
  };

  return table;
};

var lhc_unload = function()  {
  dataProviderReset();
};


var lhc_body = function()  {
  var prt  = the_displayObject['external_print'];
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector;

  selector = LHCStatus(msg);
  body.appendChild(selector);
  body.className = 'MainBody';
  setWindowTitle('LHC Status Display');
  if ( msg > 0 )
    selector.logger   = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'StatusLogger');
  else
    selector.logger   = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'StatusLogger');
  if ( prt ) selector.logger.print = prt;
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  selector.build();
  selector.subscribe();
  selector.provider.start();
  body.style.cursor = 'default';
};

if ( _debugLoading ) alert('Script lhcb.display.detstatus.cpp loaded successfully');
