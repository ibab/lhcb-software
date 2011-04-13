_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadScript('wz_jsgraphics.js');
_loadFile('lhcb.display.general','css');

var CENTER_X = 520;
var CENTER_Y = 220;
var SECTOR_WIDTH = 200;

var VeloDetector = function(msg) {
  var div = document.createElement('div');
  div.id = 'Velo';
  div.style.backgroundColor = '#FFFFFF';
  div.style.borderWidth = 1;
  div.style.borderStyle = 'solid';
  div.style.width = '1050px';
  div.style.height = '500px';
  div.style.overflow = 'auto';

  div.jg = null;
  div.beam = {x: 0, y:0 };

  div.setBeamPosition = function(x, y) {

    var pos_x = CENTER_X+x;
    var pos_y = CENTER_Y+y;

    if ( this.jg ) this.jg.clear();//div.removeChild(this.jg.cnv);
    else this.jg = new jsGraphics('Velo', div);

    this.beam.x = x;
    this.beam.y = y;

    this.jg.setColor('lightgrey');
    this.jg.fillRect(pos_x-50,  pos_y-50, 100, 100);
    this.jg.setColor('#00FF00');
    this.jg.drawEllipse(pos_x-48,pos_y-48, 95,  95);
    this.jg.setColor('black');
    this.jg.drawRect(pos_x-50,  pos_y-50, 100, 100);
    if ( this.left.position < -15 ) {
      this.jg.setColor('grey');
      this.jg.fillRect(pos_x-160, pos_y-50, 110, 100);
      this.jg.setColor('black');
      this.jg.setStroke(1);
      this.jg.drawRect(pos_x-160, pos_y-50, 110, 100);
      this.jg.drawLine(pos_x-160, pos_y+50, pos_x-160, 450);
      this.jg.drawString('-16 mm',pos_x-180,460);
    }
    if ( this.left.position < -5.1 ) {
      this.jg.setColor('black');
      this.jg.setStroke(1);
      this.jg.drawLine(pos_x-50, pos_y+50, pos_x-50, 450);
      this.jg.drawString('-5 mm',pos_x-70,460);
    }
    if ( this.right.position > 15 ) {
      this.jg.setColor('grey');
      this.jg.fillRect(pos_x+50,  pos_y-50, 110, 100);
      this.jg.setColor('black');
      this.jg.setStroke(1);
      this.jg.drawRect(pos_x+50,  pos_y-50, 110, 100);
      this.jg.drawLine(pos_x+160, pos_y+50, pos_x +160, 450);
      this.jg.drawString('+16 mm',pos_x+140,460);
    }
    if ( this.right.position > 5.1 ) {
      this.jg.setColor('black');
      this.jg.setStroke(1);
      this.jg.drawLine(pos_x+50, pos_y+50, pos_x +50, 450);
      this.jg.drawString('+5 mm',pos_x+30, 460);
    }
    if ( this.right.position > 24 ) {
      this.jg.drawString(sprintf('Y=%7.2f mm',y),pos_x+170,pos_y-25,'left');
    }
    else {
      this.jg.drawString(sprintf('Y=%7.2f mm',y),pos_x+440,pos_y-25,'left');
    }
    this.jg.drawRect(pos_x-50,  pos_y-50, 100, 100);

    this.jg.setStroke(1);
    this.jg.setColor('blue');
    this.jg.drawLine(pos_x + x, 0, pos_x + x, 450);
    this.jg.drawLine(30, pos_y + y, 1010, pos_y + y);

    // Nominal position
    this.jg.setColor('green');
    if ( this.left.position < -23 ) {
      this.jg.drawLine(30, CENTER_Y+48, CENTER_X-160, CENTER_Y+48);
      this.jg.drawLine(30, CENTER_Y-48, CENTER_X-160, CENTER_Y-48);
      this.jg.drawString('+4.8 mm', CENTER_X-220, CENTER_Y-65);
      this.jg.drawString('-4.8 mm', CENTER_X-220, CENTER_Y+30);
    }
    else {
      this.jg.drawLine(30, CENTER_Y+48, CENTER_X-50, CENTER_Y+48);
      this.jg.drawLine(30, CENTER_Y-48, CENTER_X-50, CENTER_Y-48);
      this.jg.drawString('-4.8 mm', 30, CENTER_Y+30);
      this.jg.drawString('+4.8 mm', 30, CENTER_Y-65);
    }

    this.jg.setColor('red');
    this.jg.fillOval(pos_x + x - 4,pos_y + y - 5,10,10);
  };

  div.canvas = document.createElement('div');
  div.canvas.style.position = 'absolute';
  div.canvas.style.left     = 0;
  div.canvas.style.top      = 35; // 48 For bib velo pics
  //div.canvas.style.width    = '100%';
  div.canvas.style.height   = 500;

  div.left = document.createElement('img');
  div.left.id             = 'Image_Velo_Left';
  div.left.canvas         = div;
  div.left.style.position = 'absolute';
  div.left.style.left     = 0;
  div.left.style.top      = 0;
  div.left.style.width    = SECTOR_WIDTH;
  div.left.style.height   = 350;
  div.left.src            = 'Images/VeloLeftSmall.jpg';
  //div.left.src            = 'Images/VL.jpg';
  div.left.position       = -28;
  /*
  div.left.onmouseover = function() {
    var img = document.getElementById('Image_Velo_Left');
    img.position += 3;
    this.setPosition(img.position);
  };
  */
  div.left._set = function(left)  {
    var x = 10*left + CENTER_X - SECTOR_WIDTH;
    this.position = left;
    this.style.left = x;
    this.canvas.jg.setStroke(1);
    this.canvas.jg.setColor('red');
    this.canvas.jg.drawLine(x+SECTOR_WIDTH, CENTER_Y+50, x+SECTOR_WIDTH, 450);
    this.canvas.jg.setColor('black');
    this.canvas.jg.drawStringRect(sprintf('XA=%7.2f mm',left),x+SECTOR_WIDTH-120, 440, 110,"right"); 
  };
  div.left.setPosition = function(left)  {
    var c = this.canvas;
    c.setBeamPosition(c.beam.x,c.beam.y);
    c.right._set(c.right.position);
    this._set(left);
    c.jg.paint();
  };

  div.right = document.createElement('img');
  div.right.id             = 'Image_Velo_Right';
  div.right.canvas         = div;
  div.right.style.position = 'absolute';
  div.right.style.left     = 650;
  div.right.style.top      = 0;
  div.right.style.width    = SECTOR_WIDTH;
  div.right.style.height   = 350;
  div.right.src            = 'Images/VeloRightSmall.jpg';
  //div.right.src            = 'Images/VR.jpg';
  div.right.position       = 28;
  /*
  div.right.onmouseover = function() {
    this.position -= 1;
    this.setPosition(this.position);
  };
  */
  div.right._set = function(right)  {
    var x = CENTER_X + 10*right;
    this.style.left = x;
    this.position = right;
    this.canvas.jg.setStroke(1);
    this.canvas.jg.setColor('red');
    this.canvas.jg.drawLine(x, CENTER_Y+50, x, 450);
    this.canvas.jg.setColor('black');
    this.canvas.jg.drawStringRect(sprintf('XC=%7.2f mm',right),x+10, 440, 110,"left"); 
  };
  div.right.setPosition = function(pos)  {
    var c = this.canvas;
    c.setBeamPosition(c.beam.x,c.beam.y);
    c.left._set(c.left.position);
    this._set(pos);
    this.canvas.jg.paint();
  };

  
  div.canvas.appendChild(div.left);
  div.canvas.appendChild(div.right);
  div.appendChild(div.canvas);
  return div;
};

