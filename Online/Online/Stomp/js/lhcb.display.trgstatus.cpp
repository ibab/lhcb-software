_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');

_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.fsm','css');


var LEDItem = function(item_name, class_name, conversion)  {
  element = document.createElement('td');
  element.name   = item_name;
  element.data   = '';
  if ( class_name ) {
    element.className = class_name;
  }
  if ( conversion ) {
    element.conversion = conversion;
    element.conversion.target = element;
  }

  /** Standard callback for data provider for updates
   *  @param data   Data value as string
   *
   *  @return On success reference to self, null otherwise
   */
  element.set = function(data) { return this.display(data);  };

  /** Display rendering function for received data
   *
   *  @param      data    Display data
   *
   *  @return On success reference to self, null otherwise
   */
  element.display = function(data)  {
    var item_data = parseInt(data[1]);
    if ( this.conversion != null )   {
      item_data = this.conversion(item_data);
    }
    if ( this.data != item_data )  
    {
      this.data = item_data;
      this.innerHTML = item_data;
    }
    return this;
  };
  /** Subscribe item to receive data from data provider object
   *
   *  @param      provider  Data provider object
   *
   *  @return On success reference to self, null otherwise
   */
  element.subscribe = function(provider) {
    provider.subscribe(this.name,this);
    return this;
  };
  return element;
};

var red_led    = null;
var green_led  = null;
var yellow_led = null;
var blue_led   = null;

/** TRGSTATUS web widgets
 *
 *  @author  M.Frank
 */
