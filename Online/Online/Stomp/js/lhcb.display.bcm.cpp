_loadScript('lhcb.display.tooltips.cpp');
_loadScript('lhcb.display.constants.cpp');
_loadScript('lhcb.display.items.cpp');
_loadScript('dom.print.cpp');
_loadFile('lhcb.display.general','css');

var _body_onloaded = false;

if ( !lhcb.widgets ) {

  lhcb.widgets = new Object();

  /** Build table with BCM summary information
   *   
   * @return Reference to HTML table containing all data items of this widget
   *
   * @author  M.Frank
   * @version 1.0
   */
  lhcb.widgets.BCM_datapoints = function(typ) {
    var post = '';
    var bcms = new Array();
    var low = typ.toLowerCase();
    var bcm = StyledItem('lbWeb.BCM_DP_S0.RS2_SUM.'+typ, null, '%7.3f');
    if (typ == 'REL' ) post = ' [%]';
    bcm.nice_name = 'S0 RS2 ('+low+') '+post;
    bcm.client = null;
    bcms[0] = bcm;
    bcm = StyledItem('lbWeb.BCM_DP_S0.RS32_SUM.'+typ, null, '%7.3f');
    bcm.nice_name = 'S0 RS32 ('+low+') '+post;
    bcm.client = null;
    bcms[1] = bcm;
    bcm = StyledItem('lbWeb.BCM_DP_S1.RS2_SUM.'+typ, null, '%7.3f');
    bcm.nice_name = 'S1 RS2 ('+low+') '+post;
    bcm.client = null;
    bcms[2] = bcm;
    bcm = StyledItem('lbWeb.BCM_DP_S1.RS32_SUM.'+typ, null, '%7.3f');
    bcm.nice_name = 'S1 RS32 ('+low+') '+post;
    bcm.client = null;
    bcms[3] = bcm;

    bcms.subscribe = function(provider) {
      for(var i=0; i<this.length; ++i)
	this[i].subscribe(provider);
      return this;
    };
    return bcms;
  };

  lhcb.widgets.BCM_rel_datapoints = function() { return lhcb.widgets.BCM_datapoints('fluxrel'); };
  lhcb.widgets.BCM_sum_datapoints = function() { return lhcb.widgets.BCM_datapoints('flux');    };

  lhcb.widgets.BCM_station_datapoints = function(station,typ) {
    var sensors = new Array();
    sensors[0] = StyledItem('lbWeb.BCM_DP_'+station+'.'+typ+'.RS1.flux', null, '%7.3f');
    //sensors[1] = StyledItem('lbWeb.BCM_DP_'+station+'.'+typ+'.RS2.flux', null, '%7.3f');
    sensors[1] = StyledItem('lbWeb.BCM_DP_'+station+'.'+typ+'.RS32.flux', null, '%7.3f');
    sensors[2] = StyledItem('lbWeb.BCM_DP_'+station+'.'+typ+'.SUM.flux', null, '%7.3f');
    sensors[0].style.width='11%';
    sensors[1].style.width='11%';
    sensors[2].style.width='10%';
    sensors.subscribe = function(provider) {
      for(var i=0; i<this.length; ++i)
	this[i].subscribe(provider);
      return this;
    };
    return sensors;
  };

  /** Build table with BCM summary information
   *   
   * @return Reference to HTML table containing all data items of this widget
   *
   * @author  M.Frank
   * @version 1.0
   */
  lhcb.widgets.BCM_summary = function() {

    var c, tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className = tb.className  = 'MonitorPage';
    //tab.height    = '60px';
    tab.bcms       = lhcb.widgets.BCM_rel_datapoints();
    tab.bcms_sum   = lhcb.widgets.BCM_sum_datapoints();

    tab.bcm_ok  = StyledItem('lbWeb.BCM_Interface.BcmOK.getStatus', null, null);
    tab.bcm_ok.colSpan = 2;
    tab.bcm_fsm = StyledItem('lbWeb.BCM_Interface.FSMStatus', null, null);
    tab.bcm_fsm.colSpan = 2;

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Operational:',2,'MonitorDataHeader'));
    c.colSpan = 2;
    c.style.backgroundColor = '#FFAAAA';
    tr.appendChild(tab.bcm_ok);
    tr.appendChild(c=Cell('FSM state:',2,'MonitorDataHeader'));
    c.colSpan = 2;
    c.style.backgroundColor = '#FFAAAA';
    tr.appendChild(tab.bcm_fsm);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Relative and absolute sums of '+
			  'BCM sensor readings in % to beam dump threshold',
			  8,'MonitorDataHeader'));
    c.style.backgroundColor = '#FFAAAA';
    tb.appendChild(tr);

    tr = document.createElement('tr');
    for(var i=0; i<2; ++i) {
      for(var j=0; j<2; ++j) {
	tr.appendChild(c=Cell(tab.bcms[2*i+j].nice_name,null,'MonitorDataHeader'));
	c.style.width='18%';
	tr.appendChild(c=tab.bcms[2*i+j]);
	c.style.width='7%';
      }
    }
    tb.appendChild(tr);
    tr = document.createElement('tr');
    for(var i=0; i<2; ++i) {
      for(var j=0; j<2; ++j) {
	tr.appendChild(c=Cell(tab.bcms_sum[2*i+j].nice_name,null,'MonitorDataHeader'));
	c.style.width='18%';
	tr.appendChild(c=tab.bcms_sum[2*i+j]);
	c.style.width='7%';
      }
    }
    tb.appendChild(tr);
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      this.bcms.subscribe(provider);
      this.bcms_sum.subscribe(provider);
      this.bcm_ok.subscribe(provider);
      this.bcm_fsm.subscribe(provider);
      return this;
    };
    return tab;
  };

  /** Build table with BCM summary information
   *   
   * @return Reference to HTML table containing all data items of this widget
   *
   * @author  M.Frank
   * @version 1.0
   */
  lhcb.widgets.BCM_sensors = function() {
    var c, tb, td, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className = 'MonitorPage';
    tb.className  = 'MonitorPage';

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Flux readings of individual stations',8,'MonitorDataHeader'));
    c.style.backgroundColor = '#FFAAAA';
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Station/Sensor',null,'MonitorDataHeader'));
    c.style.width='18%';
    tr.appendChild(Cell('RS1',null,'MonitorDataHeader'));
    tr.appendChild(Cell('RS32',null,'MonitorDataHeader'));
    tr.appendChild(Cell('Sum',null,'MonitorDataHeader'));
    tr.appendChild(c=Cell('Station/Sensor',null,'MonitorDataHeader'));
    c.style.width='18%';
    tr.appendChild(Cell('RS1',null,'MonitorDataHeader'));
    tr.appendChild(Cell('RS32',null,'MonitorDataHeader'));
    tr.appendChild(Cell('Sum',null,'MonitorDataHeader'));
    tb.appendChild(tr);

    tab.sensors= new Array();
    var sensors = new Array('SENSOR_0','SENSOR_1','SENSOR_2','SENSOR_3','SENSOR_4','SENSOR_5','SENSOR_6','SENSOR_7');
    for(var i=0; i<sensors.length; ++i) {
      tr = document.createElement('tr');
      var s = lhcb.widgets.BCM_station_datapoints('S0',sensors[i]);
      tab.sensors[2*i] = s;
      tr.appendChild(c=Cell('S0:'+sensors[i],null,'MonitorDataHeader'));
      tr.appendChild(s[0]);
      tr.appendChild(s[1]);
      tr.appendChild(s[2]);
      s = lhcb.widgets.BCM_station_datapoints('S1',sensors[i]);
      tab.sensors[2*i+1] = s;
      tr.appendChild(c=Cell('S1:'+sensors[i],null,'MonitorDataHeader'));
      tr.appendChild(s[0]);
      tr.appendChild(s[1]);
      tr.appendChild(s[2]);
      tb.appendChild(tr);
    }
    tab.appendChild(tb);

    tab.subscribe = function(provider) {
      for( var i=0; i<this.sensors.length;++i)
      this.sensors[i].subscribe(provider);
      return this;
    };
    return tab;
  };

  lhcb.widgets.BCM_rel_charts = function() {
    var fr, tb, td, tr, tab = document.createElement('table');
    var j;
    tb = document.createElement('tbody');

    tab.className         = 'MonitorPage';
    tb.className          = 'MonitorPage';
    tab.bcms              = lhcb.widgets.BCM_rel_datapoints();

    tab.handle_data = function(data) {
      //var c = this.c_chart;
      var d = this.c_data;
      var date = new Date();
      var tim  = date.getTime()-date.getTimezoneOffset()*60*1000;
      date.setTime(tim);
      if ( d.length>100 ) d.splice(0,d.length-100);
      d.push([date.getTime(),data]);
      this.c_series.getDataHandler().setArray(d);
      this.c_series.reload();
      return data;
    };
    tr = document.createElement('tr');
    for(j=0; j<2; ++j) {
      td = document.createElement('td');
      fr = document.createElement('iframe');
      fr.src = lhcb.constants.lhcb_comet_url()+'/lhcb.chart.htm';
      fr.id           = 'frame_chart_'+j;
      fr.bgcolor      = '#CCCCCC';
      fr.style.width  = '430px';
      fr.style.height = '430px';
      fr.scrolling    = 'auto';
      fr.marginWidth  = 1;
      fr.marginHeight = 1;
      td.appendChild(fr);
      
      td.style.width  = '430px';
      td.style.height = '430px';
      td.style.border = 'none';
      td.className    = 'MonitorData';
      tr.appendChild(td);
      tab[fr.id] = fr;
      tab.bcms[2*j].conversion = tab.handle_data;
      tab.bcms[2*j+1].conversion = tab.handle_data;
    }
    tb.appendChild(tr);
    tab.appendChild(tb);
    return tab;
  }
};

