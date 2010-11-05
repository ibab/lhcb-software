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
#/usr/bin/java -Xms256M -Xmx256M -jar `dirname ${0}`/../web/env-js.jar <$OUT
if test "${HOST}" = "web01"
then
  /sw/java/i386_redhat61/jdk/sun-1.5.0/bin/java -Xms256M -Xmx256M -jar `dirname ${0}`/../web/env-js.jar <$OUT;
else
  /usr/bin/java -Xms256M -Xmx256M -jar `dirname ${0}`/../web/env-js.jar <$OUT;
fi;
##load('http://frankm.web.cern.ch/frankm/Online/packer/packer.js');
##pack_javascript('$1','/afs/cern.ch/user/f/frankm/WWW/Online/packer');
##EOF
