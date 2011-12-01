#!/bin/bash
#
# Run the tracker
#
cd /group/online/dataflow/cmtuser/BitTorrent/Online/TorrentLoader/cmt
. setup.x86_64-slc5-gcc43-dbg.vars
export INTERFACE=`python -c "import socket;print socket.gethostbyname(socket.gethostname().split('.')[0])"`;
export SUBFARM_TRACKER=`python -c "import os,socket;print socket.gethostbyname(os.environ[\"DIM_DNS_NODE\"])"`;
export TOP_TRACKER=`python -c "import os,socket;print socket.gethostbyname('storectl01')"`;
#
exec -a ${UTGID} `which TorrentLoader.exe` \
    -interface=${INTERFACE} \
    -save_to=/dev/shm \
    -announce=http://${SUBFARM_TRACKER}:6969/announce,http://${TOP_TRACKER}:6969/announce \
    -parent=/STORECTL01_Torrent
