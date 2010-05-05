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
  };

  table.className = table.body.className = 'MonitorPage';
  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  table.LHCb_HT_header = lhcb.widgets.LHCb_HT_header;
  tooltips.set(table.LHCb_HT_header,'HT summary state of the LHCb subdetector');

  table.hv_conf = function(val) {
    var v = val.split('/',4);
    if ( v.length>1 ) return v[1];
    return val;
  };

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
      };
      return val;
    };
    this['hvTrips'+nam] = StyledItem('lbWeb.LHCb.HT.Trips.'+nam,null,null);
    tr.appendChild(this['hvState'+nam]);
    tr.appendChild(this['hvTrips'+nam]);
    tooltips.set(tr,'HT state of the '+nam+' subdetector');
    return tr;
  };

  /**
     Build table with HT state of all subdetectors
  */
  table.LHCb_HT_summary = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');

    //tooltips.set(tab,'HT state of the LHCb subdetectors');
    tab.className = tb.className  = 'MonitorPage';
    tab.height    = '120px';
    tr.appendChild(Cell('Subdetector',1,'MonitorDataHeader'));
    tr.appendChild(Cell('HV State',1,'MonitorDataHeader'));
    tr.appendChild(Cell('Trips',1,'MonitorDataHeader'));
    tb.appendChild(tr);
    tb.appendChild(this.addHTState('VELOA'));
    tb.appendChild(this.addHTState('VELOC'));
    tb.appendChild(this.addHTState('RICH1'));
    tb.appendChild(this.addHTState('TT'));
    tb.appendChild(this.addHTState('IT'));
    tb.appendChild(this.addHTState('OTA'));
    tb.appendChild(this.addHTState('OTC'));
    tb.appendChild(this.addHTState('RICH2'));
    tb.appendChild(this.addHTState('PRS'));
    //tb.appendChild(this.addHTState('SPD'));
    tb.appendChild(this.addHTState('ECAL'));
    tb.appendChild(this.addHTState('HCAL'));
    tb.appendChild(this.addHTState('MUONA'));
    tb.appendChild(this.addHTState('MUONC'));

    tab.appendChild(tb);
    return tab;
  };

  table.Magnet_summary = function() {
    var c, tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className = tb.className = 'MonitorPage';
    tooltips.set(tab,'Brief overview information<br>of the LHCb magnet status<br>Click to see magnet page.');    
    tab.onclick = function() { document.location = "lhcb.display.htm?type=magnet";};

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Magnet Status:',null,'MonitorDataHeader'));
    c.style.width='30%';
    this.lastMagnetReading = StyledItem('lbWeb.lbHyst.B',null,null);
    this.lastMagnetReading.style.width='30%';
    this.lastMagnetReading.conversion = function(data) {
      return data>0.5 ? 'ON' : 'OFF';
    };
    tr.appendChild(this.lastMagnetReading);
    tr.appendChild(c=Cell('Polarity:',null,'MonitorDataHeader'));
    c.style.width='20%';
    this.magnetPolarity = StyledItem('lbWeb.LbMagnet.Polarity',null,null);
    this.magnetPolarity.style.width='30%';
    this.magnetPolarity.conversion = function(data) {
      if ( data>0 )
	return '+&nbsp;(Down)';
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
    this.magnetCurrentSet.style.width='120px';
    tr.appendChild(this.magnetCurrentSet);
    tb.appendChild(tr);

    this.magnetField0 = StyledItem('lbWeb.LbMagnet.BSensor0.Babs',null,'%7.3f');
    this.magnetField1 = StyledItem('lbWeb.LbMagnet.BSensor1.Babs',null,'%7.3f');
    this.magnetField2 = StyledItem('lbWeb.LbMagnet.BSensor2.Babs',null,'%7.3f');
    this.magnetField3 = StyledItem('lbWeb.LbMagnet.BSensor3.Babs',null,'%7.3f');
    this.magnetTemp0  = StyledItem('lbWeb.LbMagnet.BSensor0.Temp',null,'%7.2f');
    this.magnetTemp1  = StyledItem('lbWeb.LbMagnet.BSensor1.Temp',null,'%7.2f');
    this.magnetTemp2  = StyledItem('lbWeb.LbMagnet.BSensor2.Temp',null,'%7.2f');
    this.magnetTemp3  = StyledItem('lbWeb.LbMagnet.BSensor3.Temp',null,'%7.2f');

    tr = document.createElement('tr');
    tr.appendChild(Cell('Probe',null,'MonitorDataHeader'));
    tr.appendChild(Cell('-1-',null,null));
    tr.appendChild(Cell('-2-',null,null));
    tr.appendChild(Cell('-3-',null,null));
    tr.appendChild(Cell('-4-',null,null));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Field [&nbsp;T&nbsp;]',null,'MonitorDataHeader'));
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
  };

  table.Clock_summary = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');
    var cell = Cell('LHC clock:',null,'MonitorDataHeader');

    tooltips.set(tab,'TTC clock information<br>Click to see LHC status');
    tab.onclick = function() { document.location = "lhcb.display.htm?type=lhc";};
    this.lhcPrepulses   = lhcb.widgets.rf2ttcPrepulses();
    this.lhcClockState  = lhcb.widgets.rf2ttcState();
    this.lhcClock       = lhcb.widgets.rf2ttcSource(this.lhcClockState);

    tab.className = tb.className   = 'MonitorPage';
    tr.appendChild(cell);
    cell.style.width = '25%';
    tr.appendChild(this.lhcClock);
    this.lhcClock.colSpan = 3;
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(cell=Cell('Clock state:',null,'MonitorDataHeader'));
    tr.appendChild(this.lhcClockState);
    this.lhcClockState.style.width = '25%';
    tr.appendChild(cell=Cell('Prepulses:',null,'MonitorDataHeader'));
    tr.appendChild(this.lhcPrepulses);
    this.lhcPrepulses.style.width = '25%';
    tb.appendChild(tr);

    tab.appendChild(tb);
    return tab;
  };

  /**
  */
  table.Velo_summary = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr = document.createElement('tr');
    var cell = Cell('Velo position:',1,'MonitorDataHeader');

    tooltips.set(tab,'Velo position summary');    
    tab.className = tb.className   = 'MonitorPage';
    cell.style.width = '25%';
    // Velo position
    this.veloPosition   = StyledItem('lbWeb.LHCCOM/LHC.LHCb.Specific.VELO.Position', null, null);
    tb.appendChild(tr);
    tr.appendChild(cell);
    tr.appendChild(this.veloPosition);

    tab.appendChild(tb);
    return tab;
  };


  table.subscribeItem = function(item) {
    this.provider.subscribe(item.name,item);
  };
  table.subscribe = function() {
    this.subscribeItem(this.hvState);
    //this.subscribeItem(this.hvStateUpdate);
    this.subscribeItem(this.hvStateVELOA);
    this.subscribeItem(this.hvStateVELOC);
    this.subscribeItem(this.hvStateRICH1);
    this.subscribeItem(this.hvStateTT);
    this.subscribeItem(this.hvStateIT);
    this.subscribeItem(this.hvStateOTA);
    this.subscribeItem(this.hvStateOTC);
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
    this.subscribeItem(this.hvTripsOTA);
    this.subscribeItem(this.hvTripsOTC);
    this.subscribeItem(this.hvTripsRICH2);
    this.subscribeItem(this.hvTripsPRS);
    //this.subscribeItem(this.hvTripsSPD);
    this.subscribeItem(this.hvTripsECAL);
    this.subscribeItem(this.hvTripsHCAL);
    this.subscribeItem(this.hvTripsMUONA);
    this.subscribeItem(this.hvTripsMUONC);

    this.subscribeItem(this.lastMagnetReading);
    this.subscribeItem(this.lhcClock);
    this.subscribeItem(this.lhcClockState);
    this.subscribeItem(this.lhcPrepulses);

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

    this.subscribeItem(this.veloPosition);

    this.background_summary.subscribe(this.provider);
    this.cooling_summary.subscribe(this.provider);
  };

  table.build = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var t1, tb1, tr1, td1, d = new Date();

    tab.className = tb.className = 'MonitorPage';
    tab.width = tb.width  = '100%';
    tab.style.fontSize = '90%';

    this.heading = document.createElement('tr');
    var cell = Cell(lhcb_online_picture()+'&nbsp;LHCb Detector Status',1,'MonitorBigHeader');
    cell.style.textAlign = 'left';
    cell.onclick = function() { document.location = "http://lhcb.cern.ch";};
    this.heading.appendChild(cell);
    cell = Cell('',1,null);
    this.heading.appendChild(cell);
    this.head_date    = Cell(d.toString(),1,'MonitorTinyHeader');
    this.head_date.id = 'current_time';
    this.head_date.textAlign = 'right';
    this.heading.appendChild(this.head_date);
    tooltips.set(this.heading,'LHCb status summary:<br>-- High voltage status<br>-- Magnet status<br>-- TTC status<br>-- Background status<br>-- Velo and cooling status<br>Click to go to LHCb home page');
    tb.appendChild(this.heading);

    this.timerHandler = function() {document.getElementById('current_time').innerHTML = (new Date()).toString(); };
    setInterval(this.timerHandler,2000);

    tr = document.createElement('tr');

    // Left half of the display
    tr.appendChild(td=document.createElement('td'));
    td.style.width = '45%';
    td.appendChild(t1=document.createElement('table'));
    t1.appendChild(tb1=document.createElement('tbody'));
    t1.className = tb1.className = 'MonitorPage';
    t1.style.border = tb1.style.border = 'none';
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.LHCb_HT_header());
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(cell=this.LHCb_HT_summary());

    // Right hand of the display
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(t1=document.createElement('table'));
    t1.appendChild(tb1=document.createElement('tbody'));
    t1.className = tb1.className = 'MonitorPage';
    t1.style.border = tb1.style.border = 'none';
    td.colSpan = 2;

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Magnet_summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Clock_summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.background_summary=lhcb.widgets.BackgroundSummary('MonitorDataHeader'));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Velo_summary());

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.cooling_summary = lhcb.widgets.CoolingSummary(null));

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

  return table;
};

var detstatus_unload = function()  {
  dataProviderReset();
  //alert('Connection reset.\n\nBye, Bye my friend....');
};


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
  body.style.cursor = 'default';
};

if ( _debugLoading ) alert('Script lhcb.display.detstatus.cpp loaded successfully');
