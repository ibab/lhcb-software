#!/bin/bash
#cd /home/frankm/raul/Online_v4r12/MyTest/cmt
cd `dirname $0`
if test "`uname -i`" = "i386";
    then
    . setup.slc4_ia32_gcc34_dbg.vars
else
    . setup.slc4_amd64_gcc34_dbg.vars
fi;
echo `pwd`
export UTGID
h=`hostname -s`
H=`echo $h|tr a-z A-Z`
exec -a ${UTGID} `which gentest.exe` libROMon.so run_tasksupervisor \
    -input=file://${ROMONROOT}/xml/${H}.xml \
    -invent=file://${ROMONROOT}/xml/TaskInventory.xml \
    -node=${h} $*