/**
 */
var BcmStatus = function(msg,chart,sensors)   {
  var tr, td, tb, tab;
  table           = document.createElement('table');
  table.body      = document.createElement('tbody');
  table.messages  = msg;
  table.provider  = null;
  table.logger    = null;
  table.charts    = chart;
  table.sensors   = sensors;

  table.add = function() {
    tr = document.createElement('tr');
    td = document.createElement('td');
    td.setAttribute('colSpan',2);
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  };

  table.className = table.body.className = 'MonitorOuterFrame';
  table.body.cellpadding = 0;
  table.body.cellspacing = 0;
  table.display    = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  /**
   */
  table.start_charts = function() {
    var c = this.onload.client;
    var bcms = c.bcm_charts.bcms;

    //alert('All elements are loaded:'+c);
    for(var j = 0; j < 2; ++j )    {
      var id = 'frame_chart_'+j;
      var fr = document.getElementById(id);
      var d = fr.contentWindow.document;
      var chart = d.getElementById("chart");
      var attrs = 	{ 
	  axis_left: 
	  {
	    color: "#000", 
	    caption: "",
	    min_extreme: 0.0,
	    max_extreme: null,
	    //grid: {color: "#000", show:true },
	    crosshair: {show: true, color: "#0F0" }
	  },
	  axis_bottom: 
	  {
	    color: "#000",
	    caption: "Time of the day",
	    //grid: {color: "#000", show: true },
	    crosshair: {show: true, color: "#0F0" },
	    formatter: chart.dateFormatter({format_string:"HH:NN:SS"})
	  },
	  background:  { color: "#F00", opacity:0, includeTitle: true },
	  auto_resize: false,
	  show_legend: true,
	  legend_state: 'minimized',
	  show_messages: false
      };
      var data, handler, series, ch = chart.createChart(chart.id,attrs);

      chart.style.width='400px';
      chart.style.height='400px';
      chart.chart = ch;
      ch.setTitle('Threshold to beam dump of S'+j+' RS2/RS32  [%]');

      series = chart.createLineSeries(chart.createArrayDataHandler(data=new Array()),
	  {title: bcms[2*j].nice_name,
	   color: "#F00"
	  });
      ch.addSeries(series);
      series.reload();
      bcms[2*j].c_chart  = chart;
      bcms[2*j].c_data   = data;
      bcms[2*j].c_series = series;

      series  = chart.createLineSeries(chart.createArrayDataHandler(data=new Array()),
	  {title: bcms[2*j+1].nice_name,
	   color: "#00F"
	  });
      ch.addSeries(series);
      series.reload();
      bcms[2*j+1].c_chart  = chart;
      bcms[2*j+1].c_data   = data;
      bcms[2*j+1].c_series = series;
    }
    if ( c.bcm_charts  ) bcms.subscribe(c.provider);
    if ( c.bcm_sensors ) c.bcm_sensors.subscribe(c.provider);
    c.bcm_summary.subscribe(c.provider);
    return this;
  };

  table.subscribe = function() {
    var c = this.onload.client;
    if ( c.bcm_charts  ) c.bcm_charts.bcms.subscribe(c.provider);
    if ( c.bcm_sensors ) c.bcm_sensors.subscribe(c.provider);
    c.bcm_summary.subscribe(c.provider);
    return this;
  };

  table.build = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var tr, td, c, d = new Date();

    tab.className = tb.className = 'MonitorInnerFrame';

    this.heading = document.createElement('tr');
    var cell = Cell(lhcb_online_picture()+'&nbsp;LHCb BCM Status',2,'MonitorBigHeader');
    cell.style.textAlign = 'left';
    cell.style.width = '360px';
    this.heading.appendChild(cell);
    this.head_date = Cell(d.toString(),1,'MonitorTinyHeader');
    this.head_date.textAlign = 'right';
    this.head_date.id = 'current_time';
    this.head_date.style.width = '360px';
    this.heading.appendChild(this.head_date);
    this.heading.onclick = function() { document.location = "http://lhcb.cern.ch";};
    tooltips.set(this.heading,'LHCb bcm summary<br>Click to go to LHCb home page');
    tb.appendChild(this.heading);

    this.timerHandler = function() {document.getElementById('current_time').innerHTML = (new Date()).toString(); };
    setInterval(this.timerHandler,2000);

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('',3,null));
    tb.appendChild(tr);
    c.style.height='20px';

    tr = document.createElement('tr');
    tr.appendChild(this.variables=Cell('',3,null));
    this.bcm_summary = lhcb.widgets.BCM_summary();
    this.variables.appendChild(this.bcm_summary);
    tb.appendChild(tr);

    if ( this.sensors ) {
      tr = document.createElement('tr');
      tr.appendChild(this.variables=Cell('',3,null));
      this.bcm_sensors = lhcb.widgets.BCM_sensors();
      this.variables.appendChild(this.bcm_sensors);
      tb.appendChild(tr);
    }

    if ( this.charts ) {
      tr = document.createElement('tr');
      tr.appendChild(c=Cell('',3,null));
      tb.appendChild(tr);
      c.style.height='20px';
      
      tr = document.createElement('tr');
      tr.appendChild(this.charts=Cell('',3,null));
      this.bcm_charts = lhcb.widgets.BCM_rel_charts();
      this.charts.appendChild(this.bcm_charts);
      tb.appendChild(tr);
    }

    // Finally add suggestions text
    tr = document.createElement('tr');
    if ( this.charts ) {
      tr.appendChild(c=Cell('Switch to fixed Y-scale',null,'DisplayButton'));
      c.id = 'scale_floating_toggle';
      c.checked = 1;
      c.onclick = toggle_floating_scale;
    }
    else {
      tr.appendChild(c=Cell('',null,null));
    }
    c.style.width = '20%';
    tr.appendChild(c=Cell('',null,null));
    c.style.width='35%';
    tr.appendChild(c=Cell('Comments and suggestions to M.Frank CERN/LHCb',1,'MonitorTinyHeader'));
    
    tb.appendChild(tr);
    tab.appendChild(tb);
    this.display.appendChild(tab);
    return this;
  };
  return table;
};

