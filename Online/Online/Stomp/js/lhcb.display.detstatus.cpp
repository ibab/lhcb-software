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

  table.className = table.body.className = 'MonitorOuterFrame';
  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  lhcb.widgets.Magnet_ProbeStatus = function() {
    var c, tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className = tb.className = 'MonitorPage';
    tooltips.set(tab,'Brief overview information<br>of the LHCb magnet status<br>Click to see magnet page.');    
    tab.onclick = function() { document.location = lhcb.constants.lhcb_display_url("magnet");};

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Magnet Status:',null,'MonitorDataHeader'));
    c.style.width='30%';
    tr.appendChild(tab.lastMagnetReading=Cell('',null,null));
    tab.lastMagnetReading.style.width='30%';
    tr.appendChild(c=Cell('Polarity:',null,'MonitorDataHeader'));
    c.style.width='20%';
    tab.magnetPolarity = StyledItem('lbWeb.LbMagnet.Polarity',null,null);
    tab.magnetPolarity.style.width='30%';
    tab.magnetPolarity.conversion = function(data) {
      if ( data>0 )
	return '+&nbsp;(Down)';
      return '-&nbsp;(Up)';
    };
    tr.appendChild(tab.magnetPolarity);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Current:',null,'MonitorDataHeader'));
    tab.magnetCurrent = StyledItem('lbWeb.LbMagnet.Current',null,'%7.0f A');
    tr.appendChild(tab.magnetCurrent);
    tab.magnetCurrent.magnetState = tab.lastMagnetReading;
    tab.magnetCurrent.conversion = function(data) {
      this.magnetState.innerHTML = Math.abs(parseFloat(data)) > 1e1 ? 'ON' : 'OFF';
      return data;
    };
    tr.appendChild(c=Cell('Set:',null,'MonitorDataHeader'));
    tab.magnetCurrentSet = StyledItem('lbWeb.LbMagnet.SetCurrent',null,'%7.0f A');
    tab.magnetCurrentSet.style.width='120px';
    tr.appendChild(tab.magnetCurrentSet);
    tb.appendChild(tr);

    tab.magnetField0 = StyledItem('lbWeb.LbMagnet.BSensor0.Babs',null,'%7.3f');
    tab.magnetField1 = StyledItem('lbWeb.LbMagnet.BSensor1.Babs',null,'%7.3f');
    tab.magnetField2 = StyledItem('lbWeb.LbMagnet.BSensor2.Babs',null,'%7.3f');
    tab.magnetField3 = StyledItem('lbWeb.LbMagnet.BSensor3.Babs',null,'%7.3f');
    tab.magnetTemp0  = StyledItem('lbWeb.LbMagnet.BSensor0.Temp',null,'%7.2f');
    tab.magnetTemp1  = StyledItem('lbWeb.LbMagnet.BSensor1.Temp',null,'%7.2f');
    tab.magnetTemp2  = StyledItem('lbWeb.LbMagnet.BSensor2.Temp',null,'%7.2f');
    tab.magnetTemp3  = StyledItem('lbWeb.LbMagnet.BSensor3.Temp',null,'%7.2f');

    tr = document.createElement('tr');
    tr.appendChild(Cell('Probe',null,'MonitorDataHeader'));
    tr.appendChild(Cell('-1-',null,null));
    tr.appendChild(Cell('-2-',null,null));
    tr.appendChild(Cell('-3-',null,null));
    tr.appendChild(Cell('-4-',null,null));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Field [&nbsp;T&nbsp;]',null,'MonitorDataHeader'));
    tr.appendChild(tab.magnetField0);
    tr.appendChild(tab.magnetField1);
    tr.appendChild(tab.magnetField2);
    tr.appendChild(tab.magnetField3);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Temperature [&#186;C]',null,'MonitorDataHeader'));
    tr.appendChild(tab.magnetTemp0);
    tr.appendChild(tab.magnetTemp1);
    tr.appendChild(tab.magnetTemp2);
    tr.appendChild(tab.magnetTemp3);
    tb.appendChild(tr);
    
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      provider.subscribeItem(this.magnetPolarity);
      provider.subscribeItem(this.magnetCurrent);
      provider.subscribeItem(this.magnetCurrentSet);
      provider.subscribeItem(this.magnetField0);
      provider.subscribeItem(this.magnetField1);
      provider.subscribeItem(this.magnetField2);
      provider.subscribeItem(this.magnetField3);
      provider.subscribeItem(this.magnetTemp0);
      provider.subscribeItem(this.magnetTemp1);
      provider.subscribeItem(this.magnetTemp2);
      provider.subscribeItem(this.magnetTemp3);
    };
    return tab;
  };

  table.subscribe = function() {
    this.voltages.subscribe(this.provider);
    this.magnet_summary.subscribe(this.provider);
    this.clock_summary.subscribe(this.provider);
    this.veloPosition.subscribe(this.provider);
    this.background_summary.subscribe(this.provider);
    this.cooling_summary.subscribe(this.provider);
    this.plan_of_day.subscribe(this.provider);
  };

  table.build = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var t1, tb1, tr1, td1, d = new Date();

    tab.className = tb.className = 'MonitorInnerFrame';
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
    this.voltages=lhcb.widgets.HVSummary(this.logger);
    this.voltages.buildTable({style:'Arial12pt',top:true,hv:true,hv_legend:true,lv:true,lv_legend:true,split:true});
    td.appendChild(this.voltages);

    td.style.width = '55%';

    // Right hand of the display
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(t1=document.createElement('table'));
    t1.appendChild(tb1=document.createElement('tbody'));
    t1.className = tb1.className = 'MonitorPage';
    t1.style.border = tb1.style.border = 'none';
    td.colSpan = 2;

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.magnet_summary=lhcb.widgets.Magnet_ProbeStatus());

    var opts = {style:'Arial12pt',legend:true,logger:this.logger};
    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.clock_summary=lhcb.widgets.ClockSummary({style:null,legend:true,logger:this.logger}));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.background_summary=lhcb.widgets.BackgroundSummary(opts));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.veloPosition=lhcb.widgets.velo.positionSummary(opts));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.cooling_summary=lhcb.widgets.CoolingSummary({style:null,legend:true,logger:this.logger}));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.plan_of_day=lhcb.widgets.LHCb_PlanOfDay({style:null,legend:true,logger:this.logger}));

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
