_loadScript('lhcb.display.tooltips.cpp');
_loadScript('lhcb.display.items.cpp');
//_loadStatic('charts/EJSChart.js');
_loadFile('lhcb.display.general','css');



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


var BlmStatus = function(msg)   {
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

  table.className      = 'MonitorPage';
  table.body.className = 'MonitorPage';
  table.body.cellpadding = 0;
  table.body.cellspacing = 0;
  table.summary  = table.add();
  table.charts   = table.add();

  table.display  = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  var div = document.createElement('div');
  div.id = 'myChart';
  //div.className = 'CanvasWhite';
  div.style.backgroundColor = '#EEEEEE';
  div.style.borderWidth = 1;
  div.style.borderStyle = 'solid';
  div.style.width = '950px';
  div.style.height = '530px';
  div.style.overflow = 'hidden';
  //div.className    = 'StatusLogger';
  table.charts.appendChild(div);


  table.add_point = function(t,s,a,p) {
    if ( a.length > 100 ) {
      a.splice(0,a.length-100);
    }
    a.push(p);
    s.getDataHandler().setArray(a);
  }
  table.handle_data = function(data) {
    var t = this.table;
    var x = (new Date()).getTime();

    t.add_point(t,t.series,t.arr_data,[x,data]);
    //t.add_point(t.series_low,t.arr_low,[x,14.9]);
    t.add_point(t,t.series_high,t.arr_high,[x,data+Math.random()*5e9]);

    //if ( (x - t.last) < 2000 ) return;
    /*
    if ( t.arr_data.length > 2 ) {
      var d = Math.floor((x - t.arr_data[0][0])*0.1);
      t.charts.axis_bottom.setExtremes(t.arr_data[0][0],x+d);
    }
    */
    t.last = x;
    //t.series.getDataHandler().setArray(t.arr_data);
    //t.series_high.getDataHandler().setArray(t.arr_high);
    t.series.reload();
    t.series_high.reload();
    return data;
  }

  table.add_chart = function() {
    this.chart=new EJSC.Chart("myChart",
	{ 
	  axis_left: 
	  {
	    color: "#0F0", 
	    grid: {color: "#000" }
	  },
	  axis_bottom: 
	  {
	    color: "#F00",
	    grid: {color: "#000" },
	    formatter: new EJSC.DateFormatter({format_string:"HH:NN:SS"})
	  }
	}
			      );

    this.last = 0;
    this.arr_low = new Array();
    this.arr_high = new Array();
    this.arr_data = new Array();
    this.array_handler = new EJSC.ArrayDataHandler(this.arr_data);
    /*
    this.array_handler_low = new EJSC.ArrayDataHandler(this.arr_low);
    this.series_low = this.chart.addSeries(new EJSC.AreaSeries(this.array_handler_low));
    this.series_low.setLineWidth(2);
    this.series_low.setColor('#FF0000');
    */    
    this.array_handler_high = new EJSC.ArrayDataHandler(this.arr_high);
    this.series_high = this.chart.addSeries(new EJSC.LineSeries(this.array_handler_high,{lineWidth:2, 
										    title:"Upper limit", 
										    color:'#FF0000'}));
    this.series_high.setColor('#FF0000');

    this.chart.axis_left.setCaption('Temperature [&#186;C]');
    this.chart.axis_left.showGrid(true);
    //chart.axis_left.setExtremes(14.6,15.4);
    this.chart.axis_bottom.setCaption('Time of day');
    this.chart.axis_bottom.showGrid(true);
    this.chart.background.color = '#AAA';
    this.chart.setTitle('Temperature of Sensor 2');
    this.chart.setShowLegend(true);
    this.chart.setAutoResize(true);
    this.series = this.chart.addSeries(new EJSC.LineSeries(this.array_handler,{lineWidth:4,
									  title:'Sensor 2',
                                                                          color:'#666666',
									  drawPoints:true,
									  pointSize:5,
                                                                          pointColor:'#000000'}));
    //this.series.setLineWidth(4);
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
    //tab.style.borderWidth = '10px';

    tr = document.createElement('tr');
    tb.appendChild(tr);
    td = document.createElement('td');
    td.innerHTML = 'ST Cooling data';
    td.colSpan = 2;
    tr.appendChild(td);

    tr = document.createElement('tr');
    this.manifold_temp01 = StyledItem('lbWeb.CaV/ItPlant.Actual.Measurements.param01',null,'Temperature 1: %7.2f &#186;C');
    tr.appendChild(this.manifold_temp01);
    this.manifold_temp02 = StyledItem('lbWeb.LHCCOM/LHC.LHC.Beam.Intensity.Beam1.totalIntensity',null,'%9.2e');
    this.manifold_temp02.conversion = this.handle_data;
    this.manifold_temp02.table = this;
    tr.appendChild(this.manifold_temp02);
    tb.appendChild(tr);
    tab.appendChild(tb);

    return tab;
  }

  table.subscribeItem = function(item) {
    this.provider.subscribe(item.name,item);
  }
  table.subscribe = function() {
    this.subscribeItem(this.manifold_temp01);
    this.subscribeItem(this.manifold_temp02);
  }


  table.build = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var t1, tb1, tr1, td1, d = new Date();

    tab.width ='100%';
    tb.width  = '100%';

    tr = document.createElement('tr');

    // Right hand of the display
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(t1=document.createElement('table'));
    td.style.width = '60%';
    td.colSpan = 2;
    t1.appendChild(tb1=document.createElement('tbody'));

    tb1.appendChild(tr1=document.createElement('tr'));
    tr1.appendChild(td1=document.createElement('td'));
    td1.appendChild(this.Cooling_summary());

    tb.appendChild(tr);

    // Finally add suggestions text
    tr = document.createElement('tr');
    tr.appendChild(Cell('',null,null));
    tr.appendChild(Cell('Comments and suggestions to J.Luisier CERN/LHCb',2,'MonitorTinyHeader'));
    tb.appendChild(tr);

    tab.appendChild(tb);
    this.display.appendChild(tab);
    this.add_chart();
    return this;
  }

  return table;
}

var blm_unload = function()  {
  dataProviderReset();
  //alert('Connection reset.\n\nBye, Bye my friend....');
}

var blm_body = function()  {
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector;

  selector = BlmStatus(msg);
  body.appendChild(selector);
  setWindowTitle('LHCb Detector High Voltage Status');
  if ( msg > 0 )
    selector.logger   = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'StatusLogger');
  else
    selector.logger   = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'StatusLogger');
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  selector.build();
  selector.subscribe();
  selector.provider.start();
}

if ( _debugLoading ) alert('Script lhcb.display.detstatus.cpp loaded successfully');
