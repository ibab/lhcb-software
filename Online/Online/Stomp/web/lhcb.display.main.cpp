_loadScript('dom.print.cpp');
_loadFile('lhcb.display.general','css');

function lhcb_online_picture() 
{ return '<IMG SRC="http://lhcb-online.web.cern.ch/lhcb-online/elog/images/lhcb-online-logo.PNG" HEIGHT="50"></IMG>';   }
var Cell = function(text,nspan,className) {
  var c = document.createElement('td');
  //var t = document.createTextNode(text);
  if ( nspan       != null ) c.colSpan = nspan;
  if ( className   != null ) c.className = className;
  c.innerHTML = text;
  //c.appendChild(t);
  return c;
}

var main_unload = function()  {
  //alert('Connection reset.\n\nBye, Bye my friend....');
}

var ViewFrame = function() {
  var tab = document.createElement('table');
  var tb = document.createElement('tbody');
  tab.width='100%';
  tb.width = '100%';
  this.heading = document.createElement('tr');
  var cell = Cell(lhcb_online_picture()+'&nbsp;VIEW',1,'MonitorBigHeader');
  cell.style.textAlign = 'left';
  this.heading.appendChild(cell);
  tb.appendChild(this.heading);
  tab.appendChild(tb);
  return tab;
}

var ManagerFrame = function() {
  var tab = document.createElement('table');
  var tb = document.createElement('tbody');
  tab.width='100%';
  tb.width = '100%';
  this.heading = document.createElement('tr');
  var cell = Cell(lhcb_online_picture()+'&nbsp;MANAGER',1,'MonitorBigHeader');
  cell.style.textAlign = 'left';
  this.heading.appendChild(cell);
  tb.appendChild(this.heading);
  tab.appendChild(tb);
  return tab;
}

var BasicDocument = function() {
  var frame, frameset = document.createElement('frameset');
  frameset.style.frameSpacing = 0;
  frameset.style.cols = '30,*';
  
  frame = document.createElement('frame');
  frame.name = 'view';
  frame.src = document.location.href+'&frame=view';
  /*
  frame.marginWidth = 0;
  frame.scrolling = 'auto';
  frame.frameBorder = 1;
  */
  frameset.appendChild(frame);

  frame = document.createElement('frame');
  frame.name = 'manager';
  frame.src = document.location.href+'&frame=manager';
  /*
  frame.marginWidth = 0;
  frame.scrolling = 'auto';
  frame.frameBorder = 1;
  frameset.appendChild(frame);
  */
  return frameset;
}

var main_body = function()  {
  var msg = the_displayObject['messages'];
  var frame = the_displayObject['frame'];
  var body = document.getElementsByTagName('body')[0];
  var selector;

  alert('frame:'+frame+'\nbody:'+body+'\n'+document.location.href);
  if ( frame ) {
    //body = window.parent.frames[frame];//.getElementsByTagName('body')[0];
    if ( frame=='view' ) {
      //selector = ViewFrame();
      document.write('VIEW');
      return;
    }
    else if ( frame=='manager' ) {
      //selector = ManagerFrame();
      document.write('MGR');
      return;
    }
  }
  else {
    selector = BasicDocument();
    body.appendChild(selector);
  }
  alert('frame:'+frame+'\nbody:'+body+'\n'+document.location.href);
  //printDOMTree(document,null);
  top.document.title = 'LHCb status displays';
}

if ( _debugLoading ) alert('Script lhcb.display.status.cpp loaded successfully');
