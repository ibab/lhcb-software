_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadFile('lhcb.display.general','css');


var MagnetStatus = function(msg)   {
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
  table.body.cellpadding = 0;
  table.body.cellspacing = 0;
  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  table.Magnet_summary = function() {
    var c, tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');

    tab.className = tb.className = 'MonitorPage';
    tooltips.set(tab,'Magnet status summary information');

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('<font size="+3">Status</font>',null,'MonitorDataHeaderRED'));
    c.style.width='20%';
    c.style.height='50px';
    this.magnetField = StyledItem('lbWeb.lbHyst.B','Text-Center',null);
    this.magnetField.style.width = '16%';
    this.magnetField.style.border = 'solid';
    this.magnetField.style.borderColor = '#000000';
    this.magnetField.style.borderWidth = 1;
    this.magnetField.conversion = function(data) {
      var d = Math.abs(data);
      if ( d>0.75 ) {
	this.style.backgroundColor = 'lightgreen';
	return '<font size="+3">ON</font>';
      }
      else if ( d<0.1 ) {
	this.style.backgroundColor = 'yellow';
	return '<font size="+2">OFF</font>';
      }
      this.style.backgroundColor = 'orange';
      return '<font size="+2">RAMP</font>';
    };
    tr.appendChild(this.magnetField);
    tr.appendChild(c=Cell('<font size="+2">Polarity:</font>',null,'Text-Center'));
    c.style.width='15%';
    this.magnetPolarity = StyledItem('lbWeb.LbMagnet.Polarity','Text-Center',null);
    this.magnetPolarity.conversion = function(data) {
      var x = document.getElementById('polarity_arrow');
      if ( data>0 ) {
	x.src = _fileBase+'/Images/Down.png';
	return '<font size="+3">+</font>';
      }
      x.src = _fileBase+'/Images/Up.png';
      return '<font size="+3">-</font>';
    };
    this.magnetPolarity.style.width='5%';
    this.magnetPolarity.style.textAlign='left';
    tr.appendChild(this.magnetPolarity);
    tr.appendChild(c=Cell('<img id="polarity_arrow" src="'+lhcb.constants.images.magnet_big.src+'" height="90%" width="80"/>',null,'Text-Left'));
    c.style.width = '10%';
    this.magnetCurrent = StyledItem('lbWeb.LbMagnet.Current',null,'Current: %7.0f A');
    this.magnetCurrent.style.width='20%';
    tr.appendChild(this.magnetCurrent);
    this.magnetCurrentSet = StyledItem('lbWeb.LbMagnet.SetCurrent',null,'Set: %7.0f A');
    this.magnetCurrentSet.style.width='20%';
    tr.appendChild(this.magnetCurrentSet);
    tb.appendChild(tr);
    tab.appendChild(tb);
    return tab;
  };

  table.Magnet_sensors = function() {
    var c, tb, tr, tab = document.createElement('table');
    tb = document.createElement('tbody');
    tab.className = tb.className = 'MonitorPage';
    tooltips.set(tab,'Sensor status and values');

    this.magnetTemp0   = StyledItem('lbWeb.LbMagnet.BSensor0.Temp','Text-Center','%7.2f &#186;C');
    this.magnetField0  = StyledItem('lbWeb.LbMagnet.BSensor0.Babs','Text-Center','%7.4f');
    this.magnetFieldX0 = StyledItem('lbWeb.LbMagnet.BSensor0.Bx',  'Text-Center','%7.4f');
    this.magnetFieldY0 = StyledItem('lbWeb.LbMagnet.BSensor0.By',  'Text-Center','%7.4f');
    this.magnetFieldZ0 = StyledItem('lbWeb.LbMagnet.BSensor0.Bz',  'Text-Center','%7.4f');

    this.magnetTemp1   = StyledItem('lbWeb.LbMagnet.BSensor1.Temp','Text-Center','%7.2f &#186;C');
    this.magnetField1  = StyledItem('lbWeb.LbMagnet.BSensor1.Babs','Text-Center','%7.4f');
    this.magnetFieldX1 = StyledItem('lbWeb.LbMagnet.BSensor1.Bx',  'Text-Center','%7.4f');
    this.magnetFieldY1 = StyledItem('lbWeb.LbMagnet.BSensor1.By',  'Text-Center','%7.4f');
    this.magnetFieldZ1 = StyledItem('lbWeb.LbMagnet.BSensor1.Bz',  'Text-Center','%7.4f');

    this.magnetTemp2   = StyledItem('lbWeb.LbMagnet.BSensor2.Temp','Text-Center','%7.2f &#186;C');
    this.magnetField2  = StyledItem('lbWeb.LbMagnet.BSensor2.Babs','Text-Center','%7.4f');
    this.magnetFieldX2 = StyledItem('lbWeb.LbMagnet.BSensor2.Bx',  'Text-Center','%7.4f');
    this.magnetFieldY2 = StyledItem('lbWeb.LbMagnet.BSensor2.By',  'Text-Center','%7.4f');
    this.magnetFieldZ2 = StyledItem('lbWeb.LbMagnet.BSensor2.Bz',  'Text-Center','%7.4f');

    this.magnetTemp3   = StyledItem('lbWeb.LbMagnet.BSensor3.Temp','Text-Center','%7.2f &#186;C');
    this.magnetField3  = StyledItem('lbWeb.LbMagnet.BSensor3.Babs','Text-Center','%7.4f');
    this.magnetFieldX3 = StyledItem('lbWeb.LbMagnet.BSensor3.Bx',  'Text-Center','%7.4f');
    this.magnetFieldY3 = StyledItem('lbWeb.LbMagnet.BSensor3.By',  'Text-Center','%7.4f');
    this.magnetFieldZ3 = StyledItem('lbWeb.LbMagnet.BSensor3.Bz',  'Text-Center','%7.4f');

    tr = document.createElement('tr');
    tr.appendChild(c=Cell('Sensors',null,'MonitorDataHeaderRED'));
    c.style.width = '20%';

    tr.appendChild(c=Cell('',null,'MonitorDataHeader'));
    c.innerHTML = '<B>|B<sub>abs</sub>|</B> [&nbsp;T&nbsp;]';
    c.style.textAlign = 'center';
    c.style.width = '16%';
    tr.appendChild(c=Cell('B<sub>x</sub> [&nbsp;T&nbsp;]',null,'MonitorDataHeader'));
    c.style.textAlign = 'center';
    c.style.width = '16%';
    tr.appendChild(c=Cell('B<sub>y</sub> [&nbsp;T&nbsp;]',null,'MonitorDataHeader'));
    c.style.textAlign = 'center';
    c.style.width = '16%';
    tr.appendChild(c=Cell('B<sub>z</sub> [&nbsp;T&nbsp;]',null,'MonitorDataHeader'));
    c.style.textAlign = 'center';
    c.style.width = '16%';
    tr.appendChild(c=Cell('Temperature',null,'MonitorDataHeader'));
    c.style.textAlign = 'center';
    c.style.width = '16%';
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Sensor 0',null,'MonitorDataHeader'));
    tr.appendChild(this.magnetField0);
    tr.appendChild(this.magnetFieldY0);
    tr.appendChild(this.magnetFieldZ0);
    tr.appendChild(this.magnetFieldX0);
    tr.appendChild(this.magnetTemp0);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Sensor 1',null,'MonitorDataHeader'));
    tr.appendChild(this.magnetField1);
    tr.appendChild(this.magnetFieldY1);
    tr.appendChild(this.magnetFieldZ1);
    tr.appendChild(this.magnetFieldX1);
    tr.appendChild(this.magnetTemp1);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Sensor 2',null,'MonitorDataHeader'));
    tr.appendChild(this.magnetField2);
    tr.appendChild(this.magnetFieldY2);
    tr.appendChild(this.magnetFieldZ2);
    tr.appendChild(this.magnetFieldX2);
    tr.appendChild(this.magnetTemp2);
    tb.appendChild(tr);

    tr = document.createElement('tr');
    tr.appendChild(Cell('Sensor 3',null,'MonitorDataHeader'));
    tr.appendChild(this.magnetField3);
    tr.appendChild(this.magnetFieldY3);
    tr.appendChild(this.magnetFieldZ3);
    tr.appendChild(this.magnetFieldX3);
    tr.appendChild(this.magnetTemp3);
    tb.appendChild(tr);
    
    tab.appendChild(tb);
    return tab;
  };

  table.addPlot = function(row, dsc, url) {
    var c = Cell(dsc,null,'DisplayButton');
    row.appendChild(c);
    //c.style.width = '6%';
    c.img = this.img;
    c.url = url;
    c.onclick = function() { 
      this.img.currentPlot = this.url;
      this.img.setAttribute('src',this.img.currentPlot + '?' + (new Date()).getTime());  
    };
    return c;
  };

  table.hidePlots = function() {
    var x=document.getElementById('magnet_plot_table');
    x.parentNode.removeChild(x);
    x.table.showPlots();
  };

  table.showPlots = function() {
    var x = document.getElementById('magnet_plots');
    var c, tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var trh = document.createElement('tr');

    tab.style.width = tb.style.width = '100%';
    tab.id    = 'magnet_plot_table';
    tab.table = this;
    trh.appendChild(c=Cell('Show plots',null,'DisplayButton'));
    c.style.fontWeight = 'bold';
    c.style.border = 'solid';
    c.style.width = '15%';
    c.style.borderWidth = '1px';
    c.style.borderColor = '#000000';
    c.onclick = function() {
      var x = document.getElementById('magnet_plots');
      x.innerHTML = '';
      x.appendChild(x.table.Magnet_plots());
    };
    trh.appendChild(c=Cell('',null,null));
    tb.appendChild(trh);
    tab.appendChild(tb);
    x.appendChild(tab);
    return tab;
  };

  table.Magnet_plots = function() {
    var field, c, tab = document.createElement('table');
    var tb    = document.createElement('tbody');
    var trh   = document.createElement('tr');
    var trp   = document.createElement('tr');
    var plot  = document.createElement('td');
    var url   = '/Online/Images/magnet/';
    this.img  = document.createElement('img');

    tab.style.width = tb.style.width = '100%';
    tab.className = tb.className = 'MonitorPage';
    tab.id    = 'magnet_plot_table';
    tab.table = this;

    trh.appendChild(Cell('Probes:',null,'DisplayButton'));
    this.addPlot(trh,'Temp 0',  url+'probe0-temp.png');
    this.addPlot(trh,'Field 0', url+'probe0-field.png');
    this.addPlot(trh,'Temp 1',  url+'probe1-temp.png');
    this.addPlot(trh,'Field 1', url+'probe1-field.png');
    this.addPlot(trh,'Temp 2',  url+'probe2-temp.png');
    this.addPlot(trh,'Field 2', url+'probe2-field.png');
    this.addPlot(trh,'Temp 3',  url+'probe3-temp.png');
    this.addPlot(trh,'Field 3', url+'probe3-field.png');
    tb.appendChild(trh);

    trh = document.createElement('tr');
    trh.appendChild(Cell('Global:',null,'DisplayButton'));
    this.addPlot(trh,'Cooling',     url+'coolingWater.png');
    this.addPlot(trh,'Current',     url+'current.png');
    this.addPlot(trh,'Voltages',    url+'voltages.png');
    field=this.addPlot(trh,'Field', url+'average-field-small.png');
    trh.appendChild(c=Cell('',4,null));
    tb.appendChild(trh);

    this.img.setAttribute('id','magnet_plot_1');
    this.img.setAttribute('width','100%');
    plot.appendChild(this.img);
    plot.colSpan = 9;
    trp.appendChild(plot);

    tb.appendChild(trp);

    trh = document.createElement('tr');
    trh.appendChild(c=Cell('Hide plots',null,'DisplayButton'));
    c.onclick = this.hidePlots;
    c.table = this;
    trh.appendChild(c=Cell('',8,null));
    tb.appendChild(trh);


    tab.appendChild(tb);
    field.onclick();
    return tab;
  };


  table.subscribeItem = function(item) {
    this.provider.subscribe(item.name,item);
  };

  table.subscribe = function() {
    this.subscribeItem(this.magnetField);
    this.subscribeItem(this.magnetPolarity);
    this.subscribeItem(this.magnetCurrent);
    this.subscribeItem(this.magnetCurrentSet);
    this.subscribeItem(this.magnetField0);
    this.subscribeItem(this.magnetFieldX0);
    this.subscribeItem(this.magnetFieldY0);
    this.subscribeItem(this.magnetFieldZ0);
    this.subscribeItem(this.magnetField1);
    this.subscribeItem(this.magnetFieldX1);
    this.subscribeItem(this.magnetFieldY1);
    this.subscribeItem(this.magnetFieldZ1);
    this.subscribeItem(this.magnetField2);
    this.subscribeItem(this.magnetFieldX2);
    this.subscribeItem(this.magnetFieldY2);
    this.subscribeItem(this.magnetFieldZ2);
    this.subscribeItem(this.magnetField3);
    this.subscribeItem(this.magnetFieldX3);
    this.subscribeItem(this.magnetFieldY3);
    this.subscribeItem(this.magnetFieldZ3);
    this.subscribeItem(this.magnetTemp0);
    this.subscribeItem(this.magnetTemp1);
    this.subscribeItem(this.magnetTemp2);
    this.subscribeItem(this.magnetTemp3);
  };

  table.build = function() {
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');
    var t1, tb1, tr1, td1, d = new Date();

    tab.className = tb.className = 'MonitorInnerFrame';

    this.heading = document.createElement('tr');
    var cell = Cell('<IMG src="'+lhcb.constants.images.magnet_big.src+'" width="100"></IMG>&nbsp;LHCb Magnet Status',1,'MonitorBigHeader');
    cell.style.textAlign = 'left';
    cell.style.width = '360px';
    this.heading.appendChild(cell);
    this.head_date    = Cell(d.toString(),1,'MonitorTinyHeader');
    this.head_date.id = 'current_time';
    this.head_date.style.width = '360px';
    this.heading.appendChild(this.head_date);
    this.head_pic = Cell(lhcb_online_picture(),1,null);
    this.heading.appendChild(this.head_pic);
    this.heading.onclick = function() { document.location = lhcb.constants.urls.lhcb.home_page.src;};
    tooltips.set(this.heading,'LHCb magnet status page<br>Click to go to LHCb home page');
    tb.appendChild(this.heading);

    this.timerHandler = function() {document.getElementById('current_time').innerHTML = (new Date()).toString(); };
    setInterval(this.timerHandler,2000);

    tr = document.createElement('tr');
    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.Magnet_summary());
    td.colSpan = 3;
    tb.appendChild(tr);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(td=document.createElement('td'));
    td.appendChild(this.Magnet_sensors());
    td.colSpan = 3;
    tb.appendChild(tr);

    tb.appendChild(tr=document.createElement('tr'));
    tr.appendChild(td=document.createElement('td'));
    td.id = 'magnet_plots';
    td.table = this;
    td.colSpan = 3;
    tb.appendChild(tr);

    // Finally add suggestions text
    tr = document.createElement('tr');
    tr.appendChild(Cell('',null,null));
    tr.appendChild(Cell('Comments and suggestions to M.Frank CERN/LHCb',2,'MonitorTinyHeader'));
    tb.appendChild(tr);

    tab.appendChild(tb);
    this.display.appendChild(tab);
    this.showPlots();

    this.plot_timer = function() {
      var x = document.getElementById('magnet_plot_1');
      // alert('Update plot....'+x);
      if ( x ) {
	x.setAttribute('src',x.currentPlot+ '?' + (new Date()).getTime());
      }
    };
    setInterval(this.plot_timer,150000);
    return this;
  };

  return table;
};

var magnet_unload = function()  {
  dataProviderReset();
};

var magnet_body = function()  {
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);
  var selector;
  var disp_size = 'Screen availible dimensions:'+screen.availWidth+'x'+screen.availHeight+'\n'+
  'Screen dimensions:'+screen.width+'x'+screen.height+'\n';
  //alert(disp_size);

  selector = MagnetStatus(msg);
  body.appendChild(selector);
  body.className = 'MainBody';
  setWindowTitle('LHCb Magnet Status');
  if ( msg > 0 )
    selector.logger   = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'StatusLogger');
  else
    selector.logger   = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'StatusLogger');
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  selector.build();
  selector.subscribe();
  selector.provider.start();
  body.style.cursor = 'default';
};

if ( _debugLoading ) alert('Script lhcb.display.magnet.cpp loaded successfully');