var TrgStatus = function(msg)   {
  lhcb.widgets.L0TriggerSummary = function(options) {
    var c, tb, td, tr, tab = document.createElement('table');
    red_led    = lhcb.widgets.red_smiley;
    green_led  = lhcb.widgets.green_smiley;
    yellow_led = lhcb.widgets.yellow_smiley;
    blue_led   = lhcb.widgets.blue_smiley;

    tb = document.createElement('tbody');  
    tab.className = tb.className  = 'MonitorPage';
    tab.logger    = options.logger;

    tab.led_conversion = function(data) {
      if ( data == 0 )      return green_led();
      else if ( data == 1 ) return yellow_led();
      else if ( data == 3 ) return blue_led();
      return red_led();
    };
    tab.led_conversion_yesno = function(data) {
      //this.style.backgroundColor = ( data == 1 ) ? 'green' : 'red';      return '';
      return (data==1 ? green_led() : red_led());
    };
    tab.led_conversion_noyes = function(data) {
      //this.style.backgroundColor = ( data != 1 ) ? 'green' : 'red';      return '';
      return ( data!=1 ) ? green_led() : red_led();
    };
    tab.led_conversion_temp = function(data) {
      //this.style.backgroundColor = ( data != 1 ) ? 'green' : 'red';      return 'Temp';
      return 'Temp:'+(data!=1 ? green_led() : red_led());
    };
    tab.led_conversion_l0du = function(data) {
      //this.style.backgroundColor = ( data != 1 ) ? 'green' : 'red';      return 'L0DU';
      return 'L0DU:'+(data!=1 ? green_led() : red_led());
    };
    tab.led_conversion_tell1 = function(data) {
      //this.style.backgroundColor = ( data != 1 ) ? 'green' : 'red';      return 'TELL1';
      return 'TELL1:'+(data!=1 ? green_led() : red_led());
    };

    tab.connect = function() {
      var o;
      for(var i=1; i<5; ++i) {
	var dp1 = 'lbWeb.Alarms_Q'+i;
	var dp2 = 'lbWeb.Monitor_Q'+i;
	o = new Object();
	o.ttcRxReady    = LEDItem(dp2+'.CB.TTCRxReady','Text-Center',this.led_conversion_yesno);
	o.l0Derandom    = LEDItem(dp1+'.CB.derandomChecks','Text-Center',this.led_conversion);
	o.QPLLReady     = LEDItem(dp2+'.CB.QPLLReady','Text-Center',this.led_conversion_yesno);
	o.TFCChecks     = LEDItem(dp1+'.CB.TFCChecks','Text-Center',this.led_conversion);

	o.bcidChecks    = LEDItem(dp1+'.OVERVIEW.BcidChecks','Text-Center',this.led_conversion);
	o.internalLinks = LEDItem(dp1+'.OVERVIEW.InternalLinks','Text-Center',this.led_conversion);
	o.opticalLinks  = LEDItem(dp1+'.OVERVIEW.OpticalLinks','Text-Center',this.led_conversion);
	o.injectionInUse= LEDItem(dp2+'.OVERVIEW.InjectionInUse','Text-Center',this.led_conversion_noyes);
	o.timeAligned   = LEDItem(dp2+'.OVERVIEW.TimeAligned','Text-Center',this.led_conversion_yesno);
	o.FPGATriggering= LEDItem(dp2+'.OVERVIEW.FPGATriggering','Text-Center',this.led_conversion_yesno);
	o.LUTsLoaded    = LEDItem(dp2+'.OVERVIEW.LUTsLoaded','Text-Center',this.led_conversion_yesno);
	o.ccserv        = LEDItem(dp1+'.OVERVIEW.ccserv','Text-Center',this.led_conversion);
	o.fsm           = FSMItem('lbWeb.TMU_Q'+i,this.logger,true);
	o.fsm.style.width     = '80px';
	this['Q'+i] = o;
      }
      o = new Object();
      o.ccserv          = LEDItem('lbWeb.GlobalL0CaloStatus.CCSERVstatus','Text-Center',this.led_conversion);
      o.opticalLinks    = LEDItem('lbWeb.GlobalL0CaloStatus.OpticalLinks','Text-Center',this.led_conversion);
      o.timeAligned     = LEDItem('lbWeb.GlobalL0CaloStatus.Aligned','Text-Center',this.led_conversion);
      o.ttcRxReady      = LEDItem('lbWeb.GlobalL0CaloStatus.TTCRXready','Text-Center',this.led_conversion);
      o.QPLLReady       = LEDItem('lbWeb.GlobalL0CaloStatus.QPLLlocked','Text-Center',this.led_conversion);
      o.envConditions   = LEDItem('lbWeb.GlobalL0CaloStatus.EnvConditions','Text-Center',this.led_conversion);
      o.fsm             = FSMItem('lbWeb.TCALO_DAQ',this.logger,true);
      o.fsm.style.width     = '80px';
      this['Calo'] = o;

      o = new Object();
      o.temp            = LEDItem('lbWeb.Alarms_L0DU_1.Temp_Summary','Text-Center',this.led_conversion_temp);
      o.tell1           = LEDItem('lbWeb.Alarms_L0DU_1.TELL1_Summary','Text-Center',this.led_conversion_tell1);
      o.l0du            = LEDItem('lbWeb.Alarms_L0DU_1.L0DU_Summary','Text-Center',this.led_conversion_l0du);
      o.fsm             = FSMItem('lbWeb.L0DU_CU',this.logger,true);
      o.fsm.style.width     = '80px';
      this['L0DU'] = o;

      o = new Object();
      o.fsm             = FSMItem('lbWeb.TPU_DAQ',this.logger,true);
      o.fsm.style.width     = '80px';
      this['TPU'] = o;
    };

    tab.connect();

    tr = document.createElement('tr');
    tr.appendChild(Cell('L&Oslash; Trigger Component',1,'MonitorBigHeaderBLUE'));
    tr.appendChild(Cell('L&Oslash;&mu;Q1',1,'MonitorBigHeaderBLUE'));
    tr.appendChild(Cell('L&Oslash;&mu;Q2',1,'MonitorBigHeaderBLUE'));
    tr.appendChild(Cell('L&Oslash;&mu;Q3',1,'MonitorBigHeaderBLUE'));
    tr.appendChild(Cell('L&Oslash;&mu;Q4',1,'MonitorBigHeaderBLUE'));
    tr.appendChild(Cell('L&Oslash;Calo',1,'MonitorBigHeaderBLUE'));
    tr.appendChild(Cell('L&Oslash;DU',1,'MonitorBigHeaderBLUE'));
    tr.appendChild(Cell('TPU',1,'MonitorBigHeaderBLUE'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('FSM state',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.fsm);
    tr.appendChild(tab.Q2.fsm);
    tr.appendChild(tab.Q3.fsm);
    tr.appendChild(tab.Q4.fsm);
    tr.appendChild(tab.Calo.fsm);
    tr.appendChild(tab.L0DU.fsm);
    tr.appendChild(tab.TPU.fsm);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('TTC',8,'Arial10pt'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('TTCRx ready',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.ttcRxReady);
    tr.appendChild(tab.Q2.ttcRxReady);
    tr.appendChild(tab.Q3.ttcRxReady);
    tr.appendChild(tab.Q4.ttcRxReady);
    tr.appendChild(tab.Calo.ttcRxReady);
    tr.appendChild(tab.L0DU.l0du);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('QPLL locked without error',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.QPLLReady);
    tr.appendChild(tab.Q2.QPLLReady);
    tr.appendChild(tab.Q3.QPLLReady);
    tr.appendChild(tab.Q4.QPLLReady);
    tr.appendChild(tab.Calo.QPLLReady);
    tr.appendChild(tab.L0DU.l0du);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('TFC Orbit signal',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.TFCChecks);
    tr.appendChild(tab.Q2.TFCChecks);
    tr.appendChild(tab.Q3.TFCChecks);
    tr.appendChild(tab.Q4.TFCChecks);
    tr.appendChild(c=Cell('n.a.',1,'Text-Center'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('ECS',8,'Arial10pt'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('CCServ status',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.ccserv);
    tr.appendChild(tab.Q2.ccserv);
    tr.appendChild(tab.Q3.ccserv);
    tr.appendChild(tab.Q4.ccserv);
    tr.appendChild(tab.Calo.ccserv);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Processor',8,'Arial10pt'));
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('LUTS loaded',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.LUTsLoaded);
    tr.appendChild(tab.Q2.LUTsLoaded);
    tr.appendChild(tab.Q3.LUTsLoaded);
    tr.appendChild(tab.Q4.LUTsLoaded);
    tr.appendChild(Cell('n.a.',1,'Text-Center'));
    tr.appendChild(tab.L0DU.tell1);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Aligned',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.timeAligned);
    tr.appendChild(tab.Q2.timeAligned);
    tr.appendChild(tab.Q3.timeAligned);
    tr.appendChild(tab.Q4.timeAligned);
    tr.appendChild(tab.Calo.timeAligned);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('FPGA Triggering',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.FPGATriggering);
    tr.appendChild(tab.Q2.FPGATriggering);
    tr.appendChild(tab.Q3.FPGATriggering);
    tr.appendChild(tab.Q4.FPGATriggering);
    tr.appendChild(Cell('n.a.',1,'Text-Center'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Optical Links',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.opticalLinks);
    tr.appendChild(tab.Q2.opticalLinks);
    tr.appendChild(tab.Q3.opticalLinks);
    tr.appendChild(tab.Q4.opticalLinks);
    tr.appendChild(tab.Calo.opticalLinks);
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Internal Links',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.internalLinks);
    tr.appendChild(tab.Q2.internalLinks);
    tr.appendChild(tab.Q3.internalLinks);
    tr.appendChild(tab.Q4.internalLinks);
    tr.appendChild(Cell('n.a.',1,'Text-Center'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('L0 Derandomizer',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.l0Derandom);
    tr.appendChild(tab.Q2.l0Derandom);
    tr.appendChild(tab.Q3.l0Derandom);
    tr.appendChild(tab.Q4.l0Derandom);
    tr.appendChild(Cell('n.a.',1,'Text-Center'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('BCID Checks',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.bcidChecks);
    tr.appendChild(tab.Q2.bcidChecks);
    tr.appendChild(tab.Q3.bcidChecks);
    tr.appendChild(tab.Q4.bcidChecks);
    tr.appendChild(Cell('n.a.',1,'Text-Center'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Inactive injection buffers',1,'MonitorDataHeader'));
    tr.appendChild(tab.Q1.injectionInUse);
    tr.appendChild(tab.Q2.injectionInUse);
    tr.appendChild(tab.Q3.injectionInUse);
    tr.appendChild(tab.Q4.injectionInUse);
    tr.appendChild(Cell('n.a.',1,'Text-Center'));
    tb.appendChild(tr);
    tr = document.createElement('tr');
    tr.appendChild(Cell('Environmental conditions',1,'MonitorDataHeader'));
    tr.appendChild(Cell('n.a.',1,'Text-Center'));
    tr.appendChild(Cell('n.a.',1,'Text-Center'));
    tr.appendChild(Cell('n.a.',1,'Text-Center'));
    tr.appendChild(Cell('n.a.',1,'Text-Center'));
    tr.appendChild(tab.Calo.envConditions);
    tr.appendChild(tab.L0DU.temp);
    tb.appendChild(tr);

    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      var o;
      for(var i=1; i<5; ++i) {
	o = this['Q'+i];
	provider.subscribeItem(o.ttcRxReady);
	provider.subscribeItem(o.l0Derandom);
	provider.subscribeItem(o.QPLLReady);
	provider.subscribeItem(o.TFCChecks);

	provider.subscribeItem(o.bcidChecks);
	provider.subscribeItem(o.internalLinks);
	provider.subscribeItem(o.opticalLinks);
	provider.subscribeItem(o.injectionInUse);
	provider.subscribeItem(o.timeAligned);
	provider.subscribeItem(o.FPGATriggering);
	provider.subscribeItem(o.LUTsLoaded);
	provider.subscribeItem(o.ccserv);
	provider.subscribeItem(o.fsm);
      }
      o = this['Calo'];
      provider.subscribeItem(o.fsm);
      provider.subscribeItem(o.ccserv);
      provider.subscribeItem(o.opticalLinks);
      provider.subscribeItem(o.timeAligned);
      provider.subscribeItem(o.ttcRxReady);
      provider.subscribeItem(o.QPLLReady);
      provider.subscribeItem(o.envConditions);
      o = this['L0DU'];
      provider.subscribeItem(o.fsm);
      provider.subscribeItem(o.temp);
      provider.subscribeItem(o.tell1);
      provider.subscribeItem(o.l0du);
      o = this['TPU'];
      provider.subscribeItem(o.fsm);

    };
    return tab;
  };

  var Legend = function() {
    var c, tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');  
    tab.className = tb.className  = 'MonitorPage';
    tb.appendChild(tr = document.createElement('tr'));
    tr.appendChild(Cell('Legend: ',1,'MonitorDataHeader'));
    tr.appendChild(Cell(green_led()+':  Status OK',1,null));
    tr.appendChild(Cell(yellow_led()+':  Warning',1,null));
    tr.appendChild(Cell(red_led()+':  ERROR',1,null));
    tr.appendChild(Cell(blue_led()+':  Status UNKNOWN',1,null));

    tab.appendChild(tb);
    return tab;
  };
  
  var table = lhcb.widgets.SubdetectorPage('L0 Trigger status');

  table.options = {logo:    null,
        	   logo_url:'http://lhcb-public.web.cern.ch/lhcb-public/Welcome.html',
		   title:   'L0 Trigger Status',
		   tips:    'L0 Trigger operations status:'
  };

  table.beforeComment = function() {
    var td, tr, opts = {style:'Arial12pt',legend:true,logger:this.logger};
    var leds = lhcb.widgets.L0TriggerSummary(opts);
    
    tr = document.createElement('tr');
    td = document.createElement('td');
    this.tbody.appendChild(tr);
    td.appendChild(leds);
    td.colSpan = 2;
    tr.appendChild(td);

    tr = document.createElement('tr');
    td = document.createElement('td');
    this.tbody.appendChild(tr);
    td.appendChild(Legend());
    td.colSpan = 2;
    tr.appendChild(td);

    this.subscriptions.push(leds);
  };

  return table;
};

var trgstatus_unload = function() { lhcb.widgets.SubdetectorPage.stop();  };
var trgstatus_body   = function()   { return lhcb.widgets.SubdetectorPage.start(TrgStatus); };
