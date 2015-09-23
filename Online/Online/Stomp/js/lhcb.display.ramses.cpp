_loadScript('lhcb.display.items.cpp');
_loadScript('wz_jsgraphics.js');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.ramses','css');

var RamsesStatus = function(msg)   {
  table           = document.createElement('table');
  table.body      = document.createElement('tbody');
  table.items     = new Array();
  table.messages  = msg;
  table.provider  = null;
  table.logger    = null;
  table.low_threshold = 15.0;
  table.high_threshold = 30.0;

  table.add = function() {
    var tr = document.createElement('tr');
    var td = document.createElement('td');
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  };

  table.className = 'MonitorPage';
  table.body.className = 'MonitorPage';
  table.body.cellpadding = 0;
  table.body.cellspacing = 0;
  table.header = table.add();
  table.chart = table.add();
  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  /// Value conversion function: 
  /// -- Put item into read if high threshold is exceeded, 
  /// -- Put item into orange if low threshold is exceeded
  /// -- Otherwise default color
  table.sensor_conversion = function(data) {
    if ( data > this.high_threshold )        // High alarm threshold --> red
      this.style.backgroundColor = 'red';
    else if ( data > this.low_threshold )   // Low alarm threshold  --> orange
      this.style.backgroundColor = 'orange';
    else 
      this.style.backgroundColor = '#bbbbcc';
    return table.format_conversion(data);
  };

  table.format_conversion = function(data) {
    var v = parseFloat(data);
    if ( data > 999.0 )  {
      return sprintf('%7.3f mSv/h',v/1000.0);
    }
    return sprintf('%7.3f &mu;Sv/h',v);
  };

  /// Install sensor as table item on the display
  table.installSensor = function(name)   {
    return '<TABLE class="RamsesItem" id="'+name+'_tab"><TR><TD class="Sensor">'+name+'</TD></TR><TR id="'+name+'_curr"></TD></TR><TR id="'+name+'_avg"></TR></TABLE>';
  };

  table.subscribe = function() {
    for(var i=0; i<this.items.length;++i) {
      var itm = this.items[i];
      this.provider.subscribe(itm.name,itm);
    }
  };

  table.bindSensor = function(name, cnv_rate, cnv_avg, alarms) {
    this[name+'_rate'] = StyledItem('lbWeb.ExternalApps/RAMSES/LHCb/'+name+'.DoseRate',   'Dose',   null);//'%7.3f &mu;Sv/h');
    document.getElementById(name+'_curr').appendChild(this[name+'_rate']);
    this.items.push(this[name+'_rate']);
    this[name+'_avg'] = StyledItem('lbWeb.ExternalApps/RAMSES/LHCb/'+name+'.MeanDoseRate','Average',null);//'%7.3f &mu;Sv/h');
    document.getElementById(name+'_avg').appendChild(this[name+'_avg']);
    this.items.push(this[name+'_avg']);

    if ( cnv_avg )   {      // Enable the display of alarms for specified items
      this[name+'_avg'].conversion = cnv_avg;
    }
    if ( cnv_rate )   {      // Enable the display of alarms for specified items
      this[name+'_rate'].conversion = cnv_rate;
    }
    if ( alarms ) {
      // Set tooltips
      tooltips.set(document.getElementById(name+'_tab'),'Radiation sensor '+name+':<br>High alarm threshold:'+this.high_threshold+' &mu;Sv/h<br>Low alarm threshold:'+this.low_threshold+' &mu;Sv/h');
    }
    else {
      tooltips.set(document.getElementById(name+'_tab'),'Radiation sensor '+name+':<br>High alarm threshold: Not set<br>Low alarm threshold: Not set');
    }
  };

  table.build = function() {
    var div, td, tr;
    var offset_y = _isInternetExplorer() ? 0 : 70;
    var offset_x = _isInternetExplorer() ? 0 : 10;
    var tab = document.createElement('table');
    var tb  = document.createElement('tbody');

    tab.width = tb.width = '100%';
    tr = document.createElement('tr');
    tr.appendChild(div=Cell(lhcb_online_picture(),null,null));
    tr.appendChild(div=Cell('LHCb RAMSES Radiation Monitoring',null,'MonitorBigHeader'));
    tr.appendChild(div=Cell('',1,'MonitorTinyHeader'));
    div.id = 'time_stamp';
    tb.appendChild(tr);
    tab.appendChild(tb);
    this.header.appendChild(tab);

    tab = document.createElement('table');
    tb  = document.createElement('tbody');
    tab.width = tb.width = '100%';
    div = document.createElement('div');
    div.id = 'Ramses';
    div.style.backgroundColor = '#FFFFFF';
    div.style.borderWidth = 1;
    div.style.borderStyle = 'solid';
    div.style.width = '970px';
    div.style.overflow = 'auto';
    this.chart.appendChild(div);
    div.innerHTML = '<IMG SRC="'+_fileBase+'/Images/LHCb/Ramses_cavern.bmp" width="940"></IMG>';
    //div.innerHTML = '<IMG SRC="'+_fileBase+'/Images/LHCb/Ramses_cavern_sensors.bmp" width="940"></IMG>';
    // Now draw the canvas with the measurement point on it....
    this.jg = new jsGraphics('Ramses');

    this.jg.setStroke(3);
    // Upper detectors
    this.jg.setColor('blue');
    this.jg.drawLine( 50+offset_x,100+offset_y,160+offset_x,250+offset_y); // PAXL8501
    this.jg.drawLine(150+offset_x,100+offset_y,230+offset_x,285+offset_y); // PAXL8521
    this.jg.drawLine(250+offset_x,100+offset_y,300+offset_x,295+offset_y); // PMIL8502
    this.jg.drawLine(350+offset_x,100+offset_y,430+offset_x,295+offset_y); // PMIL8514
    this.jg.drawLine(450+offset_x,100+offset_y,440+offset_x,250+offset_y); // PMIL8513
    this.jg.setColor('green');
    this.jg.drawLine(550+offset_x,100+offset_y,520+offset_x,200+offset_y); // PMIL8611
    this.jg.setColor('blue');
    this.jg.drawLine(650+offset_x,100+offset_y,480+offset_x,370+offset_y); // PMIL8501
    this.jg.drawLine(750+offset_x,100+offset_y,520+offset_x,330+offset_y); // PMIL8531
    // Lower detectors
    this.jg.drawLine( 50+offset_x,600+offset_y,250+offset_x,330+offset_y); // PAXL8531
    this.jg.drawLine(150+offset_x,600+offset_y,230+offset_x,400+offset_y); // PAXL8512
    this.jg.drawLine(250+offset_x,600+offset_y,430+offset_x,420+offset_y); // PMIL8511
    this.jg.drawLine(350+offset_x,600+offset_y,510+offset_x,470+offset_y); // PMIL8515
    this.jg.setColor('green');
    this.jg.drawLine(450+offset_x,600+offset_y,520+offset_x,530+offset_y); // PMIL8411
    this.jg.setColor('blue');
    this.jg.drawLine(550+offset_x,600+offset_y,590+offset_x,350+offset_y); // PMIL8512
    this.jg.drawLine(650+offset_x,600+offset_y,690+offset_x,340+offset_y); // PATL8511

    this.jg.setStroke(1);
    this.jg.drawString(this.installSensor('PAXL8501'), 20+offset_x,70+offset_y);
    this.jg.drawString(this.installSensor('PAXL8521'),120+offset_x,70+offset_y);
    this.jg.drawString(this.installSensor('PMIL8502'),220+offset_x,70+offset_y);
    this.jg.drawString(this.installSensor('PMIL8514'),320+offset_x,70+offset_y);
    this.jg.drawString(this.installSensor('PMIL8513'),420+offset_x,70+offset_y);
    this.jg.drawString(this.installSensor('PMIL8611'),520+offset_x,70+offset_y);
    this.jg.drawString(this.installSensor('PMIL8501'),620+offset_x,70+offset_y);
    this.jg.drawString(this.installSensor('PMIL8531'),720+offset_x,70+offset_y);

    this.jg.drawString(this.installSensor('PAXL8531'), 20+offset_x,585+offset_y);
    this.jg.drawString(this.installSensor('PAXL8512'),120+offset_x,585+offset_y);
    this.jg.drawString(this.installSensor('PMIL8511'),220+offset_x,585+offset_y);
    this.jg.drawString(this.installSensor('PMIL8515'),320+offset_x,585+offset_y);
    this.jg.drawString(this.installSensor('PMIL8411'),420+offset_x,585+offset_y);
    this.jg.drawString(this.installSensor('PMIL8512'),520+offset_x,585+offset_y);
    this.jg.drawString(this.installSensor('PATL8511'),620+offset_x,585+offset_y);

    this.jg.drawString(this.installSensor('Legend'),  800+offset_x,585+offset_y);

    // Environment values
    this.jg.drawString('<TABLE border="solid" id="Env_tab"><TR><TD colspan="2" class="Sensor">Environment</TD></TR><TR id="VMG982"><TD class="Sensor">VMG982</TD></TR></TABLE>',10+offset_x,10+offset_y);
    var s = StyledItem('lbWeb.ExternalApps/RAMSES/ENV_RAD/PMVG982/VMG982.value','Dose','%7.3f kBq/m<sup>3</sup>');

    this.jg.paint();

    // Subscribe to data
    document.getElementById('VMG982').appendChild(s);
    tooltips.set(document.getElementById('Env_tab'),'Display of environmental parameters in the cavern.');
    this.items.push(s);

    this.bindSensor('PAXL8501',this.format_conversion,this.sensor_conversion,true);
    this.bindSensor('PAXL8512',this.format_conversion,this.sensor_conversion,true);
    this.bindSensor('PAXL8521',this.format_conversion,this.sensor_conversion,true);
    this.bindSensor('PAXL8531',this.format_conversion,this.sensor_conversion,true);
    this.bindSensor('PATL8511',this.format_conversion,this.sensor_conversion,true);
    this.bindSensor('PMIL8411',this.format_conversion,this.format_conversion,false);
    this.bindSensor('PMIL8501',this.format_conversion,this.format_conversion,false);
    this.bindSensor('PMIL8502',this.format_conversion,this.format_conversion,false);
    this.bindSensor('PMIL8511',this.format_conversion,this.format_conversion,false);
    this.bindSensor('PMIL8512',this.format_conversion,this.format_conversion,false);
    this.bindSensor('PMIL8513',this.format_conversion,this.format_conversion,false);
    this.bindSensor('PMIL8514',this.format_conversion,this.format_conversion,false);
    this.bindSensor('PMIL8515',this.format_conversion,this.format_conversion,false);
    this.bindSensor('PMIL8531',this.format_conversion,this.format_conversion,false);
    this.bindSensor('PMIL8611',this.format_conversion,this.format_conversion,false);

    // Add Legend
    document.getElementById('Legend_curr').appendChild(Cell('Current Rate',null,'Dose'));
    document.getElementById('Legend_avg').appendChild(Cell('Average Rate',null,'Average'));
    // Finally add suggestions text
    tr = document.createElement('tr');
    tr.appendChild(Cell('Comments and suggestions to M.Frank CERN/LHCb',null,'MonitorTinyHeader'));
    tb.appendChild(tr);

    tab.appendChild(tb);
    this.display.appendChild(tab);
    setInterval(function(){document.getElementById('time_stamp').innerHTML=(new Date()).toString();},2000);
    return this;
  };

  return table;
};

var ramses_unload = function()  {  dataProviderReset();   };

var ramses_body = function()  {
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);

  var selector = RamsesStatus(msg);
  body.appendChild(selector);
  setWindowTitle('LHCb RAMSES Radiation Monitoring');
  if ( msg > 0 )
    selector.logger   = new OutputLogger(selector.logDisplay, 200, LOG_INFO, 'StatusLogger');
  else
    selector.logger   = new OutputLogger(selector.logDisplay,  -1, LOG_INFO, 'StatusLogger');
  selector.provider = new DataProvider(selector.logger);
  selector.provider.topic = '/topic/status';
  selector.build();
  selector.subscribe();
  selector.provider.start();
};

if ( _debugLoading ) alert('Script lhcb.display.ramses.cpp loaded successfully');
