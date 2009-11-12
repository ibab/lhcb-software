_loadScript('lhcb.display.tooltips.cpp');
_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');

/*

+- Subdetector HT status --++-------------- Magnet ---------------------------------+
| Aleph HT is :   not ON   ||    Last read: 13-NOV 18:37:51                         |
| Updated: 11-DEC 18:01:15 |+-------------------------------------------------------+
+--------------------------+|   Electrical circuit state: ON              Negative  |
| Mode Manual        Trips ||   Helium     circuit state: Ready           polarity  |
| VDET              ?    0 |+-------------------------------------------------------+
| ITC               ?    0 ||         Magnet currents (Amps)     Helium Dewar       |
| TPC               ?    0 ||        Required    Set   Measured                     |
| TPC dE/dx         ?    0 || Coil      0.0     0.0     3.1  Press:    1.4 mB       |
| ECAL end A        ?    0 || Comp A    0.0     0.0    12.1  Temp:     3.2  K       |
| ECAL barrel       ?    0 || Comp B    0.0     0.0     9.9  Level:   58.9  %       |
| ECAL end B        ?    0 |+-------------------------------------------------------+
| HCAL/mu end A     ?    0 || TPC clock: 11209 kHz  Last measured  8:02 on  2-NOV   |
| HCAL/mu barrel    ?    0 |+-------------  Background  ----------------------------+
| HCAL/mu end B     ?    0 || Updated 11-DEC 17:59:59  VDET inst.dose (rad/hr) 0.09 |
| SAMBA             ?      |+-------------------------------------------------------+
| SiCAL             ?    0 ||   Currents (µA)     Rates     : Side A  Side B        |
| LCAL              ?    0 || ITC  inner:    0.01  SAMBA ele        0.      0.      |
| BCAL              ?    0 || ITC  outer:    0.00  SAMBA gam        0.      0.      |
+--------------------------+| TPC  total:   -0.391    BCAL          0.      0.      |
| Trigger analog    ?    0 |+-------------- Figures of merit  ----------------------+
| Trigger CAMAC     ?    0 || TPC: -0.80  S_ELE :  0.00  S_GAM :  0.00 ITC:  0.01   |
+--------------------------++-------------------------------------------------------+

*/

