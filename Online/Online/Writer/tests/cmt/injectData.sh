#!/bin/bash


#export MINITERM='xterm -iconic -sl 10000 -ls -132 -geometry 132x10 -title '

#start_py_task()
#{
#  $MINITERM ${1}@${HOST}   -e "export UTGID=${NODENAME}/${1};   exec -a \${UTGID} $Class1_task -opt=command=\"${2}\"&"
#}


echo $*
id=0

for i in $* ; do
#start_py_task Mdf2Mbm9 "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMFile(['TestWriter'],fname='root:$1',partitionBuffers=True)"
    start_py_task Mdf2Mbm_$id "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBMFile(['TestWriter'],fname='root:$i')"
    sleep 10
    id=$(($id+1))

done

