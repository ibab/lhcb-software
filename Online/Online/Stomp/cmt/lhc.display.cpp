var disp;
load('env.rhino.cpp');
function alert(msg) {   print(msg);   }
disp = 'http://lbcomet.cern.ch/static/RunStatus/display.htm';
disp = 'http://comet.lbdaq.cern.ch/static/RunStatus/display.htm';
disp = 'http://plus18.lbdaq.cern.ch:8000/static/RunStatus/display.htm';
window.onload=function() {  print('================= Document '+window.location+' loaded');  }

//window.location=disp+'?type=status&system=LHCb&messages=1';
//window.location=disp+'?type=lhc&messages=1';
window.location=disp+'?type=lhc';
//window.location=disp+'?type=page1&messages=1';