var DetStatus = function(msg)   {
  var tr, td, tb, tab;
  table           = document.createElement('table');
  table.body      = document.createElement('tbody');
  table.messages  = msg;
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

  table.LHCb_HT_header = lhcb.widgets.LHCb_HT_header;

  table.hv_conf = function(val) {
    var v = val.split('/',4);
    if ( v.length>1 ) return v[1];
    return val;
  }

  table.addHTState = function(nam) {
    var cell,tr = document.createElement('tr');
    tr.appendChild(cell=Cell(nam,1,null));
    cell.style.width='120px';
    // this['hvState'+nam] = StyledItem('lbWeb.LHCb.HT.State.'+nam,null,null);
    this['hvState'+nam] = StyledItem('lbWeb.'+nam+'_HV.FSM.state',null,null);
    this['hvState'+nam].conversion = function(val) {
      if ( val ) {
	var v=val.split('/',4);
	if ( v.length > 1 ) return v[1];
      }
      return val;
    };
    this['hvTrips'+nam] = StyledItem('lbWeb.LHCb.HT.Trips.'+nam,null,null);
    tr.appendChild(this['hvState'+nam]);
    tr.appendChild(this['hvTrips'+nam]);
    return tr;
  }

  /**
     Build table with HT state of all subdetectors
  */
  table.LHCb_HT_summary = function() {

    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className = 'MonitorData';
    tb.className  = 'MonitorData';
    tab.width     = '100%';
    tab.height    = '120px';
    tr = document.createElement('tr');
    tr.appendChild(Cell('Subdetector',1,'MonitorDataHeader'));
    tr.appendChild(Cell('HV State',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Trips',1,'MonitorDataHeader'));
    tb.appendChild(tr);
    tb.appendChild(this.addHTState('VELOA'));
    tb.appendChild(this.addHTState('VELOC'));
    tb.appendChild(this.addHTState('RICH1'));
    tb.appendChild(this.addHTState('TT'));
    tb.appendChild(this.addHTState('IT'));
    tb.appendChild(this.addHTState('OT'));
    tb.appendChild(this.addHTState('RICH2'));
    tb.appendChild(this.addHTState('PRS'));
    //tb.appendChild(this.addHTState('SPD'));
    tb.appendChild(this.addHTState('ECAL'));
    tb.appendChild(this.addHTState('HCAL'));
    tb.appendChild(this.addHTState('MUONA'));
    tb.appendChild(this.addHTState('MUONC'));

    tab.appendChild(tb);
    return tab;
  }

  table.Magnet_summary = function() {
    var c, tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.width = '100%';
    tab.className = 'MonitorData';
    tb.className = 'MonitorData';

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Magnet Status:',null,'MonitorDataHeader'));
    c.style.width='120px';
    this.lastMagnetReading = StyledItem('lbWeb.LHCb.LastMagnetReading',null,null);
    tr.appendChild(this.lastMagnetReading);
    tr.appendChild(c=Cell('Polarity:',null,'MonitorDataHeader'));
    c.style.width='120px';
    this.magnetPolarity = StyledItem('lbWeb.LHCb.LbMagnet.Polarity',null,null);
    this.magnetPolarity.style.width='120px';
    tr.appendChild(this.magnetPolarity);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Current:',null,'MonitorDataHeader'));
    this.magnetCurrent = StyledItem('lbWeb.LHCb.LbMagnet.Current',null,null);
    this.magnetCurrent.style.width='120px';
    tr.appendChild(this.magnetCurrent);
    tr.appendChild(c=Cell('Set:',null,'MonitorDataHeader'));
    c.style.width='120px';
    this.magnetCurrentSet = StyledItem('lbWeb.LHCb.LbMagnet.SetCurrent',null,null);
    this.magnetCurrentSet.style.width='120px';
    tr.appendChild(this.magnetCurrentSet);
    tb.appendChild(tr);

    this.magnetField0 = StyledItem('lbWeb.BSensor0.Babs',null,'%7.4f');
    this.magnetField1 = StyledItem('lbWeb.BSensor1.Babs',null,'%7.4f');
    this.magnetField2 = StyledItem('lbWeb.BSensor2.Babs',null,'%7.4f');
    this.magnetField3 = StyledItem('lbWeb.BSensor3.Babs',null,'%7.4f');
    this.magnetTemp0  = StyledItem('lbWeb.BSensor0.Temp',null,'%7.4f');
    this.magnetTemp1  = StyledItem('lbWeb.BSensor1.Temp',null,'%7.4f');
    this.magnetTemp2  = StyledItem('lbWeb.BSensor2.Temp',null,'%7.4f');
    this.magnetTemp3  = StyledItem('lbWeb.BSensor3.Temp',null,'%7.4f');

    tr = document.createElement('tr');
    tr.appendChild(Cell('Probe',null,'MonitorDataHeader'));
    tr.appendChild(Cell('-1-',null,null));
    tr.appendChild(Cell('-2-',null,null));
    tr.appendChild(Cell('-3-',null,null));
    tr.appendChild(Cell('-4-',null,null));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Field [T]',null,'MonitorDataHeader'));
    tr.appendChild(this.magnetField0);
    tr.appendChild(this.magnetField1);
    tr.appendChild(this.magnetField2);
    tr.appendChild(this.magnetField3);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Temperature [&#186;C]',null,'MonitorDataHeader'));
    tr.appendChild(this.magnetTemp0);
    tr.appendChild(this.magnetTemp1);
    tr.appendChild(this.magnetTemp2);
    tr.appendChild(this.magnetTemp3);
    tb.appendChild(tr);
    
    tab.appendChild(tb);
    return tab;
  }

  table.Background_summary = function() {
    var c, tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.width = '100%';
    tab.className = 'MonitorData';
    tb.className = 'MonitorData';

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Background Status:',7,'MonitorDataHeader'));
    c.style.width='150px';
    tb.appendChild(tr);

    this.bcmBeamPermit1 = StyledItem('lbWeb.BCM_Interface.BeamPermit.getStatus', null, null);
    this.bcmBeamPermit2 = StyledItem('lbWeb.BCM_Interface.InjPermit1.getStatus', null, null);
    this.bcmBeamPermit3 = StyledItem('lbWeb.BCM_Interface.InjPermit2.getStatus', null, null);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Beam permits',1,'MonitorDataHeader'));
    tr.appendChild(this.bcmBeamPermit1);
    this.bcmBeamPermit1.colSpan = 2;
    tr.appendChild(this.bcmBeamPermit2);
    this.bcmBeamPermit2.colSpan = 2;
    tr.appendChild(this.bcmBeamPermit3);
    this.bcmBeamPermit3.colSpan = 2;
    tb.appendChild(tr);

    this.figureOfMerit1 = StyledItem('lbWeb.BCM_DP_S0.RS2_REL',                           null, '%7.3f');
    this.figureOfMerit2 = StyledItem('lbWeb.BCM_DP_S0.RS32_REL',                          null, '%7.3f');
    this.figureOfMerit3 = StyledItem('lbWeb.BCM_DP_S1.RS2_REL',                           null, '%7.3f');
    this.figureOfMerit4 = StyledItem('lbWeb.BCM_DP_S1.RS32_REL',                          null, '%7.3f');
    tr = document.createElement('tr');
    tr.appendChild(Cell('Figure of Merit',null,'MonitorDataHeader'));
    tr.appendChild(Cell('S0.RS2/32:',1,'MonitorDataHeader'));
    tr.appendChild(this.figureOfMerit1);
    tr.appendChild(this.figureOfMerit2);
    tr.appendChild(Cell('S1.RS2/32:',1,'MonitorDataHeader'));
    tr.appendChild(this.figureOfMerit3);
    tr.appendChild(this.figureOfMerit4);
    tb.appendChild(tr);
    
    tab.appendChild(tb);
    return tab;
  }

  table.Clock_summary = function() {
    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');
    tab.width     = '100%';
    tab.className = 'MonitorData';
    tb.className  = 'MonitorData';

    tr = document.createElement('tr');
    tb.appendChild(tr);

    tr.appendChild(Cell('LHC clock:',null,'MonitorDataHeader'));
    this.lhcClock = StyledItem('lbWeb.LHC.Clock',null,null);
    tr.appendChild(this.lhcClock);
    tr.appendChild(Cell('Last measured:',null,null));
    this.lhcClockReading = StyledItem('lbWeb.LHC.LastClockReading',null,null);
    tr.appendChild(this.lhcClockReading);

    tr = document.createElement('tr');

    tb.appendChild(tr);

    tab.appendChild(tb);
    return tab;
  }

  /**
  */
  table.Velo_summary = function() {
    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className  = 'MonitorData';
    tab.width      = '100%';
    tb.className   = 'MonitorData';
    tb.cellSpacing = 0;
    tb.cellPadding = 0;

    // Velo position
    this.veloPosition   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.RunControl.VeloPos',          null, null);
    tr = document.createElement('tr');
    tb.appendChild(tr);
    tr.appendChild(Cell('Velo position:',1,'MonitorDataHeader'));
    tr.appendChild(this.veloPosition);

    tab.appendChild(tb);
    return tab;
  }

  /**
  */
  table.Cooling_summary = function() {
    var tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className  = 'MonitorData';
    tab.width      = '100%';
    tb.className   = 'MonitorData';
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
    tr.appendChild(Cell('Cooling',1,'MonitorDataHeader'));
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
    return tab;
  }

  table.subscribeItem = function(item) {
    this.provider.subscribe(item.name,item);
  }
  table.subscribe = function() {
    this.subscribeItem(this.hvState);
    //this.subscribeItem(this.hvStateUpdate);
    this.subscribeItem(this.hvStateVELOA);
    this.subscribeItem(this.hvStateVELOC);
    this.subscribeItem(this.hvStateRICH1);
    this.subscribeItem(this.hvStateTT);
    this.subscribeItem(this.hvStateIT);
    this.subscribeItem(this.hvStateOT);
    this.subscribeItem(this.hvStateRICH2);
    this.subscribeItem(this.hvStatePRS);
    //this.subscribeItem(this.hvStateSPD);
    this.subscribeItem(this.hvStateECAL);
    this.subscribeItem(this.hvStateHCAL);
    this.subscribeItem(this.hvStateMUONA);
    this.subscribeItem(this.hvStateMUONC);

    this.subscribeItem(this.hvTripsVELOA);
    this.subscribeItem(this.hvTripsVELOC);
    this.subscribeItem(this.hvTripsRICH1);
    this.subscribeItem(this.hvTripsTT);
    this.subscribeItem(this.hvTripsIT);
    this.subscribeItem(this.hvTripsOT);
    this.subscribeItem(this.hvTripsRICH2);
    this.subscribeItem(this.hvTripsPRS);
    //this.subscribeItem(this.hvTripsSPD);
    this.subscribeItem(this.hvTripsECAL);
    this.subscribeItem(this.hvTripsHCAL);
    this.subscribeItem(this.hvTripsMUONA);
    this.subscribeItem(this.hvTripsMUONC);

    this.subscribeItem(this.lastMagnetReading);
    this.subscribeItem(this.lhcClock);
    this.subscribeItem(this.lhcClockReading);

    this.subscribeItem(this.magnetPolarity);
    this.subscribeItem(this.magnetCurrent);
    this.subscribeItem(this.magnetCurrentSet);
    this.subscribeItem(this.magnetField0);
    this.subscribeItem(this.magnetField1);
    this.subscribeItem(this.magnetField2);
    this.subscribeItem(this.magnetField3);
    this.subscribeItem(this.magnetTemp0);
    this.subscribeItem(this.magnetTemp1);
    this.subscribeItem(this.magnetTemp2);
    this.subscribeItem(this.magnetTemp3);

    this.subscribeItem(this.bcmBeamPermit1);
    this.subscribeItem(this.bcmBeamPermit2);
    this.subscribeItem(this.bcmBeamPermit3);

    this.subscribeItem(this.figureOfMerit1);
    this.subscribeItem(this.figureOfMerit2);
    this.subscribeItem(this.figureOfMerit3);
    this.subscribeItem(this.figureOfMerit4);

    this.subscribeItem(this.veloPosition);

    this.subscribeItem(this.itCoolingAlarms);
    this.subscribeItem(this.itCoolingFaults);
    this.subscribeItem(this.otCoolingAlarms);
    this.subscribeItem(this.otCoolingFaults);
    this.subscribeItem(this.ttCoolingAlarms);
    this.subscribeItem(this.ttCoolingFaults);
    this.subscribeItem(this.richCoolingAlarms);
    this.subscribeItem(this.richCoolingFaults);

    /*
    this.subscribeItem(this.);
    this.subscribeItem(this.);
    this.subscribeItem(this.);
    this.subscribeItem(this.);
    */
  }

  table.build = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var t1, tb1, tr1, td1, d = new Date();

    tab.width ='100%';
    tb.width  = '100%';
    this.heading = document.createElement('tr');
    var cell = Cell('LHCb Detector Status',1,'MonitorBigHeader');
    cell.style.textAlign = 'left';
    cell.style.width = '360px';
    this.heading.appendChild(cell);
    this.head_date    = Cell(d.toString(),1,'MonitorTinyHeader');
    this.head_date.id = 'current_time';
    this.head_date.textAlign = 'right';
    this.head_date.style.width = '360px';
    this.heading.appendChild(this.head_date);
    this.head_pic = Cell(lhcb_online_picture(),1,null);
    this.heading.appendChild(this.head_pic);
    tb.appendChild(this.heading);

    this.timerHandler = function() {document.getElementById('current_time').innerHTML = (new Date()).toString(); }
    setInterval(this.timerHandler,2000);

    tr = document.createElement('tr');

    // Left half of the display
    tr.appendChild(td=document.createElement('td'));
    td.style.width = '40%';
    td.appendChild(t1=document.createElement('table'));
    t1.appendChild(tb1=document.createElement('tbody'));
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.LHCb_HT_header());
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.LHCb_HT_summary());

    // Right hand of the display
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(t1=document.createElement('table'));
    td.style.width = '60%';
    td.colSpan = 2;
    t1.appendChild(tb1=document.createElement('tbody'));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Magnet_summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Clock_summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Background_summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Velo_summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Cooling_summary());

    tb.appendChild(tr);

    // Finally add suggestions text
    tr = document.createElement('tr');
    tr.appendChild(Cell('',null,null));
    tr.appendChild(Cell('Comments and suggestions to M.Frank CERN/LHCb',2,'MonitorTinyHeader'));
    tb.appendChild(tr);

    tab.appendChild(tb);
    this.display.appendChild(tab);
    return this;
  }

  return table;
}

var detstatus_unload = function()  {
  dataProviderReset();
  //alert('Connection reset.\n\nBye, Bye my friend....');
}


var detstatus_body = function()  {
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector;

  selector = DetStatus(msg);
  body.appendChild(selector);
  body.className = 'MainBody';
  setWindowTitle('LHCb Detector High Voltage Status');
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

if ( _debugLoading ) alert('Script lhcb.display.detstatus.cpp loaded successfully');
