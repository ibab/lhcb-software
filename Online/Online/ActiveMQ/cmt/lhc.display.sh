#!/bin/bash
##cd /home/frankm/JS
export UTGID=LHC_`hostname -s`
echo "Starting UTGID=${UTGID}"
exec -a ${UTGID} /usr/bin/java -jar ../web/env-js.jar <<EOF
var loc = 'http://ecs03.lbdaq.cern.ch:8000/static/';
//loc = 'http://lbcomet.cern.ch/static/';
//loc = 'http://comet.lbdaq.cern.ch/static/';
load(loc+'RunStatus/env.rhino.cpp');
//load('file:///home/frankm/cmtuser/Online_v4r29/Online/Stomp/web/env.rhino.cpp');
function alert(msg) {   print(msg);   }
window.onload=function() {  print('================= Document '+window.location+' loaded');  }
window.location=loc+'RunStatus/display.htm?${1}';
EOF
