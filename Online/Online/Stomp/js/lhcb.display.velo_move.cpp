_loadScript('lhcb.display.items.cpp');
_loadScript('lhcb.display.widgets.cpp');
_loadFile('lhcb.display.general','css');
_loadScript('wz_jsgraphics.js');

var isIE = _isInternetExplorer();

var CENTER_X = isIE ? 500 : 625;
var CENTER_Y = isIE ? 260 : 440;
var SECTOR_WIDTH    = 170;
var PLOT_HEIGHT     = 600;
var SECTOR_OVERLAP  = 20;
var PLOT_OFFSET     = isIE ? 20 : 140;
var VELO_POSITION   = null;
var MOVE_MAX_Y = 20;
var MOVE_MAX_X = 20;

var VeloDetector = function(msg) {

  var div = document.createElement('div');
  div.id = 'Velo';
  div.style.backgroundColor = '#FFFFFF';
  div.style.borderWidth = 1;
  div.style.borderStyle = 'solid';
  div.style.width  = isIE ? '100%' : 1180;
  div.style.height = PLOT_HEIGHT+20;
  div.style.top    = 0;
  div.style.overflow = 'hidden';

  div.jg = null;
  div.beam = {x: 0, y:0 };

  div.setBeamPosition = function(x, y) {

    var pos_x = CENTER_X+10*x, b_x=10*x;
    var pos_y = CENTER_Y+10*y, b_y=10*x;
    var pl_h  = CENTER_Y+(isIE ? 330 : 300);

    if ( this.jg ) this.jg.clear();
    else this.jg = new jsGraphics('Velo', window);

    this.beam.x = x;
    this.beam.y = y;

    this.jg.setColor('lightgrey');
    this.jg.fillEllipse(pos_x-MOVE_MAX_Y,  pos_y-MOVE_MAX_Y, 2*MOVE_MAX_Y-1, 2*MOVE_MAX_Y-1);
    this.jg.setColor('#00FF00');
    this.jg.drawEllipse(pos_x-MOVE_MAX_X,  pos_y-MOVE_MAX_Y, 2*MOVE_MAX_X-1, 2*MOVE_MAX_Y-1);
    this.jg.setColor('black');
    this.jg.drawRect(pos_x-MOVE_MAX_X,  pos_y-MOVE_MAX_Y, 2*MOVE_MAX_X, 2*MOVE_MAX_Y);
    if ( this.left.position > 15 ) {
      this.jg.setColor('#999999');
      this.jg.fillRect(pos_x-160, pos_y-MOVE_MAX_Y, 160-MOVE_MAX_X, 2*MOVE_MAX_Y);
      this.jg.setColor('black');
      this.jg.setStroke(1);
      this.jg.drawRect(pos_x-160, pos_y-MOVE_MAX_Y, 160-MOVE_MAX_Y, 2*MOVE_MAX_Y);
      this.jg.drawLine(pos_x-160, pos_y+MOVE_MAX_Y, pos_x-160, pl_h);
      this.jg.drawString('16 mm',pos_x-200,pl_h);
    }
    if ( this.left.position > 5.1 ) {
      this.jg.setColor('black');
      this.jg.setStroke(1);
      this.jg.drawLine(pos_x-MOVE_MAX_X, pos_y+MOVE_MAX_Y, pos_x-MOVE_MAX_X, pl_h);
      this.jg.drawString('2 mm',pos_x-MOVE_MAX_X-40,pl_h);
    }
    if ( this.right.position < -15 ) {
      this.jg.setColor('#999999');
      this.jg.fillRect(pos_x+MOVE_MAX_X,  pos_y-MOVE_MAX_Y, 160-MOVE_MAX_X, 2*MOVE_MAX_Y);
      this.jg.setColor('black');
      this.jg.setStroke(1);
      this.jg.drawRect(pos_x+MOVE_MAX_X,  pos_y-MOVE_MAX_Y, 160-MOVE_MAX_X, 2*MOVE_MAX_Y);
      this.jg.drawLine(pos_x+160, pos_y+MOVE_MAX_Y, pos_x +160, pl_h);
      this.jg.drawString('-16 mm',pos_x+160,pl_h);
    }
    if ( this.right.position < -5.1 ) {
      this.jg.setColor('black');
      this.jg.setStroke(1);
      this.jg.drawLine(pos_x+MOVE_MAX_X, pos_y+MOVE_MAX_Y, pos_x+MOVE_MAX_Y, pl_h);
      this.jg.drawString('-2 mm',pos_x+MOVE_MAX_X+10, pl_h);
    }
    this.jg.drawRect(pos_x-MOVE_MAX_X,  pos_y-MOVE_MAX_Y, 2*MOVE_MAX_X, 2*MOVE_MAX_Y);

    this.jg.setStroke(1);
    this.jg.setColor('blue');
    this.jg.drawLine(pos_x + b_x, PLOT_OFFSET, pos_x + b_x, pl_h);
    this.jg.drawLine(30, pos_y + b_y, 1010, pos_y + b_y);

    // Nominal position
    this.jg.setColor('green');
    if ( this.left.position > 23 ) {
      this.jg.drawLine(MOVE_MAX_X, CENTER_Y+MOVE_MAX_Y, CENTER_X-160, CENTER_Y+MOVE_MAX_Y);
      this.jg.drawLine(MOVE_MAX_X, CENTER_Y-MOVE_MAX_Y, CENTER_X-160, CENTER_Y-MOVE_MAX_Y);
      this.jg.drawString(sprintf('+%2.1f mm',MOVE_MAX_Y/10), CENTER_X-220, CENTER_Y-MOVE_MAX_Y-25);
      this.jg.drawString(sprintf('-%2.1f mm',MOVE_MAX_Y/10), CENTER_X-220, CENTER_Y+MOVE_MAX_Y+10);
    }
    else {
      this.jg.drawLine(30, CENTER_Y+MOVE_MAX_Y, CENTER_X-MOVE_MAX_X, CENTER_Y+MOVE_MAX_Y);
      this.jg.drawLine(30, CENTER_Y-MOVE_MAX_Y, CENTER_X-MOVE_MAX_X, CENTER_Y-MOVE_MAX_Y);
      this.jg.drawString(sprintf('-%2.1f mm',MOVE_MAX_Y/10), 30, CENTER_Y+MOVE_MAX_Y+10);
      this.jg.drawString(sprintf('+%2.1f mm',MOVE_MAX_Y/10), 30, CENTER_Y-MOVE_MAX_Y-25);
    }
    this.jg.setColor('#00FF00');
    this.jg.drawString(sprintf('<font size="+1"><B>Y=%7.2f mm</B></font>',y),15,pos_y-65,'left');

    this.jg.setColor('red');
    this.jg.fillOval(pos_x + b_x - 4,pos_y + b_y - 5,10,10);
  };

  div.canvas = document.createElement('div');
  div.canvas.style.position = 'absolute';
  div.canvas.style.left     = 0;
  div.canvas.style.top      = 0;
  div.canvas.style.width    = div.style.width;
  div.canvas.style.height   = PLOT_HEIGHT+PLOT_OFFSET;
  div.canvas.style.overflow = 'hidden';

  div.left2 = document.createElement('img');
  div.left2.id             = 'Image_Velo_Right_Small';
  div.left2.style.position = 'absolute';
  div.left2.style.left     = SECTOR_WIDTH;
  div.left2.style.top      = CENTER_Y-295;
  div.left2.style.width    = 2*SECTOR_WIDTH;
  div.left2.style.height   = 600;
  div.left2.src            = 'Images/VeloLeft.png';
  div.canvas.appendChild(div.left2);

  div.left = document.createElement('img');
  div.left.id             = 'Image_Velo_Left_Small';
  div.left.canvas         = div;
  div.left.style.position = 'absolute';
  div.left.style.left     = 2*SECTOR_WIDTH;
  div.left.style.top      = CENTER_Y-125;
  div.left.style.width    = SECTOR_WIDTH;
  div.left.style.height   = 250;
  div.left.src            = 'Images/VeloLeftSmall.png';
  div.left.position       = -28;
  div.canvas.appendChild(div.left);

  div.left._set = function(left)  {
    if ( VELO_POSITION ) left=VELO_POSITION;
    var c = this.canvas;
    var x = -10*left + CENTER_X + SECTOR_OVERLAP - 10*c.beam.x;
    this.position = left;
    this.style.left = x - SECTOR_WIDTH;
    c.left2.style.left = x - 2*SECTOR_WIDTH;
    c.jg.setStroke(2);
    c.jg.setColor('#FF4444');
    c.jg.drawLine(x-SECTOR_OVERLAP-1, CENTER_Y+65, x-SECTOR_OVERLAP-1, CENTER_Y+310);
    c.jg.drawStringRect(sprintf('<font size="+1"><B>X<sub>A</sub>=%7.2f mm</B></font>',left),
			x-230,CENTER_Y+80, 200,"right"); 
  };
  div.left.setPosition = function(left)  {
    var c = this.canvas;
    c.setBeamPosition(c.beam.x,c.beam.y);
    c.right._set(c.right.position);
    this._set(left);
    c.jg.paint();
  };

  div.right2 = document.createElement('img');
  div.right2.id             = 'Image_Velo_Right';
  div.right2.style.position = 'absolute';
  div.right2.style.left     = CENTER_X;
  div.right2.style.top      = CENTER_Y-295;
  div.right2.style.width    = 2*SECTOR_WIDTH;
  div.right2.style.height   = 600;
  div.right2.src            = 'Images/VeloRight.png';
  div.canvas.appendChild(div.right2);

  div.right = document.createElement('img');
  div.right.id             = 'Image_Velo_Right_Small';
  div.right.canvas         = div;
  div.right.style.position = 'absolute';
  div.right.style.left     = CENTER_X;
  div.right.style.top      = CENTER_Y-125;
  div.right.style.width    = SECTOR_WIDTH;
  div.right.style.height   = 250;
  div.right.src            = 'Images/VeloRightSmall.png';
  div.right.position       = 28;
  div.canvas.appendChild(div.right);

  div.right._set = function(right)  {
    if ( VELO_POSITION ) right=VELO_POSITION;
    var c = this.canvas;
    var x = -10*right + CENTER_X - SECTOR_OVERLAP - 10*c.beam.x;
    this.position       = right;
    this.style.left     = x;
    c.right2.style.left = x;
    c.jg.setStroke(2);
    c.jg.setColor('#FFFF00');
    c.jg.drawLine(x+SECTOR_OVERLAP+1, CENTER_Y+65, x+SECTOR_OVERLAP+1, CENTER_Y+310);
    c.jg.drawStringRect(sprintf('<font size="+1"><B>X<sub>C</sub>=%7.2f mm</B></font>',right),x+30,
			CENTER_Y+80,200,"left"); 
  };
  div.right.setPosition = function(right)  {
    var c = this.canvas;
    c.setBeamPosition(c.beam.x,c.beam.y);
    c.left._set(c.left.position);
    this._set(right);
    this.canvas.jg.paint();
  };

  /*
  div.left.onmouseover = function() {
    this.position += 3;
    this.setPosition(this.position);
  };
  div.right.onmouseover = function() {
    this.position -= 3;
    this.setPosition(this.position);
  };
  */
  div.appendChild(div.canvas);
  return div;
};

var VeloMotion = function(msg)   {
  var table       = document.createElement('table');
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
  table.header  = table.add();
  table.data    = table.add();
  table.display = table.add();
  table.logDisplay = table.add();
  table.appendChild(table.body);

  table.velo_left = function(data) {
    //data = 15.49;
    this.motion.left.setPosition(data);
    return data;
  };
  table.velo_right = function(data) {
    //data = -15.49;
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

    tab = document.createElement('table');
    tb  = document.createElement('tbody');
    tab.width = tb.width = '100%';
    tab.style.height = tb.style.height = '100%';
    tr = document.createElement('tr');
    td = document.createElement('td');

    this.motion = VeloDetector(msg);
    this.display.appendChild(this.motion);
    this.motion.setBeamPosition(0,0);
    this.motion.left.setPosition(29.0);
    this.motion.right.setPosition(-29.0);
    this.veloPosition.veloOpening.motion     = this.motion; 
    this.veloPosition.veloOpening.conversion = this.velo_left;
    this.veloPosition.veloCenter.motion      = this.motion;
    this.veloPosition.veloCenter.conversion  = this.velo_right;
    this.veloPosition.veloY.motion           = this.motion; 
    this.veloPosition.veloY.conversion       = this.velo_height;

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
