#!/bin/bash
base=`basename $1`;
dir=`dirname $1`;
/usr/bin/java -jar ~/WWW/test/Online/Stomp/web/env-js.jar <<EOF
load('http://frankm.web.cern.ch/frankm/test/Online/Stomp/packer/packer.js');
pack_javascript('$1','$2/$base');
EOF
