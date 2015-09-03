#!/bin/bash
pack_all_files()
{
    echo $1
    FILES=`ls $1`;
    OUT=./pack_files.data;
    echo "load('http://frankm.web.cern.ch/frankm/test/Online/Stomp/packer/packer.js');">$OUT;
    echo $FILES;
    for i in $FILES; do 
	base=`basename $i`;
	dir=`dirname $i`;
	echo "pack_javascript('${1}/${base}','${2}/${base}');">>$OUT;
    done;
#/usr/bin/java -Xms256M -Xmx256M -jar `dirname ${0}`/../web/env-js.jar <$OUT
    if test -f /sw/java/i386_redhat61/jdk/sun-1.5.0/bin/java;
    then
	/sw/java/i386_redhat61/jdk/sun-1.5.0/bin/java -Xms256M -Xmx256M -jar `dirname ${0}`/../web/env-js.jar <${OUT};
    else
	/usr/bin/java -Xms256M -Xmx256M -jar `dirname ${0}`/../web/env-js.jar <${OUT};
    fi;
}
usage()
{
    echo "packer <input-directory>  <output-directory>";
}

if test -z "${1}"; then
    usage;
elif test -z "${2}"; then
    usage;
else
    pack_all_files ${1} ${2};
fi;
##load('http://frankm.web.cern.ch/frankm/Online/packer/packer.js');
##pack_javascript('$1','/afs/cern.ch/user/f/frankm/WWW/Online/packer');
##EOF
