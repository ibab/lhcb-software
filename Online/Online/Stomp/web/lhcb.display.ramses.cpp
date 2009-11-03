_loadScript('lhcb.display.items.cpp');
_loadScript('wz_jsgraphics.js');
_loadFile('lhcb.display.general','css');
_loadFile('lhcb.display.ramses','css');

var s_offset = _isInternetExplorer() ? 0 : 70;

var RamsesStatus = function(msg)   {
  table           = document.createElement('table');
  table.body      = document.createElement('tbody');
  table.items     = new Array();
  table.messages  = msg;
  table.provider  = null;
  table.logger    = null;

  table.add = function() {
    var tr = document.createElement('tr');
    var td = document.createElement('td');
    td.setAttribute('colSpan',2);
    tr.appendChild(td);
    this.body.appendChild(tr);
    return td;
  }

  table.className = 'MonitorPage';
  table.body.className = 'MonitorPage';
  table.body.cellpadding = 0;
  table.body.cellspacing = 0;
  table.chart = table.add();
  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  table.installSensor = function(name)   {
    return '<TABLE><TR><TD class="Sensor">'+name+'</TD></TR><TR id="'+name+'_curr"></TD></TR><TR id="'+name+'_avg"></TR></TABLE>';
  }

  table.subscribe = function() {
    for(var i=0; i<this.items.length;++i) {
      var itm = this.items[i];
      this.provider.subscribe(itm.name,itm);
    }
  }

  table.bindSensor = function(name) {
    this[name+'_rate'] = StyledItem('lbWeb.ExternalApps/RAMSES/LHCb/'+name+'.DoseRate',   'Dose',   '%7.3f &mu;Sv/h');
    document.getElementById(name+'_curr').appendChild(this[name+'_rate']);
    this.items.push(this[name+'_rate']);
    this[name+'_avg'] = StyledItem('lbWeb.ExternalApps/RAMSES/LHCb/'+name+'.MeanDoseRate','Average','%7.3f &mu;Sv/h');
    document.getElementById(name+'_avg').appendChild(this[name+'_avg']);
    this.items.push(this[name+'_avg']);
  }

  table.build = function() {
    var div, hdr, td, tr;
    var tab = document.createElement('table');
    var tb = document.createElement('tbody');

    tab.width = tb.width = '100%';
    hdr = document.createElement('H1');
    hdr.innerHTML='LHCb RAMSES Radiation Monitoring';
    table.chart.appendChild(hdr);

    div = document.createElement('div');
    div.id = 'Ramses';
    div.style.backgroundColor = '#FFFFFF';
    div.style.borderWidth = 1;
    div.style.borderStyle = 'solid';
    div.style.width = '950px';
    div.style.overflow = 'auto';
    table.chart.appendChild(div);
    div.innerHTML = '<IMG SRC="'+_fileBase+'/Images/LHCb/Ramses_LHCb_2.jpg" width="940"></IMG>';
    // Now draw the canvas with the measurement point on it....
    this.jg = new jsGraphics('Ramses');
    this.jg.drawString(this.installSensor('PAXL8501'),200,20+s_offset);
    this.jg.drawString(this.installSensor('PAXL8521'),420,20+s_offset);
    this.jg.drawString(this.installSensor('PAXL8512'),200,500+s_offset);
    this.jg.drawString(this.installSensor('PMIL8501'),450,100+s_offset);
    this.jg.drawString(this.installSensor('PMIL8513'),600,20+s_offset);
    this.jg.drawString(this.installSensor('PMIL8511'),450,450+s_offset);
    this.jg.drawString(this.installSensor('PMIL8512'),700,50+s_offset);
    this.jg.drawString(this.installSensor('PMIL8531'),580,500+s_offset);
    this.jg.drawString(this.installSensor('Legend'),850,580+s_offset);
    this.jg.paint();
    // Subscribe to data
    this.bindSensor('PAXL8501');
    this.bindSensor('PAXL8512');
    this.bindSensor('PAXL8521');
    this.bindSensor('PMIL8501');
    this.bindSensor('PMIL8511');
    this.bindSensor('PMIL8512');
    this.bindSensor('PMIL8513');
    this.bindSensor('PMIL8531');
    // Add Legend
    td = document.createElement('td');
    td.className = 'Dose';
    td.innerHTML = 'Current Rate';
    document.getElementById('Legend_curr').appendChild(td);
    td = document.createElement('td');
    td.className = 'Average';
    td.innerHTML = 'Average Rate';
    document.getElementById('Legend_avg').appendChild(td);
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

var ramses_unload = function()  {
  dataProviderReset();
}

var ramses_body = function()  {
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  //var tips = init_tooltips(body);

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
}

if ( _debugLoading ) alert('Script lhcb.display.detstatus.cpp loaded successfully');
