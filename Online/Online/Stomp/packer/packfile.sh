#!/bin/bash
base=`basename $1`;
dir=`dirname $1`;
OUT=./pack_files.data;
echo "load('http://frankm.web.cern.ch/frankm/test/Online/Stomp/packer/packer.js');">$OUT;
echo "pack_javascript('${dir}/${base}','${2}/${base}');">>$OUT;
if test -f /sw/java/i386_redhat61/jdk/sun-1.5.0/bin/java;
then
  /sw/java/i386_redhat61/jdk/sun-1.5.0/bin/java -Xms256M -Xmx256M -jar `dirname ${0}`/../web/env-js.jar <$OUT;
else
  /usr/bin/java -Xms256M -Xmx256M -jar `dirname ${0}`/../web/env-js.jar <$OUT;
fi;