var VeloMotion = function(msg)   {
  var table           = document.createElement('table');
  table.body      = document.createElement('tbody');
  table.items     = new Array();
  table.messages  = msg;
  table.provider  = null;
  table.logger    = null;

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
  table.data   = table.add();
  table.chart  = table.add();
  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  table.velo_left = function(data) {
    this.motion.left.setPosition(data);
    return data;
  };
  table.velo_right = function(data) {
    this.motion.right.setPosition(data);
    return data;
  };
  table.velo_height = function(data) {
    this.motion.setBeamPosition(0,data);
    this.motion.left._set(this.motion.left.position);
    this.motion.right._set(this.motion.right.position);
    this.motion.jg.paint();
    return data;
  };

  table.subscribe = function() {
    for(var i=0; i<this.items.length;++i) {
      var itm = this.items[i];
      this.provider.subscribe(itm.name,itm);
    }
    //this.veloMove.subscribe(this.provider);
    this.veloPosition.subscribe(this.provider);
  };

  table.build = function() {
    var div, td, tr;
    var offset_y = _isInternetExplorer() ? 0 : 70;
    var offset_x = _isInternetExplorer() ? 0 : 10;
    var tab = document.createElement('table');
    var tb  = document.createElement('tbody');

    tab.width = tb.width = '100%';
    tab.style.height = tb.style.height = '100%';
    tr = document.createElement('tr');
    tr.appendChild(div=Cell(lhcb_online_picture(),null,null));
    tr.appendChild(div=Cell('VELO Motion Monitor',null,'MonitorBigHeader'));
    tr.appendChild(div=Cell('',1,'MonitorTinyHeader'));
    div.id = 'time_stamp';
    tb.appendChild(tr);
    tab.appendChild(tb);
    this.header.appendChild(tab);
    
    var opts = {style:'Arial12pt',legend:true,logger:this.logger};
    opts.tips = 'Velo position<br>Click to see VELO online page';
    opts.onclick = function() {document.location=lhcb.constants.urls.lhcb.subdetectors.velo.src;};

    this.veloPosition = lhcb.widgets.velo.positionSummary(opts);
    this.data.appendChild(this.veloPosition);

    //this.veloMove = lhcb.widgets.velo.moveSummary(opts);
    //this.data.appendChild(this.veloMove);

    tab = document.createElement('table');
    tb  = document.createElement('tbody');
    tab.width = tb.width = '100%';
    tab.style.height = tb.style.height = '100%';
    tr = document.createElement('tr');
    td = document.createElement('td');

    this.motion = VeloDetector(msg);
    this.display.appendChild(this.motion);
    this.motion.setBeamPosition(0,0);
    this.motion.left.setPosition(-29.0);
    this.motion.right.setPosition(29.0);
    this.veloPosition.veloOpening.motion = this.motion; 
    this.veloPosition.veloOpening.conversion = this.velo_right;    
    this.veloPosition.veloCenter.motion = this.motion; 
    this.veloPosition.veloCenter.conversion = this.velo_left;
    this.veloPosition.veloY.motion = this.motion; 
    this.veloPosition.veloY.conversion = this.velo_height;

    td.appendChild(this.motion);
    tr.appendChild(td);
    tb.appendChild(tr);

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

var velo_move_unload = function()  {  dataProviderReset();   };

var velo_move_body = function()  {
  var msg  = the_displayObject['messages'];
  var body = document.getElementsByTagName('body')[0];
  var tips = init_tooltips(body);

  var selector = VeloMotion(msg);
  body.appendChild(selector);
  setWindowTitle('VELO Motion Monitor');
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

if ( _debugLoading ) alert('Script lhcb.display.velo_move.cpp loaded successfully');
