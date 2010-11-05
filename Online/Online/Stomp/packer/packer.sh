#!/bin/bash
echo $1
FILES=`ls $1`;
OUT=./pack_files.data;
echo "load('http://frankm.web.cern.ch/frankm/test/Online/Stomp/packer/packer.js');">$OUT;
echo $FILES;
for i in $FILES; do 
    base=`basename $i`;
    dir=`dirname $i`;
    echo "pack_javascript('${dir}/${base}','${2}/${base}');">>$OUT;
done;
/usr/bin/java -Xms256M -Xmx256M -jar ../web/env-js.jar <$OUT
##load('http://frankm.web.cern.ch/frankm/Online/packer/packer.js');
##pack_javascript('$1','/afs/cern.ch/user/f/frankm/WWW/Online/packer');
##EOF
