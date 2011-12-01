#!/bin/bash
cd /group/online/dataflow/cmtuser/BitTorrent/Online/TorrentLoader/cmt;
. setup.x86_64-slc5-gcc43-dbg.vars;
####export DIM_DNS_NODE=ecs04;
if [ -r /etc/sysconfig/dim ]; then
   . /etc/sysconfig/dim
   export DIM_DNS_NODE
fi
if [ -z $DIM_DNS_NODE ]; then
   echo DIM_DNS_NODE undefined and /etc/sysconfig/dim not readable
   # exit 1
fi;
#
cd /dev/shm;
`which dimget.exe` -torrent=$*;