var toggle_floating_scale = function() {
  var obj  = document.getElementById('scale_floating_toggle');
  var body = document.getElementsByTagName('body')[0];
  var bcms = body.onload.client.bcm_charts.bcms;
  var v = obj.checked;
  for(var i=0; i<bcms.length; i+= 2) {
    bcms[i].c_chart.chart.axis_left.setExtremes(0.,v ? 115. : null);
    bcms[i].c_series.reload();
    //bcms[i].c_chart.chart.axis_left.show();
  }
  obj.innerHTML = v ? 'Switch to floating Y-scale' : 'Switch to fixed Y-scale';
  obj.checked = v ? 0 : 1;
};

var bcm_unload = function()  {
  dataProviderReset();
};

var bcm_body = function()  {
  var msg  = the_displayObject['messages'];
  var chart= the_displayObject['charts'];
  var sensors= the_displayObject['sensors'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector;

  selector = BcmStatus(msg,chart,sensors);
  body.appendChild(selector);
  body.onload = function() {};
  body.onload1 = chart ? selector.start_charts : selector.subscribe;
  body.onload.client = selector;
  body.className = 'MainBody';
  setWindowTitle('LHCb BCM Status');
  if ( msg > 0 )
    selector.logger   = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'StatusLogger');
  else
    selector.logger   = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'StatusLogger');
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  selector.build();
  selector.provider.start();
  if ( _isInternetExplorer() )  {
    body.onload = body.onload1;
    body.onload1.client = selector;
  }
  else {
    setTimeout(function(){document.getElementsByTagName('body')[0].onload1(); },4000);
  }
  body.style.cursor = 'default';
};
if ( _debugLoading ) alert('Script lhcb.display.detstatus.cpp loaded successfully');
